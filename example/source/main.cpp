#include <amethyst.hpp>
#include <iostream>

class Example : public Amy::App {
 public:
  Example() {
    Amy::GTrace::Scope s("Init");
    gfxInitDefault();
    romfsInit();
    C3D::Init();
    Top = C3D::CreateScreen(GFX_TOP);
    Mgr = new Amy::AssetMgr();
    Iron::Init();
    Mgr->AutoLoad("icon", "romfs:/icon.png");
    auto fnt = Iron::Font::New();
    fnt->LoadBMF("romfs:/ComicNeue.png");
    Mgr->Add("font", fnt);
    // Mgr->AutoLoad("font", "romfs:/ComicNeue.ttf");
    dl = Iron::Drawlist::New();
    dl->SetFont(Mgr->Get<Iron::Font>("font"));
  }

  ~Example() {
    dl.reset();
    delete Top;
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
    std::string stats = std::format(
        "Delta: {:.3} -> {:.3} FPS\nTime: {}\nLinearRam: {}\nVIT: "
        "[{}/{}/{}]\nAssets: {}\nMain: {}",
        this->Delta(), 1000.0 / this->Delta(),
        Amy::Utils::FormatMillis(this->Time() * 1000.f),
        Amy::Utils::FormatBytes(linearSpaceFree()), Amy::Iron::VerticesDrawn(),
        Amy::Iron::IndicesDrawn(), Amy::Iron::IndicesDrawn() / 3,
        Mgr->CountAll(),
        Amy::Utils::FormatNanos(
            Amy::GTrace::GetTraceRef("Main")->GetLastDiff()));
    C3D::StartFrame();
    Amy::GTrace::Beg("Main");
    Top->Clear();
    Top->Use();
    dl->DrawTex(Mgr->Get<Amy::Texture>("icon"));
    dl->DrawRectFilled(Amy::fvec2(50, 0), 48, Amy::Color(255, 255, 255, 160));
    dl->DrawCircleFilled(Amy::fvec2(200, 120), 50, Amy::Color("#ffffff"), 40);
    dl->DrawSolid();
    dl->DrawRectFilled(0, 50, Amy::Color(0.f, 1.f, 0.f, 1.f));
    dl->DrawText(Amy::fvec2(5, 50), stats, Amy::Color(255, 0, 255));

    Iron::NewFrame();
    Iron::DrawOn(Top);
    Iron::Draw(dl->Data());
    dl->Clear();
    C3D::EndFrame();
    Amy::GTrace::End("Main");
  }

  C3D::Screen* Top;
  Amy::AssetMgr* Mgr;
  Iron::Drawlist::Ref dl = nullptr;
};

int main() {
  Amy::RegisterCxxExceptionHandler();
  Example App;
  App.Run();
  return 0;
}