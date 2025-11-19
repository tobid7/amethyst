#include <3ds.h>

#include <amethyst/ctru.hpp>

namespace amy {
namespace ctru {
void init(unsigned int srvs) {
  if (srvs & romfs) {
    romfsInit();
  }
  if (srvs & gfx_def) {
    gfxInitDefault();
  }
}
}  // namespace ctru
}  // namespace amy