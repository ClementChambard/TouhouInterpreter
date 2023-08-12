#include "./AsciiManager.hpp"
#include "./AnmOpener/AnmManager.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>

AsciiManager *ASCII_MANAGER_PTR = nullptr;

// Should refer to a variable inside SUPERVISOR
static const int resolution = (RESOLUTION_MULT * 2 - 2);

AsciiManager::AsciiManager() {
  ASCII_MANAGER_PTR = this;
  const char *anms[] = {"ascii.anm", "ascii_960.anm", "ascii_1280.anm"};
  ascii_anm = AnmManager::LoadFile(2, anms[resolution]);
  if (!ascii_anm) {
    std::cerr << "データが壊れています\n";
  }

  on_tick = new UpdateFunc([this]() { return this->f_on_tick(); });
  on_tick->flags &= 0xfffffffd;
  UPDATE_FUNC_REGISTRY->register_on_tick(on_tick, 5);

  on_draw = new UpdateFunc([this]() { return this->render_group(0); });
  on_draw->flags &= 0xfffffffd;
  UPDATE_FUNC_REGISTRY->register_on_draw(on_draw, 82);

  on_draw_2 = new UpdateFunc([this]() {
    // SUPERVISOR.set_camera_by_index(0);
    this->__vm_1.bitflags.originMode = 2;
    this->render_group(1);
    this->__vm_1.bitflags.originMode = 0;
    return 1;
  });
  on_draw_2->flags &= 0xfffffffd;
  UPDATE_FUNC_REGISTRY->register_on_draw(on_draw_2, 54);

  on_draw_3 = new UpdateFunc([this]() {
    // SUPERVISOR.set_camera_by_index(0);
    this->render_group(2);
    return 1;
  });
  on_draw_3->flags &= 0xfffffffd;
  UPDATE_FUNC_REGISTRY->register_on_draw(on_draw_3, 67);

  __vm_1.reset();
  __vm_1.anm_loaded_index = 2;
  ascii_anm->setSprite(&__vm_1, 0);

  __vm_2.reset();
  __vm_2.anm_loaded_index = 2;
  ascii_anm->setSprite(&__vm_2, 0x62);
}

AsciiManager::~AsciiManager() {
  if (on_tick)
    UPDATE_FUNC_REGISTRY->unregister(on_tick);
  if (on_draw)
    UPDATE_FUNC_REGISTRY->unregister(on_draw);
  if (on_draw_2)
    UPDATE_FUNC_REGISTRY->unregister(on_draw_2);
  if (on_draw_3)
    UPDATE_FUNC_REGISTRY->unregister(on_draw_3);
  AnmManager::getLoaded(2)->Cleanup();
  AnmManager::getLoaded(0)->Cleanup();
  ASCII_MANAGER_PTR = NULL;
}

int AsciiManager::render_group(int gid) {
  for (int i = 0; i < this->num_strings; i++) {
    if (strings[i].render_group == gid)
      render_string(strings[i]);
  }
  // SUPERVISOR.set_camera_by_index(2);
  return 1;
}

int AsciiManager::f_on_tick() {
  int newI = 0;
  for (int i = 0; i < num_strings; i++) {
    strings[i].remaining_time--;
    if (-1 < strings[i].remaining_time) {
      if (newI != i) {
        strings[newI] = strings[i];
      }
      newI++;
    }
  }
  num_ticks_alive++;
  num_strings = newI;
  return 1;
}

