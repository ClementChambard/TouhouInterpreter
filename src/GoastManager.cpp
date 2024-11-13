#include "./GoastManager.h"
#include "./Anm/AnmManager.h"
#include "./Ecl/EnemyManager.h"
#include "./ItemManager.h"
#include "./Spellcard.h"
#include "./Player.h"
#include "./GlobalData.h"
#include "Bomb.hpp"
#include "Bullet/BulletManager.h"
#include "Gui.hpp"
#include "Laser/LaserManager.h"
#include <math/math.hpp>
#include <input.hpp>
#include <NSEngine.hpp>

GoastManager* GOAST_MANAGER_PTR = nullptr;

GoastManager::GoastManager() {
    flags |= 2;
    GOAST_MANAGER_PTR = this;
    anm_file = anm::loadFile(30, "beast.anm");
    // if (!anm_file) debug message & exit

    on_tick = new UpdateFunc([this]() -> int { return this->_on_tick(); });
    on_tick->flags &= 0xfffffffd;
    UPDATE_FUNC_REGISTRY.register_on_tick(on_tick, 31);

    on_draw = new UpdateFunc([]() -> int { return 1; });
    on_draw->flags = (on_draw->flags & 0xfffffffdU) | 1;
    UPDATE_FUNC_REGISTRY.register_on_draw(on_draw, 34);

    // if (GAME_THREAD_PTR->field47_0xa8 == 0) {
    //     GLOBALS.inner.HYPER_FILL = 0;
    //     GLOBALS.inner.TOKENS[0] = 0;
    //     GLOBALS.inner.TOKENS[1] = 0;
    //     GLOBALS.inner.TOKENS[2] = 0;
    //     GLOBALS.inner.TOKENS[3] = 0;
    //     GLOBALS.inner.TOKENS[4] = 0;
    //     GLOBALS.inner.__unknown_timer = 0;
    //     GLOBALS.inner.HYPER_TIME = 0;
    //     GLOBALS.inner.MAX_HYPER_TIME = 0;
    //     GLOBALS.inner.HYPER_TYPE = 0;
    //     GLOBALS.inner.HYPER_REFILL = 0;
    //     GLOBALS.inner.HYPER_FLAGS = 0;
    // }
    reset();
}

GoastManager::~GoastManager() {
  if (on_tick) UPDATE_FUNC_REGISTRY.unregister(on_tick);
  if (on_draw) UPDATE_FUNC_REGISTRY.unregister(on_draw);
  delete_tokens();
  anm::getLoaded(30)->Cleanup();
  GOAST_MANAGER_PTR = nullptr;
}

void GoastManager::delete_tokens() {
    auto node = list_head.next;
    while (node) {
        auto ptr = node->entry;
        node = ptr->node.next;
        anm::delete_vm(ptr->anm_id);
        delete ptr;
        token_count--;
    }
    list_head.entry = nullptr;
    list_head.next = nullptr;
    list_head.prev = nullptr;
    GLOBALS.inner.HYPER_FLAGS |= 0x200;
}

int GoastManager::_on_tick() {
    auto node = list_head.next;
    while (node) {
        auto ptr = node->entry;
        node = ptr->node.next;
        if (ptr->update()) {
            if (ptr->node.next) ptr->node.next->prev = ptr->node.prev;
            if (ptr->node.prev) ptr->node.prev->next = ptr->node.next;
            ptr->node.next = nullptr;
            ptr->node.prev = nullptr;
            anm::delete_vm(ptr->anm_id);
            delete ptr;
            token_count--;
        }
    }

    if (!(GLOBALS.inner.HYPER_FLAGS & 1U)) {
        if (PLAYER_PTR && 400.0 < PLAYER_PTR->inner.pos.y &&
            PLAYER_PTR->inner.pos.x < -64.0) {
            anm::interrupt_tree(gui_anmid, 3);
            GLOBALS.inner.HYPER_FLAGS |= 1;
        }
    } else if (PLAYER_PTR && (PLAYER_PTR->inner.pos.y < 384.0 ||
               -64.0 < PLAYER_PTR->inner.pos.x)) {
        anm::interrupt_tree(gui_anmid, 2);
        GLOBALS.inner.HYPER_FLAGS &= 0xfffffffe;
    }

    if (extra_beast_anmid) FUN_0040e920();

    if (GLOBALS.inner.HYPER_FLAGS & 2U) FUN_0040ee70();

    return 1;
}

