#include <amethyst.hpp>
#include <exception>
#include <iostream>
#include <map>
#include <vector>

#define TRACK_NEW_DELETE

#ifdef AMY_3DS
#include <3ds.h>

void NpiD7CxxExceptionHandler() {
  gfxInitDefault();
  consoleInit(GFX_TOP, nullptr);
  std::cout << "[C++ Error Handler]" << std::endl;
  try {
    std::rethrow_exception(std::current_exception());
  } catch (const std::exception &e) {
    std::cout << "\n\n" << e.what() << std::endl;
  }
  aptSetHomeAllowed(false);
  aptSetSleepAllowed(false);
  while (aptMainLoop()) {
    hidScanInput();
    if (hidKeysDown() & KEY_START) {
      break;
    }
  }
  std::abort();
}
#endif

static Amy::Memory::MemMetrics metrics;

#ifdef TRACK_NEW_DELETE

constexpr uint32_t MemDBGMagic = 0x774733d7;

struct alignas(16) MemDBGHeader {
  uint32_t Magic = MemDBGMagic;
  size_t Size = 0;
  size_t SizeInv = 0;
  void *RawPtr;
};

extern "C" {
void *__real_malloc(size_t size);
void __real_free(void *ptr);
void *__real_realloc(void *ptr, size_t size);
void *__real_calloc(size_t nmemb, size_t size);
void *__real_memalign(size_t alignment, size_t size);

void *__wrap_malloc(size_t size) {
  void *ptr = __real_malloc(size + sizeof(MemDBGHeader));
  if (!ptr) return reinterpret_cast<void *>(ptr);
  MemDBGHeader *hdr = reinterpret_cast<MemDBGHeader *>(ptr);
  hdr->Magic = MemDBGMagic;
  hdr->Size = size;
  hdr->SizeInv = ~hdr->Size;
  hdr->RawPtr = ptr;
  metrics.Allocated.fetch_add(hdr->Size, std::memory_order_relaxed);
  metrics.Current.fetch_add(hdr->Size, std::memory_order_relaxed);
  metrics.Allocations.fetch_add(1, std::memory_order_relaxed);
  metrics._malloc++;
  return reinterpret_cast<void *>(hdr + 1);
}

void __wrap_free(void *ptr) {
  if (!ptr) return;
  MemDBGHeader *hdr = reinterpret_cast<MemDBGHeader *>(ptr) - 1;
  metrics._free++;
  if (hdr->Magic == MemDBGMagic && hdr->Size == ~hdr->SizeInv) {
    hdr->Magic = 0;
    size_t szs = hdr->Size;
    metrics.Deleted.fetch_add(szs, std::memory_order_relaxed);
    metrics.Current.fetch_sub(szs, std::memory_order_relaxed);
    metrics.Allocations.fetch_sub(1, std::memory_order_relaxed);
    __real_free(hdr->RawPtr);
  } else {
    __real_free(ptr);
  }
}

void *__wrap_calloc(size_t nmemb, size_t size) {
  size_t total = nmemb * size;
  void *ptr = __wrap_malloc(total);
  if (ptr) memset(ptr, 0, total);
  metrics._calloc++;
  return ptr;
}

void *__wrap_memalign(size_t alignment, size_t size) {
  size_t total = size + alignment + sizeof(MemDBGHeader);
  void *raw = __real_malloc(total);
  metrics._memalign++;
  if (!raw) return nullptr;

  uintptr_t base = reinterpret_cast<uintptr_t>(raw) + sizeof(MemDBGHeader);
  uintptr_t aligned = (base + alignment - 1) & ~(alignment - 1);

  auto *hdr = reinterpret_cast<MemDBGHeader *>(aligned) - 1;
  hdr->Magic = MemDBGMagic;
  hdr->Size = size;
  hdr->SizeInv = ~size;
  hdr->RawPtr = raw;

  metrics.Allocated.fetch_add(size, std::memory_order_relaxed);
  metrics.Current.fetch_add(size, std::memory_order_relaxed);
  metrics.Allocations.fetch_add(1, std::memory_order_relaxed);

  return reinterpret_cast<void *>(aligned);
}

void *__wrap_aligned_alloc(size_t alignment, size_t size) {
  return __wrap_memalign(alignment, size);
}

void *__wrap_realloc(void *ptr, size_t newSize) {
  if (!ptr) return __wrap_malloc(newSize);
  if (newSize == 0) {
    __wrap_free(ptr);
    return nullptr;
  }
  metrics._realloc++;

  auto *hdr = reinterpret_cast<MemDBGHeader *>(ptr) - 1;
  if (hdr->Magic != MemDBGMagic) return __real_realloc(ptr, newSize);

  size_t oldSize = hdr->Size;

  void *newPtr = __wrap_malloc(newSize);
  if (!newPtr) return nullptr;

  memcpy(newPtr, ptr, std::min(oldSize, newSize));
  __wrap_free(ptr);

  return newPtr;
}
}
#endif

namespace Amy {

void RegisterCxxExceptionHandler() {
#ifdef AMY_3DS
  std::set_terminate(NpiD7CxxExceptionHandler);
#endif
}
void *Malloc(size_t size) { return malloc(size); }
void Free(void *ptr) { free(ptr); }
ull Memory::GetTotalAllocated() { return metrics.Allocated; }
ull Memory::GetTotalFreed() { return metrics.Deleted; }
ull Memory::GetCurrent() { return metrics.CurrentlyAllocated(); }
ull Memory::GetAllocationCount() { return metrics.Allocations; }
const Memory::MemMetrics &Memory::GetMetrics() { return metrics; }
}  // namespace Amy
