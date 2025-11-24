#include <3ds.h>

#include <amethyst/app.hpp>
#include <amethyst/ctru.hpp>

namespace amy {
void app::run() {
  while (aptMainLoop()) {
    ull c = ctru::getTime();
    m_delta = static_cast<double>(c) - static_cast<float>(m_last);
    m_last = c;
    main();
  }
}
}  // namespace amy