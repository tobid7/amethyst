#include <amethyst/image.hpp>
#include <amethyst/utils.hpp>
#include <stdexcept>

// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Amy {
void Image::Load(ksr path) {
  int c = 0;
  uc* buf = stbi_load(path.c_str(), &pW, &pH, &c, 4);
  if (c == 3) {
    // Releoading the Image with tree channels requestet
    // Still need to find a better way for this :(
    stbi_image_free(buf);
    buf = stbi_load(path.c_str(), &pW, &pH, &c, 3);
    pBuffer.assign(buf, buf + (pW * pH * 3));
    pFmt = RGB;
    stbi_image_free(buf);
  } else if (c == 4) {
    pBuffer.assign(buf, buf + (pW * pH * 4));
    pFmt = RGBA;
    stbi_image_free(buf);
  } else {
    throw std::runtime_error(path + " is using an unsupported image format!");
  }
}

void Image::Load(const std::vector<uc>& data) {
  int c = 0;
  uc* buf = stbi_load_from_memory(data.data(), data.size(), &pW, &pH, &c, 4);
  if (c == 3) {
    // Releoading the Image with tree channels requestet
    // Still need to find a better way for this :(
    stbi_image_free(buf);
    buf = stbi_load_from_memory(data.data(), data.size(), &pW, &pH, &c, 3);
    pBuffer.assign(buf, buf + (pW * pH * 3));
    pFmt = RGB;
    stbi_image_free(buf);
  } else if (pFmt == 4) {
    pBuffer.assign(buf, buf + (pW * pH * 4));
    pFmt = RGBA;
    stbi_image_free(buf);
  } else {
    throw std::runtime_error("image mem using an unsupported image format!");
  }
}

void Image::Copy(kvr<uc> pixels, int w, int h, const Format& fmt) {
  int f = GetBppOfFmt(fmt);
  if (pixels.size() != static_cast<size_t>(w * h * f)) {
    throw std::runtime_error("Connot copy image due to size error!");
  }
  pFmt = fmt;
  pW = w;
  pH = h;
  pBuffer.clear();
  pBuffer.resize(w * h * f);
  for (size_t i = 0; i < pBuffer.size(); i++) {
    pBuffer[i] = pixels[i];
  }
}

int Image::GetBppOfFmt(const Image::Format& fmt) {
  switch (fmt) {
    case RGBA:
    case ABGR:
    case BGRA:
      return 4;
      break;
    case RGB:
    case BGR:
      return 3;
      break;
    case RGB565:
      return 2;
      break;

    default:
      return 0;  // Unknown
      break;
  }
}

void Image::Convert(Image& img, const Format& dst) {
  if (img.pFmt == dst) {
    return;
  } else if (img.pFmt == RGB && dst == BGR) {
    Utils::Image::ReverseBuf(img.pBuffer, img.pW, img.pH, 3);
    img.pFmt = BGR;
  } else if (img.pFmt == RGB && dst == RGBA) {
    Utils::Image::AddAlphaChannel(img.pBuffer, img.pW, img.pH);
    img.pFmt = RGBA;
  } else if (img.pFmt == RGBA && dst == RGB) {
    Utils::Image::RemoveAlphaChannel(img.pBuffer, img.pW, img.pH);
    img.pFmt = RGB;
  } else if (img.pFmt == RGBA && dst == ABGR) {
    Utils::Image::ReverseBuf(img.pBuffer, img.pW, img.pH, 4);
    img.pFmt = ABGR;
  } else if (img.pFmt == RGB && dst == ABGR) {
    Convert(img, RGBA);
    Convert(img, ABGR);
  } else if (img.pFmt == RGBA && dst == RGB565) {
    Convert(img, RGB);
    Convert(img, RGB565);
  } else if (img.pFmt == RGB && dst == RGB565) {
    // Inlined make pixel 565 func
    auto f = [](uc r, uc g, uc b) -> unsigned short {
      unsigned short _r = (r >> 3);
      unsigned short _g = (g >> 2);
      unsigned short _b = (b >> 3);
      return (_r << 11) | (_g << 5) | _b;
    };
    std::vector<uc> cpy = img.pBuffer;
    img.pBuffer.resize(img.pW * img.pH * 2);
    for (int y = 0; y < img.pW; y++) {
      for (int x = 0; x < img.pH; x++) {
        int src = (y * img.pW + x) * 3;
        int dst = (y * img.pW + x) * 2;
        unsigned short new_px = f(cpy[src + 0], cpy[src + 1], cpy[src + 2]);
        img.pBuffer[dst + 0] = new_px >> 8;
        img.pBuffer[dst + 1] = new_px & 0xff;
      }
    }
    img.pFmt = RGB565;
  }
}

void Image::Retile(Image& img, std::function<ui(int x, int y, int w)> src,
                   std::function<ui(int x, int y, int w)> dst) {
  std::vector<uc> cpy = img.pBuffer;
  /** could use fmt here but for 565 that woulnt work as it is not supported by
   * file loading where fmt is used */
  int bpp = img.Bpp();
  for (int y = 0; y < img.pH; y++) {
    for (int x = 0; x < img.pW; x++) {
      int src_idx = src(x, y, img.pW);
      int dst_idx = dst(x, y, img.pW);
      for (int i = 0; i < bpp; i++) {
        img.pBuffer[dst_idx + i] = cpy[src_idx + i];
      }
    }
  }
}
}  // namespace Amy