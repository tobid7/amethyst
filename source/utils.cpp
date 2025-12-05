#include <amethyst/utils.hpp>
#include <fstream>

namespace Amy {
namespace Utils {
vec<uc> LoadFile2Mem(ksr path) {
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

str FormatBytes(ull bytes) {
  static const vec<str> endings = {
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

ui HashMemory(kvr<uc> data) {
  ui hash = 4477;
  for (auto& it : data) {
    hash = (hash * 33) + it;
  }
  return hash;
}

ui NextPow2(ui v) {
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;
  return (v >= 64 ? v : 64);
}

bool IsSingleBitNum(ui v) { return v && !(v & (v - 1)); }

ull GetTimeNano() {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}
ull GetTimeMicro() {
  return std::chrono::duration_cast<std::chrono::microseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}
ull GetTimeMilli() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}

void String2U16(us* res, const std::string& src, size_t max) {
  /// GOT FORCED TO REPLACE std::wstring_convert by some
  /// manual work as it got removed in cxx20
  /// TODO ///
  /// ADD SOME ERROR API IN HERE
  if (max == 0) return;
  size_t len = 0;
  size_t i = 0;
  while (i < src.size() && len < max) {
    uc c = src[i];

    if (c < 0x80) {
      // 1byte
      res[len++] = c;
      i++;
    } else if ((c >> 5) == 0x6) {
      // 2byte
      if (i + 1 >= src.size())
        throw std::invalid_argument("Invalid UTF-8 sequence");
      res[len++] = ((c & 0x1F) << 6) | (src[i + 1] & 0x3F);
      i += 2;
    } else if ((c >> 4) == 0xE) {
      // 3byte
      if (i + 2 >= src.size())
        throw std::invalid_argument("Invalid UTF-8 sequence");
      res[len++] =
          ((c & 0x0F) << 12) | ((src[i + 1] & 0x3F) << 6) | (src[i + 2] & 0x3F);
      i += 3;
    } else if ((c >> 3) == 0x1E) {
      // 4byte
      if (i + 3 >= src.size())
        throw std::invalid_argument("Invalid UTF-8 sequence");
      ui codepoint = ((c & 0x07) << 18) | ((src[i + 1] & 0x3F) << 12) |
                     ((src[i + 2] & 0x3F) << 6) | (src[i + 3] & 0x3F);
      codepoint -= 0x10000;
      res[len++] = 0xD800 | ((codepoint >> 10) & 0x3FF);
      res[len++] = 0xDC00 | (codepoint & 0x3FF);
      i += 4;
    } else {
      return;
    }
  }
}

std::string U16toU8(us* in, size_t max) {
  /// GOT FORCED TO REPLACE std::wstring_convert by some
  /// manual work as it got removed in cxx20
  if (!in || max == 0) {
    return "";
  }

  std::string result;
  result.reserve(max * 3);

  for (size_t i = 0; i < max; i++) {
    uint16_t c = in[i];

    if (c < 0x80) {
      result.push_back(static_cast<char>(c));
    } else if (c < 0x800) {
      result.push_back(static_cast<char>(0xC0 | (c >> 6)));
      result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
    } else if (c < 0x10000) {
      result.push_back(static_cast<char>(0xE0 | (c >> 12)));
      result.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
      result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
    } else {
      continue;
    }
  }

  return result;
}

namespace Image {
void ReverseBuf(vec<uc>& buf, int w, int h, int c) {
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

void RemoveAlphaChannel(vec<uc>& buf, int w, int h) {
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

void AddAlphaChannel(vec<uc>& buf, int w, int h) {
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
}  // namespace Image
}  // namespace Utils
}  // namespace Amy