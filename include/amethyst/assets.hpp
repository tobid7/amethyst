#pragma once

#include <amethyst/texture.hpp>
#include <amethyst/types.hpp>

namespace amy {
class assets {
 public:
  assets() = default;
  ~assets() = default;

  void add(cstr& id, asset* v) { m_assets[id] = v; }
  void remove(cstr& id) {
    if (m_assets.count(id)) {
      m_assets.erase(id);
    }
  }
  template <typename T>
  T* get(cstr& id) {
    auto r = m_assets.find(id);
    if (r == m_assets.end()) {
      throw std::runtime_error("[amy] assets: unable to find " + id);
    }
    if (auto v = dynamic_cast<T*>(r->second)) {
      return v;
    } else {
      throw std::runtime_error(id + " is not of type " + typeid(T).name());
    }
  }

 private:
  std::map<str, asset*> m_assets;
};
}  // namespace amy