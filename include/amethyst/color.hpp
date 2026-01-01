#pragma once

#include <amethyst/types.hpp>
#include <amethyst/utils.hpp>

// We all know where the code is from ... RenderD7 -> Palladium/Amy

namespace Amy {
class Color {
 public:
  /**
   * Default Constructor (all variables are set to 0)
   */
  constexpr Color() : r(0), g(0), b(0), a(0) {}
  constexpr ~Color() {}
  /**
   * Constructor for 32Bit Color Input
   * @param color 32Bit Color value
   */
  constexpr Color(ui clr) {
    a = (clr >> 24) & 0xff;
    b = (clr >> 16) & 0xff;
    g = (clr >> 8) & 0xff;
    r = clr & 0xff;
  }
  /**
   * Constructor for 8Bit Input
   * @param r Red Value
   * @param g Green Value
   * @param b Blue Value
   * @param a Optional Alpha Value (Defaults to 255)
   */
  constexpr Color(int r, int g, int b, int a = 255) : r(r), g(g), b(b), a(a) {}
  /**
   * Constructor for float Input
   * @param r Red Value
   * @param g Green Value
   * @param b Blue Value
   * @param a Optional Alpha Value (Defaults to 1.0f)
   * @note There is no Check if the number is between 0.0 and 1.0
   */
  constexpr Color(float r, float g, float b, float a = 1.f)
      : r(static_cast<uc>(255.f * r)),
        g(static_cast<uc>(255.f * g)),
        b(static_cast<uc>(255.f * b)),
        a(static_cast<uc>(255.f * a)) {}
  /**
   * Constructor for Hex Input
   * @param hex Hex String in `#ffffff` or `#ffffffff` format
   */
  constexpr Color(const std::string& hex) { Hex(hex); }

  /**
   * Create Color Object by Hex String
   * @param hex Hex String in `#ffffff` or `#ffffffff` format
   * @return Color class itself
   */
  constexpr Color& Hex(const std::string& hex) {
    if (!(hex.length() == 7 || hex.length() == 9)) {
      throw "[PD] Color: hex string is not rgb or rgba!";
    }
    r = Utils::HexChar2Int(hex[1]) * 16 + Utils::HexChar2Int(hex[2]);
    g = Utils::HexChar2Int(hex[3]) * 16 + Utils::HexChar2Int(hex[4]);
    b = Utils::HexChar2Int(hex[5]) * 16 + Utils::HexChar2Int(hex[6]);
    if (hex.length() == 9) {
      a = Utils::HexChar2Int(hex[7]) * 16 + Utils::HexChar2Int(hex[8]);
    } else {
      a = 255;
    }
    return *this;
  }
  /**
   * Convert this Color Object to Hex string
   * @param rgba [default false] sets if 8 or 6 digit color should be returned
   * @return Color Hex String
   */
  std::string Hex(bool rgba = false) const;

  /**
   * Fade from Current to another Color
   * @param color Color to fade to
   * @param p Amount (supports -1.0 to 1.0 for use of sine)
   * @return Class Reference
   */
  constexpr Color& Fade(const Color& color, float p) {
    a = static_cast<uc>((color.a - a) * ((p + 1.f) / 2));
    b = static_cast<uc>((color.b - b) * ((p + 1.f) / 2));
    g = static_cast<uc>((color.g - g) * ((p + 1.f) / 2));
    r = static_cast<uc>((color.r - r) * ((p + 1.f) / 2));
    return *this;
  }

  /**
   * Get 32Bit Color Value
   * @return 32Bit Color Value (ABGR iirc)
   */
  constexpr ui Get() const { return (a << 24) | (b << 16) | (g << 8) | r; }
  /**
   * Get The Luminance of the Color
   * @return luminance (from 0.0 to 1.0)
   */
  constexpr float Luminance() const {
    // For Reference https://en.wikipedia.org/wiki/HSL_and_HSV#Lightness
    return (0.3 * (r / 255.f) + 0.59 * (g / 255.f) + 0.11 * (b / 255.f));
  }
  /**
   * Check if the Color is Light or Dark
   * @return true if light
   */
  constexpr bool IsLight() const { return (Luminance() >= 0.5); }

  /**
   * Operator to cast Color to 32Bit Value
   * @return 32Bit Color Value
   */
  constexpr operator ui() const { return Get(); }

  /** Public Access Data section */
  uc r;
  uc g;
  uc b;
  uc a;
};
}  // namespace Amy