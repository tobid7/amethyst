#pragma once

#include <amethyst/texture.hpp>
#include <amethyst/types.hpp>

namespace Amy {
class Assets {
 public:
  Assets() = default;
  ~Assets() = default;

  void add(ksr id, Asset* v) { pAssets[id] = v; }
  void remove(ksr id) {
    if (pAssets.count(id)) {
      pAssets.erase(id);
    }
  }
  template <typename T>
  T* get(ksr id) {
    auto r = pAssets.find(id);
    if (r == pAssets.end()) {
      throw std::runtime_error("[amy] assets: unable to find " + id);
    }
    if (auto v = dynamic_cast<T*>(r->second)) {
      return v;
    } else {
      throw std::runtime_error(id + " is not of type " + typeid(T).name());
    }
  }

 private:
  std::map<str, Asset*> pAssets;
};
}  // namespace Amy