#include <format>
#include <iostream>
#include <stdexcept>

using u64 = unsigned long long;

constexpr u64 permutation(int ac) {
  u64 ret = 0;
  if (ac < 1 || ac > 15) {
    throw std::runtime_error("[amy] " + std::to_string(ac) +
                             " is out of range (1...15)!");
  }
  for (int i = 0; i < ac; i++) {
    ret = (ret << 4) | (ac - 1 - i);
  }
  return ret;
}

int main(int argc, char** argv) {
  int v = 3;
  if (argc == 2) {
    v = std::stoi(argv[1]);
  } else {
    std::cout << "No input provided! using example..." << std::endl;
  }
  std::cout << std::format("{} -> {:#x} ({})", v, permutation(v),
                           permutation(v))
            << std::endl;
  return 0;
}