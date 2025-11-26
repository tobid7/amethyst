#include <3ds.h>

#include <algorithm>
#include <amethyst/image.hpp>
#include <amethyst/texture.hpp>
#include <amethyst/utils.hpp>
#include <stdexcept>

namespace Amy {
ui tile3dsTex(int x, int y, int w) {
  return ((((y >> 3) * ((int)w >> 3) + (x >> 3)) << 6) +
          ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) |
           ((x & 4) << 2) | ((y & 4) << 3)));
}

GPU_TEXCOLOR image2TexFmt(const Image::Format& fmt) {
  switch (fmt) {
    case Image::RGB:
      return GPU_RGB8;
      break;
    case Image::RGBA:
      return GPU_RGBA8;
      break;
    case Image::RGB565:
      return GPU_RGB565;
      break;
    default:
      // Dummy
      return GPU_A4;
      throw std::runtime_error(
          "[amy] texture: Unsupported texture Format used!");
      break;
  }
}
Texture::Texture(ksr path) { Load(path); }

Texture::~Texture() { Unload(); }

void Texture::Unload() {
  if (pLoaded) {
    C3D_TexDelete(&pTex);
    pLoaded = false;
  }
}

void Texture::Load(ksr path) {
  Image img(path);
  if (img.Width() > 1024 || img.Height() > 1024) {
    throw std::runtime_error("Max Texture Size is 1024x1024!");
  }
  Load(img.GetBuffer(), img.Width(), img.Height(), img.Bpp(), img.Fmt());
}

void Texture::Load(kvr<uc> pixels, int w, int h, int bpp, Image::Format fmt) {
  if (w > 1024 || h > 1024) {
    throw std::runtime_error("Max Texture Size is 1024x1024!");
  }
  Unload();

  pSize.x = w;
  if (Utils::IsSingleBitNum(pSize.x)) {
    pSize.x = Utils::NextPow2(pSize.x);
  }
  pSize.y = h;
  if (Utils::IsSingleBitNum(pSize.y)) {
    pSize.y = Utils::NextPow2(pSize.y);
  }
  auto filter = GPU_NEAREST;
  auto Format = image2TexFmt(fmt);
  C3D_TexInit(&pTex, (u16)pSize.x, (u16)pSize.y, Format);
  C3D_TexSetFilter(&pTex, filter, filter);
  // Using std::fill_n instead cause i hate this error lines
  // under the memset func in my editor
  std::fill_n((unsigned char*)pTex.data, pTex.size, 0);
  for (int x = 0; x < w; x++) {
    for (int y = 0; y < h; y++) {
      int dst_pos = tile3dsTex(x, y, pSize.x) * bpp;
      int src_pos = (y * w + x) * bpp;
      /// Best idea i had
      for (int i = 0; i < bpp; i++) {
        ((u8*)pTex.data)[dst_pos + bpp - 1 - i] = pixels[src_pos + i];
      }
    }
  }
  C3D_TexFlush(&pTex);
  pTex.border = 0x00000000;
  C3D_TexSetWrap(&pTex, GPU_REPEAT, GPU_REPEAT);
  pLoaded = true;
}

void Texture::Bind(int reg) { C3D_TexBind(reg, &pTex); }
}  // namespace Amy