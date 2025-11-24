#pragma once

#include <citro3d.h>

#include <amethyst/asset.hpp>
#include <amethyst/image.hpp>
#include <amethyst/rect.hpp>
#include <amethyst/types.hpp>

namespace amy {
class texture : public asset {
 public:
  texture() = default;
  texture(cstr& path);
  ~texture();
  void load(cstr& path);
  void load(const std::vector<uc>& pixels, int w, int h, int bpp = 4,
            image::format fmt = image::RGBA);
  void unload();

  int w() const { return m_size.x; }
  int& w() { return m_size.x; }
  int h() const { return m_size.y; }
  int& h() { return m_size.y; }
  ivec2 size() const { return m_size; }
  ivec2& size() { return m_size; }
  rect& uv() { return m_uv; }

  C3D_Tex* ptr() { return m_loaded ? &m_tex : nullptr; }

  void bind(int reg = 0);

 private:
  C3D_Tex m_tex;
  ivec2 m_size;
  rect m_uv;
  bool m_loaded = false;
};
}  // namespace amy