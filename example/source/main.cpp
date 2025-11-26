#include <amethyst.hpp>
#include <iostream>

#include "amethyst/iron.hpp"

class example : public Amy::App {
 public:
  example() {
    Amy::Ctr::Init();
    consoleInit(GFX_BOTTOM, NULL);
    Amy::C3D::Init();
    m_top = Amy::C3D::CreateScreen(GFX_TOP, GFX_LEFT);
    Amy::Iron::Init();
    dl = new Amy::Iron::Drawlist();
    dl->DrawSolid();
  };
  ~example() {
    Amy::C3D::DeleteScreen(m_top);
    Amy::C3D::Deinit();
  };

  void Main() override {
    Amy::C3D::StartFrame();
    m_top->Use();
    m_top->Clear();
    dl->DrawRectFilled(0, 50, 0xff00ff00);
    Amy::Iron::NewFrame();
    Amy::Iron::DrawOn(m_top);
    Amy::Iron::Draw(*dl);
    dl->Clear();
    Amy::C3D::EndFrame();
    std::cout << std::format("\x1b[1;1HDelta: {:.3f}\x1b[K", this->Delta());
    std::cout << std::format("\x1b[2;1HTime: {:.2f}\x1b[K", this->Time());
  }

 private:
  Amy::C3D::Screen* m_top = nullptr;
  Amy::Iron::Drawlist* dl = nullptr;
};

int main() {
  Amy::RegisterCxxExceptionHandler();
  example app;
  app.Run();
  return 0;
}