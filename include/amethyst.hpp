#pragma once

#include <amethyst/app.hpp>
#include <amethyst/assets.hpp>
#include <amethyst/c3d.hpp>
#include <amethyst/color.hpp>
#include <amethyst/godtrace.hpp>
#include <amethyst/image.hpp>
#include <amethyst/iron.hpp>
#include <amethyst/renderer.hpp>
#include <amethyst/texture.hpp>
#include <amethyst/utils.hpp>
#include <atomic>

namespace Amy {
void RegisterCxxExceptionHandler();
void* Malloc(size_t size);
void Free(void* ptr);
namespace Memory {
struct MemMetrics {
  std::atomic<ull> Allocated = 0;    ///< Total Allocated Memory
  std::atomic<ull> Current = 0;      ///< Current Allocated Memory
  std::atomic<ull> Deleted = 0;      ///< Total Deleted Memory
  std::atomic<ull> Allocations = 0;  ///< Current Allocations count
  /// @brief Gets the Currently Allocated Memory
  ull CurrentlyAllocated() { return Current; }
};
ull GetTotalAllocated();
ull GetTotalFreed();
ull GetCurrent();
ull GetAllocationCount();
}  // namespace Memory
}  // namespace Amy

using Iron = Amy::Iron;
using C3D = Amy::C3D;
using GTrace = Amy::GTrace;
