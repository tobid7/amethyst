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

void *operator new(size_t size) {
  void *ptr = malloc(size + sizeof(size_t));
  if (!ptr) return ptr;
  *((size_t *)ptr) = size;
  metrics.Allocated.fetch_add(size, std::memory_order_relaxed);
  metrics.Current.fetch_add(size, std::memory_order_relaxed);
  metrics.Allocations.fetch_add(1, std::memory_order_relaxed);
  return ptr + sizeof(size_t);
}

void operator delete(void *ptr, size_t size) noexcept {
  void *raw = ptr - sizeof(size_t);
  size_t szs = *((size_t *)raw);
  metrics.Deleted.fetch_add(szs, std::memory_order_relaxed);
  metrics.Current.fetch_sub(szs, std::memory_order_relaxed);
  metrics.Allocations.fetch_sub(1, std::memory_order_relaxed);
  free(raw);
}

void operator delete(void *ptr) noexcept {
  void *raw = ptr - sizeof(size_t);
  size_t szs = *((size_t *)raw);
  metrics.Deleted.fetch_add(szs, std::memory_order_relaxed);
  metrics.Current.fetch_sub(szs, std::memory_order_relaxed);
  metrics.Allocations.fetch_sub(1, std::memory_order_relaxed);
  free(raw);
}

void *operator new[](size_t size) {
  void *ptr = malloc(size + sizeof(size_t));
  if (!ptr) return ptr;

  *((size_t *)ptr) = size;
  metrics.Allocated.fetch_add(size, std::memory_order_relaxed);
  metrics.Current.fetch_add(size, std::memory_order_relaxed);
  metrics.Allocations.fetch_add(1, std::memory_order_relaxed);

  return ptr + sizeof(size_t);
}

void operator delete[](void *ptr) noexcept {
  void *raw = ptr - sizeof(size_t);
  size_t szs = *((size_t *)raw);
  metrics.Deleted.fetch_add(szs, std::memory_order_relaxed);
  metrics.Current.fetch_sub(szs, std::memory_order_relaxed);
  metrics.Allocations.fetch_sub(1, std::memory_order_relaxed);
  free(raw);
}
#endif

namespace Amy {

void RegisterCxxExceptionHandler() {
#ifdef AMY_3DS
  std::set_terminate(NpiD7CxxExceptionHandler);
#endif
}
void *Malloc(size_t size) {
#ifdef TRACK_NEW_DELETE
  void *ptr = malloc(size + sizeof(size_t));
  if (!ptr) return nullptr;

  *((size_t *)ptr) = size;
  metrics.Allocated.fetch_add(size, std::memory_order_relaxed);
  metrics.Current.fetch_add(size, std::memory_order_relaxed);
  metrics.Allocations.fetch_add(1, std::memory_order_relaxed);

  return ptr + sizeof(size_t);
#else
  return malloc(size);
#endif
}
void Free(void *ptr) {
#ifdef TRACK_NEW_DELETE
  void *raw = ptr - sizeof(size_t);
  size_t szs = *((size_t *)raw);
  metrics.Deleted.fetch_add(szs, std::memory_order_relaxed);
  metrics.Current.fetch_sub(szs, std::memory_order_relaxed);
  metrics.Allocations.fetch_sub(1, std::memory_order_relaxed);
  free(raw);
#else
  free(ptr);
#endif
}
ull Memory::GetTotalAllocated() { return metrics.Allocated; }
ull Memory::GetTotalFreed() { return metrics.Deleted; }
ull Memory::GetCurrent() { return metrics.CurrentlyAllocated(); }
ull Memory::GetAllocationCount() { return metrics.Allocations; }
}  // namespace Amy
