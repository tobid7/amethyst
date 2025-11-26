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

#include <amethyst/maths/mat.hpp>

namespace Amy {
mat4 mat4::rotateX(float a) {
  float c = std::cos(a);
  float s = std::sin(a);
  mat4 ret = identity();
  ret(1, 1) = c;
  ret(1, 2) = -s;
  ret(2, 1) = s;
  ret(2, 2) = c;
  return ret;
}

mat4 mat4::rotateY(float a) {
  float c = std::cos(a);
  float s = std::sin(a);
  mat4 ret = identity();
  ret(0, 0) = c;
  ret(0, 2) = s;
  ret(2, 0) = -s;
  ret(2, 2) = c;
  return ret;
}

mat4 mat4::rotateZ(float a) {
  float c = std::cos(a);
  float s = std::sin(a);
  mat4 ret = identity();
  ret(0, 0) = c;
  ret(0, 1) = -s;
  ret(1, 0) = s;
  ret(1, 1) = c;
  return ret;
}

mat4 mat4::rotate(fvec3 axis, float a) {
  float s = std::sin(a);
  float c = std::cos(a);
  float t = 1.f - c;
  axis = axis.Normalize();
  float x = axis.x;
  float y = axis.y;
  float z = axis.z;
  mat4 ret = identity();
  ret(0, 0) = t * x * x + c;
  ret(0, 1) = t * x * y - z * s;
  ret(0, 2) = t * x * z + y * s;

  ret(1, 0) = t * x * y + z * s;
  ret(1, 1) = t * y * y + c;
  ret(1, 2) = t * y * z - x * s;

  ret(2, 0) = t * x * z - y * s;
  ret(2, 1) = t * y * z + x * s;
  ret(2, 2) = t * z * z + c;
  return ret;
}

mat4 mat4::perspective(float fov, float aspect, float n, float f) {
  float _fov = std::tan(fov / 2.f);
  mat4 ret;
  ret(0, 0) = 1.f / (aspect * _fov);
  ret(1, 1) = 1.f / _fov;
#ifdef __3DS__
  ret(2, 3) = f * n / (n - f);
  ret(2, 2) = -(-1.f) * n / (n - f);
#else
  ret(2, 2) = -(f + n) / (f - n);
  ret(2, 3) = -(2.f * f * n) / (f - n);
#endif
  ret(3, 2) = -1.f;
  ret(3, 3) = 0.0f;
  return ret;
}

mat4 mat4::lookAt(const fvec3& pos, const fvec3& center, const fvec3& up) {
  auto f = fvec3(center - pos).Normalize();
  auto s = f.Cross(up).Normalize();
  auto u = s.Cross(f);

  mat4 ret = identity();
  ret(0, 0) = s.x;
  ret(0, 1) = s.y;
  ret(0, 2) = s.z;
  ret(1, 0) = u.x;
  ret(1, 1) = u.y;
  ret(1, 2) = u.z;
  ret(2, 0) = -f.x;
  ret(2, 1) = -f.y;
  ret(2, 2) = -f.z;
  ret(0, 3) = -s.Dot(pos);
  ret(1, 3) = -u.Dot(pos);
  ret(2, 3) = f.Dot(pos);
  return ret;
}
}  // namespace Amy