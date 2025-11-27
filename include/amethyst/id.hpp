#pragma once

#include <amethyst/types.hpp>
#include <amethyst/utils.hpp>

namespace Amy {
class ID {
 public:
  constexpr ID(const ui& id) { pID = id; }
  constexpr ID(std::string name) {
    pID = Utils::FNV32(name);
#ifdef AMY_KEEP_STR_ID
    pName = name;
#endif
  }
  constexpr ID(const char* name) {
    pID = Utils::FNV32(name);
#ifdef AMY_KEEP_STR_ID
    pName = name;
#endif
  }
  ~ID() {}

  ui GetID() { return pID; }
  std::string GetName() const {
#ifdef AMY_KEEP_STR_ID
    return pName;
#else
    return std::format("hash({:#08x})", pID);
#endif
  }

  operator ui() const { return pID; }
  operator std::string() const { return GetName(); }

 private:
  ui pID;
#ifdef AMY_KEEP_STR_ID
  str pName;
#endif
};
}  // namespace Amy