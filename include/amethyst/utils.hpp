#pragma once

#include <amethyst/types.hpp>

namespace amy {
namespace utils {
ui fastHash(cstr& s);
vec<uc> loadFile2Mem(cstr& path);
ui hashMemory(cvec<uc>& data);
ui nextPow2(ui v);
bool isSingleBitNum(ui v);
namespace image {
void reverseBuf(vec<uc>& buf, int w, int h, int c);
void removeAlphaChannel(vec<uc>& buf, int w, int h);
void addAlphaChannel(vec<uc>& buf, int w, int h);
}  // namespace image
}  // namespace utils
}  // namespace amy