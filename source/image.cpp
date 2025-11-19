#include <amethyst/image.hpp>
#include <amethyst/utils.hpp>
#include <stdexcept>

// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace amy {
void image::load(cstr& path) {
  int c = 0;
  uc* buf = stbi_load(path.c_str(), &m_w, &m_h, &c, 4);
  if (c == 3) {
    // Releoading the Image with tree channels requestet
    // Still need to find a better way for this :(
    stbi_image_free(buf);
    buf = stbi_load(path.c_str(), &m_w, &m_h, &c, 3);
    m_buffer.assign(buf, buf + (m_w * m_h * 3));
    m_fmt = RGB;
    stbi_image_free(buf);
  } else if (c == 4) {
    m_buffer.assign(buf, buf + (m_w * m_h * 4));
    m_fmt = RGBA;
    stbi_image_free(buf);
  } else {
    throw std::runtime_error(path + " is using an unsupported image format!");
  }
}

void image::load(const std::vector<uc>& data) {
  int c = 0;
  uc* buf =
      stbi_load_from_memory(data.data(), data.size(), &m_w, &m_h, &c, 4);
  if (c == 3) {
    // Releoading the Image with tree channels requestet
    // Still need to find a better way for this :(
    stbi_image_free(buf);
    buf = stbi_load_from_memory(data.data(), data.size(), &m_w, &m_h, &c, 3);
    m_buffer.assign(buf, buf + (m_w * m_h * 3));
    m_fmt = RGB;
    stbi_image_free(buf);
  } else if (m_fmt == 4) {
    m_buffer.assign(buf, buf + (m_w * m_h * 4));
    m_fmt = RGBA;
    stbi_image_free(buf);
  } else {
    throw std::runtime_error("image mem using an unsupported image format!");
  }
}

void image::copy(const std::vector<uc>& pixels, int w, int h,
                 const format& fmt) {
  int f = getBppOfFmt(fmt);
  if (pixels.size() != static_cast<size_t>(w * h * f)) {
    throw std::runtime_error("Connot copy image due to size error!");
  }
  m_fmt = fmt;
  m_w = w;
  m_h = h;
  m_buffer.clear();
  m_buffer.resize(w * h * f);
  for (size_t i = 0; i < m_buffer.size(); i++) {
    m_buffer[i] = pixels[i];
  }
}

int image::getBppOfFmt(const image::format& fmt) {
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

void image::convert(image& img, const format& dst) {
  if (img.m_fmt == dst) {
    return;
  } else if (img.m_fmt == RGB && dst == BGR) {
    utils::image::reverseBuf(img.m_buffer, img.m_w, img.m_h, 3);
    img.m_fmt = BGR;
  } else if (img.m_fmt == RGB && dst == RGBA) {
    utils::image::addAlphaChannel(img.m_buffer, img.m_w, img.m_h);
    img.m_fmt = RGBA;
  } else if (img.m_fmt == RGBA && dst == RGB) {
    utils::image::removeAlphaChannel(img.m_buffer, img.m_w, img.m_h);
    img.m_fmt = RGB;
  } else if (img.m_fmt == RGBA && dst == ABGR) {
    utils::image::reverseBuf(img.m_buffer, img.m_w, img.m_h, 4);
    img.m_fmt = ABGR;
  } else if (img.m_fmt == RGB && dst == ABGR) {
    convert(img, RGBA);
    convert(img, ABGR);
  } else if (img.m_fmt == RGBA && dst == RGB565) {
    convert(img, RGB);
    convert(img, RGB565);
  } else if (img.m_fmt == RGB && dst == RGB565) {
    // Inlined make pixel 565 func
    auto f = [](uc r, uc g,
                uc b) -> unsigned short {
      unsigned short _r = (r >> 3);
      unsigned short _g = (g >> 2);
      unsigned short _b = (b >> 3);
      return (_r << 11) | (_g << 5) | _b;
    };
    std::vector<uc> cpy = img.m_buffer;
    img.m_buffer.resize(img.m_w * img.m_h * 2);
    for (int y = 0; y < img.m_w; y++) {
      for (int x = 0; x < img.m_h; x++) {
        int src = (y * img.m_w + x) * 3;
        int dst = (y * img.m_w + x) * 2;
        unsigned short new_px = f(cpy[src + 0], cpy[src + 1], cpy[src + 2]);
        img.m_buffer[dst + 0] = new_px >> 8;
        img.m_buffer[dst + 1] = new_px & 0xff;
      }
    }
    img.m_fmt = RGB565;
  }
}

void image::retile(image& img,
                   std::function<ui(int x, int y, int w)> src,
                   std::function<ui(int x, int y, int w)> dst) {
  std::vector<uc> cpy = img.m_buffer;
  /** could use fmt here but for 565 that woulnt work as it is not supported by
   * file loading where fmt is used */
  int bpp = img.bpp();
  for (int y = 0; y < img.m_h; y++) {
    for (int x = 0; x < img.m_w; x++) {
      int src_idx = src(x, y, img.m_w);
      int dst_idx = dst(x, y, img.m_w);
      for (int i = 0; i < bpp; i++) {
        img.m_buffer[dst_idx + i] = cpy[src_idx + i];
      }
    }
  }
}
}  // namespace amy