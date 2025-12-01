#pragma once

#include <amethyst/asset.hpp>
#include <amethyst/types.hpp>
#include <amethyst/utils.hpp>

namespace Amy {
class App {
 public:
  App() {
    pDelta = 0.0;
    pTime = 0.0;
    pLast = 0LL;
    pExit = false;
  }
  ~App() {}

  virtual void Main() {}
  void Run();
  double Delta() const { return pDelta; }
  double Time() const { return pTime; }
  void Exit() { pExit = true; }

 private:
  bool pExit;
  ull pLast;
  double pDelta;
  double pTime;
};
}  // namespace Amy