int GoastManager::FUN_0040e920() {
  float fVar8;
  float local_20;
  float local_1c;
  float local_18;
  float local_14;

  if (auto vm = anm::get_vm(extra_beast_anmid); vm) {
    vm->entity_pos = 2.f * PLAYER_PTR->inner.pos;
  }
  GLOBALS.inner.__unknown_timer++;
  if (GLOBALS.inner.HYPER_FLAGS & 0x200U) {
    GLOBALS.inner.__unknown_timer--;
  }
  if (GLOBALS.inner.__unknown_timer < 120) {
    return 0;
  }
  anm::delete_vm(extra_beast_anmid);
  extra_beast_anmid = 0;
  if (PLAYER_PTR->inner.pos.y < 160.0) {
    if (PLAYER_PTR->inner.pos.x < -80.f) {
      fVar8 = 0.5235988;
      local_14 = 0.9075712;
      local_18 = 1.047198;
      local_1c = 0.6632251;
      local_20 = 0.7853982;
    } else if (PLAYER_PTR->inner.pos.x > 80.f) {
      fVar8 = 2.617994;
      local_14 = 2.234022;
      local_18 = 2.094395;
      local_1c = 2.478368;
      local_20 = 2.356194;
    } else {
      fVar8 = 0.5235988;
      local_14 = 2.007129;
      local_18 = 2.617994;
      local_1c = 0.8726647;
      local_20 = 2.356194;
    }
  } else {
    if (PLAYER_PTR->inner.pos.x < -80) {
      fVar8 = -0.5235988;
      local_14 = -0.9075712;
      local_18 = -1.047198;
      local_1c = -0.6632251;
      local_20 = -0.7853982;
    } else if (PLAYER_PTR->inner.pos.x > 80.0) {
      fVar8 = 3.665192;
      local_14 = 4.049164;
      local_18 = 4.18879;
      local_1c = 3.804818;
      local_20 = 3.926991;
    } else {
      fVar8 = -2.617994;
      local_14 = -1.134464;
      local_18 = -0.5235988;
      local_1c = -2.268928;
      local_20 = -0.7853982;
    }
  }
  // SoundManager::play_sound_at_position(0x3e);
  ns::vec3 pos = PLAYER_PTR->inner.pos - ns::vec3(0, 64, 0);
  int flagthing = ((GLOBALS.inner.HYPER_FLAGS >> 4) & 0xf);
  if (flagthing == 0) {
    spawn_token(pos, rand() % 3 + 0xf, fVar8);
    spawn_token(pos, rand() % 3 + 0xf, local_18);
  } else if (flagthing == 2) {
    spawn_token(pos, rand() % 3 + 0xf, fVar8);
    spawn_token(pos, rand() % 3 + 0xf, local_18);
    pos.y -= 32.0;
    spawn_token(pos, 5, local_1c);
    spawn_token(pos, 4, local_20);
    spawn_token(pos, rand() % 3 + 0xf, local_14);
  } else if (flagthing == 3) {
    spawn_token(pos, 6, fVar8);
    spawn_token(pos, 7, local_18);
    pos.y -= 32.0;
    spawn_token(pos, 5, local_1c);
    spawn_token(pos, 4, local_20);
    spawn_token(pos, 4, local_14);
  } else {
    spawn_token(pos, rand() % 3 + 0xf, fVar8);
    spawn_token(pos, rand() % 3 + 0xf, local_18);
    pos.y -= 32.0;
    spawn_token(pos, 5, local_1c);
    spawn_token(pos, rand() % 3 + 0xf, local_20);
    spawn_token(pos, rand() % 3 + 0xf, local_14);
  }
  return 1;
}

