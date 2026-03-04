#include <amethyst/utils.hpp>
#include <format>
#include <iostream>

int main() {
  std::cout << "testing Amy::Utils::U8Iterator\n";
  const char* test = "Hello äöü";
  uint32_t c;
  Amy::Utils::U8Iterator it(test);

  while (it.Decode32(c)) {
    std::cout << std::format("Codepoint: U+{:04X}\n", c);
  }
}