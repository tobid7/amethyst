#pragma once

#include <amethyst/types.hpp>

namespace Amy {
class Rect {
 public:
  Rect() : pTop(0), pBot(0) {}
  ~Rect() = default;
  Rect(const fvec4& t, const fvec4& b) : pTop(t), pBot(b) {}
  Rect(const fvec2& tl, const fvec2& tr, const fvec2& bl, const fvec2& br)
      : pTop(tl, tr), pBot(bl, br) {}
  Rect(const fvec4& duv)
      : pTop(duv.x, duv.y, duv.z, duv.y), pBot(duv.x, duv.w, duv.z, duv.w) {}

  fvec2 TopLeft() const { return fvec2(pTop.x, pTop.y); }
  fvec2 TopRight() const { return fvec2(pTop.z, pTop.w); }
  fvec2 BotLeft() const { return fvec2(pBot.x, pBot.y); }
  fvec2 BotRight() const { return fvec2(pBot.z, pBot.w); }

  fvec4 Top() const { return pTop; }
  fvec4& Top() { return pTop; }
  fvec4 Bot() const { return pBot; }
  fvec4& Bot() { return pBot; }

  Rect& TopLeft(const fvec2& v) {
    pTop.x = v.x;
    pTop.y = v.y;
    return *this;
  }

  Rect& TopRight(const fvec2& v) {
    pTop.z = v.x;
    pTop.w = v.y;
    return *this;
  }

  Rect& BotLeft(const fvec2& v) {
    pBot.x = v.x;
    pBot.y = v.y;
    return *this;
  }

  Rect& BotRight(const fvec2& v) {
    pBot.z = v.x;
    pBot.w = v.y;
    return *this;
  }

  void SwapVec2XY() {
    pTop.SwapXY();
    pTop.SwapZW();
    pBot.SwapXY();
    pBot.SwapZW();
  }

 private:
  fvec4 pTop;
  fvec4 pBot;
};
}  // namespace Amy