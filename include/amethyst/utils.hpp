#pragma once

#include <amethyst/types.hpp>

namespace Amy {
namespace Utils {
ui FastHash(ksr s);
vec<uc> LoadFile2Mem(ksr path);
ui HashMemory(kvr<uc> data);
ui NextPow2(ui v);
bool IsSingleBitNum(ui v);
ull GetTimeNano();
ull GetTimeMicro();
ull GetTimeMilli();
namespace Image {
void ReverseBuf(vec<uc>& buf, int w, int h, int c);
void RemoveAlphaChannel(vec<uc>& buf, int w, int h);
void AddAlphaChannel(vec<uc>& buf, int w, int h);
}  // namespace Image
}  // namespace Utils
}  // namespace Amy