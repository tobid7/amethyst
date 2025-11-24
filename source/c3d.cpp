#include <citro3d.h>

#include <amethyst/c3d.hpp>
#include <amethyst/utils.hpp>
#include <pica.hpp>

namespace amy {

const auto DISPLAY_TRANSFER_FLAGS =
    GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) |
    GX_TRANSFER_RAW_COPY(0) | GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) |
    GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) |
    GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO);
C3D_TexEnv* c3d::frag::m_env = nullptr;
int c3d::m_drawcalls = 0;
int c3d::m__dc__ = 0;

void c3d::init() { C3D_Init(C3D_DEFAULT_CMDBUF_SIZE); }

void c3d::deinit() { C3D_Fini(); }

void c3d::startFrame(bool sync) {
  C3D_FrameBegin(sync ? C3D_FRAME_SYNCDRAW : C3D_FRAME_NONBLOCK);
  m__dc__ = 0;
}

void c3d::endFrame() {
  C3D_FrameEnd(0);
  m_drawcalls = m__dc__;
}

c3d::screen* c3d::createScreen(gfxScreen_t screen, gfx3dSide_t side) {
  auto t = C3D_RenderTargetCreate(240, screen == GFX_TOP ? 400 : 320,
                                  GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTargetSetOutput(t, screen, side, DISPLAY_TRANSFER_FLAGS);
  return new c3d::screen(t);
}

void c3d::deleteScreen(c3d::screen* screen) { delete screen; }

c3d::shader::shader(const std::string& path) { load(path); }

c3d::shader::~shader() {}

void c3d::shader::load(const std::string& path) {
  auto code = utils::loadFile2Mem(path);
  if (!code.size()) {
    throw std::runtime_error(
        std::format("[amy] unsable to load shader ({})", path));
  }
  load(code);
}

void c3d::shader::load(const std::vector<uc>& data) {
  m_code = DVLB_ParseFile((u32*)&data[0], data.size());
  shaderProgramInit(&m_program);
  shaderProgramSetVsh(&m_program, &m_code->DVLE[0]);
  C3D_BindProgram(&m_program);
  AttrInfo_Init(&m_info);
}

void c3d::shader::compile(const std::string& code) {
  auto ret = Pica::AssembleCode(code.c_str());
  load(ret);
}

void c3d::shader::use() {
  C3D_BindProgram(&m_program);
  // for some reason i need both ???
  // code works perfectly without C3D_BindProgram
  // but nor withour shaderProgramUse ...
  shaderProgramUse(&m_program);
  C3D_SetAttrInfo(&m_info);
}

void c3d::shader::setMat4(int loc, C3D_Mtx* m) {
  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, loc, m);
}

void c3d::shader::setMat4(int loc, const mat4& m) {
  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, loc, (const C3D_Mtx*)&m);
}

int c3d::shader::loc(const std::string& name) {
  return shaderInstanceGetUniformLocation(m_program.vertexShader, name.c_str());
}

void c3d::shader::input(int reg, GPU_FORMATS f, int num) {
  AttrInfo_AddLoader(&m_info, reg, f, num);
}

void c3d::frag::src(C3D_TexEnvMode mode, GPU_TEVSRC s1, GPU_TEVSRC s2,
                    GPU_TEVSRC s3) {
  C3D_TexEnvSrc(m_env, mode, s1, s2, s3);
}
void c3d::frag::func(C3D_TexEnvMode mode, GPU_COMBINEFUNC func) {
  C3D_TexEnvFunc(m_env, mode, func);
}
void c3d::frag::edit(int id) {
  m_env = C3D_GetTexEnv(id);
  C3D_TexEnvInit(m_env);
}

void c3d::drawArrays(int start, int count, GPU_Primitive_t prim) {
  C3D_DrawArrays(prim, start, count);
  m__dc__++;
}

void c3d::drawElements(int count, const void* idx_ptr, int type,
                       GPU_Primitive_t prim) {
  C3D_DrawElements(prim, count, type, idx_ptr);
  m__dc__++;
}

void c3d::depthTest(bool on, GPU_TESTFUNC func, GPU_WRITEMASK mask) {
  C3D_DepthTest(on, func, mask);
}

void c3d::disableScissor() { C3D_SetScissor(GPU_SCISSOR_DISABLE, 0, 0, 0, 0); }

void c3d::enableScissor(const ivec4 rect) {
  C3D_SetScissor(GPU_SCISSOR_NORMAL, rect.x, rect.y, rect.z, rect.w);
}

void c3d::bufCfg(void* ptr, int stride, int shader_attribs, u64 permutation) {
  auto buf = C3D_GetBufInfo();
  BufInfo_Init(buf);
  BufInfo_Add(buf, ptr, stride, shader_attribs, permutation);
}

void c3d::bufCfg(void* ptr, int stride, int shader_attribs) {
  bufCfg(ptr, stride, shader_attribs, permutation(shader_attribs));
}
}  // namespace amy