int Token_t::update() {
    if (flags & 1) {
        speed = (PLAYER_PTR->inner.pos - pos) * 0.25f;
        pos += speed * ns::get_instance()->game_speed();
        if (__timer_30 >= 10) {
            anm::delete_vm(anm_id);
            anm_id = 0;
            return 1;
        } else {
            if (auto vm = anm::get_vm(anm_id); vm) {
                vm->entity_pos = pos;
            }
            __timer_30++;
            return 0;
        }
    }
    if ((pos.y - PLAYER_PTR->inner.pos.y) * (pos.y - PLAYER_PTR->inner.pos.y) +
        (pos.x - PLAYER_PTR->inner.pos.x) * (pos.x - PLAYER_PTR->inner.pos.x)
            < 576.0) {
        // SoundManager::play_sound_at_position(0x3f);
        GOAST_MANAGER_PTR->get_token(token_type);
        flags |= 1;
        anm::interrupt_tree(anm_id, 1);
        __timer_30 = 0;
    }

    float d2_to_pl = (PLAYER_PTR->inner.pos.x - pos.x) *
        (PLAYER_PTR->inner.pos.x - pos.x) + (PLAYER_PTR->inner.pos.y - pos.y) *
        (PLAYER_PTR->inner.pos.y - pos.y);

    if (flags & 2U) {
        if (d2_to_pl <= 3600.0) {
            if (!(flags & 4U)) anm::interrupt_tree(anm_id, 8);
            if (__timer_58 < 0x3c) __timer_58 = 0x5a;
            flags |= 4;
        } else {
            __timer_58--;
            if (flags & 4) {
                if (__timer_58 < 0x3d) {
                    anm::interrupt_tree(anm_id, 2);
                    flags &= 0xfffffffb;
                } else {
                    anm::interrupt_tree(anm_id, 3);
                    flags &= 0xfffffffb;
                }
            } else {
                flags &= 0xfffffffb;
            }
        }

        if (__timer_58.had_value(0x3c)) {
            anm::interrupt_tree(anm_id, 2);
        } else if (__timer_58 < 1) {
            __timer_58 = 0xb4;
            token_type = (token_type % 3) + 1;
            anm::delete_vm(anm_id);
            anm_id = GOAST_MANAGER_PTR->anm_file->createEffect(token_type - 1);
            if (0x1e77 < __timer_30) anm::interrupt_tree(anm_id, 9);
            // SoundManager::play_sound_at_position(0x4e);
        }
    }
    if (ns::abs(pos.x) > 208.0 || ns::abs(pos.y - 224.0) > 240.0) {
        __timer_44++;
        if (0x77 < __timer_44 || 0x81 < __timer_30) {
            anm::delete_vm(anm_id);
            anm_id = 0;
            return 1;
        }
    } else {
        __timer_44 = 0;
    }
    pos += ((d2_to_pl <= 3600.0) ? 0.5f : 1.0f) * speed * ns::get_instance()->game_speed();
    if (__timer_30.had_value(0x1e78)) anm::interrupt_tree(anm_id, 9);
    if (__timer_30 < 0x20d0) {
        bool bounced = false;
        if (pos.x <= -180.0 && speed.x < 0) { speed.x *= -1; bounced = true; }
        if (pos.x >= 180.0 && speed.x > 0) { speed.x *= -1; bounced = true; }
        if (pos.y <= 128.0 && speed.y < 0) { speed.y *= -1; bounced = true; }
        if (pos.y >= 384.0 && speed.y > 0) { speed.y *= -1; bounced = true; }
        if (bounced) {
            if (ns::abs(speed.x) < 0.2) speed.x = (speed.x <= 0) ? ns::frand()
                * -0.3 - 0.5 : ns::frand() * 0.3 + 0.5;
            if (ns::abs(speed.y) < 0.2) speed.y = (speed.y <= 0) ? ns::frand()
                * -0.3 - 0.5 : ns::frand() * 0.3 + 0.5;
            GOAST_MANAGER_PTR->choose_angle(this, ns::point_direction(0, 0,
                                                            speed.x, speed.y));
        }
    }
    if (auto vm = anm::get_vm(anm_id); vm) {
        vm->entity_pos = pos;
    }
    __timer_30++;
    return 0;
}

void GoastManager::choose_angle(Token_t* token, float angle) {
    token->speed = {ns::lengthdir_vec(1, angle), 0};

    for (auto node = list_head.next; node; node = node->entry->node.next) {
        auto other = node->entry;
        if (other != token && ns::point_distance_sq(other->pos, token->pos)
            < 576.0 && ns::dot(other->speed, token->speed) > 0.96) {
            angle += 3.883222;
            token->speed = {ns::lengthdir_vec(1, angle), 0};
        }
    }
}

