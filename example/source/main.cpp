#include <amethyst.hpp>
#include <iostream>

class Example : public Amy::App {
 public:
  Example() {
    gfxInitDefault();
    romfsInit();
    consoleInit(GFX_BOTTOM, NULL);
    C3D::Init();
    Top = C3D::CreateScreen(GFX_TOP);
    Mgr = new Amy::AssetMgr();
    Mgr->AutoLoad("icon", "romfs:/icon.png");
    Iron::Init();
    dl = new Iron::Drawlist();
  }
  ~Example() {
    delete Top;
    delete dl;
    delete Mgr;
    Iron::Exit();
    C3D::Deinit();
    romfsExit();
    gfxExit();
  }

  void Main() {
    hidScanInput();
    if (hidKeysDown() & KEY_START) {
      Exit();
    }
    std::cout << std::format("\x1b[1;1HDelta: {:.3f} -> {:.3} FPS\x1b[K",
                             this->Delta(), 1000.0 / this->Delta());
    std::cout << std::format("\x1b[2;1HTime: {:.3f}\x1b[K", this->Time());
    std::cout << std::format(
        "\x1b[3;1H\nLin: {}\nVertices: {}\nIndices: "
        "{}\x1b[K",
        Amy::Utils::FormatBytes(linearSpaceFree()), Iron::VerticesDrawn(),
        Iron::IndicesDrawn());
    C3D::StartFrame();
    Top->Clear();
    Top->Use();
    dl->DrawTex(Mgr->Get<Amy::Texture>("icon"));
    dl->DrawRectFilled(Amy::fvec2(50, 0), 48, 0x99999999);
    dl->DrawCircleFilled(Amy::fvec2(200, 120), 50, 0xffffffff, 40);
    dl->DrawSolid();
    dl->DrawRectFilled(0, 50, 0x4400ff00);

    Iron::NewFrame();
    Iron::DrawOn(Top);
    Iron::Draw(*dl);
    dl->Clear();
    C3D::EndFrame();
  }

  C3D::Screen* Top;
  Amy::AssetMgr* Mgr;
  Iron::Drawlist* dl;
};

int main() {
  Amy::RegisterCxxExceptionHandler();
  Example App;
  App.Run();
  return 0;
}