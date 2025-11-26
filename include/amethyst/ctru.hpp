#pragma once

#include <amethyst/types.hpp>

namespace Amy {
namespace Ctr {
enum Services {
  Romfs = 1 << 0,
  Cfgu = 1 << 1,
  Gfx = 1 << 2,
  GfxDefault = 1 << 3,
  Default = Romfs | GfxDefault,
};
void Init(ui srv = Default);
ull GetTime();
}  // namespace Ctr
}  // namespace Amy