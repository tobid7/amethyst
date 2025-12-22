#include <amethyst/iron.hpp>
#include <amethyst/utils.hpp>
#include <filesystem>
#include <fstream>

#if AMY_STB_TT == 1
#define STB_TRUETYPE_IMPLEMENTATION
#endif
#include <stb_truetype.h>

namespace Amy {
// Dont read this code please ... tnaks
void Iron::Font::LoadBMF(ksr path) {
  Image img(path);
  img.Convert(Image::A8);
  if (img.Width() != img.Height() || img.Bpp() != 1 || img.Width() <= 0 ||
      img.Height() <= 0) {
    throw std::runtime_error(
        "[Amy] Font: BMF is not in rgba or not 1x1 dimensioned!");
  }
  auto base = Amy::Texture::New();
  base->Load(img.GetBuffer(), img.Width(), img.Height(), img.Bpp(), img.Fmt());
  base->Unloadable(false);
  PxHeight = img.Height() / 16;
  for (int i = 0; i < img.Height(); i += PxHeight) {
    for (int j = 0; j < img.Width(); j += PxHeight) {
      int maxw = 0;
      Amy::Texture::Ref tex = Amy::Texture::New();
      for (int y = i; y < i + PxHeight; y++) {
        for (int x = j; x < j + PxHeight; x++) {
          if (img.GetBuffer()[y * img.Width() + x] != 0) {
            maxw = std::max(maxw, x - j);
          }
        }
      }
      Codepoint cp;
      cp.Valid = maxw != 0;
      cp.Cp = (i / PxHeight) * 16 + (j / PxHeight);
      cp.Offset = 0.f;
      if (cp.Valid) {
        tex->Load(base->Ptr(), base->Size(), base->Uv());
        cp.Size = fvec2(maxw + 1, PxHeight);
        cp.Tex = tex;
        cp.Uv = fvec4(float(j) / float(img.Width()),
                      1.f - float(i) / float(img.Height()),
                      float(j + maxw + 1) / float(img.Width()),
                      1.f - float(i + PxHeight) / float(img.Height()));
        Textures.push_back(tex);
      }
      pCodeMap[(i / PxHeight) * 16 + (j / PxHeight)] = cp;
    }
  }
}

void Iron::Font::LoadTTF(ksr path, int size) {
  auto data = Amy::Utils::LoadFile2Mem(path);
  LoadTTF(data, size);
}

void Iron::Font::LoadTTF(const vec<uc>& data, int size) {
  /**
   * Some additional Info:
   * Removed the stbtt get bitmapbox as we dont need to place
   * the glyps nicely in the tex. next step would be using the free
   * space on the y axis to get mor glyphs inside
   */
  PxHeight = size;
  int texszs = Amy::Utils::NextPow2(PxHeight * 16);
  if (texszs > 1024) {
    texszs = 1024;  // Max size
  }

  stbtt_fontinfo inf;
  if (!stbtt_InitFont(&inf, data.data(), 0)) {
    return;
  }

  float scale = stbtt_ScaleForPixelHeight(&inf, PxHeight);

  int ascent, descent, lineGap;
  stbtt_GetFontVMetrics(&inf, &ascent, &descent, &lineGap);
  int baseline = static_cast<int>(ascent * scale);

  // Cache to not render same codepoint tex twice
  std::map<u32, int> buf_cache;

  std::vector<u8> font_tex(texszs * texszs, 0);
  auto tex = Texture::New();
  fvec2 off;

  bool empty = true;

  for (u32 ii = 0x0000; ii <= 0xFFFF; ii++) {
    int gi = stbtt_FindGlyphIndex(&inf, ii);
    if (gi == 0) continue;
    if (stbtt_IsGlyphEmpty(&inf, gi)) continue;

    int w = 0, h = 0, xo = 0, yo = 0;
    unsigned char* bitmap =
        stbtt_GetCodepointBitmap(&inf, scale, scale, ii, &w, &h, &xo, &yo);
    if (!bitmap || w <= 0 || h <= 0) {
      if (bitmap) free(bitmap);
      continue;
    }

    u32 hashed_map =
        Utils::HashMemory(std::vector<u8>(bitmap, bitmap + (w * h)));
    if (buf_cache.find(hashed_map) != buf_cache.end()) {
      Codepoint c = GetCodepoint(buf_cache[hashed_map]);
      c.Cp = ii;
      pCodeMap[ii] = c;
      free(bitmap);
      continue;
    } else {
      buf_cache[hashed_map] = ii;
    }

    // Next row
    if (off.x + w > texszs) {
      off.y += PxHeight;
      off.x = 0.0f;
    }
    // Bake cause we go out of the tex
    if (off.y + PxHeight > texszs) {
      pMakeAtlas(false, font_tex, texszs, tex);
      tex = Texture::New();
      off = 0;
      std::fill(font_tex.begin(), font_tex.end(), 0);
      empty = true;
    }

    // UVs & Codepoint
    Codepoint c;
    fvec4 uvs;
    // cast the ints to floats and not the floats...
    // dont know where my mind was when creating the code
    uvs.x = off.x / static_cast<float>(texszs);
    uvs.y = off.y / static_cast<float>(texszs);
    uvs.z = (off.x + w) / static_cast<float>(texszs);
    uvs.w = (off.y + h) / static_cast<float>(texszs);
    // Flip (too lazy to place it into the seq at top)
    uvs.y = 1.f - uvs.y;
    uvs.w = 1.f - uvs.w;
    c.Uv = uvs;
    c.Tex = tex;
    c.Size = fvec2(w, h);
    c.Offset = baseline + yo;
    c.Cp = ii;

    for (int y = 0; y < h; ++y) {
      for (int x = 0; x < w; ++x) {
        int map_pos = ((static_cast<int>(off.y) + y) * texszs +
                       (static_cast<int>(off.x) + x));
        font_tex[map_pos] = bitmap[x + y * w];
      }
    }

    empty = false;
    pCodeMap[ii] = c;
    free(bitmap);

    // offset by 1 (prevents visual glitches i had)
    off.x += w + 1;
  }

  if (!empty) {
    pMakeAtlas(true, font_tex, texszs, tex);
  }
}

void Iron::Font::pMakeAtlas(bool final, vec<uc>& font_tex, int texszs,
                            Texture::Ref tex) {
  tex->Load(font_tex, texszs, texszs, 1, Amy::Image::A8);
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

fvec2 Iron::Font::GetTextBounds(ksr text, float scale) {
  // Use wstring for exemple for german äöü
  auto wtext = std::filesystem::path(text).wstring();
  // Create a temp position and offset as [0, 0]
  fvec2 res;
  float x = 0;
  // Curent Font Scale
  float cfs = (PxFactor * scale) / (float)PxHeight;
  float lh = (float)PxHeight * cfs;
  size_t index = 0;
  for (auto& it : wtext) {
    if (it == L'\0') {
      break;
    }
    index++;
    auto cp = GetCodepoint(it);
    if (!cp.Valid && it != '\n' && it != '\t' && it != ' ') {
      continue;
    }
    switch (it) {
      case L'\n':
        res.y += lh;
        res.x = std::max(res.x, x);
        x = 0.f;
        break;
      case L'\t':
        x += 16 * cfs;
        break;
      case L' ':
        x += 4 * cfs;
      // Fall trough here to get the same result as in
      // TextCommand if/else Section
      default:
        x += cp.Size.x * cfs;
        if (index != wtext.size()) {
          x += 2 * cfs;
        }
        break;
    }
  }
  res.x = std::max(res.x, x);
  res.y += lh;
  return res;
}

void Iron::Font::CmdTextEx(vec<Command::Ref>& cmds, const fvec2& pos, ui color,
                           float scale, const std::string& text,
                           AmyTextFlags flags, const fvec2& box) {
  fvec2 off;
  float cfs = (PxFactor * scale) / (float)PxHeight;
  float lh = (float)PxHeight * cfs;
  fvec2 td;
  fvec2 rpos = pos;
  fvec2 rbox = box;
  if (flags & (AmyTextFlags_AlignMid | AmyTextFlags_AlignRight)) {
    td = GetTextBounds(text, scale);
  }
  if (flags & AmyTextFlags_AlignMid) {
    rpos = rbox * 0.5 - td * 0.5 + pos;
  }
  if (flags & AmyTextFlags_AlignRight) {
    rpos.x = rpos.x - td.x;
  }

  std::vector<std::string> lines;
  std::istringstream iss(text);
  std::string tmp;
  while (std::getline(iss, tmp)) {
    lines.push_back(tmp);
  }

  for (auto& it : lines) {
    /*if (flags & AmyTextFlags_Short) {
      fvec2 tmp_dim;
      it = ShortText(it, box.x() - pos.x(), tmp_dim);
    }*/
    // Bitte nicht nachmachen... Also ernsthaft jz, bitte macht das nicht
    auto wline = std::filesystem::path(it).wstring();
    auto cmd = Command::New();
    auto Tex = GetCodepoint(wline[0]).Tex;
    cmd->Tex = Tex;
    for (auto& jt : wline) {
      auto cp = GetCodepoint(jt);
      if ((!cp.Valid && jt != L' ' && jt != L'\n' && jt != L'\t') &&
          jt != L'\r') {
        continue;
      }
      if (Tex != cp.Tex) {
        cmds.push_back(std::move(cmd));
        cmd = Command::New();
        Tex = cp.Tex;
        cmd->Tex = Tex;
      }
      if (jt == L'\t') {
        off.x += 16 * cfs;
      } else {
        if (jt != L' ') {
          if (flags & AmyTextFlags_Shaddow) {
            // Draw
            Rect rec = Iron::PrimRect(
                rpos + vec2(off.x + 1, off.y + (cp.Offset * cfs)) + 1,
                cp.Size * cfs, 0.f);
            Iron::CmdQuad(cmd.get(), rec, cp.Uv, 0xff111111);
          }
          // Draw
          Rect rec = Iron::PrimRect(rpos + off + fvec2(0, (cp.Offset * cfs)),
                                    cp.Size * cfs, 0.f);
          Iron::CmdQuad(cmd.get(), rec, cp.Uv, color);
          off.x += cp.Size.x * cfs + 2 * cfs;
        } else {
          off.x += 4 * cfs;
        }
      }
    }
    cmds.push_back(std::move(cmd));
    off.y += lh;
    off.x = 0;
  }
}
}  // namespace Amy