int GoastManager::spawn_token(ns::vec3 const& pos, int type, float param_4) {
    if (token_count > 0x27) return -1;
    auto token = new Token_t();
    token->anm_id = 0;
    token->node.entry = token;
    token->node.next = list_head.next;
    token->node.prev = &list_head;
    if (list_head.next) list_head.next->prev = &token->node;
    list_head.next = &token->node;
    token->__timer_30 = 0;
    token->__timer_44 = 0;
    if (type - 15 < 3 && type - 15 >= 0) {
        // token->anm_id = anm_file->create_effect(
        // nullptr,type - 15,-1,nullptr);
        token->anm_id = anm_file->createEffect(type - 15);
        token->token_type = type - 14;
        token->flags = (token->flags & 0xfffffffb) | 2;
        token->__timer_58 = 0xf0;
    } else {
        // token->anm_id = anm_file->create_effect(nullptr,type - 1,-1,nullptr);
        token->anm_id = anm_file->createEffect(type - 1);
        if (type - 1 < 3) anm::interrupt_tree(token->anm_id, 7);
        token->token_type = type;
    }
    token->pos = pos;
    if (auto vm = anm::get_vm(token->anm_id); vm) {
        vm->entity_pos = pos;
    }
    choose_angle(token, param_4);
    token_count++;
    if (type < 15 && type > 8) {
        // if (GAME_THREAD_PTR->field47_0xa8 == 0) FUN_0045d8a0(type + 0x19);
        // SoundManager::play_sound_at_position(0x4a);
        return 0;
    }
    // SoundManager::play_sound_at_position(0x3b);
    return 0;
}

void GoastManager::FUN_0040ee70() {
  if ((!GUI_PTR->msg && !(GLOBALS.inner.HYPER_FLAGS & 0x200U)) ||
     (GLOBALS.inner.HYPER_FLAGS & 4U)) {
    GLOBALS.inner.HYPER_TIME--;
    GLOBALS.inner.HYPER_FLAGS &= 0xfffffbff;
    for (int i = 0; i < 0x100; i++) {
      if (PLAYER_PTR->inner.bullets[i].active)
        goto LAB_0040ef69;
    }
    if (field_0x50) {
LAB_0040ef69:
      GLOBALS.inner.HYPER_TIME--;
      GLOBALS.inner.HYPER_FLAGS |= 0x400;
    }
    if ((GLOBALS.inner.HYPER_FLAGS & 0x400U) != 0) {
      auto vm = anm::get_vm(hyper_time_bar_anmid2);
      if (!vm) {
        hyper_time_bar_anmid2 = 0;
        // vm = &ANM_MANAGER_PTR->__anm_vm_dc;
      } else {
        vm->update();
      }
    }
    if (GLOBALS.inner.HYPER_FLAGS & 0x100U) {
      if (auto vm = anm::get_vm(hyper_bg_anmid); vm) {
        vm->set_flag_1_rec();
        // vm->clear_flag_20_rec();
      }
      GLOBALS.inner.HYPER_FLAGS &= 0xfffffeff;
      if (GLOBALS.inner.HYPER_TYPE == 2) {
        create_otter_hyper_anms();
      }
    }
  } else {
    if (!(GLOBALS.inner.HYPER_FLAGS & 0x100U) &&
         (GLOBALS.inner.HYPER_TYPE == 2)) {
      for (int i = 0; i < 3; i++) {
        anm::interrupt_tree(otter_hyper_anms[i], 1);
        otter_hyper_anms[i] = 0;
      }
    }
    if (auto vm = anm::get_vm(hyper_bg_anmid); vm) {
      vm->clear_flag_1_rec();
      // vm->set_flag_20_rec();
    }
    GLOBALS.inner.HYPER_FLAGS |= 0x100;
  }
  if (field_0x50) {
    field_0x50--;
  }
  if (!(GLOBALS.inner.HYPER_FLAGS & 4U)) {
    if (auto vm = anm::get_vm(hyper_time_bar_anmid1); vm) {
      vm->entity_pos = 2.f * PLAYER_PTR->inner.pos;
    }
    if (auto vm = anm::get_vm(hyper_time_bar_anmid2); vm) {
      vm->entity_pos = 2.f * PLAYER_PTR->inner.pos;
    }
    if (auto vm = anm::get_vm(hyper_bg_anmid); vm) {
      vm->entity_pos = PLAYER_PTR->inner.pos;
    }
    if (auto vm = anm::get_vm(hyper_aura_anmid); vm) {
      vm->entity_pos = PLAYER_PTR->inner.pos;
    }
    if ((GLOBALS.inner.HYPER_FLAGS & 0x100U) == 0) {
      if (GLOBALS.inner.HYPER_TYPE == 2) {
        for (int i = 0; i < 3; i++) {
          auto vm = anm::get_vm(otter_hyper_anms[i]);
          if (!vm) {
            otter_hyper_anms[i] = 0;
          } else {
            vm->entity_pos = PLAYER_PTR->inner.pos;
            auto pos = PLAYER_PTR->inner.pos + ns::lengthdir_vec3(96, vm->rotation.z - 0.3490658);
            BULLET_MANAGER_PTR->cancel_radius_as_bomb(pos, 1, 28.0);
            LASER_MANAGER_PTR->cancel_in_radius(pos, 1, 1, 28.0);
            auto ds = create_damage_source_3(pos,2,5,28.0,0.0);
            PLAYER_PTR->inner.damage_sources[ds + -1].ds_on_hit = 4;
          }
        }
      }
      if (ns::keyboard::pressed(ns::Key::X)) {
        hyper_die(false);
      }
    }
  }
  if (GLOBALS.inner.HYPER_TIME < 1) {
    if ((GLOBALS.inner.HYPER_FLAGS & 4U) == 0) {
      if (extra_beast_anmid) {
        anm::delete_vm(extra_beast_anmid);
      }
      extra_beast_anmid =
        anm_file->createEffectPos(0x2c, 0.0, 2.f * PLAYER_PTR->inner.pos);
      GLOBALS.inner.__unknown_timer = 0;
    }
    if (GLOBALS.inner.HYPER_TYPE == 2) {
      for (int i = 0; i < 3; i++) {
        anm::interrupt_tree(otter_hyper_anms[i], 1);
        otter_hyper_anms[i] = 0;
      }
    }
    // SoundManager::play_sound_centered(0x42);
    anm::interrupt_tree(hyper_bg_anmid, 1);
    hyper_bg_anmid = 0;
    hyper_end_spawn_items();
    GLOBALS.inner.HYPER_FLAGS &= 0xfffffff9;
    anm::interrupt_tree(hyper_time_bar_anmid1, 1);
    anm::interrupt_tree(hyper_time_bar_anmid2, 1);
    hyper_time_bar_anmid1 = 0;
    hyper_time_bar_anmid2 = 0;
    field_0x34 = 0;
    anm::interrupt_tree(hyper_aura_anmid, 1);
    hyper_aura_anmid = 0;
    if (GLOBALS.inner.HYPER_TYPE == 3) {
      PLAYER_PTR->flags |= 0x40;
    }
    GLOBALS.inner.HYPER_FILL = 0;
    for (int i = 0; i < 5; i++) {
      GLOBALS.inner.TOKENS[i] = 0;
      update_gui(i);
    }
    for (int i = 0; i < GLOBALS.inner.HYPER_FILL2; i++) {
      get_token(GLOBALS.inner.TOKENS2[i]);
      GLOBALS.inner.TOKENS2[i] = 0;
      update_gui(i);
    }
    GLOBALS.inner.HYPER_FILL2 = 0;
    return;
  }
  if (!(GLOBALS.inner.HYPER_FLAGS & 4U)) {
    update_hyper_bar();
    return;
  }
  if (!field_0x40) return;
  PlayerDamageSource_t* ds = &PLAYER_PTR->inner.damage_sources[field_0x40 - 1];
  if (!ds) return;
  float fVar18 = 1.0 - (1.0 - GLOBALS.inner.HYPER_TIME.current_f / 40.0);
  fVar18 = (1.0 - fVar18 * fVar18 * fVar18 * fVar18) * 200.0;
  ds->field_0x4 = fVar18;
  BULLET_MANAGER_PTR->cancel_radius_as_bomb(ds->pos.pos,
            ~((uint)GLOBALS.inner.HYPER_FLAGS >> 3) & 1,
                fVar18);
  LASER_MANAGER_PTR->cancel_in_radius(ds->pos.pos,
            (~(GLOBALS.inner.HYPER_FLAGS >> 3) & 1),
            1, fVar18);
  return;
}


