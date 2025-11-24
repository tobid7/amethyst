#pragma once

#include <amethyst/asset.hpp>
#include <amethyst/types.hpp>

namespace amy {
class app {
 public:
  app() {}
  ~app() {}

  virtual void main() {}
  void run();
  double delta() const { return m_delta; }

 private:
  ull m_last;
  double m_delta;
};
}  // namespace amy