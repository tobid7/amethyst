#include <amethyst.hpp>
#include <cstdlib>
#include <iostream>
#include <map>

#include "amethyst/iron.hpp"

struct memory_metrics {
  uint64_t t_TotalAllocated = 0;  ///< Total Allocated Memory
  uint64_t t_TotalFreed = 0;      ///< Total Deleted Memory
  /// @brief Gets the Currently Allocated Memory
  uint32_t t_CurrentlyAllocated() { return t_TotalAllocated - t_TotalFreed; }
};

static memory_metrics metrics;

bool rd7i_enable_memtrack = true;

void* operator new(size_t size) {
  void* ptr = malloc(size);
  if (rd7i_enable_memtrack) metrics.t_TotalAllocated += size;
  return ptr;
}

void operator delete(void* memory, size_t size) {
  if (rd7i_enable_memtrack) metrics.t_TotalFreed += size;
  free(memory);
}

size_t GetTotalAllocated() { return metrics.t_TotalAllocated; }

size_t GetTotalFreed() { return metrics.t_TotalFreed; }

size_t GetCurrent() { return metrics.t_CurrentlyAllocated(); }

std::string FormatBytes(unsigned long long bytes) {
  static const std::vector<std::string> endings = {
      "B", "KB", "MB", "GB", "TB", "Unk",
  };
  int i = 0;
  double b = bytes;
  while (b > 1024) {
    i++;
    b /= 1024;
  }
  if (i >= (int)endings.size()) {
    i = (int)endings.size() - 1;
  }
  return std::format("{:.1f} {}", b, endings[i]);
}

class example : public Amy::App {
 public:
  example() {
    Amy::Ctr::Init();
    consoleInit(GFX_BOTTOM, NULL);
    Amy::C3D::Init();
    m_top = Amy::C3D::CreateScreen(GFX_TOP);
    Amy::Iron::Init();
    dl = new Amy::Iron::Drawlist();
    dl->DrawSolid();
    mgr.AutoLoad("shader", "romfs:/shaders/shader2d.shbin");
    mgr.Get<Amy::C3D::Shader>("shader")->Input(GPU_FLOAT, 3);
    mgr.Get<Amy::C3D::Shader>("shader")->Input(GPU_FLOAT, 3);
    mgr.AutoLoad("icon", "romfs:/icon.png");
  };
  ~example() {
    Amy::C3D::DeleteScreen(m_top);
    Amy::C3D::Deinit();
  };

  void Main() override {
    std::cout << std::format("\x1b[1;1HDelta: {:.3f} -> {:.3} FPS\x1b[K", this->Delta(), 1000.0/this->Delta());
    std::cout << std::format("\x1b[2;1HTime: {:.3f}\x1b[K", this->Time());
    std::cout << std::format(
        "\x1b[3;1HMem: {}\n +{}\n -{}\nLin: {}\x1b[K",
        FormatBytes(GetCurrent()), FormatBytes(GetTotalAllocated()),
        FormatBytes(GetTotalFreed()), FormatBytes(linearSpaceFree()));

    Amy::C3D::StartFrame();
    m_top->Use();
    m_top->Clear();
    dl->DrawTex(mgr.Get<Amy::Texture>("icon"));
    dl->DrawRectFilled(100, 48, 0xffffffff);
    dl->DrawSolid();
    dl->DrawRectFilled(0, 50, 0xffffffff);
    Amy::Iron::NewFrame();
    Amy::Iron::DrawOn(m_top);
    Amy::Iron::Draw(*dl);
    dl->Clear();
    mgr.Get<Amy::C3D::Shader>("shader")->Use();
    mgr.Get<Amy::C3D::Shader>("shader")->SetMat4(0, mtx);
    C3D_ImmDrawBegin(GPU_TRIANGLES);
    C3D_ImmSendAttrib(200, 50, 0, 0);
    C3D_ImmSendAttrib(1, 0, 0, 1);
    C3D_ImmSendAttrib(300, 190, 0, 0);
    C3D_ImmSendAttrib(0, 1, 0, 1);
    C3D_ImmSendAttrib(100, 190, 0, 0);
    C3D_ImmSendAttrib(0, 0, 1, 1);
    C3D_ImmDrawEnd();
    Amy::C3D::EndFrame();
  }

 private:
  Amy::C3D::Screen* m_top = nullptr;
  Amy::Iron::Drawlist* dl = nullptr;
  Amy::C3D::Shader* test = nullptr;
  Amy::AssetMgr mgr;
  Amy::mat4 mtx =
      Amy::mat4::identity() * Amy::mat4::ortho(0, 400, 240, 0, 0, 1);
};

int main() {
  Amy::RegisterCxxExceptionHandler();
  example app;
  app.Run();
  return 0;
}