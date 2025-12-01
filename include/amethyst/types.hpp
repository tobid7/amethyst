#pragma once

#include <algorithm>
#include <amethyst/maths/vec.hpp>
#include <chrono>
#include <cinttypes>
#include <functional>
#include <format>
#include <map>
#include <memory>
#include <numbers>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

namespace Amy {
using uc = unsigned char;
using us = unsigned short;
using ui = unsigned int;
using ull = unsigned long long;
// string
using str = std::string;
// const string
using kstr = const std::string;
// const string reference
using ksr = const std::string&;
// vector
template <typename T>
using vec = std::vector<T>;
// const vector reference
template <typename T>
using kvr = const std::vector<T>&;
// const vector
template <typename T>
using kvec = const std::vector<T>;
// unique pointer
template <typename T>
using up = std::unique_ptr<T>;
// shared pointer
template <typename T>
using sp = std::shared_ptr<T>;
}  // namespace Amy