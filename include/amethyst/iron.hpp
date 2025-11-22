#pragma once

#include <algorithm>
#include <amethyst/c3d.hpp>
#include <amethyst/linearAlloc.hpp>
#include <amethyst/maths/mat.hpp>
#include <amethyst/maths/vec.hpp>

namespace amy {
class iron {
 public:
  struct vertex {
    vertex(float x, float y, float u, float v, u32 clr) {
      pos.x = x;
      pos.y = y;
      uv.x = x;
      uv.y = y;
      color = clr;
    }
    vertex() {}

    amy::fvec2 pos;
    amy::fvec2 uv;
    u32 color = 0;
  };
  iron() = default;
  ~iron() = default;

  static void init();
  static void newFrame();
  static void drawOn(c3d::screen* screen);

 private:
  static void setupShader();
  static void fragConfig();

  static std::vector<vertex, linearAllocator<vertex>> m_vbuf;
  static std::vector<u16, linearAllocator<u16>> m_ibuf;
  static int uLocProj;
  static c3d::shader* m_shader;
  static mat4 m_mtx;
  static int m_idx, m_vtx;
};
}  // namespace amy