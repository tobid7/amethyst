#pragma once

#include <amethyst/types.hpp>

namespace amy {
class image {
 public:
  enum format {
    RGBA,    // bpp == 4
    RGB,     // bpp == 3
    RGB565,  // bpp == 2 (not supported in laoding)
    BGR,     // bpp == 3
    ABGR,    // bpp == 4
    BGRA,    // bpp == 4
  };
  image() = default;
  image(cstr& path) { this->load(path); }
  image(const std::vector<uc>& buf) { this->load(buf); }
  image(const std::vector<uc>& buf, int w, int h, const format& fmt) {
    this->copy(buf, w, h, fmt);
  }
  ~image() = default;

  void load(cstr& path);
  void load(const std::vector<uc>& buf);
  void copy(const std::vector<uc>& buf, int w, int h, const format& fmt);

  std::vector<uc>& getBuffer() { return m_buffer; }
  std::vector<uc> getBuffer() const { return m_buffer; }

  int width() const { return m_w; }
  int height() const { return m_h; }
  int bpp() const { return getBppOfFmt(m_fmt); }
  format fmt() const { return m_fmt; }
  void convert(const format& dst) { convert(*this, dst); }
  void retile(std::function<ui(int x, int y, int w)> src,
              std::function<ui(int x, int y, int w)> dst) {
    retile(*this, src, dst);
  }

  uc& operator[](int idx) { return m_buffer[idx]; }
  uc operator[](int idx) const { return m_buffer[idx]; }

  static void convert(image& img, const format& dst);
  static void retile(image& img, std::function<ui(int x, int y, int w)> src,
                     std::function<ui(int x, int y, int w)> dst);
  static int getBppOfFmt(const format& fmt);

 private:
  std::vector<uc> m_buffer;
  int m_w = 0;
  int m_h = 0;
  format m_fmt = RGBA;
};
}  // namespace amy