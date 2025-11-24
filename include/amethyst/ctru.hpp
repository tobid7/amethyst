#pragma once

#include <amethyst/types.hpp>

namespace amy {
namespace ctru {
enum services {
  romfs = 1 << 0,
  cfgu = 1 << 1,
  gfx = 1 << 2,
  gfx_def = 1 << 3,
  def = romfs | gfx_def
};
void init(unsigned int srv = def);
ull getTime();
}  // namespace ctru
}  // namespace amy