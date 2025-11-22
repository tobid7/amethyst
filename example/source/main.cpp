#include "amethyst/iron.hpp"
#include <amethyst.hpp>

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

static C3D_Mtx projection;

int main() {
  amy::registerCxxExceptionHandler();
  amy::ctru::init();
  amy::c3d::init();
  auto top = amy::c3d::createScreen(GFX_TOP, GFX_LEFT);
  amy::iron::init();
  while (aptMainLoop()) {
    amy::c3d::startFrame();
    top->startDraw();
    top->clear();
    amy::c3d::endFrame();
  }
  amy::c3d::deleteScreen(top);
  return 0;
}