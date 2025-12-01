#pragma once

#include <3ds.h>

#include <memory>
#include <stdexcept>

// Custom C++ Allocator class to interface with libctru linear heap memory
// based on this guide:
// https://johnfarrier.com/custom-allocators-in-c-high-performance-memory-management/

namespace Amy {
template <typename T>
class LinearAllocator {
 public:
  using value_type = T;
  LinearAllocator() noexcept = default;
  template <typename U>
  constexpr LinearAllocator(const LinearAllocator<U>&) noexcept {}

  T* allocate(std::size_t n) {
    if (n > max_size()) {
      throw std::runtime_error("[Amy] LinearAllocator: Bad alloc!");
    }
    return static_cast<T*>(linearAlloc(n * sizeof(T)));
  }
  void deallocate(T* p, std::size_t) noexcept { linearFree(p); }

  template <class U, class... Args>
  void construct(U* p, Args&&... args) {
    ::new ((void*)p) U(std::forward<Args>(args)...);
  }

  template <class U>
  void destroy(U* p) {
    p->~U();
  }

  friend bool operator==(const LinearAllocator, const LinearAllocator) {
    return true;
  }
  friend bool operator!=(const LinearAllocator, const LinearAllocator) {
    return false;
  }

  // Use linearSpace free as max_size to not allocate out of bounds
  // or to b eable to see a crash report screen.
  size_t max_size() const noexcept { return linearSpaceFree(); }
};
}  // namespace Amy