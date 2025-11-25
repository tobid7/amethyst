#include <amethyst.hpp>

#include "amethyst/iron.hpp"

class example : public amy::app {
 public:
  example() {
    amy::ctru::init();
    consoleInit(GFX_BOTTOM, NULL);
    amy::c3d::init();
    m_top = amy::c3d::createScreen(GFX_TOP, GFX_LEFT);
    amy::iron::init();
    dl = new amy::iron::drawlist();
    dl->drawSolid();
    auto cmd = std::make_unique<amy::iron::command>();
    cmd->index = 0;
    cmd->layer = 0;
    cmd->tex = amy::iron::whiteTex();
    cmd->add(0).add(1).add(2);
    cmd->add(amy::iron::vertex(200, 50, 0, 0, 0xff0000ff));
    cmd->add(amy::iron::vertex(100, 190, 0, 0, 0xff00ff00));
    cmd->add(amy::iron::vertex(300, 190, 0, 0, 0xffff0000));
    cmdl.push_back(std::move(cmd));
    //  throw std::runtime_error(std::format(
    //      "solid tex: {:#08x}\nsize: {}\nptr: {:#08x}",
    //      (amy::ui)amy::iron::whiteTex(), amy::iron::whiteTex()->size(),
    //      (amy::ui)amy::iron::whiteTex()->ptr()));
  };
  ~example() {
    amy::c3d::deleteScreen(m_top);
    amy::c3d::deinit();
  };

  void main() {
    amy::c3d::startFrame();
    m_top->startDraw();
    m_top->clear();
    // dl->drawRectFilled(0, 50, 0xff00ff00);
    amy::iron::newFrame();
    amy::iron::drawOn(m_top);
    // amy::iron::draw(dl->data());
    amy::iron::draw(cmdl);
    // dl->clear();
    amy::c3d::endFrame();
  }

 private:
  amy::c3d::screen* m_top = nullptr;
  amy::iron::drawlist* dl = nullptr;
  std::vector<amy::iron::command::ref> cmdl;
};

int main() {
  amy::registerCxxExceptionHandler();
  example app;
  app.run();
  return 0;
}