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
  unload();
  image img(path);
  if (img.width() > 1024 || img.height() > 1024) {
    throw std::runtime_error("Max Texture Size is 1024x1024!");
  }

  int bpp = img.bpp();
  m_w = img.width();
  if (utils::isSingleBitNum(m_w)) {
    m_w = utils::nextPow2(m_w);
  }
  m_h = img.width();
  if (utils::isSingleBitNum(m_h)) {
    m_h = utils::nextPow2(m_h);
  }
  auto filter = GPU_NEAREST;
  auto format = image2TexFmt(img.fmt());
  C3D_TexInit(&m_tex, (u16)m_w, (u16)m_h, format);
  C3D_TexSetFilter(&m_tex, filter, filter);
  // Using std::fill_n instead cause i hate this error lines
  // under the memset func in my editor
  std::fill_n((unsigned char*)m_tex.data, m_tex.size, 0);
  for (int x = 0; x < img.width(); x++) {
    for (int y = 0; y < img.height(); y++) {
      int dst_pos = tile3dsTex(x, y, m_w) * bpp;
      int src_pos = (y * img.width() + x) * bpp;
      /// Best idea i had
      for (int i = 0; i < bpp; i++) {
        ((u8*)m_tex.data)[dst_pos + bpp - 1 - i] = img[src_pos + i];
      }
    }
  }
  C3D_TexFlush(&m_tex);
  m_tex.border = 0x00000000;
  C3D_TexSetWrap(&m_tex, GPU_REPEAT, GPU_REPEAT);
  m_loaded = true;
}
}  // namespace amy