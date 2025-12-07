#pragma once

#include <amethyst/types.hpp>

namespace Amy {
class Asset {
 public:
  Asset() = default;
  virtual ~Asset() = default;
  AMY_SHARED(Asset);
};
}  // namespace Amy
