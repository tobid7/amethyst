#include <amethyst/color.hpp>

namespace Amy {
// The Solution of the biggest performance issue
// A Simple Lookup table
static const std::unordered_map<char, int> HEX_DEC = {
    {'0', 0},  {'1', 1},  {'2', 2},  {'3', 3},  {'4', 4},  {'5', 5},
    {'6', 6},  {'7', 7},  {'8', 8},  {'9', 9},  {'a', 10}, {'b', 11},
    {'c', 12}, {'d', 13}, {'e', 14}, {'f', 15}, {'A', 10}, {'B', 11},
    {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15}};

Color& Color::Hex(const std::string& hex) {
  // zu dumm nen safetey check zu schreiben wadafuk
  /**#ifndef AMY_GOD_DEV
    /// Safetey check (not required if you just program well xd)
    if (hex.length() != 7 || hex.length() != 9 || hex.length() != 6 ||
        hex.length() != 8 || std::find_if(hex.begin(), hex.end(), [](char c) {
                               return !std::isxdigit(c);
                             }) != hex.end()) {
      return *this;
    }
  #endif*/
  int offset = ((hex.length() == 7 || hex.length() == 9) ? 1 : 0);
  r = HEX_DEC.at(hex[offset]) * 16 + HEX_DEC.at(hex[offset + 1]);
  offset += 2;
  g = HEX_DEC.at(hex[offset]) * 16 + HEX_DEC.at(hex[offset + 1]);
  offset += 2;
  b = HEX_DEC.at(hex[offset]) * 16 + HEX_DEC.at(hex[offset + 1]);
  offset += 2;
  if (hex.length() == 9) {
    a = HEX_DEC.at(hex[offset]) * 16 + HEX_DEC.at(hex[offset + 1]);
  } else {
    a = 255;
  }
  return *this;
}

std::string Color::Hex(bool rgba) const {
  /** Need to int cast (so it is used as num and not char...) */
  std::stringstream s;
  s << "#";
  s << std::hex << std::setw(2) << std::setfill('0') << (int)r;
  s << std::hex << std::setw(2) << std::setfill('0') << (int)g;
  s << std::hex << std::setw(2) << std::setfill('0') << (int)b;
  if (rgba) {
    s << std::hex << std::setw(2) << std::setfill('0') << (int)a;
  }
  return s.str();
}
}  // namespace Amy