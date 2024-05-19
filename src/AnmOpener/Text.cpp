#include "./Text.hpp"
#include "AnmManager.h"
#include <GL/gl.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <codecvt>
#include <cstdio>
#include <locale>
#include <string>
#include <Error.h>

#include <cstdarg>
#include <cwchar>

#include <TextureManager.h>

#include <SDL2/SDL_ttf.h>

namespace anm::text {

static TTF_Font* fonts[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static SDL_Surface* fontRenderSurf = nullptr;

void surf_to_texture(SDL_Surface* surf,
                     ns::Texture* tex,
                     int tex_x,
                     int tex_y,
                     int tex_w,
                     int tex_h,
                     int ox = 0,
                     int oy = 0) {
  auto rgba = SDL_CreateRGBSurface(0, tex_w, tex_h, 32,
                  0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
  if (!surf || !tex || !rgba) return;

  int w = fmin(tex_w, surf->w);
  int h = fmin(tex_h, surf->h);
  SDL_Rect src {ox, oy, w, h};
  SDL_Rect dst {0, 0, w, h};
  SDL_BlitSurface(surf, &src, rgba, &dst);

  anm::flush_vbos();
  anm::reset_texture();
  tex->use();
  glTexSubImage2D(GL_TEXTURE_2D, 0, tex_x, tex_y, tex_w, tex_h,
                  GL_RGBA, GL_UNSIGNED_BYTE, rgba->pixels);
  tex->unuse();

  SDL_FreeSurface(rgba);
}

void render_text(int font_id, int x, int y,
                 std::string text, ns::Color col) {
  if (!fontRenderSurf) return;
  if (font_id < 0 || font_id > 9 || fonts[font_id] == 0) return;

  auto surf = TTF_RenderUTF8_Blended(fonts[font_id], text.c_str(),
                {col.r, col.g, col.b, col.a});
  if (!surf) {
    ns::error("failed to render text", text, "with font", font_id);
    return;
  }

  SDL_Rect rect = SDL_Rect{x, y, surf->w, surf->h};
  SDL_BlitSurface(surf, NULL, fontRenderSurf, &rect);

  SDL_FreeSurface(surf);
}

// Function to check if a byte is the start of a UTF-8 character
bool isUTF8StartByte(unsigned char byte) {
    return (byte & 0xC0) != 0x80;
}

// Function to decode a UTF-8 character
int decodeUTF8Character(const std::string& str, size_t& index) {
    int codePoint = 0;
    unsigned char firstByte = str[index];
    
    if ((firstByte & 0x80) == 0) {
        // Single-byte character (ASCII)
        codePoint = firstByte;
        index += 1;
    } else if ((firstByte & 0xE0) == 0xC0) {
        // Two-byte character
        codePoint = firstByte & 0x1F;
        index += 1;
        if (index < str.length() && isUTF8StartByte(str[index])) {
            codePoint = (codePoint << 6) | (str[index] & 0x3F);
            index += 1;
        }
    } else if ((firstByte & 0xF0) == 0xE0) {
        // Three-byte character
        codePoint = firstByte & 0x0F;
        index += 1;
        if (index < str.length() && isUTF8StartByte(str[index])) {
            codePoint = (codePoint << 6) | (str[index] & 0x3F);
            index += 1;
        }
        if (index < str.length() && isUTF8StartByte(str[index])) {
            codePoint = (codePoint << 6) | (str[index] & 0x3F);
            index += 1;
        }
    } else if ((firstByte & 0xF8) == 0xF0) {
        // Four-byte character
        codePoint = firstByte & 0x07;
        index += 1;
        if (index < str.length() && isUTF8StartByte(str[index])) {
            codePoint = (codePoint << 6) | (str[index] & 0x3F);
            index += 1;
        }
        if (index < str.length() && isUTF8StartByte(str[index])) {
            codePoint = (codePoint << 6) | (str[index] & 0x3F);
            index += 1;
        }
        if (index < str.length() && isUTF8StartByte(str[index])) {
            codePoint = (codePoint << 6) | (str[index] & 0x3F);
            index += 1;
        }
    } else {
        // Invalid UTF-8 character
        index += 1;
    }

    return codePoint;
}

void write_text_on_framebuffer([[maybe_unused]] glm::vec4 const& dest_rect,
                               [[maybe_unused]] int32_t xoffset,
                               [[maybe_unused]] int param_3,
                               [[maybe_unused]] ns::Color text_color,
                               [[maybe_unused]] ns::Color shadow_color,
                               [[maybe_unused]] std::string str,
                               [[maybe_unused]] ns::Texture* texture,
                               [[maybe_unused]] int font_id,
                               [[maybe_unused]] int chr_spacing,
                               [[maybe_unused]] int shadow) {
  Uint32 clearColor = SDL_MapRGBA(fontRenderSurf->format, 0, 0, 0, 0);
  SDL_FillRect(fontRenderSurf, NULL, clearColor);
  if (chr_spacing == 0) {
    if (shadow) {
      render_text(font_id, xoffset + 1, 0, str, shadow_color);
      render_text(font_id, xoffset + 2, 1, str, shadow_color);
      render_text(font_id, xoffset + 0, 1, str, shadow_color);
      render_text(font_id, xoffset + 2, 3, str, shadow_color);
      render_text(font_id, xoffset + 0, 3, str, shadow_color);
      render_text(font_id, xoffset + 2, 2, str, shadow_color);
      render_text(font_id, xoffset + 0, 2, str, shadow_color);
      render_text(font_id, xoffset + 1, 4, str, shadow_color);
    }
    render_text(font_id, xoffset + 1, 2, str, text_color);
  } else {
    int x = xoffset - 1;
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    // Convert the UTF-8 string to a wide string containing Unicode code points
    std::u32string utf32Str = converter.from_bytes(str);
    // Loop through each code point and convert it to a one-character UTF-8 string
    for (char32_t codePoint : utf32Str) {
        std::u32string codePointStr = {codePoint};
        std::string utf8CharStr = converter.to_bytes(codePointStr);
        if (shadow) {
          render_text(font_id, x + 2, 4, utf8CharStr, shadow_color);
          render_text(font_id, x + 0, 4, utf8CharStr, shadow_color);
          render_text(font_id, x + 2, 0, utf8CharStr, shadow_color);
          render_text(font_id, x + 0, 0, utf8CharStr, shadow_color);
        }
        render_text(font_id, x + 1, 2, utf8CharStr, text_color);
        x += chr_spacing;
    }
  }
  int oy = 0;
  if (font_id != 6 && font_id != 2) {
    oy = 6;
  }
  surf_to_texture(fontRenderSurf, texture, dest_rect.x, dest_rect.y,
                  dest_rect.z - dest_rect.x, dest_rect.w - dest_rect.y, 0, oy);
  return; /*
  int iVar1;
  char cVar2;
  int iVar3;
  HDC hdc;
  void *__src;
  int iVar5;
  int iVar6;
  int iVar7;
  HFONT h;
  ushort *puVar9;
  char *x;
  ushort *puVar10;
  IDirect3DSurface9 *local_3c;
  uint local_38;
  int local_34;
  ushort *local_30;
  ushort *local_2c;
  undefined4 local_28;
  RECT src_rect;
  uint local_14;
  
  h = fonts[font_id];
  local_34 = xoffset;
  _memset(IMAGE_BUFFER_FOR_FONT_4b66b0,0,IMAGE_BUFFER_FOR_FONT_SIZE_4b669c);
  hdc = hdc_004b66a4;
  local_3c = (IDirect3DSurface9 *)SelectObject(hdc_004b66a4,h);
  iVar7 = 17;
  if (16 < param_3) {
    iVar7 = param_3;
  }
  local_38 = iVar7 * 2 + 12;
  something_with_image_buffer_for_font_45f870(local_38);
  SetBkMode(hdc, 1);
  if (chr_spacing == 0) {
    if (shadow) {
      SetTextColor(hdc, shadow_color);
      TextOutA(hdc, xoffset + 2, 1, str, strlen(str));
      TextOutA(hdc, xoffset + 0, 1, str, strlen(str));
      TextOutA(hdc, xoffset + 2, 3, str, strlen(str));
      TextOutA(hdc, xoffset + 0, 3, str, strlen(str));
      TextOutA(hdc, xoffset + 2, 2, str, strlen(str));
      TextOutA(hdc, xoffset + 0, 2, str, strlen(str));
      TextOutA(hdc, xoffset + 1, 0, str, strlen(str));
      TextOutA(hdc, xoffset + 1, 4, str, strlen(str));
    }
    SetTextColor(hdc, text_color);
    TextOutA(hdc, xoffset + 1, 2, str, strlen(str));
  } else {
    puVar9 = strlen(str);
    local_28 = str;
    local_28._0_3_ = (uint3)(ushort)local_28;
    local_2c = (ushort *)xoffset;
    if (0 < (int)puVar9) {
      puVar9 = local_2c + -1;
      local_2c = (ushort *)(str + 1);
      local_34 = ((uint)((int)puVar9 - 1U) >> 1) + 1;
      do {
        local_28._0_2_ = CONCAT11(*(char *)local_2c,*(char *)((int)local_2c + -1));
        local_30 = puVar9;
        if (shadow) {
          SetTextColor(hdc,shadow_color);
          TextOutA(hdc,(int)(puVar9 + 2),4,(LPCSTR)&local_28,2);
          TextOutA(hdc,(int)local_30,4,(LPCSTR)&local_28,2);
          TextOutA(hdc,(int)(puVar9 + 2),0,(LPCSTR)&local_28,2);
          puVar9 = local_30;
          TextOutA(hdc,(int)local_30,0,(LPCSTR)&local_28,2);
        }
        SetTextColor(hdc,text_color);
        TextOutA(hdc,(int)(puVar9 + 1),2,(LPCSTR)&local_28,2);
        puVar9 = (ushort *)((int)puVar9 + chr_spacing);
        local_2c = local_2c + 1;
        local_34 += -1;
        local_30 = puVar9;
      } while (local_34 != 0);
    }
  }
  SelectObject(hdc,local_3c);
  something_with_image_buffer_for_font_45f870(local_38);
  __src = IMAGE_BUFFER_FOR_FONT_4b66b0;
  if (IMAGE_BUFFER_FOR_FONT_FORMAT_4b6690 == 0x1a) {
    local_30 = malloc(local_38 * DAT_004b6694 * 2 + 1);
    memcpy(local_30, __src, local_38 * DAT_004b6694 * 2);
    iVar3 = DAT_004b6694;
    local_28 = (wchar_t *)-DAT_004b6694;
    iVar5 = 1 - DAT_004b6694;
    iVar6 = -1 - DAT_004b6694;
    iVar7 = DAT_004b6694 + 1;
    iVar1 = DAT_004b6694 + -1;
    local_2c = (ushort *)(DAT_004b6694 * 2 + (int)__src);
    puVar10 = local_30 + DAT_004b6694;
    local_34 = 0;
    puVar9 = local_2c;
    if (0 < (int)((local_38 - 2) * DAT_004b6694)) {
      do {
        if ((*puVar10 < 0x1000) && (local_34 % DAT_004b6694 != 0)) {
          *puVar9 = (ushort)(((uint)(puVar10[iVar1] >> 0xc) +
                              ((uint)(puVar10[-iVar3] >> 0xc) + (uint)(puVar10[iVar3] >> 0xc) +
                               (uint)(puVar10[-1] >> 0xc) + (uint)(puVar10[1] >> 0xc)) * 2 +
                              (uint)(puVar10[iVar7] >> 0xc) + (uint)(puVar10[iVar6] >> 0xc) +
                             (uint)(puVar10[iVar5] >> 0xc)) / 0xe << 0xc) | *puVar9 & 0xfff;
        }
        local_34 += 1;
        puVar10 = puVar10 + 1;
        puVar9 = puVar9 + 1;
      } while (local_34 < (int)((local_38 - 2) * DAT_004b6694));
    }
    if (local_30 != NULL) {
      _free(local_30);
    }
  }
  smthing_w_image_buffer_for_font_format_45f4e0(local_38);
  SelectObject(hdc,local_3c);
  auto src = { 0, 0, dst.right - dst.left, dst.bottom - dst.top };
  if (0x400 < src.right) src.right = 0x400;
  // depending on the font, shift downwards the src rectangle
  // if (((_HAS_MEIRIO_FONT) && (font_id != 6)) && (font_id != 2)) {
  //   src.bottom += 6;
  //   src.top = 6;
  // }
  auto fb;
  texture->GetSurfaceLevel(0, &fb);
  D3DXLoadSurfaceFromMemory(
    fb,
    NULL,
    &dst,
    IMAGE_BUFFER_FOR_FONT_4b66b0,
    IMAGE_BUFFER_FOR_FONT_FORMAT_4b6690,
    IMAGE_BUFFER_FOR_FONT_PITCH_4b66a0,
    NULL,
    &src,
    1,
    0);
  if (fb) fb.Release(); */
}

static const int DAT_004a11b8[] = {
    0x11, 0x11, 0x11, 0x11, 0x15, 0x15, 0x15, 0x15, 0xE, 0xC, 0, 0, 0, 0
};

void FUN_00475000(VM *vm,
                  ns::Color text_color,
                  ns::Color shadow_color,
                  int font_id,
                  int h_x_offset,
                  int h_sep_w,
                  std::string const& string) {
    int iVar7 = DAT_004a11b8[font_id];
    auto spr = vm->getSprite();
    glm::vec4 rect = {spr.x, spr.y, spr.x + spr.w, spr.y + spr.h};
    if (iVar7 < 1) {
      iVar7 = 17;
    } else if (iVar7 < 9) {
      vm->bitflags.visible = true;
      return;
    }
    if (vm->bitflags.f534_12) shadow_color = {0, 0, 0, 0};
    write_text_on_framebuffer(
        rect,
        h_x_offset * 2,
        iVar7,
        text_color,
        shadow_color,
        string,
        ns::TextureManager::GetTexture(spr.texID),
        font_id,
        h_sep_w * 2,
        !vm->bitflags.f534_12);
    vm->bitflags.visible = true;
}

void FUN_00475120(VM *vm,
                  ns::Color text_color,
                  ns::Color shadow_color,
                  int font_id,
                  int h_chr_spacing,
                  std::string const& string,
                  size_t str_siz) {
    int iVar7 = DAT_004a11b8[font_id];
    auto spr = vm->getSprite();
    int iVar5 = iVar7 * 2;
    if (iVar7 < 1) {
      iVar7 = 0x11;
    } else if (iVar7 < 9) {
      vm->bitflags.visible = true;
      return;
    }
    if (vm->bitflags.f534_12) shadow_color = c_black;
    int xoff = spr.w - iVar5 * (((str_siz + 1) / 2) - 1);
    write_text_on_framebuffer({spr.x, spr.y, spr.x + spr.w, spr.y + spr.h},
                              xoff,
                              iVar7,
                              text_color,
                              shadow_color,
                              string,
                              ns::TextureManager::GetTexture(spr.texID),
                              font_id,
                              h_chr_spacing * 2,
                              !vm->bitflags.f534_12);
    vm->bitflags.visible = true;
}

void init() {
    if (TTF_Init() < 0) {
        ns::error("Couldn't initialize TTF:", SDL_GetError());
        return;
    }
    std::string gothic = "assets/fonts/togoshi-gothic.ttf";
    std::string mincho = "assets/fonts/togoshi-mincho.ttf";
    fonts[0] = TTF_OpenFont(gothic.c_str(), 0x20);
    TTF_SetFontStyle(fonts[0], TTF_STYLE_NORMAL);
    fonts[1] = TTF_OpenFont(mincho.c_str(), 0x20);
    TTF_SetFontStyle(fonts[1], TTF_STYLE_BOLD);
    fonts[2] = TTF_OpenFont(gothic.c_str(), 0xf);
    TTF_SetFontStyle(fonts[2], TTF_STYLE_BOLD);
    fonts[3] = TTF_OpenFont(mincho.c_str(), 0xf);
    TTF_SetFontStyle(fonts[3], TTF_STYLE_BOLD);
    fonts[4] = TTF_OpenFont(gothic.c_str(), 0x28);
    TTF_SetFontStyle(fonts[4], TTF_STYLE_NORMAL);
    fonts[5] = TTF_OpenFont(mincho.c_str(), 0x28);
    TTF_SetFontStyle(fonts[5], TTF_STYLE_BOLD);
    fonts[6] = TTF_OpenFont(gothic.c_str(), 0xf);
    TTF_SetFontStyle(fonts[6], TTF_STYLE_BOLD);
    fonts[7] = TTF_OpenFont(mincho.c_str(), 0xf);
    TTF_SetFontStyle(fonts[7], TTF_STYLE_BOLD);
    fonts[8] = TTF_OpenFont(gothic.c_str(), 0x1c);
    TTF_SetFontStyle(fonts[8], TTF_STYLE_NORMAL);
    fonts[9] = TTF_OpenFont(gothic.c_str(), 0x18);
    TTF_SetFontStyle(fonts[9], TTF_STYLE_NORMAL);
    fontRenderSurf = SDL_CreateRGBSurface(0, 1024, 512, 32,
                          0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
}

void cleanup() {
    if (!TTF_WasInit()) return;
    if (fonts[0]) TTF_CloseFont(fonts[0]);
    if (fonts[1]) TTF_CloseFont(fonts[1]);
    if (fonts[2]) TTF_CloseFont(fonts[2]);
    if (fonts[3]) TTF_CloseFont(fonts[3]);
    if (fonts[4]) TTF_CloseFont(fonts[4]);
    if (fonts[5]) TTF_CloseFont(fonts[5]);
    if (fonts[6]) TTF_CloseFont(fonts[6]);
    if (fonts[7]) TTF_CloseFont(fonts[7]);
    if (fonts[8]) TTF_CloseFont(fonts[8]);
    if (fonts[9]) TTF_CloseFont(fonts[9]);
    TTF_Quit();
    SDL_FreeSurface(fontRenderSurf);
}

} // namespace anm::text