void GoastManager::create_otter_hyper_anms() {
    for (int i = 0; i < 3; i++) {
        anm::delete_vm(otter_hyper_anms[i]);
        otter_hyper_anms[i] =
            anm_file->createEffectPos(0x25 + i, 0, PLAYER_PTR->inner.pos);
    }
}


void GoastManager::hyper_die(bool actually_die) {
    field_0x34 = anm_file->createEffectPos(0x21, 0.0, PLAYER_PTR->inner.pos);
    // SoundManager::play_sound_centered(0x2c);
    anm::interrupt_tree(hyper_bg_anmid, 2);
    anm::interrupt_tree(hyper_time_bar_anmid1, 1);
    anm::interrupt_tree(hyper_time_bar_anmid2, 1);
    anm::interrupt_tree(hyper_aura_anmid, 1);
    hyper_time_bar_anmid1 = 0;
    hyper_time_bar_anmid2 = 0;
    hyper_aura_anmid = 0;
    if (GLOBALS.inner.HYPER_TYPE == 2) {
        for (int i = 0; i < 3; i++) {
            anm::delete_vm(otter_hyper_anms[i]);
            otter_hyper_anms[i] = 0;
        }
    }
    GLOBALS.inner.HYPER_FLAGS =
        ((((actually_die) << 3 ^ GLOBALS.inner.HYPER_FLAGS) &
            8U) ^ GLOBALS.inner.HYPER_FLAGS) | 4;
    GLOBALS.inner.HYPER_TIME = 40;
    field_0x40 = create_damage_source_3(PLAYER_PTR->inner.pos, 0x28, 5, 8.0, 0.0);
    if (GLOBALS.inner.HYPER_TYPE == 3) {
        PLAYER_PTR->flags |= 0x40;
    }
    if ((!(GLOBALS.inner.HYPER_FLAGS & 0x800U) && !actually_die) &&
       (SPELLCARD_PTR->flags & 1)) {
        if (SPELLCARD_PTR->__timer_20 < 0x3c) {
            if (BOMB_PTR->active == 1) {
                SPELLCARD_PTR->flags |= 0x20;
            }
        } else {
            SPELLCARD_PTR->bonus = 0;
            SPELLCARD_PTR->flags &= 0xffffffdd;
        }
    }
    PLAYER_PTR->inner.iframes = 60;
    ENEMY_MANAGER_PTR->bomb_count++;
    ENEMY_MANAGER_PTR->can_still_capture_spell = 0;
}



