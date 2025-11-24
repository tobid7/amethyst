#include <amethyst/iron.hpp>

/** Setup for everything (oder so) */
enum LiPathRectFlags_ : amy::ui {
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

namespace amy {
constexpr auto __pi = std::numbers::pi;

void iron::drawlist::merge(iron::drawlist* list) {
  for (size_t i = 0; i < list->m_data.size(); i++) {
    m_data.push_back(std::move(list->m_data[i]));
  }
  list->clear();
}

void iron::drawlist::clear() { m_data.clear(); }

iron::command::ref iron::drawlist::newCommand() {
  auto ret = std::make_unique<command>();
  ret->layer = m_layer;
  ret->index = m_data.size();
  ret->tex = m_tex;
  return ret;
}

void iron::drawlist::clipCmd(command* ptr) {
  if (!clipRects.empty()) {
    ptr->scissorOn = true;
    ptr->scissorRect = ivec4(clipRects.top());
  }
}

void iron::drawlist::push(command::ref cmd) {
  m_data.push_back(std::move(cmd));
}

void iron::drawlist::drawSolid() { m_tex = iron::whiteTex(); }

void iron::drawlist::pathArcToN(const fvec2& c, float radius, float a_min,
                                float a_max, int segments) {
  // pathAdd(c)
  pathReserve(segments + 1);
  for (int i = 0; i < segments; i++) {
    float a = a_min + ((float)i / (float)segments) * (a_max - a_min);
    pathAdd(fvec2(c.x + std::cos(a) * radius, c.y + std::sin(a) * radius));
  }
}

void iron::drawlist::pathFastArcToN(const fvec2& c, float r, float amin,
                                    float amax, int s) {
  /**
   * Funcion with less division overhead
   * Usefull for stuff where a lot of calculations are required
   */
  float d = (amax - amin) / s;
  pathReserve(s + 1);
  for (int i = 0; i <= s; i++) {
    float a = amin + i * d;
    pathAdd(fvec2(c.x + std::cos(a) * r, c.y + std::sin(a) * r));
  }
}

void iron::drawlist::pathRect(const fvec2& a, const fvec2& b, float rounding) {
  if (rounding == 0.f) {
    pathAdd(a);
    pathAdd(fvec2(b.x, a.y));
    pathAdd(b);
    pathAdd(fvec2(a.x, b.y));
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
    pathAdd(fvec2(a.x + r, a.y));
    pathFastArcToN(fvec2(b.x - r, a.y + r), r, -__pi / 2.0f, 0.0f, segments);
    /** Top Right */
    pathAdd(fvec2(b.x, b.y - r));
    pathFastArcToN(fvec2(b.x - r, b.y - r), r, 0.0f, __pi / 2.0f, segments);
    /** Bottom Right */
    pathAdd(fvec2(a.x + r, b.y));
    pathFastArcToN(fvec2(a.x + r, b.y - r), r, __pi / 2.0f, __pi, segments);
    /** Bottom Left */
    pathAdd(fvec2(a.x, a.y + r));
    pathFastArcToN(fvec2(a.x + r, a.y + r), r, __pi, 3.0f * __pi / 2.0f,
                   segments);
  }
}

void iron::drawlist::pathRectEx(const fvec2& a, const fvec2& b, float rounding,
                                ui flags) {
  if (rounding == 0.f) {
    pathAdd(a);
    pathAdd(fvec2(b.x, a.y));
    pathAdd(b);
    pathAdd(fvec2(a.x, b.y));
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
      pathAdd(a);
    } else {
      pathAdd(fvec2(a.x + r, a.y));
      pathFastArcToN(fvec2(b.x - r, a.y + r), r, -__pi / 2.0f, 0.0f, segments);
    }

    /** Top Right */
    if (flags & LiPathRectFlags_KeepTopRight) {
      pathAdd(fvec2(b.x, a.y));
    } else {
      pathAdd(fvec2(b.x, b.y - r));
      pathFastArcToN(fvec2(b.x - r, b.y - r), r, 0.0f, __pi / 2.0f, segments);
    }
    /** Bottom Right */
    if (flags & LiPathRectFlags_KeepBotRight) {
      pathAdd(b);
    } else {
      pathAdd(fvec2(a.x + r, b.y));
      pathFastArcToN(fvec2(a.x + r, b.y - r), r, __pi / 2.0f, __pi, segments);
    }
    /** Bottom Left */
    if (flags & LiPathRectFlags_KeepBotLeft) {
      pathAdd(fvec2(a.x, b.y));
    } else {
      pathAdd(fvec2(a.x, a.y + r));
      pathFastArcToN(fvec2(a.x + r, a.y + r), r, __pi, 3.0f * __pi / 2.0f,
                     segments);
    }
  }
}

void iron::drawlist::drawRect(const fvec2& pos, const fvec2& size, ui color,
                              int thickness) {
  pathRect(pos, pos + size);
  pathStroke(color, thickness, 1);
}

void iron::drawlist::drawRectFilled(const fvec2& pos, const fvec2& size,
                                    ui color) {
  pathRect(pos, pos + size);
  pathFill(color);
}

void iron::drawlist::drawTriangle(const fvec2& a, const fvec2& b,
                                  const fvec2& c, ui color, int thickness) {
  pathAdd(a);
  pathAdd(b);
  pathAdd(c);
  pathStroke(color, thickness, 1);
}

void iron::drawlist::drawTriangleFilled(const fvec2& a, const fvec2& b,
                                        const fvec2& c, ui color) {
  pathAdd(a);
  pathAdd(b);
  pathAdd(c);
  pathFill(color);
}

void iron::drawlist::drawCircle(const fvec2& center, float rad, ui color,
                                int segments, int thickness) {
  if (segments <= 0) {
    // Auto Segment
  } else {
    float am = (__pi * 2.0f) * ((float)segments) / (float)segments;
    pathArcToN(center, rad, 0.f, am, segments);
  }
  drawSolid();  // Only Solid Color Supported
  pathStroke(color, thickness, 1);
}

void iron::drawlist::drawCircleFilled(const fvec2& center, float rad, ui color,
                                      int segments) {
  if (segments <= 0) {
    // Auto Segment
  } else {
    float am = (__pi * 2.0f) * ((float)segments) / (float)segments;
    pathArcToN(center, rad, 0.f, am, segments);
  }
  pathFill(color);
}

void iron::drawlist::drawPolyLine(const std::vector<fvec2>& points, ui clr,
                                  ui flags, int thickness) {
  if (points.size() < 2) {
    return;
  }
  drawSolid();
  auto cmd = newCommand();
  bool close = (flags & (1 << 0));
  int num_points = close ? (int)points.size() : (int)points.size() - 1;
  if (flags & (1 << 1)) {
    // TODO: Find a way to draw less garbage looking lines
  } else {
    // Non antialiased lines look awful when rendering with thickness != 1
    for (int i = 0; i < num_points; i++) {
      int j = (i + 1) == (int)points.size() ? 0 : (i + 1);
      auto line = primLine(points[i], points[j], thickness);
      cmdQuad(cmd.get(), line, fvec4(0.f, 1.f, 1.f, 0.f), clr);
    }
  }
  push(std::move(cmd));
}

void iron::drawlist::drawConvexPolyFilled(const std::vector<fvec2>& points,
                                          ui clr) {
  if (points.size() < 3) {
    return;  // Need at least three points
  }
  auto cmd = newCommand();
  cmdConvexPolyFilled(cmd.get(), points, clr, m_tex);
  push(std::move(cmd));
}

void iron::drawlist::drawText(const fvec2& pos, const std::string& text,
                              ui color) {
  /*if (!pCurrentFont) {
    return;
  }
  std::vector<Command::Ref> cmds;
  pCurrentFont->CmdTextEx(cmds, pos, color, pFontScale, text);
  for (size_t i = 0; i < cmds.size(); i++) {
    cmds[i]->Index = pDrawList.size();
    cmds[i]->Layer = Layer;
    AddCommand(std::move(cmds[i]));
  }*/
}

void iron::drawlist::drawTextEx(const fvec2& p, const std::string& text,
                                ui color, ui flags, const fvec2& box) {
  /*if (!pCurrentFont) {
    return;
  }
  std::vector<Command::Ref> cmds;
  pCurrentFont->CmdTextEx(cmds, p, color, pFontScale, text, flags, box);
  for (size_t i = 0; i < cmds.size(); i++) {
    cmds[i]->Index = pDrawList.size();
    cmds[i]->Layer = Layer;
    AddCommand(std::move(cmds[i]));
  }*/
}

void iron::drawlist::drawLine(const fvec2& a, const fvec2& b, ui color, int t) {
  pathAdd(a);
  pathAdd(b);
  pathStroke(color, t);
}
}  // namespace amy