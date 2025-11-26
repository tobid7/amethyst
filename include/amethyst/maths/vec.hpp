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

#include <amethyst/maths/vec2.hpp>
#include <amethyst/maths/vec3.hpp>
#include <amethyst/maths/vec4.hpp>
#include <format>

/** Define Formatters for C++ 20 */

/**
 * WHY DOES MSVC ALWAYS NEED THESE EXTRA THINGS
 */

template <typename T, typename CharT>
struct std::formatter<Amy::vec2<T>, CharT> : std::formatter<T, CharT> {
  template <typename FormatContext>
  auto format(const Amy::vec2<T>& v, FormatContext& ctx) const {
    return std::format_to(ctx.out(), "{}, {}", v.x, v.y);
  }
};

template <typename T, typename CharT>
struct std::formatter<Amy::vec3<T>, CharT> : std::formatter<T, CharT> {
  template <typename FormatContext>
  auto format(const Amy::vec3<T>& v, FormatContext& ctx) const {
    return std::format_to(ctx.out(), "{}, {}, {}", v.x, v.y, v.z);
  }
};

template <typename T, typename CharT>
struct std::formatter<Amy::vec4<T>, CharT> : std::formatter<T, CharT> {
  template <typename FormatContext>
  auto format(const Amy::vec4<T>& v, FormatContext& ctx) const {
    return std::format_to(ctx.out(), "{}, {}, {}, {}", v.x, v.y, v.z, v.w);
  }
};