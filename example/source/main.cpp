#include <amethyst.hpp>

#include "amethyst/iron.hpp"

const char* shader_ = R"(; Example PICA200 vertex shader

; Uniforms
.fvec projection[4]

; Constants
.constf myconst(0.0, 1.0, -1.0, 0.1)
.constf myconst2(0.3, 0.0, 0.0, 0.0)
.alias  zeros myconst.xxxx ; Vector full of zeros
.alias  ones  myconst.yyyy ; Vector full of ones

; Outputs
.out outpos position
.out outclr color

; Inputs (defined as aliases for convenience)
.alias inpos v0
.alias inclr v1

.bool test

.proc main
	; Force the w component of inpos to be 1.0
	mov r0.xyz, inpos
	mov r0.w,   ones

	; outpos = projectionMatrix * inpos
	dp4 outpos.x, projection[0], r0
	dp4 outpos.y, projection[1], r0
	dp4 outpos.z, projection[2], r0
	dp4 outpos.w, projection[3], r0

	; outclr = inclr
	mov outclr, inclr

	; We're finished
	end
.end
)";

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
    // throw std::runtime_error(std::format(
    //     "solid tex: {:#08x}\nsize: {}\nptr: {:#08x}",
    //     (amy::ui)amy::iron::whiteTex(), amy::iron::whiteTex()->size(),
    //     (amy::ui)amy::iron::whiteTex()->ptr()));
  }; 
  ~example() {
    amy::c3d::deleteScreen(m_top);
    amy::c3d::deinit();
  };

  void main() {
    amy::c3d::startFrame();
    m_top->startDraw();
    m_top->clear();
    dl->drawRectFilled(0, 50, 0xff00ff00);
    amy::iron::newFrame();
    amy::iron::drawOn(m_top);
    amy::iron::draw(dl->data());
    dl->clear();
    amy::c3d::endFrame();
  }

 private:
  amy::c3d::screen* m_top = nullptr;
  amy::iron::drawlist* dl = nullptr;
};

int main() {
  amy::registerCxxExceptionHandler();
  example app;
  app.run();
  return 0;
}