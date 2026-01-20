#include <amethyst/iron.hpp>
#include <amethyst/utils.hpp>
#include <fstream>
#include <iostream>

namespace Amy {
const char* __ironshader__ = R"(; LI7 Shader
; Constants
.constf myconst(0.0, 1.0, 0.00392156862745, 0.0)
.alias  ones  	myconst.yyyy ; Vector full of ones

; Uniforms
.fvec projection[4]

; Outputs
.out out_position		position
.out out_color			color
.out out_uv				texcoord0

; Inputs
.alias in_xy		v0
.alias in_uvc 		v1
.alias in_col	    v2

.entry vmain
.proc vmain
	mov r0.xy, in_xy.xy
	mov r0.w, ones

	dp4 out_position.x, projection[0], r0
	dp4 out_position.y, projection[1], r0
	dp4 out_position.z, projection[2], r0
	dp4 out_position.w, projection[3], r0

	mov out_uv, in_uvc.xy

	mul r1, myconst.zzzz, in_col
	mov out_color, r1
	end
.end)";

std::vector<Iron::Vertex, LinearAllocator<Iron::Vertex>> Iron::m_vbuf;
std::vector<u16, LinearAllocator<u16>> Iron::m_ibuf;
int Iron::uLocProj = 0;
C3D::Shader* Iron::m_shader = nullptr;
mat4 Iron::m_mtx;
int Iron::m_idx = 0, Iron::m_vtx = 0;
Texture::Ref Iron::m_solid = nullptr;
ui Iron::VertexCount = 0;
ui Iron::IndexCount = 0;

void Iron::Init() {
  pSetupShader();
  m_vbuf.resize(4 * 4096);
  m_ibuf.resize(6 * 4096);
  pInitSolidTex();
}

void Iron::Exit() {
  m_solid.reset();
  delete m_shader;
  m_vbuf.clear();
  m_ibuf.clear();
}

void Iron::NewFrame() {
  VertexCount = m_vtx;
  IndexCount = m_idx;
  m_idx = 0;
  m_vtx = 0;
}

void Iron::DrawOn(C3D::Screen* screen) {
  m_shader->Use();
  m_mtx = mat4::ortho(0.f, (float)screen->Width(), (float)screen->Height(), 0.f,
                      1.f, -1.f);
  m_shader->SetMat4(uLocProj, m_mtx);
}

void Iron::Draw(const CmdPool& data) {
  // disable depthtest cause we have no z buffer
  C3D::DepthTest(false);
  size_t i = 0;
  while (i < data.Size()) {
    Texture::Ref tex = data.GetCmd(i)->Tex;
    if (!tex || !tex->Ptr()) {
      i++;
      continue;
    }
    pFragConfig(tex->Ptr()->fmt);
    auto scissorOn = data.GetCmd(i)->ScissorOn;
    auto scissor = data.GetCmd(i)->ScissorRect;
    auto start = m_idx;

    // Loop until a statgechange and copy all data into Vertex/index buf
    while (i < data.Size() && scissorOn == data.GetCmd(i)->ScissorOn &&
           scissor == data.GetCmd(i)->ScissorRect &&
           tex == data.GetCmd(i)->Tex) {
      auto c = data.GetCmd(i);

      if (!pCheckSize(c->IndexBuf.size(), c->VertexBuf.size())) {
        throw Error("iron: too much draw data!!!" +
                    std::format("\nIdx: {}\nVtx: {}", c->IndexBuf.size(),
                                c->VertexBuf.size()));
      }

      for (int j = 0; j < c->IndexBuf.size(); j++) {
        m_ibuf[m_idx++] = m_vtx + c->IndexBuf[j];
      }
      for (int j = 0; j < c->VertexBuf.size(); j++) {
        m_vbuf[m_vtx++] = std::move(c->VertexBuf[j]);
      }
      i++;
    }
    ///// SCISSOR LOGIC BEG /////
    ///// SCISSOR LOGIC END /////
    tex->Bind();
    C3D::BufCfg<3>(m_vbuf.data(), sizeof(Vertex));
    C3D::DrawElements(m_idx - start, m_ibuf.data() + start);
  }
  C3D::DepthTest(true);
}

bool Iron::pCheckSize(size_t idx, size_t vtx) {
  return idx < m_ibuf.size() && vtx < m_vbuf.size();
}

void Iron::pSetupShader() {
  m_shader = new C3D::Shader();
  m_shader->Compile(__ironshader__);
  m_shader->Input(GPU_FLOAT, 2);          // pos
  m_shader->Input(GPU_FLOAT, 2);          // uv
  m_shader->Input(GPU_UNSIGNED_BYTE, 4);  // color
  uLocProj = m_shader->loc("projection");
}

void Iron::pFragConfig(GPU_TEXCOLOR clr) {
  C3D::Frag::Edit();
  switch (clr) {
    case GPU_A4:
    case GPU_A8:
    case GPU_L4:
    case GPU_L8:
      C3D::Frag::Src(C3D_Alpha, GPU_TEXTURE0);
      C3D::Frag::Func(C3D_RGB, GPU_REPLACE);
      C3D::Frag::Func(C3D_Alpha, GPU_MODULATE);
      break;

    default:
      C3D::Frag::Src(C3D_Both, GPU_TEXTURE0);
      C3D::Frag::Func(C3D_Both, GPU_MODULATE);
      break;
  }
}

