#include <3ds.h>

#include <amethyst/app.hpp>
#include <amethyst/ctru.hpp>
#include <amethyst/utils.hpp>

namespace Amy {
void App::Run() {
  pLast = Utils::GetTimeNano();
  while (aptMainLoop()) {
    ull c = Utils::GetTimeNano();
    pDelta = static_cast<double>(static_cast<double>(c) -
                                 static_cast<double>(pLast)) *
             0.000001;
    pTime += pDelta * 0.001;
    pLast = c;
    Main();
  }
}
}  // namespace Amy