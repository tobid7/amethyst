#include <amethyst/assets.hpp>
#include <amethyst/c3d.hpp>
#include <amethyst/iron.hpp>
#include <amethyst/texture.hpp>
#include <filesystem>

namespace Amy {
void AssetMgr::AutoLoad(const ID& id, ksr path) {
  if (path.ends_with(".png") || path.ends_with(".jpg") ||
      path.ends_with(".bmp")) {
    auto tex = Texture::New();
    tex->Load(path);
    Add(id, tex);
  } else if (path.ends_with(".shbin")) {
    auto shader = C3D::Shader::New();
    shader->Load(path);
    Add(id, shader);
  } else if (path.ends_with(".ttf")) {
    auto font = Iron::Font::New();
    font->LoadTTF(path);
    Add(id, font);
  } else {
    throw std::runtime_error("[amy]: assets: " + id.GetName() + " (" + path +
                             ") is unsupported for AssetMgr::AutoLoad!");
  }
}

const std::string AssetMgr::TryFind(const std::string& path) {
  for (const auto& it : pPath) {
    std::string r = it + path;
    if (std::filesystem::exists(r)) {
      return r;
    }
  }
  throw std::runtime_error("Path '" + path + "' does not exist in $ENV{PATH}!");
  return "";
}
}  // namespace Amy