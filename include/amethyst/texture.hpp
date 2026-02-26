#pragma once

#include <citro3d.h>

#include <amethyst/asset.hpp>
#include <amethyst/image.hpp>
#include <amethyst/rect.hpp>
#include <amethyst/types.hpp>

namespace Amy {
class Texture : public Asset {
 public:
  enum Wrap_ {
    ClampToEdge,
    ClampToBorder,
    Repeat,
  };
  Texture() = default;
  Texture(ksr path);
  Texture(C3D_Tex* tex, const ivec2& size, const Rect& uv,
          const Wrap_& wrap = ClampToBorder) {
    Load(tex, size, uv);
  }
  ~Texture();
  AMY_SHARED(Texture);

  void Load(ksr path, const Wrap_& wrap = ClampToBorder);
  void Load(kvr<uc> pixels, int w, int h, int bpp = 4,
            Image::Format fmt = Image::RGBA, const Wrap_& wrap = ClampToBorder);
  void Load(C3D_Tex* tex, const ivec2& size, const Rect& uv) {
    Unload();
    pTex = tex;
    pSize = size;
    pUv = uv;
    // Dont set as loaded as only the root tex can be loaded
  }
  void Unload();
  void Unloadable(bool v) { pLoaded = v; }
  bool Unloadable() { return pLoaded; }
  void Wrap(const Wrap_& wrap) { Wrap(wrap, wrap); }
  void Wrap(const Wrap_& wx, const Wrap_& wy);

  int W() const { return pSize.x; }
  int& W() { return pSize.x; }
  int H() const { return pSize.y; }
  int& H() { return pSize.y; }
  ivec2 Size() const { return pSize; }
  ivec2& Size() { return pSize; }
  Rect& Uv() { return pUv; }

  C3D_Tex* Ptr() { return pTex; }

  void Bind(int reg = 0);

 private:
  C3D_Tex* pTex = nullptr;
  ivec2 pSize;
  Rect pUv = fvec4(0, 1, 1, 0);
  bool pLoaded = false;
};
}  // namespace Amy