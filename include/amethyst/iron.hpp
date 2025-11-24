#pragma once

#include <amethyst/c3d.hpp>
#include <amethyst/linearAlloc.hpp>
#include <amethyst/maths/mat.hpp>
#include <amethyst/maths/vec.hpp>
#include <amethyst/rect.hpp>
#include <amethyst/texture.hpp>
#include <amethyst/types.hpp>

namespace amy {
class iron {
 public:
  struct vertex {
    vertex(float x, float y, float u, float v, ui clr) {
      pos.x = x;
      pos.y = y;
      uv.x = x;
      uv.y = y;
      color = clr;
    }
    vertex(const fvec2& pos, const fvec2& uv, ui clr) {
      this->pos = pos;
      this->uv = uv;
      this->color = clr;
    }
    vertex() {}

    amy::fvec2 pos;
    amy::fvec2 uv;
    ui color = 0;
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

    std::vector<command::ref>& data() { return m_data; }

    void merge(drawlist* list);
    command::ref newCommand();
    void push(command ::ref cmd);
    void clear();

    void drawSolid();
    void drawTex(texture* tex) { m_tex = tex; }

    /** Draw Api */
    void drawRect(const fvec2& pos, const fvec2& size, ui color,
                  int thickness = 1);
    void drawRectFilled(const fvec2& pos, const fvec2& size, ui color);
    void drawTriangle(const fvec2& a, const fvec2& b, const fvec2& c, ui color,
                      int thickness = 1);
    void drawTriangleFilled(const fvec2& a, const fvec2& b, const fvec2& c,
                            ui color);
    void drawCircle(const fvec2& center, float radius, ui color, int segments,
                    int thickness = 1);
    void drawCircleFilled(const fvec2& center, float radius, ui color,
                          int segments);
    void drawText(const fvec2& pos, const std::string& text, ui color);
    void drawTextEx(const fvec2& pos, const std::string& text, ui color,
                    ui flags, const fvec2& box = 0);
    void drawLine(const fvec2& a, const fvec2& b, ui color, int thickness = 1);
    void drawPolyLine(const std::vector<fvec2>& points, ui color, ui flags = 0,
                      int thickness = 1);
    void drawConvexPolyFilled(const std::vector<fvec2>& points, ui color);

    /** Path api */
    void pathAdd(const fvec2& pos) { m_path.push_back(std::move(pos)); }
    void pathClear() { m_path.clear(); }
    void pathReserve(size_t count) { m_path.reserve(m_path.size() + count); }
    void pathStroke(ui color, int thickness = 1, ui flags = 0) {
      drawPolyLine(m_path, color, flags, thickness);
      pathClear();
    }
    void pathFill(ui color) { drawConvexPolyFilled(m_path, color); }
    void pathArcToN(const fvec2& c, float radius, float a_min, float a_max,
                    int segments);
    void pathFastArcToN(const fvec2& c, float radius, float a_min, float a_max,
                        int segments);
    void pathRect(const fvec2& a, const fvec2& b, float rounding = 0.f);
    void pathRectEx(const fvec2& a, const fvec2& b, float rounfing, ui flags);

    void pushClipRect(const fvec4& clip) { clipRects.push(clip); }
    void popClipRect() {
      if (!clipRects.empty()) clipRects.pop();
    }

    operator std::vector<command::ref>&() { return m_data; }

   private:
    void clipCmd(command* ptr);
    std::vector<command::ref> m_data;
    std::vector<fvec2> m_path;
    texture* m_tex = nullptr;
    std::stack<fvec4> clipRects;
    int m_layer = 0;
  };
  iron() = default;
  ~iron() = default;

  static void init();
  static void newFrame();
  static void drawOn(c3d::screen* screen);
  static void draw(const std::vector<command::ref>& data);
  static texture* whiteTex() { return m_solid; }

  /** Static renderer utility funcs */

  static void rotateCorner(fvec2& pos, float s, float c);
  static rect primRect(const fvec2& pos, const fvec2& size, float angle = 0.f);
  static rect primLine(const fvec2& a, const fvec2& b, int thickness = 1);
  static void cmdQuad(command* cmd, const rect& q, const rect& uv, ui color);
  static void cmdTriangle(command* cmd, const fvec2& a, const fvec2& b,
                          const fvec2& c, ui clr);
  static void cmdConvexPolyFilled(command* cmd,
                                  const std::vector<fvec2>& points, ui clr,
                                  texture* tex);
  static bool inBox(const fvec2& pos, const fvec2& size, const fvec4& area);
  static bool inBox(const fvec2& pos, const fvec4& area);
  static bool inBox(const fvec2& a, const fvec2& b, const fvec2& c,
                    const fvec4& area);

 private:
  static void setupShader();
  static void fragConfig();
  static void initSolidTex();

  static std::vector<vertex, linearAllocator<vertex>> m_vbuf;
  static std::vector<u16, linearAllocator<u16>> m_ibuf;
  static int uLocProj;
  static c3d::shader* m_shader;
  static mat4 m_mtx;
  static int m_idx, m_vtx;
  static texture* m_solid;
};
}  // namespace amy