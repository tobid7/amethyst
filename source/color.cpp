#include <amethyst/color.hpp>

namespace Amy {
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