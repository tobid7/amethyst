#pragma once

#include <3ds.h>
#include <citro3d.h>

#include <amethyst/asset.hpp>
#include <amethyst/maths/mat.hpp>
#include <amethyst/maths/vec.hpp>
#include <amethyst/types.hpp>
#include <string>

namespace amy {
class c3d {
 public:
  c3d() = default;
  ~c3d() = default;

  class screen {
   public:
    screen(C3D_RenderTarget* t) {
      m_target = t;
      // Width and height are swapped on 3ds due to screen layout
      m_width = m_target->frameBuf.height;
      m_height = m_target->frameBuf.width;
    }
    ~screen() { C3D_RenderTargetDelete(m_target); }

    int width() const { return m_width; }
    int height() const { return m_height; }
    ivec2 size() const { return ivec2(m_width, m_height); }
    void clear() { C3D_RenderTargetClear(m_target, C3D_CLEAR_ALL, 0, 0); }
    void startDraw() { C3D_FrameDrawOn(m_target); }

   private:
    C3D_RenderTarget* m_target = nullptr;
    int m_width = 0;
    int m_height = 0;
  };

  class shader : public asset {
   public:
    shader(const std::string& path);
    shader() {}
    ~shader();

    void load(const std::string& path);
    void load(const std::vector<uc>& data);
    void compile(const std::string& code);
    void use();
    void input(int reg, GPU_FORMATS f, int num);
    void input(GPU_FORMATS f, int num) { input(m_reg++, f, num); }
    void setMat4(int loc, C3D_Mtx* m);
    void setMat4(int loc, const mat4& m);
    int loc(const std::string& name);

   private:
    C3D_AttrInfo m_info;
    shaderProgram_s m_program;
    DVLB_s* m_code;
    int m_reg = 0;
  };

  class frag {
   public:
    frag() = default;
    ~frag() = default;

    static void edit(int id = 0);
    static void src(C3D_TexEnvMode mode, GPU_TEVSRC s1 = GPU_PRIMARY_COLOR,
                    GPU_TEVSRC s2 = GPU_PRIMARY_COLOR,
                    GPU_TEVSRC s3 = GPU_PRIMARY_COLOR);
    static void func(C3D_TexEnvMode mode, GPU_COMBINEFUNC func);

   private:
    static C3D_TexEnv* m_env;
  };

  static void init();
  static void deinit();
  static void startFrame(bool sync = true);
  static void endFrame();
  static screen* createScreen(gfxScreen_t screen, gfx3dSide_t side = GFX_LEFT);
  static void deleteScreen(screen* screen);
  static void drawArrays(int start, int count,
                         GPU_Primitive_t prim = GPU_TRIANGLES);
  static void drawElements(int count, const void* idx_ptr,
                           int type = C3D_UNSIGNED_SHORT,
                           GPU_Primitive_t prim = GPU_TRIANGLES);
  static void depthTest(bool on, GPU_TESTFUNC func = GPU_GREATER,
                        GPU_WRITEMASK mask = GPU_WRITE_ALL);
  static void disableScissor();
  static void enableScissor(const ivec4 rect);
  /**
   * Buf cfg die permutation at runtime berechnet
   */
  static void bufCfg(void* ptr, int stride, int shader_attribs);
  /**
   * Klassische config bei der man selber die permutation eintragen muss
   */
  static void bufCfg(void* ptr, int stride, int shader_attribs,
                     u64 permutation);
  /**
   * Hacky funktion um die permutation automatisch at compile time zu berechnen,
   * falls diese immer gleich bleibt.
   * In der <> steht die anzahl der shader input werte
   * usage: c3d::bufCfg<3>(data, sizeof(vertex));
   */
  template <int attribs>
  constexpr static void bufCfg(void* ptr, int stride) {
    auto buf = C3D_GetBufInfo();
    BufInfo_Init(buf);
    constexpr int pm = permutation(attribs);
    BufInfo_Add(buf, ptr, stride, attribs, pm);
  }

  static int drawcalls() { return m_drawcalls; }

 private:
  static int m_drawcalls;
  static int m__dc__;

  /**
   * Funktion die anhand von **ac** genau den permu station wert für BufInfo
   * ausrechnet wie z.B
   * ```
   * ac = 3 -> 0x210
   * ac = 4 -> 0x3210
   * ```
   */
  constexpr static u64 permutation(int ac) {
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
}  // namespace amy