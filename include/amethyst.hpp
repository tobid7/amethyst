#pragma once

#include <amethyst/app.hpp>
#include <amethyst/assets.hpp>
#include <amethyst/c3d.hpp>
#include <amethyst/color.hpp>
#include <amethyst/image.hpp>
#include <amethyst/iron.hpp>
#include <amethyst/renderer.hpp>
#include <amethyst/texture.hpp>

namespace Amy {
void RegisterCxxExceptionHandler();
}

using Iron = Amy::Iron;
using C3D = Amy::C3D;