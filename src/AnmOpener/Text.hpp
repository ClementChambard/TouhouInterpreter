#ifndef TEXT_INCLUDED_H
#define TEXT_INCLUDED_H

#include <string>
#include "AnmVM.h"
#include <color.h>

namespace anm::text {

void FUN_00475000(
  VM *vm,
  ns::Color color,
  ns::Color shadow_color,
  int font_id,
  int h_x_offset,
  int h_sep_w,
  std::string const& string
);

void FUN_00475120(
  VM *vm,
  ns::Color text_color,
  ns::Color shadow_color,
  int font_id,
  int h_chr_spacing,
  std::string const& string,
  size_t str_siz
);

void init();
void cleanup();

} // namespace anm::text

#endif // !TEXT_INCLUDED_H
