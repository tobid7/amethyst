#pragma once

#include <algorithm>
#include <amethyst/c3d.hpp>
#include <amethyst/linearAlloc.hpp>
#include <amethyst/maths/mat.hpp>
#include <amethyst/maths/vec.hpp>
#include <amethyst/texture.hpp>
#include <amethyst/types.hpp>

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
  class command {
   public:
    command() = default;
    using ref = up<command>;
    command& add(const u16& idx) {
      indexBuf.push_back(vertexBuf.size() + idx);
      return *this;
    }
    command& add(const vertex& vtx) {
      vertexBuf.push_back(std::move(vtx));
      return *this;
    }

    std::vector<vertex> vertexBuf;
    std::vector<u16> indexBuf;
    ivec4 scissorRect;
    bool scissorOn = false;
    int layer;
    int index;
    texture* tex;
  };
  class drawlist {
   public:
    drawlist() { drawSolid(); }
    ~drawlist() { m_data.clear(); }

    // required due to memory management
    drawlist(const drawlist&) = delete;
    drawlist& operator=(const drawlist&) = delete;
    drawlist(drawlist&&) noexcept = default;
    drawlist& operator=(drawlist&&) noexcept = default;

    void merge(drawlist* list);
    command::ref newCommand();
    void push(command ::ref cmd);
    void clear();

    void drawSolid();
    void drawTex(texture* tex) { m_tex = tex; }

   private:
    std::vector<command::ref> m_data;
    texture* m_tex;
  };
  iron() = default;
  ~iron() = default;

  static void init();
  static void newFrame();
  static void drawOn(c3d::screen* screen);
  static void draw(const std::vector<command::ref>& data);

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