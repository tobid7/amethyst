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

struct MemDBGHeader {
  MemDBGHeader(size_t s) {
    Magic = MemDBGMagic;
    Size = s;
  }
  uint32_t Magic = MemDBGMagic;
  size_t Size = 0;
};

void *MemDBGAllocate(size_t size) {
  uintptr_t ptr = (uintptr_t)malloc(size + sizeof(MemDBGHeader));
  if (!ptr) return reinterpret_cast<void *>(ptr);
  *((MemDBGHeader *)ptr) = MemDBGHeader(size);
  metrics.Allocated.fetch_add(size, std::memory_order_relaxed);
  metrics.Current.fetch_add(size, std::memory_order_relaxed);
  metrics.Allocations.fetch_add(1, std::memory_order_relaxed);
  return reinterpret_cast<void *>(ptr + sizeof(MemDBGHeader));
}

void MemDBGFree(void *ptr) {
  uintptr_t raw = (uintptr_t)ptr - sizeof(MemDBGHeader);
  if (((MemDBGHeader *)raw)->Magic == MemDBGMagic) {
    size_t szs = ((MemDBGHeader *)raw)->Size;
    metrics.Deleted.fetch_add(szs, std::memory_order_relaxed);
    metrics.Current.fetch_sub(szs, std::memory_order_relaxed);
    metrics.Allocations.fetch_sub(1, std::memory_order_relaxed);
    free(reinterpret_cast<void *>(raw));
  } else {
    free(ptr);
  }
}

void *operator new(size_t size) { return MemDBGAllocate(size); }
void operator delete(void *ptr, size_t) noexcept { MemDBGFree(ptr); }
void operator delete(void *ptr) noexcept { MemDBGFree(ptr); }
void *operator new[](size_t size) { return MemDBGAllocate(size); }
void operator delete[](void *ptr) noexcept { MemDBGFree(ptr); }
#endif

namespace Amy {

void RegisterCxxExceptionHandler() {
#ifdef AMY_3DS
  std::set_terminate(NpiD7CxxExceptionHandler);
#endif
}
void *Malloc(size_t size) {
#ifdef TRACK_NEW_DELETE
  return MemDBGAllocate(size);
#else
  return malloc(size);
#endif
}
void Free(void *ptr) {
#ifdef TRACK_NEW_DELETE
  MemDBGFree(ptr);
#else
  free(ptr);
#endif
}
ull Memory::GetTotalAllocated() { return metrics.Allocated; }
ull Memory::GetTotalFreed() { return metrics.Deleted; }
ull Memory::GetCurrent() { return metrics.CurrentlyAllocated(); }
ull Memory::GetAllocationCount() { return metrics.Allocations; }
}  // namespace Amy
