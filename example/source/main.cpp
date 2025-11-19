#include <amethyst.hpp>

static C3D_Mtx projection;

int main() {
  amy::registerCxxExceptionHandler();
  amy::ctru::init();
  amy::c3d::init();
  auto top = amy::c3d::createScreen(GFX_TOP, GFX_LEFT);
  auto shader = new amy::c3d::shader("romfs:/shaders/shader2d.shbin");
  shader->input(GPU_FLOAT, 3);
  shader->input(GPU_FLOAT, 3);
  amy::c3d::frag::edit();
  amy::c3d::frag::src(C3D_Both);
  amy::c3d::frag::func(C3D_Both, GPU_REPLACE);
  Mtx_Identity(&projection);
  Mtx_OrthoTilt(&projection, 0.0, 400.0, 0.0, 240.0, 0.0, 1.0, true);
  while (aptMainLoop()) {
    amy::c3d::startFrame();
    top->startDraw();
    top->clear();
    shader->use();
    shader->setMat4(0, &projection);
    C3D_ImmDrawBegin(GPU_TRIANGLES);
    C3D_ImmSendAttrib(200, 200, 0.5, 0);
    C3D_ImmSendAttrib(1, 0, 0, 1);
    C3D_ImmSendAttrib(100, 40, 0.5, 0);
    C3D_ImmSendAttrib(0, 1, 0, 1);
    C3D_ImmSendAttrib(300, 40, 0.5, 0);
    C3D_ImmSendAttrib(0, 0, 1, 1);
    C3D_ImmDrawEnd();
    amy::c3d::endFrame();
  }
  amy::c3d::deleteScreen(top);
  return 0;
}