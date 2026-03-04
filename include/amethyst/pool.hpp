#pragma once

#include <amethyst/types.hpp>
#include <memory>

namespace Amy {
template <typename T, typename Alloc = std::allocator<T>>
class Pool {
 public:
  Pool() = default;
  ~Pool() = default;
  AMY_RAW(Pool);

  static Pool::Ref Create(size_t size) { return Pool::New(size); }

  void Init(size_t size) {
    pPos = 0;
    pCap = size;
    pPool.resize(size);
  }

  Pool(const Pool&) = delete;
  Pool(Pool&&) = delete;
  Pool& operator=(const Pool&) = delete;
  Pool& operator=(Pool&&) = delete;

  T* Allocate(size_t num = 1) {
    if (Approve(num)) return nullptr;
    T* ret = &pPool[pPos];
    pPos += num;
    return ret;
  }

  bool Approve(size_t num) {
    if ((pPos + num) >= pCap) {
      throw std::runtime_error(
          std::format("[Amy::Pool]: Trying to allocate {} elements but this is "
                      "going out of range ({}/{})",
                      num, pPos + num, pCap));
      return true;
    }
    return false;
  }

  void Reset() {
    // Should Clear the data as well but for performance leave it out
    pPos = 0;
  }

 private:
  Pool(size_t size) : pCap(size), pPos(0) { pPool.resize(size); }
  size_t pCap = 0;
  size_t pPos = 0;
  std::vector<T, Alloc> pPool;
};
}  // namespace Amy