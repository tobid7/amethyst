#pragma once

#include <amethyst/types.hpp>

namespace Amy {
class Error : public std::runtime_error {
 public:
  Error(ksr str) : std::runtime_error("[amy] " + str) {}
};
namespace Utils {
vec<uc> LoadFile2Mem(ksr path);
str FormatBytes(ull bytes);
ui HashMemory(kvr<uc> data);
ui NextPow2(ui v);
bool IsSingleBitNum(ui v);
ull GetTimeNano();
ull GetTimeMicro();
ull GetTimeMilli();
void String2U16(us* res, ksr src, size_t max);
std::string U16toU8(us* in, size_t max);
/**
 * FNV Hash functiom (32 Bit)
 * https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
 * Able to run at compile time btw
 */
constexpr ui FNV32(std::string_view str) {
  ui ret = 0x811C9DC5;
  for (char it : str) {
    ret ^= static_cast<ui>(it);
    ret *= 16777619;
  }
  return ret;
}

namespace Image {
void ReverseBuf(vec<uc>& buf, int w, int h, int c);
void RemoveAlphaChannel(vec<uc>& buf, int w, int h);
void AddAlphaChannel(vec<uc>& buf, int w, int h);
}  // namespace Image
}  // namespace Utils
}  // namespace Amy