#include <amethyst/iron.hpp>

namespace Amy {
void Iron::Font::LoadBMF(ksr path) {}

void Iron::Font::LoadTTF(ksr path, int size) {}

void Iron::Font::pMakeAtlas(bool final, vec<uc>& font_tex, int texszs,
                            Texture* tex) {
  tex->Load(font_tex, texszs, texszs);
  Textures.push_back(tex);
}

Iron::Font::Codepoint& Iron::Font::GetCodepoint(ui cp) {
  // Check if codepoijt exist or return a static invalid one
  auto res = pCodeMap.find(cp);
  if (res == pCodeMap.end()) {
    static Codepoint invalid;
    invalid.Valid = false;
    return invalid;
  }
  return res->second;
}
}  // namespace Amy