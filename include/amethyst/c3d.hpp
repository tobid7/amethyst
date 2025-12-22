#pragma once

#include <3ds.h>
#include <citro3d.h>

#include <amethyst/asset.hpp>
#include <amethyst/maths/mat.hpp>
#include <amethyst/maths/vec.hpp>
#include <amethyst/types.hpp>
#include <string>

namespace Amy {
class C3D {
 public:
  C3D() = default;
  ~C3D() = default;

  class Screen {
   public:
    Screen(C3D_RenderTarget* t) {
      pTarget = t;
      // Width and height are swapped on 3ds due to screen layout
      pWidth = pTarget->frameBuf.height;
      pHeight = pTarget->frameBuf.width;
    }
    ~Screen() { C3D_RenderTargetDelete(pTarget); }

    int Width() const { return pWidth; }
    int Height() const { return pHeight; }
    ivec2 Size() const { return ivec2(pWidth, pHeight); }
    void Clear(ui clr = 0x0, ui depth_clr = 0x0) {
      C3D_RenderTargetClear(pTarget, C3D_CLEAR_ALL, clr, depth_clr);
    }
    void Use() { C3D_FrameDrawOn(pTarget); }
    C3D_RenderTarget* Ptr() { return pTarget; }

   private:
    C3D_RenderTarget* pTarget = nullptr;
    int pWidth = 0;
    int pHeight = 0;
  };

  class Shader : public Asset {
   public:
    Shader(ksr path);
    Shader() {}
    ~Shader();
    AMY_SHARED(Shader);

    void Load(ksr path);
    void Load(kvr<uc> data);
    void Compile(ksr code);
    void Use();
    void Input(int reg, GPU_FORMATS f, int num);
    void Input(GPU_FORMATS f, int num) { Input(pReg++, f, num); }
    void SetMat4(int loc, C3D_Mtx* m);
    void SetMat4(int loc, const mat4& m);
    int loc(ksr name);

    // private:
    C3D_AttrInfo pInfo;
    shaderProgram_s pProgram;
    DVLB_s* pCode = nullptr;
    int pReg = 0;
    std::vector<uc> pRawData;
  };

  class Frag {
   public:
    Frag() = default;
    ~Frag() = default;

    static void Edit(int id = 0);
    static void Src(C3D_TexEnvMode mode, GPU_TEVSRC s1 = GPU_PRIMARY_COLOR,
                    GPU_TEVSRC s2 = GPU_PRIMARY_COLOR,
                    GPU_TEVSRC s3 = GPU_PRIMARY_COLOR);
    static void Func(C3D_TexEnvMode mode, GPU_COMBINEFUNC func);

   private:
    static C3D_TexEnv* pEnv;
  };

  static void Init();
  static void Deinit();
  static void StartFrame(bool sync = true);
  static void EndFrame();
  static Screen* CreateScreen(gfxScreen_t screen, gfx3dSide_t side = GFX_LEFT);
  static void DeleteScreen(Screen* screen);
  static void DrawArrays(int start, int count,
                         GPU_Primitive_t prim = GPU_TRIANGLES);
  static void DrawElements(int count, const void* idx_ptr,
                           int type = C3D_UNSIGNED_SHORT,
                           GPU_Primitive_t prim = GPU_TRIANGLES);
  static void DepthTest(bool on, GPU_TESTFUNC func = GPU_GREATER,
                        GPU_WRITEMASK mask = GPU_WRITE_ALL);
  static void DisableScissor();
  static void EnableScissor(const ivec4 rect);
  /**
   * Buf cfg die permutation at runtime berechnet
   */
  static void BufCfg(void* ptr, int stride, int shader_attribs);
  /**
   * Klassische config bei der man selber die permutation eintragen muss
   */
  static void BufCfg(void* ptr, int stride, int shader_attribs,
                     u64 permutation);
  /**
   * Hacky funktion um die permutation automatisch at compile time zu berechnen,
   * falls diese immer gleich bleibt.
   * In der <> steht die anzahl der shader input werte
   * usage: C3D::bufCfg<3>(data, sizeof(vertex));
   */
  template <int attribs>
  constexpr static void BufCfg(void* ptr, int stride) {
    auto buf = C3D_GetBufInfo();
    BufInfo_Init(buf);
    constexpr int pm = pPermutation(attribs);
    BufInfo_Add(buf, ptr, stride, attribs, pm);
  }

  static int Drawcalls() { return pDrawcalls; }

 private:
  static int pDrawcalls;
  static int p__dc__;  // draw-calls internal

  /**
   * Funktion die anhand von **ac** genau den permu station wert für BufInfo
   * ausrechnet wie z.B
   * ```
   * ac = 3 -> 0x210
   * ac = 4 -> 0x3210
   * ```
   */
  constexpr static u64 pPermutation(int ac) {
    u64 ret = 0;
    if (ac < 1 || ac > 15) {
      throw std::runtime_error("[amy] " + std::to_string(ac) +
                               " is out of range (1...15)!");
    }
    for (int i = 0; i < ac; i++) {
      ret = (ret << 4) | (ac - 1 - i);
    }
    return ret;
  }
};
}  // namespace Amy