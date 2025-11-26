#pragma once

#include <citro3d.h>

#include <amethyst/asset.hpp>
#include <amethyst/image.hpp>
#include <amethyst/rect.hpp>
#include <amethyst/types.hpp>

namespace Amy {
class Texture : public Asset {
 public:
  Texture() = default;
  Texture(ksr path);
  ~Texture();
  void Load(ksr path);
  void Load(kvr<uc> pixels, int w, int h, int bpp = 4,
            Image::Format fmt = Image::RGBA);
  void Unload();

  int W() const { return pSize.x; }
  int& W() { return pSize.x; }
  int H() const { return pSize.y; }
  int& H() { return pSize.y; }
  ivec2 Size() const { return pSize; }
  ivec2& Size() { return pSize; }
  Rect& Uv() { return pUv; }

  C3D_Tex* Ptr() { return pLoaded ? &pTex : nullptr; }

  void Bind(int reg = 0);

 private:
  C3D_Tex pTex;
  ivec2 pSize;
  Rect pUv;
  bool pLoaded = false;
};
}  // namespace Amy