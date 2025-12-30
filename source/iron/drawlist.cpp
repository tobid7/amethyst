#include <amethyst/iron.hpp>

/** Setup for everything (oder so) */
enum LiPathRectFlags_ : Amy::ui {
  LiPathRectFlags_None = 0,
  LiPathRectFlags_KeepTopLeft = (1 << 0),
  LiPathRectFlags_KeepTopRight = (1 << 1),
  LiPathRectFlags_KeepBotRight = (1 << 2),
  LiPathRectFlags_KeepBotLeft = (1 << 3),
  LiPathRectFlags_KeepTop = (1 << 0) | (1 << 1),
  LiPathRectFlags_KeepBot = (1 << 2) | (1 << 3),
  LiPathRectFlags_KeepLeft = (1 << 0) | (1 << 3),
  LiPathRectFlags_KeepRight = (1 << 1) | (1 << 2),
};

namespace Amy {
constexpr auto __pi = std::numbers::pi;

void Iron::Drawlist::Merge(Iron::Drawlist* list) {
  for (size_t i = 0; i < list->pData.size(); i++) {
    pData.push_back(std::move(list->pData[i]));
  }
  list->Clear();
}

void Iron::Drawlist::Clear() {
  pData.clear();
  pPath.clear();
  DrawSolid();
  while (!ClipRects.empty()) {
    ClipRects.pop();
  }
  pLayer = 0;
}

Iron::Command::Ref Iron::Drawlist::NewCommand() {
  auto ret = Command::New();
  ret->Layer = pLayer;
  ret->Index = pData.size();
  ret->Tex = pTex;
  return std::move(ret);
}

void Iron::Drawlist::clipCmd(Command* ptr) {
  if (!ClipRects.empty()) {
    ptr->ScissorOn = true;
    ptr->ScissorRect = ivec4(ClipRects.top());
  }
}

void Iron::Drawlist::Push(Command::Ref cmd) { pData.push_back(std::move(cmd)); }

void Iron::Drawlist::DrawSolid() { pTex = Iron::WhiteTex(); }

void Iron::Drawlist::PathArcToN(const fvec2& c, float radius, float a_min,
                                float a_max, int segments) {
  // pathAdd(c)
  PathReserve(segments + 1);
  for (int i = 0; i < segments; i++) {
    float a = a_min + ((float)i / (float)segments) * (a_max - a_min);
    PathAdd(fvec2(c.x + std::cos(a) * radius, c.y + std::sin(a) * radius));
  }
}

void Iron::Drawlist::PathFastArcToN(const fvec2& c, float r, float amin,
                                    float amax, int s) {
  /**
   * Funcion with less division overhead
   * Usefull for stuff where a lot of calculations are required
   */
  float d = (amax - amin) / s;
  PathReserve(s + 1);
  for (int i = 0; i <= s; i++) {
    float a = amin + i * d;
    PathAdd(fvec2(c.x + std::cos(a) * r, c.y + std::sin(a) * r));
  }
}

void Iron::Drawlist::PathRect(const fvec2& a, const fvec2& b, float rounding) {
  if (rounding == 0.f) {
    PathAdd(a);
    PathAdd(fvec2(b.x, a.y));
    PathAdd(b);
    PathAdd(fvec2(a.x, b.y));
  } else {
    float r = std::min({rounding, (b.x - a.x) * 0.5f, (b.y - a.y) * 0.5f});
    /** Calculate Optimal segment count automatically */
    float corner = __pi * 0.5f;
    int segments = std::max(3, int(std::ceil(corner / (6.0f * __pi / 180.0f))));

    /**
     * To Correctly render filled shapes with Paths API
     * The Commands need to be setup clockwise
     */
    /** Top Left */
    PathAdd(fvec2(a.x + r, a.y));
    PathFastArcToN(fvec2(b.x - r, a.y + r), r, -__pi / 2.0f, 0.0f, segments);
    /** Top Right */
    PathAdd(fvec2(b.x, b.y - r));
    PathFastArcToN(fvec2(b.x - r, b.y - r), r, 0.0f, __pi / 2.0f, segments);
    /** Bottom Right */
    PathAdd(fvec2(a.x + r, b.y));
    PathFastArcToN(fvec2(a.x + r, b.y - r), r, __pi / 2.0f, __pi, segments);
    /** Bottom Left */
    PathAdd(fvec2(a.x, a.y + r));
    PathFastArcToN(fvec2(a.x + r, a.y + r), r, __pi, 3.0f * __pi / 2.0f,
                   segments);
  }
}

void Iron::Drawlist::PathRectEx(const fvec2& a, const fvec2& b, float rounding,
                                ui flags) {
  if (rounding == 0.f) {
    PathAdd(a);
    PathAdd(fvec2(b.x, a.y));
    PathAdd(b);
    PathAdd(fvec2(a.x, b.y));
  } else {
    float r = std::min({rounding, (b.x - a.x) * 0.5f, (b.y - a.y) * 0.5f});
    /** Calculate Optimal segment count automatically */
    float corner = __pi * 0.5f;
    int segments = std::max(3, int(std::ceil(corner / (6.0f * __pi / 180.0f))));

    /**
     * To Correctly render filled shapes with Paths API
     * The Commands need to be setup clockwise
     */
    /** Top Left */
    if (flags & LiPathRectFlags_KeepTopLeft) {
      PathAdd(a);
    } else {
      PathAdd(fvec2(a.x + r, a.y));
      PathFastArcToN(fvec2(b.x - r, a.y + r), r, -__pi / 2.0f, 0.0f, segments);
    }

    /** Top Right */
    if (flags & LiPathRectFlags_KeepTopRight) {
      PathAdd(fvec2(b.x, a.y));
    } else {
      PathAdd(fvec2(b.x, b.y - r));
      PathFastArcToN(fvec2(b.x - r, b.y - r), r, 0.0f, __pi / 2.0f, segments);
    }
    /** Bottom Right */
    if (flags & LiPathRectFlags_KeepBotRight) {
      PathAdd(b);
    } else {
      PathAdd(fvec2(a.x + r, b.y));
      PathFastArcToN(fvec2(a.x + r, b.y - r), r, __pi / 2.0f, __pi, segments);
    }
    /** Bottom Left */
    if (flags & LiPathRectFlags_KeepBotLeft) {
      PathAdd(fvec2(a.x, b.y));
    } else {
      PathAdd(fvec2(a.x, a.y + r));
      PathFastArcToN(fvec2(a.x + r, a.y + r), r, __pi, 3.0f * __pi / 2.0f,
                     segments);
    }
  }
}

void Iron::Drawlist::DrawRect(const fvec2& pos, const fvec2& size, ui color,
                              int thickness) {
  PathRect(pos, pos + size);
  PathStroke(color, thickness, 1);
}

void Iron::Drawlist::DrawRectFilled(const fvec2& pos, const fvec2& size,
                                    ui color) {
  PathRect(pos, pos + size);
  PathFill(color);
}

void Iron::Drawlist::DrawTriangle(const fvec2& a, const fvec2& b,
                                  const fvec2& c, ui color, int thickness) {
  PathAdd(a);
  PathAdd(b);
  PathAdd(c);
  PathStroke(color, thickness, 1);
}

void Iron::Drawlist::DrawTriangleFilled(const fvec2& a, const fvec2& b,
                                        const fvec2& c, ui color) {
  PathAdd(a);
  PathAdd(b);
  PathAdd(c);
  PathFill(color);
}

void Iron::Drawlist::DrawCircle(const fvec2& center, float rad, ui color,
                                int segments, int thickness) {
  if (segments <= 0) {
    // Auto Segment
  } else {
    float am = (__pi * 2.0f) * ((float)segments) / (float)segments;
    PathArcToN(center, rad, 0.f, am, segments);
  }
  DrawSolid();  // Only Solid Color Supported
  PathStroke(color, thickness, 1);
}

void Iron::Drawlist::DrawCircleFilled(const fvec2& center, float rad, ui color,
                                      int segments) {
  if (segments <= 0) {
    // Auto Segment
  } else {
    float am = (__pi * 2.0f) * ((float)segments) / (float)segments;
    PathArcToN(center, rad, 0.f, am, segments);
  }
  PathFill(color);
}

void Iron::Drawlist::DrawCutTex(fvec2 pos, fvec2 size, fvec4 cut_rect,
                                ui color) {
  auto r = Iron::PrimRect(pos, size);
  auto cmd = NewCommand();
  auto uv = cut_rect / fvec4(pTex->Size(), pTex->Size());
  uv.y = 1.f - uv.y;
  uv.w = 1.f - uv.w;
  Iron::CmdQuad(cmd.get(), r, uv, color);
  Push(std::move(cmd));
}

void Iron::Drawlist::DrawPolyLine(const std::vector<fvec2>& points, ui clr,
                                  ui flags, int thickness) {
  if (points.size() < 2) {
    return;
  }
  DrawSolid();
  auto cmd = NewCommand();
  bool close = (flags & (1 << 0));
  int num_points = close ? (int)points.size() : (int)points.size() - 1;
  if (flags & (1 << 1)) {
    // TODO: Find a way to draw less garbage looking lines
  } else {
    // Non antialiased lines look awful when rendering with thickness != 1
    for (int i = 0; i < num_points; i++) {
      int j = (i + 1) == (int)points.size() ? 0 : (i + 1);
      auto line = PrimLine(points[i], points[j], thickness);
      CmdQuad(cmd.get(), line, fvec4(0.f, 1.f, 1.f, 0.f), clr);
    }
  }
  Push(std::move(cmd));
}

void Iron::Drawlist::DrawConvexPolyFilled(const std::vector<fvec2>& points,
                                          ui clr) {
  if (points.size() < 3) {
    return;  // Need at least three points
  }
  auto cmd = NewCommand();
  CmdConvexPolyFilled(cmd.get(), points, clr, pTex);
  Push(std::move(cmd));
}

void Iron::Drawlist::DrawText(const fvec2& pos, const std::string& text,
                              ui color) {
  if (!pCurrentFont) {
    return;
  }
  std::vector<Command::Ref> cmds;
  pCurrentFont->CmdTextEx(cmds, pos, color, pFontScale, text);
  for (size_t i = 0; i < cmds.size(); i++) {
    cmds[i]->Index = pData.size();
    cmds[i]->Layer = pLayer;
    Push(std::move(cmds[i]));
  }
}

void Iron::Drawlist::DrawTextEx(const fvec2& p, const std::string& text,
                                ui color, ui flags, const fvec2& box) {
  if (!pCurrentFont) {
    return;
  }
  std::vector<Command::Ref> cmds;
  pCurrentFont->CmdTextEx(cmds, p, color, pFontScale, text, flags, box);
  for (size_t i = 0; i < cmds.size(); i++) {
    cmds[i]->Index = pData.size();
    cmds[i]->Layer = pLayer;
    Push(std::move(cmds[i]));
  }
}

void Iron::Drawlist::DrawLine(const fvec2& a, const fvec2& b, ui color, int t) {
  PathAdd(a);
  PathAdd(b);
  PathStroke(color, t);
}
}  // namespace Amy