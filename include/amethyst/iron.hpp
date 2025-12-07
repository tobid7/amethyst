#pragma once

#include <amethyst/c3d.hpp>
#include <amethyst/linearAlloc.hpp>
#include <amethyst/maths/mat.hpp>
#include <amethyst/maths/vec.hpp>
#include <amethyst/rect.hpp>
#include <amethyst/texture.hpp>
#include <amethyst/types.hpp>

namespace Amy {
class Iron {
 public:
  struct Vertex {
    Vertex(float x, float y, float u, float v, ui clr) {
      pos.x = x;
      pos.y = y;
      uv.x = u;
      uv.y = v;
      color = clr;
    }
    Vertex(const fvec2& pos, const fvec2& uv, ui clr) {
      this->pos = pos;
      this->uv = uv;
      this->color = clr;
    }
    Vertex() {}

    fvec2 pos;
    fvec2 uv;
    ui color = 0;
  };
  class Command {
   public:
    Command() = default;
    ~Command() = default;
    AMY_UNIQUE(Command)

    Command& Add(const u16& idx) {
      IndexBuf.push_back(VertexBuf.size() + idx);
      return *this;
    }
    Command& Add(const Vertex& vtx) {
      VertexBuf.push_back(std::move(vtx));
      return *this;
    }

    std::vector<Vertex> VertexBuf;
    std::vector<u16> IndexBuf;
    ivec4 ScissorRect;
    bool ScissorOn = false;
    int Layer = 0;
    int Index = 0;
    Texture* Tex = nullptr;
  };

  class Font {
   public:
    struct Codepoint {
      ui Cp = 0;
      fvec4 Uv;
      Texture* Tex;
      fvec2 Size;
      float Offset = 0;  // Unused??
      bool Valid = true;
    };

    Font() = default;
    ~Font() = default;
    AMY_SHARED(Font)

    void LoadTTF(ksr path, int pxh = 32);
    void LoadTTF(kvr<uc> data, int pxh = 32);
    /**
     * Bitmap Font BTW
     */
    void LoadBMF(ksr path);

    Codepoint& GetCodepoint(ui c);

    fvec2 GetTextBounds(ksr text, float scale);
    void CmdTextEx(vec<Command::Ref>& cmds, const fvec2& pos, ui color,
                   float scale, ksr text, ui flags = 0, const fvec2& box = 0);
    void pMakeAtlas(bool final, vec<uc>& pixels, int size, Texture* tex);

    int PxHeight;
    int PxFactor = 24;
    vec<Texture*> Textures;
    std::map<u32, Codepoint> pCodeMap;
  };

  class Drawlist {
   public:
    Drawlist() { DrawSolid(); }
    ~Drawlist() { pData.clear(); }

    // required due to memory management
    Drawlist(const Drawlist&) = delete;
    Drawlist& operator=(const Drawlist&) = delete;
    Drawlist(Drawlist&&) noexcept = default;
    Drawlist& operator=(Drawlist&&) noexcept = default;

    std::vector<Command::Ref>& Data() { return pData; }

    void Merge(Drawlist* list);
    Command::Ref NewCommand();
    void Push(Command ::Ref cmd);
    void Clear();

    void DrawSolid();
    void DrawTex(Texture* tex) { pTex = tex; }

    /** Draw Api */
    void DrawRect(const fvec2& pos, const fvec2& size, ui color,
                  int thickness = 1);
    void DrawRectFilled(const fvec2& pos, const fvec2& size, ui color);
    void DrawTriangle(const fvec2& a, const fvec2& b, const fvec2& c, ui color,
                      int thickness = 1);
    void DrawTriangleFilled(const fvec2& a, const fvec2& b, const fvec2& c,
                            ui color);
    void DrawCircle(const fvec2& center, float radius, ui color, int segments,
                    int thickness = 1);
    void DrawCircleFilled(const fvec2& center, float radius, ui color,
                          int segments);
    void DrawText(const fvec2& pos, const std::string& text, ui color);
    void DrawTextEx(const fvec2& pos, const std::string& text, ui color,
                    ui flags, const fvec2& box = 0);
    void DrawLine(const fvec2& a, const fvec2& b, ui color, int thickness = 1);
    void DrawPolyLine(const std::vector<fvec2>& points, ui color, ui flags = 0,
                      int thickness = 1);
    void DrawConvexPolyFilled(const std::vector<fvec2>& points, ui color);

    /** Path api */
    void PathAdd(const fvec2& pos) { pPath.push_back(std::move(pos)); }
    void PathClear() { pPath.clear(); }
    void PathReserve(size_t count) { pPath.reserve(pPath.size() + count); }
    void PathStroke(ui color, int thickness = 1, ui flags = 0) {
      DrawPolyLine(pPath, color, flags, thickness);
      PathClear();
    }
    void PathFill(ui color) {
      DrawConvexPolyFilled(pPath, color);
      PathClear();
    }
    void PathArcToN(const fvec2& c, float radius, float a_min, float a_max,
                    int segments);
    void PathFastArcToN(const fvec2& c, float radius, float a_min, float a_max,
                        int segments);
    void PathRect(const fvec2& a, const fvec2& b, float rounding = 0.f);
    void PathRectEx(const fvec2& a, const fvec2& b, float rounfing, ui flags);

    void PushClipRect(const fvec4& clip) { ClipRects.push(clip); }
    void PopClipRect() {
      if (!ClipRects.empty()) ClipRects.pop();
    }

    operator std::vector<Command::Ref>&() { return pData; }

   private:
    void clipCmd(Command* ptr);
    std::vector<Command::Ref> pData;
    std::vector<fvec2> pPath;
    Texture* pTex = nullptr;
    std::stack<fvec4> ClipRects;
    Font* pCurrentFont;
    int pLayer = 0;
  };
  Iron() = default;
  ~Iron() = default;

  static void Init();
  static void Exit();
  static void NewFrame();
  static void DrawOn(C3D::Screen* screen);
  static void Draw(const std::vector<Command::Ref>& data);
  static Texture* WhiteTex() { return m_solid; }

  /** Static renderer utility funcs */

  static void RotateCorner(fvec2& pos, float s, float c);
  static Rect PrimRect(const fvec2& pos, const fvec2& size, float angle = 0.f);
  static Rect PrimLine(const fvec2& a, const fvec2& b, int thickness = 1);
  static void CmdQuad(Command* cmd, const Rect& q, const Rect& uv, ui color);
  static void CmdTriangle(Command* cmd, const fvec2& a, const fvec2& b,
                          const fvec2& c, ui clr);
  static void CmdConvexPolyFilled(Command* cmd,
                                  const std::vector<fvec2>& points, ui clr,
                                  Texture* tex);
  static bool InBox(const fvec2& pos, const fvec2& size, const fvec4& area);
  static bool InBox(const fvec2& pos, const fvec4& area);
  static bool InBox(const fvec2& a, const fvec2& b, const fvec2& c,
                    const fvec4& area);

  static ui VerticesDrawn() { return VertexCount; }
  static ui IndicesDrawn() { return IndexCount; }

 private:
  static void pSetupShader();
  static void pFragConfig();
  static void pInitSolidTex();
  static bool pCheckSize(size_t idx, size_t vtx);

  static std::vector<Vertex, LinearAllocator<Vertex>> m_vbuf;
  static std::vector<u16, LinearAllocator<u16>> m_ibuf;
  static int uLocProj;
  static C3D::Shader* m_shader;
  static mat4 m_mtx;
  static int m_idx, m_vtx;
  static Texture* m_solid;
  static ui VertexCount;
  static ui IndexCount;
};
}  // namespace Amy