void GoastManager::update_gui(int tid) {
  anm::VM* child1 = nullptr, *child2 = nullptr;
  anm::VM* vm = anm::get_vm(gui_anmid);
  if (!vm) {
    gui_anmid = 0;
  } else {
    child1 = vm->search_children(tid + 0xe, 0);
    child2 = vm->search_children(tid + 0x18, 0);
  }

  if (child1) {
    if (tid < GLOBALS.inner.HYPER_FILL) {
      child1->interrupt(4);
      child1->update();
      child1->interrupt(6 + GLOBALS.inner.TOKENS[tid]);
      child1->update();
      if (child2) {
        child2->interrupt(4);
        child2->update();
      }
      return;
    } else {
      child1->interrupt(5);
      child1->update();
      if (child2) {
        child2->interrupt(5);
        child2->update();
      }
    }
  }
  return;
}


void GoastManager::hyper_end_spawn_items(void) {
  ns::vec3 pos = PLAYER_PTR->inner.pos;
  pos.y -= 80.0;
  float posy_2 = pos.y;
  for (int i = 0; i < 5; i++) {
    switch (GLOBALS.inner.TOKENS[i]) {
    case 4:
      ITEM_MANAGER_PTR->spawn_item(6, pos, -ns::PI_1_2<f32>, 2.2, 10, -1);
      pos.y -= 30.0;
      break;
    case 5:
      ITEM_MANAGER_PTR->spawn_item(4, pos, -ns::PI_1_2<f32>, 2.2, 10, -1);
      pos.y -= 30.0;
      break;
    case 6:
    case 7:
    {
      int t = GLOBALS.inner.TOKENS[i] - 5;
      float r = 0.0;
      for (int i = 10; i < 70; i += 2) {
        float a = ns::frandangle();
        ns::vec3 p = {r*ns::cos(a), r*ns::sin(a)/2, 0};
        p.x += pos.x;
        p.y += posy_2;
        ITEM_MANAGER_PTR->spawn_item(t, p, -ns::PI_1_2<f32>, 2.2, i, 0x3e);
        r += 2.0;
      }
      posy_2 -= 30.0;
    } break;
    }
  }
}

void GoastManager::hyper_spawn_item(int i, ns::vec3 const& pos) {
    switch (i) {
    case 4:
      ITEM_MANAGER_PTR->spawn_item(6, pos, -ns::PI_1_2<f32>, 2.2, 10, -1);
      break;
    case 5:
      ITEM_MANAGER_PTR->spawn_item(4, pos, -ns::PI_1_2<f32>, 2.2, 10, -1);
      break;
    case 6:
    case 7:
    {
      int t = i - 5;
      float r = 0.0;
      for (int i = 10; i < 70; i += 2) {
        float a = ns::frandangle();
        ns::vec3 p = {r*ns::cos(a), r*ns::sin(a)/2, 0};
        ITEM_MANAGER_PTR->spawn_item(t, pos + p, -ns::PI_1_2<f32>, 2.2, i, 0x3e);
        r += 2.0;
      }
    } break;
    }
}

