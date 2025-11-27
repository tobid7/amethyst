#pragma once

#include <amethyst/types.hpp>

namespace Amy {
class Image {
 public:
  enum Format {
    RGBA,    // bpp == 4
    RGB,     // bpp == 3
    RGB565,  // bpp == 2 (not supported in laoding)
    BGR,     // bpp == 3
    ABGR,    // bpp == 4
    BGRA,    // bpp == 4
    A8,      // bpp == 1 (not supported in laoding)
  };
  Image() = default;
  Image(ksr path) { this->Load(path); }
  Image(kvr<uc> buf) { this->Load(buf); }
  Image(kvr<uc> buf, int w, int h, const Format& fmt) {
    this->Copy(buf, w, h, fmt);
  }
  ~Image() = default;

  void Load(ksr path);
  void Load(kvr<uc> buf);
  void Copy(kvr<uc> buf, int w, int h, const Format& fmt);

  vec<uc>& GetBuffer() { return pBuffer; }
  vec<uc> GetBuffer() const { return pBuffer; }

  int Width() const { return pW; }
  int Height() const { return pH; }
  int Bpp() const { return GetBppOfFmt(pFmt); }
  Format Fmt() const { return pFmt; }
  void Convert(const Format& dst) { Convert(*this, dst); }
  void Retile(std::function<ui(int x, int y, int w)> src,
              std::function<ui(int x, int y, int w)> dst) {
    Retile(*this, src, dst);
  }

  uc& operator[](int idx) { return pBuffer[idx]; }
  uc operator[](int idx) const { return pBuffer[idx]; }

  static void Convert(Image& img, const Format& dst);
  static void Retile(Image& img, std::function<ui(int x, int y, int w)> src,
                     std::function<ui(int x, int y, int w)> dst);
  static int GetBppOfFmt(const Format& fmt);

 private:
  vec<uc> pBuffer;
  int pW = 0;
  int pH = 0;
  Format pFmt = RGBA;
};
}  // namespace Amy