void Iron::pInitSolidTex() {
  // i know there is a lot of memory wasted :(
  std::vector<uc> pixels(16 * 16, 0xff);
  m_solid = Texture::New();
  m_solid->Load(pixels, 16, 16, 1, Amy::Image::A8);
  if (!m_solid->Ptr()) {
    throw Error("white tex failed to load!");
  }
}

bool Iron::InBox(const fvec2& pos, const fvec2& size, const fvec4& area) {
  return (pos.x + size.x >= area.x && pos.y + size.y >= area.y &&
          pos.x <= area.z && pos.y <= area.w);
}

bool Iron::InBox(const fvec2& pos, const fvec4& area) {
  return (pos.x > area.x && pos.x < area.x + area.z && pos.y > area.y &&
          pos.y < area.y + area.w);
}

bool Iron::InBox(const fvec2& a, const fvec2& b, const fvec2& c,
                 const fvec4& area) {
  return ((a.x < area.z && b.x < area.z && c.x < area.z) ||
          (a.y < area.w && b.y < area.w && c.y < area.w) ||
          (a.x > 0 && b.x > 0 && c.x > 0) || (a.y > 0 && b.y > 0 && c.y > 0));
}

void Iron::RotateCorner(fvec2& pos, float s, float c) {
  float x = pos.x * c - pos.y * s;
  float y = pos.y * c - pos.x * s;
  pos = fvec2(x, y);
}

Rect Iron::PrimRect(const fvec2& pos, const fvec2& size, float angle) {
  fvec2 c = size * 0.5f;  // Center
  fvec2 corner[4] = {
      fvec2(-c.x, -c.y),
      fvec2(-c.x + size.x, -c.y),
      fvec2(-c.x, -c.y + size.y),
      fvec2(-c.x + size.x, -c.y + size.y),
  };

  // Only rotate if required
  if (angle != 0.f) {
    float s = std::sin(angle);
    float co = std::cos(angle);
    for (int i = 0; i < 4; i++) {
      RotateCorner(corner[i], s, co);
    }
  }

  // Return Result
  return Rect(corner[0] + pos + c, corner[1] + pos + c, corner[2] + pos + c,
              corner[3] + pos + c);
}

Rect Iron::PrimLine(const fvec2& a, const fvec2& b, int thickness) {
  // Using the vec maths api makes the code as short as it is
  fvec2 dir = a - b;
  float len = dir.Len();
  fvec2 unit_dir = dir / len;
  fvec2 perpendicular(-unit_dir.y, unit_dir.x);
  fvec2 off = perpendicular * ((float)thickness * 0.5f);

  return Rect(a + off, b + off, a - off, b - off);
}

void Iron::CmdQuad(Command* cmd, const Rect& q, const Rect& uv, ui color) {
  cmd->Add(0).Add(1).Add(2);
  cmd->Add(0).Add(2).Add(3);
  cmd->Add(Vertex(q.BotRight(), uv.BotRight(), color));
  cmd->Add(Vertex(q.TopRight(), uv.TopRight(), color));
  cmd->Add(Vertex(q.TopLeft(), uv.TopLeft(), color));
  cmd->Add(Vertex(q.BotLeft(), uv.BotLeft(), color));
}

void Iron::CmdTriangle(Command* cmd, const fvec2& a, const fvec2& b,
                       const fvec2& c, ui color) {
  cmd->Add(2).Add(1).Add(0);  // reverse cause otherwise invisible
  cmd->Add(Vertex(a, fvec2(0, 1), color));
  cmd->Add(Vertex(b, fvec2(1, 1), color));
  cmd->Add(Vertex(c, fvec2(1, 0), color));
}

void Iron::CmdConvexPolyFilled(Command* cmd, const std::vector<fvec2>& points,
                               ui color, Texture::Ref tex) {
  if (points.size() < 3 || tex == nullptr) {
#ifdef AMY_GOD_DEV
    return;
#else
    throw std::runtime_error("[amy] iron: trying to render convex poly with " +
                             std::to_string(points.size()) +
                             " points and Texture " +
                             std::to_string((ui)tex.get()));
#endif
  }
  // Support for Custom Textures (UV calculation)
  float minX = points[0].x, minY = points[0].y;
  float maxX = minX, maxY = minY;
  // Check for the max and min Positions
  for (const auto& it : points) {
    if (it.x < minX) minX = it.x;
    if (it.y < minY) minY = it.y;
    if (it.x > maxX) maxX = it.x;
    if (it.y > maxY) maxY = it.y;
  }
  // Get Short defines for UV
  // (Bottom Right is not required)
  auto uv_tl = tex->Uv().TopLeft();
  auto uv_tr = tex->Uv().TopRight();
  auto uv_bl = tex->Uv().BotLeft();

  // Render
  for (int i = 2; i < (int)points.size(); i++) {
    cmd->Add(0).Add(i).Add(i - 1);
  }
  for (int i = 0; i < (int)points.size(); i++) {
    // Calculate U and V coords
    float u =
        uv_tl.x + ((points[i].x - minX) / (maxX - minX)) * (uv_tr.x - uv_tl.x);
    float v =
        uv_tl.y + ((points[i].y - minY) / (maxY - minY)) * (uv_bl.y - uv_tl.y);
    cmd->Add(Vertex(points[i], fvec2(u, v), color));
  }
}
}  // namespace Amy