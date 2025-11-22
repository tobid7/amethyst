#pragma once

#include <3ds.h>

#include <memory>
#include <stdexcept>

// Custom C++ Allocator class to interface with libctru linear heap memory
// based on this guide:
// https://johnfarrier.com/custom-allocators-in-c-high-performance-memory-management/

namespace amy {
template <typename T>
class linearAllocator {
 public:
  using value_type = T;
  linearAllocator() = default;
  template <typename U>
  constexpr linearAllocator(const linearAllocator<U>&) noexcept {}

  T* allocate(std::size_t n) {
    if (n > std::allocator_traits<linearAllocator>::max_size(*this)) {
      throw std::bad_alloc();
    }
    return static_cast<T*>(linearAlloc(n * sizeof(T)));
  }
  void deallocate(T* p, std::size_t) noexcept { linearFree(p); }

  friend bool operator==(const linearAllocator, const linearAllocator) {
    return true;
  }
  friend bool operator!=(const linearAllocator, const linearAllocator) {
    return false;
  }
};
}  // namespace amy