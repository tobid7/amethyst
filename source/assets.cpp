#include <amethyst/assets.hpp>
#include <amethyst/c3d.hpp>
#include <amethyst/texture.hpp>

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
  } else {
    throw std::runtime_error("[amy]: assets: " + id.GetName() + " (" + path +
                             ") is unsupported for AssetMgr::AutoLoad!");
  }
}
}  // namespace Amy