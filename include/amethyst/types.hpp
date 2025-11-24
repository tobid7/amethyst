#pragma once

#include <amethyst/maths/vec.hpp>
#include <cinttypes>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace amy {
using uc = unsigned char;
using us = unsigned short;
using ui = unsigned int;
using ull = unsigned long long;
using str = std::string;
using cstr = const std::string;
using ksr = const std::string&;
template <typename T>
using vec = std::vector<T>;
template <typename T>
using cvec = const std::vector<T>;
template <typename T>
using up = std::unique_ptr<T>;
}  // namespace amy