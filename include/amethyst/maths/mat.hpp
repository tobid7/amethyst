#pragma once

/*
MIT License

Copyright (c) 2024 - 2025 tobid7

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <amethyst/maths/vec3.hpp>
#include <array>
#include <cmath>
#include <numbers>

namespace amy {
namespace numbers {
constexpr float Tau = std::numbers::pi * 2.f;
}
constexpr float Radians(float v) { return v * (numbers::Tau / 360.0f); }
/**
 * Minimal Mtx4 Lib that precomputes
 * basic stuff stuff at compiletime
 *
 * This Lib includes Patches for work with Citro3D as well
 *
 * @note That this is not a full Matrix Library
 */

struct mat4 {
  std::array<float, 16> m;
  constexpr mat4() : m{} {}
  constexpr static mat4 diagonal(float x, float y, float z, float w) {
    mat4 ret;
    ret(0, 0) = x;
    ret(1, 1) = y;
    ret(2, 2) = z;
    ret(3, 3) = w;
    return ret;
  }
  constexpr static mat4 identity() { return diagonal(1, 1, 1, 1); }

  constexpr float* ptr() { return m.data(); }
  constexpr const float* ptr() const { return m.data(); }

  constexpr float& operator()(int row, int col) {
#ifdef __3DS__
    // 3ds is full reverse order iirc
    return m[row * 4 + (3 - col)];
#else
    return m[col * 4 + row];
#endif
  }
  constexpr float operator()(int row, int col) const {
#ifdef __3DS__
    // 3ds is full reverse order iirc
    return m[row * 4 + (3 - col)];
#else
    return m[col * 4 + row];
#endif
  }

  constexpr mat4 operator*(const mat4& v) const {
    mat4 ret;
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        float t = 0.f;
        for (int k = 0; k < 4; k++) {
          t += (*this)(i, k) * v(k, j);
        }
        ret(i, j) = t;
      }
    }
    return ret;
  }

  constexpr mat4& operator*=(const mat4& v) {
    *this = *this * v;
    return *this;
  }

  constexpr static mat4 translate(float x, float y, float z) {
    mat4 ret = identity();
    ret(0, 3) = x;
    ret(1, 3) = y;
    ret(2, 3) = z;
    return ret;
  }

  constexpr static mat4 scale(float x, float y, float z) {
    mat4 ret;
    ret(0, 0) = x;
    ret(1, 1) = y;
    ret(2, 2) = z;
    ret(3, 3) = 1.f;
    return ret;
  }

  constexpr static mat4 ortho(float l, float r, float b, float t, float n,
                              float f) {
    mat4 ret;
#ifdef __3DS__  // Patch to rotate the Matrix correctly
    ret(0, 1) = 2.f / (t - b);
    ret(0, 3) = (b + t) / (b - t);
    ret(1, 0) = 2.f / (l - r);
    ret(1, 3) = (l + r) / (r - l);
    ret(2, 2) = 1.f / (n - f);
    ret(2, 3) = 0.5f * (n + f) / (n - f) - 0.5f;
#else
    ret(0, 0) = 2.0f / (r - l);
    ret(0, 3) = -(r + l) / (r - l);
    ret(1, 1) = 2.0f / (t - b);
    ret(1, 3) = -(t + b) / (t - b);
    ret(2, 2) = -2.0f / (f - n);
    ret(2, 3) = -(f + n) / (f - n);
#endif
    ret(3, 3) = 1.f;
    return ret;
  }

  static mat4 rotate(fvec3 axis, float a);
  static mat4 rotateX(float a);
  static mat4 rotateY(float a);
  static mat4 rotateZ(float a);
  static mat4 perspective(float fov, float aspect, float n, float f);
  static mat4 lookAt(const fvec3& pos, const fvec3& center, const fvec3& up);
};
}  // namespace amy