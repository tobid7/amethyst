#include <3ds.h>

#include <amethyst/ctru.hpp>

namespace Amy {
namespace Ctr {
void Init(ui srvs) {
  if (srvs & Romfs) {
    romfsInit();
  }
  if (srvs & GfxDefault) {
    gfxInitDefault();
  }
}
ull GetTime() { return osGetTime(); }
}  // namespace Ctr
}  // namespace Amy