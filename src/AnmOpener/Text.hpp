#ifndef TEXT_INCLUDED_H
#define TEXT_INCLUDED_H

#include <string>
#include "AnmVM.h"
#include <color.h>

void FUN_00475000(
  AnmVM *vm,
  NSEngine::Color color,
  NSEngine::Color shadow_color,
  int font_id,
  int h_x_offset,
  int h_sep_w,
  std::string const& string
);

void FUN_00475120(
  AnmVM *vm,
  NSEngine::Color text_color,
  NSEngine::Color shadow_color,
  int font_id,
  int h_chr_spacing,
  std::string const& string,
  size_t str_siz
);

struct TextRenderer {
    static void init();
    static void cleanup();
};

#endif // !TEXT_INCLUDED_H
