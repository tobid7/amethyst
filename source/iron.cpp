#include <amethyst/iron.hpp>

namespace amy {
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
std::vector<iron::vertex, linearAllocator<iron::vertex>> iron::m_vbuf;
std::vector<u16, linearAllocator<u16>> iron::m_ibuf;
int iron::uLocProj = 0;
c3d::shader* iron::m_shader = nullptr;
mat4 iron::m_mtx;
int iron::m_idx = 0, iron::m_vtx = 0;

void iron::init() {
  setupShader();
  m_vbuf.resize(4 * 4096);
  m_ibuf.resize(6 * 4096);
}

void iron::newFrame() {
  m_idx = 0;
  m_vtx = 0;
}

void iron::drawOn(c3d::screen* screen) {
  m_shader->use();
  m_mtx = mat4::ortho(0.f, (float)screen->width(), (float)screen->height(), 0.f,
                      1.f, -1.f);
  m_shader->setMat4(uLocProj, m_mtx);
}

void iron::setupShader() {
  m_shader = new c3d::shader();
  m_shader->compile(__ironshader__);
  m_shader->input(GPU_FLOAT, 2);          // pos
  m_shader->input(GPU_FLOAT, 2);          // uv
  m_shader->input(GPU_UNSIGNED_BYTE, 4);  // color
  uLocProj = m_shader->loc("projection");
}

void iron::fragConfig() {
  c3d::frag::edit();
  c3d::frag::src(C3D_Both, GPU_TEXTURE0);
  c3d::frag::func(C3D_Both, GPU_MODULATE);
}
}  // namespace amy