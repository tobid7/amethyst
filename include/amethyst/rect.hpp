#pragma once

#include <amethyst/types.hpp>

namespace amy {
class rect {
 public:
  rect() : m_top(0), m_bot(0) {}
  ~rect() = default;
  rect(const fvec4& t, const fvec4& b) : m_top(t), m_bot(b) {}
  rect(const fvec2& tl, const fvec2& tr, const fvec2& bl, const fvec2& br)
      : m_top(tl, tr), m_bot(bl, br) {}
  rect(const fvec4& duv)
      : m_top(duv.x, duv.y, duv.z, duv.y), m_bot(duv.x, duv.w, duv.z, duv.w) {}

  fvec2 topLeft() const { return fvec2(m_top.x, m_top.y); }
  fvec2 topRight() const { return fvec2(m_top.z, m_top.w); }
  fvec2 botLeft() const { return fvec2(m_bot.x, m_bot.y); }
  fvec2 botRight() const { return fvec2(m_bot.z, m_bot.w); }

  fvec4 top() const { return m_top; }
  fvec4& top() { return m_top; }
  fvec4 bot() const { return m_bot; }
  fvec4& bot() { return m_bot; }

  rect& topLeft(const fvec2& v) {
    m_top.x = v.x;
    m_top.y = v.y;
    return *this;
  }

  rect& topRight(const fvec2& v) {
    m_top.z = v.x;
    m_top.w = v.y;
    return *this;
  }

  rect& botLeft(const fvec2& v) {
    m_bot.x = v.x;
    m_bot.y = v.y;
    return *this;
  }

  rect& botRight(const fvec2& v) {
    m_bot.z = v.x;
    m_bot.w = v.y;
    return *this;
  }

  void swapVec2XY() {
    m_top.SwapXY();
    m_top.SwapZW();
    m_bot.SwapXY();
    m_bot.SwapZW();
  }

 private:
  fvec4 m_top;
  fvec4 m_bot;
};
}  // namespace amy