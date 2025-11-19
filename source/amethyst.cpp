#include <amethyst.hpp>
#include <exception>
#include <iostream>

#ifdef AMY_3DS
#include <3ds.h>

void NpiD7CxxExceptionHandler() {
  gfxInitDefault();
  consoleInit(GFX_TOP, nullptr);
  std::cout << "[C++ Error Handler]" << std::endl;
  try {
    std::rethrow_exception(std::current_exception());
  } catch (const std::exception& e) {
    std::cout << "\n\n" << e.what() << std::endl;
  }
  aptSetHomeAllowed(false);
  aptSetSleepAllowed(false);
  while (aptMainLoop()) {
    hidScanInput();
    if (hidKeysDown() & KEY_START) {
      break;
    }
  }
  std::abort();
}
#endif

namespace amy {
void registerCxxExceptionHandler() {
#ifdef AMY_3DS
  std::set_terminate(NpiD7CxxExceptionHandler);
#endif
}
}  // namespace amy