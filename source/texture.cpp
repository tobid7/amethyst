#include <3ds.h>

#include <algorithm>
#include <amethyst/image.hpp>
#include <amethyst/texture.hpp>
#include <amethyst/utils.hpp>
#include <stdexcept>

namespace amy {
ui tile3dsTex(int x, int y, int w) {
  return ((((y >> 3) * ((int)w >> 3) + (x >> 3)) << 6) +
          ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) |
           ((x & 4) << 2) | ((y & 4) << 3)));
}

GPU_TEXCOLOR image2TexFmt(const image::format& fmt) {
  switch (fmt) {
    case image::RGB:
      return GPU_RGB8;
      break;
    case image::RGBA:
      return GPU_RGBA8;
      break;
    case image::RGB565:
      return GPU_RGB565;
      break;
    default:
      // Dummy
      return GPU_A4;
      throw std::runtime_error(
          "[amy] texture: Unsupported texture format used!");
      break;
  }
}
texture::texture(cstr& path) { load(path); }

texture::~texture() { unload(); }

void texture::unload() {
  if (m_loaded) {
    C3D_TexDelete(&m_tex);
    m_loaded = false;
  }
}

void texture::load(cstr& path) {
  image img(path);
  if (img.width() > 1024 || img.height() > 1024) {
    throw std::runtime_error("Max Texture Size is 1024x1024!");
  }
  load(img.getBuffer(), img.width(), img.height(), img.bpp(), img.fmt());
}

void texture::load(const std::vector<uc>& pixels, int w, int h, int bpp,
                   image::format fmt) {
  if (w > 1024 || h > 1024) {
    throw std::runtime_error("Max Texture Size is 1024x1024!");
  }
  unload();

  m_size.x = w;
  if (utils::isSingleBitNum(m_size.x)) {
    m_size.x = utils::nextPow2(m_size.x);
  }
  m_size.y = h;
  if (utils::isSingleBitNum(m_size.y)) {
    m_size.y = utils::nextPow2(m_size.y);
  }
  auto filter = GPU_NEAREST;
  auto format = image2TexFmt(fmt);
  C3D_TexInit(&m_tex, (u16)m_size.x, (u16)m_size.y, format);
  C3D_TexSetFilter(&m_tex, filter, filter);
  // Using std::fill_n instead cause i hate this error lines
  // under the memset func in my editor
  std::fill_n((unsigned char*)m_tex.data, m_tex.size, 0);
  for (int x = 0; x < w; x++) {
    for (int y = 0; y < h; y++) {
      int dst_pos = tile3dsTex(x, y, m_size.x) * bpp;
      int src_pos = (y * w + x) * bpp;
      /// Best idea i had
      for (int i = 0; i < bpp; i++) {
        ((u8*)m_tex.data)[dst_pos + bpp - 1 - i] = pixels[src_pos + i];
      }
    }
  }
  C3D_TexFlush(&m_tex);
  m_tex.border = 0x00000000;
  C3D_TexSetWrap(&m_tex, GPU_REPEAT, GPU_REPEAT);
  m_loaded = true;
}

void texture::bind(int reg) { C3D_TexBind(reg, &m_tex); }
}  // namespace amy