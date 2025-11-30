#pragma once

#include <amethyst/id.hpp>
#include <amethyst/texture.hpp>
#include <amethyst/types.hpp>

namespace Amy {
class AssetMgr {
 public:
  AssetMgr() = default;
  ~AssetMgr() = default;

  void AutoLoad(const ID& name, ksr path);

  void Add(const ID& id, Asset* v) {
    if (pAssets.count(id)) {
      throw std::runtime_error("[amy]: assets: " + id.GetName() +
                               " already exists!");
    }
    pAssets[id] = v;
  }

  void Remove(const ID& id) {
    if (pAssets.count(id)) {
      pAssets.erase(id);
    }
  }

  template <typename T>
  T* Get(const ID& id) {
    auto r = pAssets.find(id);
    if (r == pAssets.end()) {
      throw std::runtime_error("[amy] assets: unable to find " + id.GetName());
    }
    if (auto v = dynamic_cast<T*>(r->second)) {
      return v;
    } else {
      throw std::runtime_error(id.GetName() + " is not of type " +
                               typeid(T).name());
    }
  }

  template <typename T>
  bool IsType(const ID& id) {
    auto r = pAssets.find(id);
    if (r == pAssets.end()) {
      throw std::runtime_error("[amy] assets: unable to find " + id.GetName());
    }
    return dynamic_cast<T*>(r->second) != nullptr;
  }

  template <typename T>
  size_t Count() const {
    size_t ret = 0;
    for (auto& it : pAssets) {
      if (dynamic_cast<T*>(it.second)) {
        ret++;
      }
    }
    return ret;
  }

 private:
  std::map<ID, Asset*> pAssets;
};
}  // namespace Amy