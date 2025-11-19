#include <amethyst/utils.hpp>
#include <fstream>

namespace amy {
namespace utils {
ui fastHash(cstr& s) {
  ui hash = 5381;
  for (auto& it : s) {
    hash = (hash * 33) + static_cast<uc>(it);
  }
  return hash;
}

vec<uc> loadFile2Mem(cstr& path) {
  std::ifstream iff(path, std::ios::binary);
  if (!iff) {
    return vec<uc>();
  }
  iff.seekg(0, std::ios::end);
  size_t szs = iff.tellg();
  iff.seekg(0, std::ios::beg);
  vec<uc> res(szs, 0);
  iff.read(reinterpret_cast<char*>(res.data()), res.size());
  iff.close();
  return res;
}

ui hashMemory(cvec<uc>& data) {
  ui hash = 4477;
  for (auto& it : data) {
    hash = (hash * 33) + it;
  }
  return hash;
}

ui nextPow2(ui v) {
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;
  return (v >= 64 ? v : 64);
}

bool isSingleBitNum(ui v) { return v && !(v & (v - 1)); }

namespace image {
void reverseBuf(vec<uc>& buf, int w, int h, int c) {
  vec<uc> cpy = buf;
  for (int x = 0; x < w; x++) {
    for (int y = 0; y < h; y++) {
      int pos = (y * w + x) * c;
      for (size_t i = 0; i < c; i++) {
        buf[pos + c - 1 - i] = cpy[pos + i];
      }
    }
  }
}

void removeAlphaChannel(vec<uc>& buf, int w, int h) {
  vec<uc> cpy = buf;
  buf.resize(w * h * 3);
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      int src = (y * w + x) * 4;
      int dst = (y * w + x) * 3;
      buf[dst + 0] = cpy[src + 0];
      buf[dst + 1] = cpy[src + 1];
      buf[dst + 2] = cpy[src + 2];
    }
  }
}

void addAlphaChannel(vec<uc>& buf, int w, int h) {
  vec<uc> cpy = buf;
  buf.resize(w * h * 4);
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      int src = (y * w + x) * 3;
      int dst = (y * w + x) * 4;
      buf[dst + 0] = cpy[src + 0];
      buf[dst + 1] = cpy[src + 1];
      buf[dst + 2] = cpy[src + 2];
      buf[dst + 3] = 255;
    }
  }
}
}  // namespace image
}  // namespace utils
}  // namespace amy