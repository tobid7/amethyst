#include <citro3d.h>

#include <amethyst/c3d.hpp>
#include <amethyst/utils.hpp>
#include <pica.hpp>

namespace Amy {

const auto DISPLAY_TRANSFER_FLAGS =
    GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) |
    GX_TRANSFER_RAW_COPY(0) | GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) |
    GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) |
    GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO);
C3D_TexEnv* C3D::Frag::pEnv = nullptr;
int C3D::pDrawcalls = 0;
int C3D::p__dc__ = 0;

void C3D::Init() { C3D_Init(C3D_DEFAULT_CMDBUF_SIZE); }

void C3D::Deinit() { C3D_Fini(); }

void C3D::StartFrame(bool sync) {
  C3D_FrameBegin(sync ? C3D_FRAME_SYNCDRAW : C3D_FRAME_NONBLOCK);
  p__dc__ = 0;
}

void C3D::EndFrame() {
  C3D_FrameEnd(0);
  pDrawcalls = p__dc__;
}

C3D::Screen* C3D::CreateScreen(gfxScreen_t screen, gfx3dSide_t side) {
  auto t = C3D_RenderTargetCreate(240, screen == GFX_TOP ? 400 : 320,
                                  GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
  C3D_RenderTargetSetOutput(t, screen, side, DISPLAY_TRANSFER_FLAGS);
  return new C3D::Screen(t);
}

void C3D::DeleteScreen(C3D::Screen* screen) { delete screen; }

C3D::Shader::Shader(const std::string& path) { Load(path); }

C3D::Shader::~Shader() {
  if (pCode) {
    shaderProgramFree(&pProgram);
    DVLB_Free(pCode);
    pCode = nullptr;
  }
}

void C3D::Shader::Load(const std::string& path) {
  auto code = Utils::LoadFile2Mem(path);
  if (!code.size()) {
    throw std::runtime_error(
        std::format("[amy] unable to load shader ({})", path));
  }
  Load(code);
}

void C3D::Shader::Load(const std::vector<uc>& data) {
  if (!data.size()) {
    throw std::runtime_error("[amy] unable to load shader from data!");
  }
  pRawData = data;
  pCode = DVLB_ParseFile((u32*)&pRawData[0], pRawData.size());
  shaderProgramInit(&pProgram);
  shaderProgramSetVsh(&pProgram, &pCode->DVLE[0]);
  AttrInfo_Init(&pInfo);
}

void C3D::Shader::Compile(const std::string& code) {
  Load(Pica::AssembleCode(code.c_str()));
}

void C3D::Shader::Use() {
  // code works perfectly without C3D_BindProgram
  // but nor without shaderProgramUse ...
  shaderProgramUse(&pProgram);
  C3D_BindProgram(&pProgram);
  C3D_SetAttrInfo(&pInfo);
}

void C3D::Shader::SetMat4(int loc, C3D_Mtx* m) {
  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, loc, m);
}

void C3D::Shader::SetMat4(int loc, const mat4& m) {
  C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, loc, (const C3D_Mtx*)&m);
}

int C3D::Shader::loc(const std::string& name) {
  return shaderInstanceGetUniformLocation(pProgram.vertexShader, name.c_str());
}

void C3D::Shader::Input(int reg, GPU_FORMATS f, int num) {
  AttrInfo_AddLoader(&pInfo, reg, f, num);
}

void C3D::Frag::Src(C3D_TexEnvMode mode, GPU_TEVSRC s1, GPU_TEVSRC s2,
                    GPU_TEVSRC s3) {
  C3D_TexEnvSrc(pEnv, mode, s1, s2, s3);
}
void C3D::Frag::Func(C3D_TexEnvMode mode, GPU_COMBINEFUNC func) {
  C3D_TexEnvFunc(pEnv, mode, func);
}
void C3D::Frag::Edit(int id) {
  pEnv = C3D_GetTexEnv(id);
  C3D_TexEnvInit(pEnv);
}

void C3D::DrawArrays(int start, int count, GPU_Primitive_t prim) {
  C3D_DrawArrays(prim, start, count);
  p__dc__++;
}

void C3D::DrawElements(int count, const void* idx_ptr, int type,
                       GPU_Primitive_t prim) {
  C3D_DrawElements(prim, count, type, idx_ptr);
  p__dc__++;
}

void C3D::DepthTest(bool on, GPU_TESTFUNC func, GPU_WRITEMASK mask) {
  C3D_DepthTest(on, func, mask);
}

void C3D::DisableScissor() { C3D_SetScissor(GPU_SCISSOR_DISABLE, 0, 0, 0, 0); }

void C3D::EnableScissor(const ivec4 rect) {
  C3D_SetScissor(GPU_SCISSOR_NORMAL, rect.x, rect.y, rect.z, rect.w);
}

void C3D::BufCfg(void* ptr, int stride, int shader_attribs, u64 permutation) {
  auto buf = C3D_GetBufInfo();
  BufInfo_Init(buf);
  BufInfo_Add(buf, ptr, stride, shader_attribs, permutation);
}

void C3D::BufCfg(void* ptr, int stride, int shader_attribs) {
  BufCfg(ptr, stride, shader_attribs, pPermutation(shader_attribs));
}
}  // namespace Amy