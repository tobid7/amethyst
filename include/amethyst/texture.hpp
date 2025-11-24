#pragma once

#include <citro3d.h>

#include <amethyst/asset.hpp>
#include <amethyst/image.hpp>
#include <amethyst/types.hpp>

namespace amy {
class texture : public asset {
 public:
  texture() = default;
  texture(cstr& path);
  ~texture();
  void load(cstr& path);
  void unload();

  int w() const { return m_size.x; }
  int& w() { return m_size.x; }
  int h() const { return m_size.y; }
  int& h() { return m_size.y; }
  ivec2 size() const { return m_size; }
  ivec2& size() { return m_size; }

  C3D_Tex* ptr() { return m_loaded ? &m_tex : nullptr; }

  void bind(int reg = 0);

 private:
  C3D_Tex m_tex;
  ivec2 m_size;
  bool m_loaded = false;
};
}  // namespace amy