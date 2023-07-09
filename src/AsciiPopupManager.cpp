#include "./AsciiPopupManager.hpp"
#include "./AnmOpener/AnmManager.h"
#include "./AsciiManager.hpp"
#include "./Player.h"

AsciiPopupManager *POPUP_MANAGER_PTR = nullptr;

AsciiPopupManager::AsciiPopupManager() {
  POPUP_MANAGER_PTR = this;
  anm_ascii = ASCII_MANAGER_PTR->ascii_anm;
  on_tick = new UpdateFunc([this]() { return this->f_on_tick(); });
  on_tick->flags &= 0xfffffffd;
  UPDATE_FUNC_REGISTRY->register_on_tick(on_tick, 21);
  on_draw = new UpdateFunc([this]() { return this->f_on_draw(); });
  on_draw->flags &= 0xfffffffd;
  UPDATE_FUNC_REGISTRY->register_on_draw(on_draw, 48);
  vm.reset();
  vm.anm_loaded_index = 2;
  vm.bitflags.originMode = 2;
  vm.bitflags.resolutionMode = 3;

  // NO
  vm.layer = 28;
  vm.bitflags.activeFlags = 2;
}

AsciiPopupManager::~AsciiPopupManager() {
  if (on_tick)
    UPDATE_FUNC_REGISTRY->unregister(on_tick);
  if (on_draw)
    UPDATE_FUNC_REGISTRY->unregister(on_draw);
  POPUP_MANAGER_PTR = nullptr;
}

extern float GAME_SPEED;

int AsciiPopupManager::f_on_tick() {
  for (int i = 0; i < 0xd; i++) {
    if (popups[i].active) {
      popups[i].__field_18 *= 0.95;
      popups[i].pos.y -= popups[i].__field_18 * GAME_SPEED;
      popups[i].time++;
      if (popups[i].time > 60) {
        popups[i].active = 0;
      }
    }
  }

  for (int i = 0; i < 5; i++) {
    if (popups[13 + i].active) {
      popups[13 + i].time++;
      if (popups[13 + i].time > 60) {
        if (popups[13 + i].color.a - 4 < 1) {
          popups[13 + i].active = 0;
        } else {
          popups[13 + i].color.a -= 4;
        }
      }
    }
  }

  return 1;
}

int AsciiPopupManager::f_on_draw() {
  // if (!(SUPERVISOR.config._40_1_ & 4)) {
  //   SUPERVISOR.disable_d3d_fog();
  // }

  vm.bitflags.scaled = true;
  for (int i = 0; i < 13; i++) {
    if (!popups[i].active)
      continue;
    float size = 8;
    if (popups[i].time < 8) {
      if (popups[i].time == 0)
        size = 0;
      else
        size /= static_cast<float>(popups[i].time);
    }
    vm.entity_pos.x = popups[i].pos.x - popups[i].nb_nums * size * 0.5;
    vm.entity_pos.y = popups[i].pos.y;
    vm.scale = {size / 8, size / 8};
    vm.color_1 = popups[i].color;
    int dist_player_sq = (PLAYER_PTR->inner.pos.y - popups[i].pos.y) *
                             (PLAYER_PTR->inner.pos.y - popups[i].pos.y) +
                         (PLAYER_PTR->inner.pos.x - popups[i].pos.x) *
                             (PLAYER_PTR->inner.pos.x - popups[i].pos.x);
    if (dist_player_sq > 0x4000) {
      vm.color_1.a = 0xff;
    } else if (dist_player_sq <= 0x1000) {
      vm.color_1.a = 0x80;
    } else {
      vm.color_1.a = 0x80 * ((dist_player_sq - 0x1000) / 0x3000) + 0x80;
    }

    for (int num = popups[i].nb_nums - 1; num >= 0; num--) {
      // cur_char 10 is FullPower popup
      if (popups[i].time < 48 || popups[i].nums[num] == 10) {
        AnmManager::getLoaded(vm.anm_loaded_index)
            ->setSprite(&vm, popups[i].nums[num] + 0x103);
      } else if (popups[i].time < 56) {
        AnmManager::getLoaded(vm.anm_loaded_index)
            ->setSprite(&vm, popups[i].nums[num] + 0x10e);
      } else {
        AnmManager::getLoaded(vm.anm_loaded_index)
            ->setSprite(&vm, popups[i].nums[num] + 0x118);
      }
      // anmvm::write_sprite_corners__without_rot(
      //     &this->vm, (zfloat3 *)sprite_temp_buffer,
      //     (zfloat3 *)(sprite_temp_buffer + 1),
      //     (zfloat3 *)(sprite_temp_buffer + 2),
      //     (zfloat3 *)(sprite_temp_buffer + 3));
      // anmmanager::draw_vm__modes_0_1_2_3(ANM_MANAGER_PTR, &this->vm,
      // 1);
      vm.draw();
      vm.entity_pos.x += size;
    }
  }

  for (int i = 0; i < 5; i++) {
    if (!popups[13 + i].active)
      continue;
    ASCII_MANAGER_PTR->font_id = 2;
    ASCII_MANAGER_PTR->group = 2;
    ASCII_MANAGER_PTR->color = popups[13 + i].color;
    ASCII_MANAGER_PTR->alignment_mode_h = 0;
    ASCII_MANAGER_PTR->alignment_mode_v = 0;
    if (popups[13 + i].ddc_bonus < 0) {
      ASCII_MANAGER_PTR->create_string_f(
          popups[13 + i].pos + glm::vec3{224.0, 16.0, 0.0}, "NO BONUS");
    } else {
      ASCII_MANAGER_PTR->create_string_f(
          popups[13 + i].pos + glm::vec3{224.0, 16.0, 0.0}, "BONUS %.1f",
          popups[13 + i].ddc_bonus_mult);
      ASCII_MANAGER_PTR->create_string_f(popups[13 + i].pos +
                                             glm::vec3{224.0, 27.0, 0.0},
                                         "%d", popups[13 + i].ddc_bonus);
    }
    ASCII_MANAGER_PTR->color = {255, 255, 255, 255};
    ASCII_MANAGER_PTR->group = 0;
    ASCII_MANAGER_PTR->font_id = 0;
    ASCII_MANAGER_PTR->alignment_mode_h = 1;
    ASCII_MANAGER_PTR->alignment_mode_v = 1;
  }

  return 1;
}

void AsciiPopupManager::generate_small_score_popup(glm::vec3 const &pos, int nb,
                                                   NSEngine::Color color) {
  if (9 < next_index) {
    next_index = 0;
  }

  if (nb < 0) {
    popups[next_index].nums[0] = 10;
    popups[next_index].nb_nums = 1;
  } else {
    if (nb == 0) {
      popups[next_index].nums[0] = 0;
      popups[next_index].nb_nums = 1;
    } else {
      int n = 0;
      while (nb != 0) {
        popups[next_index].nums[n++] = nb % 10;
        nb /= 10;
      }
      popups[next_index].nb_nums = n;
    }
  }

  popups[next_index].active = true;
  popups[next_index].color = color;
  popups[next_index].time = 0;
  popups[next_index].pos = pos;
  popups[next_index].__field_18 = 1.f;
  next_index++;
  return;
}
