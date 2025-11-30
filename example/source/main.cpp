#include <amethyst.hpp>

class Example : public Amy::App {
public:
  Example() {
    Amy::Ctr::Init();
    Amy::C3D::Init();
    Top = Amy::C3D::CreateScreen(GFX_TOP);
  }
  ~Example() { delete Top; }

  void Main() {
    Amy::C3D::StartFrame();
    Top->Clear();
    Top->Use();
    Amy::C3D::EndFrame();
  }

  Amy::C3D::Screen *Top;
};

int main() {
  Example App;
  App.Run();
  return 0;
}