int GoastManager::get_token(int typ) {
  if (GLOBALS.inner.HYPER_FILL < 5) {
    GLOBALS.inner.TOKENS[GLOBALS.inner.HYPER_FILL] = typ;
    GLOBALS.inner.HYPER_FILL += 1;
    update_gui(GLOBALS.inner.HYPER_FILL - 1);
    if (4 < GLOBALS.inner.HYPER_FILL) {
      hyper_start();
      return GLOBALS.inner.HYPER_FILL;
    }
  } else if (!(GLOBALS.inner.HYPER_FLAGS & 4)) {
    if (typ - 4U < 4) {
      ns::vec3 p = PLAYER_PTR->inner.pos;
      p.y -= 80.0;
      hyper_spawn_item(typ, p);
    }
    GLOBALS.inner.HYPER_TIME += GLOBALS.inner.HYPER_REFILL;
    if (0x1e < GLOBALS.inner.HYPER_REFILL) {
      GLOBALS.inner.HYPER_REFILL *= 2 / 3;
    }
    if (GLOBALS.inner.MAX_HYPER_TIME < GLOBALS.inner.HYPER_TIME) {
      GLOBALS.inner.HYPER_TIME = GLOBALS.inner.MAX_HYPER_TIME;
      return GLOBALS.inner.HYPER_FILL;
    }
  } else if (GLOBALS.inner.HYPER_FILL2 < 5) {
    GLOBALS.inner.TOKENS2[GLOBALS.inner.HYPER_FILL2] = typ;
    GLOBALS.inner.HYPER_FILL2 += 1;
    return GLOBALS.inner.HYPER_FILL;
  }
  return GLOBALS.inner.HYPER_FILL;
}


int GoastManager::hyper_start() {
  static const int hyper_times[] = {0x348, 0x474, 0x5DC};
  int tok_typ = 0;
  int tok_cnt = 0;
  for (; tok_typ < 3; tok_typ++) {
    tok_cnt = 0;
    if (GLOBALS.inner.TOKENS[0] == tok_typ + 1) tok_cnt++;
    if (GLOBALS.inner.TOKENS[1] == tok_typ + 1) tok_cnt++;
    if (GLOBALS.inner.TOKENS[2] == tok_typ + 1) tok_cnt++;
    if (GLOBALS.inner.TOKENS[3] == tok_typ + 1) tok_cnt++;
    if (GLOBALS.inner.TOKENS[4] == tok_typ + 1) tok_cnt++;
    if (tok_cnt > 2) break;
  }

  if (tok_cnt < 3) {
    GLOBALS.inner.HYPER_TYPE = 4;
    GLOBALS.inner.MAX_HYPER_TIME = 0x474;
  } else {
    GLOBALS.inner.HYPER_TYPE = tok_typ + 1;
    GLOBALS.inner.MAX_HYPER_TIME = hyper_times[tok_cnt-3];
    if (tok_typ == GLOBALS.inner.SHOTTYPE) {
      GLOBALS.inner.MAX_HYPER_TIME += 0x1e0;
    }
  }
  GLOBALS.inner.HYPER_TIME = GLOBALS.inner.MAX_HYPER_TIME;
  anm::interrupt_tree_run(gui_anmid, tok_typ + 0x1c);
  GLOBALS.inner.HYPER_FLAGS |= 2;
  GLOBALS.inner.HYPER_REFILL = 0xb4;

  anm::VM* vm;
  anm::delete_vm(hyper_time_bar_anmid1);
  hyper_time_bar_anmid1 = anm_file->createEffect(0x1e, -1, &vm);
  vm->bitflags.scaled = true;
  vm->sprite_size.x = GLOBALS.inner.MAX_HYPER_TIME * 0.125 + 2.0;
  anm::delete_vm(hyper_time_bar_anmid2);
  hyper_time_bar_anmid2 = anm_file->createEffect(0x1f, -1, &vm);
  vm->bitflags.scaled = true;
  vm->sprite_size.x = GLOBALS.inner.HYPER_TIME.current_f * 0.125;
  if (GLOBALS.inner.HYPER_TYPE == 2) {
    create_otter_hyper_anms();
  } else {
    PLAYER_PTR->flags |= 0x40;
  }
  if (GLOBALS.inner.HYPER_TYPE != 4) {
    hyper_aura_anmid = anm_file->createEffectPos(
        GLOBALS.inner.HYPER_TYPE + 0x28, 0, PLAYER_PTR->inner.pos);
  }
  anm::delete_vm(hyper_bg_anmid);
  hyper_bg_anmid = anm_file->createEffectPos(0x20, 0.0, PLAYER_PTR->inner.pos);
  // SoundManager::play_sound_centered(0x4d);
  GLOBALS.inner.HYPER_FLAGS &= 0xffffff0f;
  if ((7 < GLOBALS.inner.TOKENS[0]) && (GLOBALS.inner.TOKENS[0] < 0xf)) {
    GLOBALS.inner.HYPER_FLAGS =
         (((GLOBALS.inner.TOKENS[0] - 7) & 0xf) << 4)
         | GLOBALS.inner.HYPER_FLAGS;
  }
  if ((7 < GLOBALS.inner.TOKENS[1]) && (GLOBALS.inner.TOKENS[1] < 0xf)) {
    GLOBALS.inner.HYPER_FLAGS =
         ((((GLOBALS.inner.TOKENS[1] - 7) * 0x10) ^
            GLOBALS.inner.HYPER_FLAGS) & 0xf0U) ^
         GLOBALS.inner.HYPER_FLAGS;
  }
  if ((7 < GLOBALS.inner.TOKENS[2]) && (GLOBALS.inner.TOKENS[2] < 0xf)) {
    GLOBALS.inner.HYPER_FLAGS ^=
         ((GLOBALS.inner.TOKENS[2] - 7) * 0x10 ^
            GLOBALS.inner.HYPER_FLAGS) & 0xf0U;
  }
  if ((7 < GLOBALS.inner.TOKENS[3]) && (GLOBALS.inner.TOKENS[3] < 0xf)) {
    GLOBALS.inner.HYPER_FLAGS ^=
         ((GLOBALS.inner.TOKENS[3] - 7) * 0x10 ^
            GLOBALS.inner.HYPER_FLAGS) & 0xf0U;
  }
  GLOBALS.inner.HYPER_FILL2 = 0;
  if ((7 < GLOBALS.inner.TOKENS[4]) && (GLOBALS.inner.TOKENS[4] < 0xf)) {
    GLOBALS.inner.HYPER_FLAGS ^=
         ((GLOBALS.inner.TOKENS[4] - 7) * 0x10 ^
            GLOBALS.inner.HYPER_FLAGS) & 0xf0U;
  }
  return 0;
}


