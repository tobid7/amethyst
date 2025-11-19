#pragma once

#include <citro3d.h>

#include <amethyst/asset.hpp>
#include <amethyst/image.hpp>

namespace amy {
class texture : public asset {
 public:
  texture() = default;
  texture(cstr& path);
  ~texture();
  void load(cstr& path);
  void unload();

  int w() const { return m_w; }
  int& w() { return m_w; }
  int h() const { return m_h; }
  int& h() { return m_h; }

  C3D_Tex* getTex() { return m_loaded ? &m_tex : nullptr; }

 private:
  C3D_Tex m_tex;
  int m_w = 0;
  int m_h = 0;
  bool m_loaded = false;
};
}  // namespace amy