#define CHRP2INT(c) static_cast<int>(*reinterpret_cast<const unsigned char*>(c))
void AsciiManager::render_string(AsciiStr_t const &str) {
  __vm_1.bitflags.resolutionMode = 0;
  __vm_1.bitflags.visible = true;
  __vm_1.bitflags.anchorX = 1;
  __vm_1.bitflags.anchorY = 1;
  __vm_1.pos = str.pos;
  __vm_1.scale = str.scale;
  __vm_1.bitflags.scaled = true;
  __vm_1.color_1 = str.color;

  // TODO: NO
  if (str.render_group == 0) __vm_1.layer = 42;
  if (str.render_group == 1) __vm_1.layer = 23;
  if (str.render_group == 2) __vm_1.layer = 29;

  float font_width = 0.0;
  float font_height = 0.0;
  switch (str.font_id) {
  case 1:
    __vm_1.bitflags.resampleMode = 0;
    font_width = str.scale.x * 6.0;
    font_height = 9.0;
    break;
  case 2:
    __vm_1.bitflags.resampleMode = 0;
    font_height = 10.0;
    font_width = str.scale.x * 7.0;
    break;
  case 3:
    __vm_1.bitflags.resampleMode = 1;
    font_height = 10.0;
    font_width = str.scale.x * 7.0;
    break;
  case 4:
    __vm_1.bitflags.resampleMode = 0;
    font_height = 16.0;
    font_width = str.scale.x * 12.0;
    break;
  case 5:
    __vm_1.bitflags.resampleMode = 1;
    font_height = 16.0;
    font_width = str.scale.x * 12.0;
    break;
  default:
    __vm_1.bitflags.resampleMode = str.scale.x != 1.0;
    font_height = 14.0;
    font_width = character_spacing_for_font_0 * str.scale.x;
  }

  const char *c;
  if (str.align_h == 0 || str.align_h == 2) {
    float factor = (str.align_h == 0 ? 0.5 : 1.0);
    if (str.font_id >= 2 && font_id <= 5) {
      c = str.text;
      while (*c != 0) {
        if ((*c == '.' && str.font_id < 4) || (str.font_id > 3 && *c == ',')) {
          __vm_1.pos.x -= str.scale.x * 4.0 * factor * RESOLUTION_MULT;
        } else {
          __vm_1.pos.x -= font_width * factor * RESOLUTION_MULT;
        }
        c++;
      }
    } else {
      __vm_1.pos.x -= strlen(str.text) * font_width * factor * RESOLUTION_MULT;
    }
  }
  if (str.align_v == 0) {
    __vm_1.pos.y -= font_height / 2.f * RESOLUTION_MULT;
  }
  if (str.align_v == 2) {
    __vm_1.pos.y -= RESOLUTION_MULT * font_height;
  }

  float next_char_pos = font_width;
  c = str.text;
  while (true) {
    if (*c == 0) {
      return;
    }
    if (*c == '\n') {
      __vm_1.pos.y += str.scale.y * font_height * RESOLUTION_MULT;
      __vm_1.pos.x = str.pos.x;
      c++;
      continue;
    }
    if (*c == ' ') {
      __vm_1.pos.x += RESOLUTION_MULT * next_char_pos;
      c++;
      continue;
    }
    switch (str.font_id) {
    case 0:
      AnmManager::getLoaded(__vm_1.anm_loaded_index)
          ->setSprite(&__vm_1, CHRP2INT(c) - 0x20);
      break;
    case 1:
      AnmManager::getLoaded(__vm_1.anm_loaded_index)
          ->setSprite(&__vm_1, CHRP2INT(c) + 0x42);
      break;
    case 2:
    case 3:
      next_char_pos = str.scale.x * 7.0;
      if (*c == '/') {
        AnmManager::getLoaded(__vm_1.anm_loaded_index)
            ->setSprite(&__vm_1, 0xce);
      } else if (*c == ':') {
        AnmManager::getLoaded(__vm_1.anm_loaded_index)
            ->setSprite(&__vm_1, 0xcf);
      } else if (*c == '-') {
        AnmManager::getLoaded(__vm_1.anm_loaded_index)
            ->setSprite(&__vm_1, 0xd0);
      } else if (*c == '*') {
        AnmManager::getLoaded(__vm_1.anm_loaded_index)
            ->setSprite(&__vm_1, 0xd1);
      } else if (*c == '%') {
        AnmManager::getLoaded(__vm_1.anm_loaded_index)
            ->setSprite(&__vm_1, 0xd2);
      } else if (*c == '$') {
        // "Failed" sprite
        AnmManager::getLoaded(__vm_1.anm_loaded_index)
            ->setSprite(&__vm_1, 0x101);
      } else if (*c == '.') {
        AnmManager::getLoaded(__vm_1.anm_loaded_index)
            ->setSprite(&__vm_1, 0xd3);
        next_char_pos = str.scale.x * 4.0;
      } else if (*c == '+') {
        AnmManager::getLoaded(__vm_1.anm_loaded_index)
            ->setSprite(&__vm_1, 0xd4);
      } else if (*c >= 'a' && *c <= 'z') {
        AnmManager::getLoaded(__vm_1.anm_loaded_index)
            ->setSprite(&__vm_1, CHRP2INT(c) + 0x74);
      } else {
        // Only numbers
        AnmManager::getLoaded(__vm_1.anm_loaded_index)
            ->setSprite(&__vm_1, CHRP2INT(c) + 0x94);
      }
      break;
    case 4:
    case 5:
      __vm_1.pos.y = str.pos.y;
      next_char_pos = str.scale.x * 12.0;
      if (*c == '/') {
        AnmManager::getLoaded(__vm_1.anm_loaded_index)
            ->setSprite(&__vm_1, 0xf9);
      } else if (*c == '.') {
        AnmManager::getLoaded(__vm_1.anm_loaded_index)
            ->setSprite(&__vm_1, 0xfa);
      } else if (*c == 's') {
        AnmManager::getLoaded(__vm_1.anm_loaded_index)
            ->setSprite(&__vm_1, 0xfb);
      } else if (*c == '*') {
        AnmManager::getLoaded(__vm_1.anm_loaded_index)
            ->setSprite(&__vm_1, 0xfc);
      } else if (*c == ',') {
        AnmManager::getLoaded(__vm_1.anm_loaded_index)
            ->setSprite(&__vm_1, 0xfd);
        next_char_pos = str.scale.x * 4.0;
        __vm_1.pos.y = RESOLUTION_MULT * 3.0 + str.pos.y;
      } else {
        // Only numbers
        AnmManager::getLoaded(__vm_1.anm_loaded_index)
            ->setSprite(&__vm_1, CHRP2INT(c) + 0xbf);
      }
      break;
    }

    if (str.draw_shadows) {
      __vm_1.color_1 = {0, 0, 0, (uint8_t)(__vm_1.color_1.a / 2)};
      __vm_1.bitflags.resampleMode = str.scale.x != 1.0;
      __vm_1.pos.x += RESOLUTION_MULT * 2.f;
      __vm_1.pos.y += RESOLUTION_MULT * 2.f;

      // AnmVm::write_sprite_corners__without_rot(
      //     &this->__vm_1, (zFloat3 *)SPRITE_TEMP_BUFFER,
      //     (zFloat3 *)(SPRITE_TEMP_BUFFER + 1),
      //     (zFloat3 *)(SPRITE_TEMP_BUFFER + 2),
      //     (zFloat3 *)(SPRITE_TEMP_BUFFER + 3));
      // ANM_MANAGER_PTR->draw_vm__modes_0_1_2_3(&this->__vm_1, 1);
      __vm_1.draw();

      __vm_1.pos.x -= RESOLUTION_MULT * 2.f;
      __vm_1.pos.y -= RESOLUTION_MULT * 2.f;
      __vm_1.color_1 = str.color;
    }

    // AnmVm::write_sprite_corners__without_rot(
    //     &this->__vm_1, (zFloat3 *)SPRITE_TEMP_BUFFER,
    //     (zFloat3 *)(SPRITE_TEMP_BUFFER + 1),
    //     (zFloat3 *)(SPRITE_TEMP_BUFFER + 2),
    //     (zFloat3 *)(SPRITE_TEMP_BUFFER + 3));
    // ANM_MANAGER_PTR->draw_vm__modes_0_1_2_3(&this->__vm_1, 1);
    __vm_1.draw();
    __vm_1.pos.x += RESOLUTION_MULT * next_char_pos;
    c++;
  }
}
#undef CHRP2INT