void GoastManager::update_hyper_bar() {
  anm::VM* vm;
  vm = anm::get_vm(hyper_time_bar_anmid1);
  if (!vm) {
    hyper_time_bar_anmid1 = 0;
  } else {
    vm->bitflags.scaled = true;
    vm->sprite_size.x = GLOBALS.inner.MAX_HYPER_TIME * 0.125 + 2.0;
  }
  vm = anm::get_vm(hyper_time_bar_anmid2);
  if (!vm) {
    hyper_time_bar_anmid2 = 0;
  } else {
    vm->bitflags.scaled = true;
    vm->sprite_size.x = GLOBALS.inner.HYPER_TIME.current_f * 0.125;
  }
}


void GoastManager::reset() {
  on_tick->flags |= 2;
  on_draw->flags |= 2;
  if (gui_anmid.val == 0) {
    gui_anmid = anm_file->createEffect(0x1d);
    anm::interrupt_tree_run(gui_anmid, 0x22);
    for (int i = 0; i < 5; i++) {
      update_gui(i);
    }
  }
  auto uVar3 = GLOBALS.inner.HYPER_FLAGS & 2;
  GLOBALS.inner.HYPER_FLAGS &= 0xfffffdfe;
  if (uVar3 != 0) {
    anm::delete_vm(hyper_time_bar_anmid1);
    hyper_time_bar_anmid1 = anm_file->createEffectPos(
        0x1e, 0, PLAYER_PTR->inner.pos);
    anm::delete_vm(hyper_time_bar_anmid2);
    hyper_time_bar_anmid2 = anm_file->createEffectPos(
        0x1f, 0, PLAYER_PTR->inner.pos);
    update_hyper_bar();
    anm::delete_vm(hyper_aura_anmid);
    if (GLOBALS.inner.HYPER_TYPE != 4) {
      hyper_aura_anmid = anm_file->createEffectPos(
        GLOBALS.inner.HYPER_TYPE + 0x28, 0, PLAYER_PTR->inner.pos);
    }
    anm::delete_vm(hyper_bg_anmid);
    hyper_bg_anmid = anm_file->createEffectPos(0x20, 0, PLAYER_PTR->inner.pos);
  }
  return;
}