void AsciiManager::create_string(glm::vec3 const &pos, const char* str) {
  if (num_strings >= 320)
    return;
  memcpy(strings[num_strings].text, str, 256);
  strings[num_strings].pos = pos * RESOLUTION_MULT;
  strings[num_strings].render_group = group;
  strings[num_strings].color = color;
  strings[num_strings].scale = scale;
  strings[num_strings].__field_11c = __str_field_11c;
  strings[num_strings].font_id = font_id;
  strings[num_strings].draw_shadows = draw_shadows;
  strings[num_strings].remaining_time = duration;
  strings[num_strings].align_h = alignment_mode_h;
  strings[num_strings].align_v = alignment_mode_v;
  num_strings++;
}

void AsciiManager::create_string_f(const glm::vec3 &pos, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char buf[256] = {};
  vsnprintf(buf, sizeof(buf), fmt, args);
  create_string(pos, buf);
}

void AsciiManager::comma_separated_number(glm::vec3 const &pos, int number) {
  char buffer[260] = {};

  if (number < 1000) {
    snprintf(buffer, sizeof(buffer), "%d", number);
  } else if (number < 1000000) {
    snprintf(buffer, sizeof(buffer), "%d,%.3d", number / 1000, number % 1000);
  } else if (number < 1000000000) {
    snprintf(buffer, sizeof(buffer), "%d,%.3d,%.3d", number / 1000000,
             (number / 1000) % 1000, number % 1000);
  } else {
    snprintf(buffer, sizeof(buffer), "%d,%.3d,%.3d,%.3d",
             (number / 1000000000) % 1000, (number / 1000000) % 1000,
             (number / 1000) % 1000, number % 1000);
  }
  create_string(pos, buffer);
}

void AsciiManager::draw_score(glm::vec3 const &pos, int num, int units) {
  char buf[260];

  if (num < 100) {
    snprintf(buf, sizeof(buf), "%d", units + num * 10);
  } else if (num < 100000) {
    snprintf(buf, sizeof(buf), "%d,%.2d%d", num / 100, num % 100, units);
  } else if (num < 100000000) {
    snprintf(buf, sizeof(buf), "%d,%.3d,%.2d%d", num / 100000,
             (num / 100) % 1000, num % 100, units);
  } else {
    snprintf(buf, sizeof(buf), "%d,%.3d,%.3d,%.2d%d", (num / 100000000) % 1000,
             (num / 100000) % 1000, (num / 100) % 1000, num % 100, units);
  }
  create_string(pos, buf);
}
