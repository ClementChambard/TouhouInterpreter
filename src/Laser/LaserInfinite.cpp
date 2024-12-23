#include "LaserInfinite.h"
#include "../Anm/AnmManager.h"
#include "../Bullet/BulletManager.h"
#include "../Hardcoded.h"
#include "../ItemManager.h"
#include "LaserManager.h"
#include <NSEngine.hpp>
#include <math/math.hpp>

#define GAME_SPEED ns::get_instance()->game_speed()

LaserInfinite::LaserInfinite() {
  vm1.sprite_id = -1;
  vm1.instr_offset = -1;
  vm2.sprite_id = -1;
  vm2.instr_offset = -1;
}

int LaserInfinite::initialize(void *arg) {
  inner = *reinterpret_cast<LaserInfiniteInner_t *>(arg);

  bullet_type = inner.type;
  bullet_color = inner.color;
  kind = 1;
  __field_10__set_to_3_by_ex_delete = 3;

  vm1.reset();
  vm1.index_of_sprite_mapping_func = LASER_ON_SPRITE_SET_FUNC;
  vm1.associated_game_entity = this;
  LASER_MANAGER_PTR->bullet_anm->load_external_vm(
      &vm1, BULLET_TYPE_TABLE[bullet_type]["script"].asInt());
  vm1.interrupt(2);
  vm1.update();
  vm1.bitflags.originMode = 1;
  vm1.bitflags.rendermode = 1;
  vm1.bitflags.anchorY = 2;
  vm1.bitflags.blendmode = 1;

  LASER_MANAGER_PTR->bullet_anm->copyFromLoaded(
      &vm2, inner.color + LASER_DATA["spawn_anm_first"].asInt());
  vm2.parent_vm = nullptr;
  vm2.__root_vm__or_maybe_not = nullptr;
  vm2.update();
  vm2.interrupt(2);
  vm2.update();
  vm2.bitflags.originMode = 1;
  vm2.bitflags.rendermode = 1;
  vm2.bitflags.blendmode = 1;

  // TODO: no
  vm1.layer = 14;
  vm2.layer = 14;

  if (inner.shot_sfx >=
      0) { /*SoundManager::play_sound_at_position(inner.shot_sfx);*/
  }

  laser_offset = inner.start_pos;

  if (inner.distance != 0.f)
    laser_offset +=
        ns::vec3(ns::lengthdir_vec(inner.distance, inner.ang_aim), 0.f);
  laser_inf_current_length = inner.laser_new_arg1;
  speed = inner.spd_1;
  angle = inner.ang_aim;
  et_ex_index = inner.et_ex_index;
  laser_st_width = 2.0;
  laser_id = inner.laser_st_on_arg1;
  __timer_2c = 0;

  return 0;
}

int LaserInfinite::on_tick() {
  while (et_ex_index < 18 && inner.ex[et_ex_index].type &&
         !(inner.ex[et_ex_index].slot == 0 && flags != 0)) {
    if (inner.ex[et_ex_index].type == 0x80) {
      ex_invuln__remaining_frames = inner.ex[et_ex_index].a;
    } else if (inner.ex[et_ex_index].type == 0x400) {
      __field_10__set_to_3_by_ex_delete = 3;
    } else if (inner.ex[et_ex_index].type == 0x100000) {
      if (inner.ex[et_ex_index].a == 0)
        vm1.bitflags.blendmode = 0;
      else
        vm1.bitflags.blendmode = 1;
      et_ex_index++;
      continue;
    }
    et_ex_index++;
  }
  if (flags != 0) {
    if (flags < 0) {
      if (ex_wait.timer <= 0)
        flags &= 0x7fffffff;
      else
        ex_wait.timer--;
    }
    if (ex_invuln__remaining_frames != 0)
      ex_invuln__remaining_frames--;
  }
  if ((laser_inf_current_length < inner.laser_new_arg2) &&
      (laser_inf_current_length = speed * GAME_SPEED + laser_inf_current_length,
       inner.laser_new_arg2 < laser_inf_current_length)) {
    laser_inf_current_length = inner.laser_new_arg2;
  }

  angle = inner.laser_inf_angular_velocity * GAME_SPEED + angle;
  ns::angle_normalize(angle);

  // if (inner.flags & 1) {
  // pzVar10 = NULL;
  // if (ENEMY_MANAGER_PTR->boss_ids[0] != 0) {
  // pzVar4 = ENEMY_MANAGER_PTR->active_enemy_list_head;
  // while ((pzVar4 != NULL && (pzVar10 = pzVar4->entry, pzVar10->enemy_id !=
  // ENEMY_MANAGER_PTR->boss_ids[0]))) { pzVar4 = pzVar4->next;
  //}
  // if (pzVar10 != NULL) {
  // pzVar10 = EnemyManager::get_boss_enemy_full(0);
  //*(undefined8*)&laser_offset = *(undefined8*)&(pzVar10->data).final_pos.pos;
  // laser_offset.z = (pzVar10->data).final_pos.pos.z;
  //}
  //}
  //}

  laser_offset += inner.speed * GAME_SPEED;
  switch (__field_10__set_to_3_by_ex_delete) {
  case 3:
    if (__timer_18 >= inner.laser_time_start) {
      __timer_18 = 0;
      __field_10__set_to_3_by_ex_delete = 4;
    }
    break;
  case 4:
    if (__timer_18 < inner.laser_trans_1) {
      laser_st_width = (__timer_18.current_f * inner.laser_new_arg4) /
                       (float)inner.laser_trans_1;
      break;
    }
    __timer_18 = 0;
    __field_10__set_to_3_by_ex_delete = 2;
    laser_st_width = inner.laser_new_arg4;
    [[fallthrough]];
  case 2:
    if (__timer_18 >= inner.laser_duration) {
      __timer_18 = 0;
      __field_10__set_to_3_by_ex_delete = 5;
      if (__timer_18 >= inner.laser_trans_2) {
        return 1;
      }
      laser_st_width =
          inner.laser_new_arg4 - (__timer_18.current_f * inner.laser_new_arg4) /
                                     (float)inner.laser_trans_2;
    }
    break;
  case 5:
    if (__timer_18 >= inner.laser_trans_2) {
      return 1;
    }
    laser_st_width =
        inner.laser_new_arg4 - (__timer_18.current_f * inner.laser_new_arg4) /
                                   (float)inner.laser_trans_2;
  }

  if (((__field_10__set_to_3_by_ex_delete == 4) ||
       (__field_10__set_to_3_by_ex_delete == 2)) &&
      (16.0 < laser_inf_current_length)) {
    // float col_w = laser_st_width;
    // if (32.0 <= laser_st_width) {
    // col_w -= (laser_st_width + 16.0) / 3.0;
    //} else {
    // col_w = laser_st_width * 0.5;
    //}
    // int collide = enemy_collide_player_rectangle(puVar11, laser_offset,
    // angle, 0, col_w, laser_inf_current_length * 0.9); if (collide == 1) {
    //     cancel_as_bomb_rectangle({PLAYER_PTR->inner.pos.x,
    //     PLAYER_PTR->inner.pos.y, 0}, {32, 32, 0}, 0, 0, 1);
    // } else if (collide == 2) {
    //     // TODO graze
    //     //if (__timer_2c % 3 == 0) {
    //         //fVar15 = math_wrap_angle_402920(angle + 1.570796);
    //         //FUN_00403bf0(&local_20, &local_1c, angle,
    //         PLAYER_PTR->inner.pos.x, PLAYER_PTR->inner.pos.y, fVar15);
    //         //local_18._0_8_ = CONCAT44(local_1c, local_20);
    //         ////Player::do_graze(&local_18);
    //     //}
    //     __timer_2c++;
    // }
  }

  auto spr = vm1.getSprite();
  vm1.bitflags.scaled = true;
  // vm1.scale.current.x = vm1.scale.goal.x = laser_st_width / spr.w;
  vm1.scale.x = laser_st_width / spr.w;
  // vm1.scale.current.y = vm1.scale.goal.y = laser_inf_current_length / spr.h;
  vm1.scale.y = laser_inf_current_length / spr.h;
  vm1.update();
  if (__field_7c__sometimes_0p01_or_0f == 0.0)
    vm2.update();
  return 0;
}

int LaserInfinite::on_draw() {
  float zrot = angle + 1.570796;
  ns::angle_normalize(zrot);
  // vm1.rotation.current.z = vm1.rotation.goal.z = zrot;
  vm1.rotation.z = zrot;
  vm1.bitflags.rotated = true;
  vm1.pos = laser_offset;
  // vm1.draw();
  anm::draw_vm(&vm1);
  if (__field_7c__sometimes_0p01_or_0f == 0.0) {
    vm2.pos = laser_offset;
    // vm2.draw();
    anm::draw_vm(&vm2);
  }
  return 0;
}

int LaserInfinite::cancel(int, int as_bomb) {
  if (as_bomb && ex_invuln__remaining_frames != 0)
    return 0;

  ns::vec3 inc = {ns::lengthdir_vec(8.f, angle), 0.f};
  ns::vec3 p = laser_offset + inc;
  inc *= 2;
  int i = 0;
  for (; (i + 1) * 16.f < laser_inf_current_length; i++) {
    if (-192.0 < p.x + 16.0 && p.x - 16.0 < 192.0 && 0.0 < p.y + 16.0 &&
        p.y - 16.0 < 448.0) {
      if (bullet_type < 18 || bullet_type == 34 || bullet_type == 38) {
        BULLET_MANAGER_PTR->bullet_anm->createEffectPos(inner.color * 2 + 0xd1,
                                                        0, p);
      } else if (bullet_type < 32) {
        BULLET_MANAGER_PTR->bullet_anm->createEffectPos(inner.color * 2 + 0x101,
                                                        0, p);
      } else if (bullet_type < 34) {
        BULLET_MANAGER_PTR->bullet_anm->createEffectPos(inner.color * 2 + 0x119,
                                                        0, p);
      }
      if (as_bomb && -192.0 < p.x + 32.0 && p.x - 32.0 < 192.0 &&
          0.0 < p.y + 32.0 && p.y - 32.0 < 448.0) {
        BULLET_MANAGER_PTR->__unknown_cancel_counter++;
        ITEM_MANAGER_PTR->spawn_item(
            9, p, ns::frandm11() * 0.1745329 - 1.570796, 2.2, 0, -1);
      }
    }
    p += inc;
  }
  __field_10__set_to_3_by_ex_delete = 1;
  return i;
}

int LaserInfinite::cancel_as_bomb_rectangle(ns::vec3 p1, ns::vec3 p2, float rot,
                                            int item, int as_bomb) {
  char canceled_node[260] = {};

  if (as_bomb && ex_invuln__remaining_frames > 0)
    return 0;

  float aa = angle - rot;
  ns::angle_normalize(aa);
  ns::vec3 rect_inc = {ns::lengthdir_vec(8.0, aa), 0.f};
  ns::vec3 rect_pos = {ns::cos(-rot) * (laser_offset.x - p1.x) -
                           ns::sin(-rot) * (laser_offset.y - p1.y) + rect_inc.x,
                       ns::sin(-rot) * (laser_offset.x - p1.x) +
                           ns::cos(-rot) * (laser_offset.y - p1.y) + rect_inc.y,
                       0.f};
  ns::vec3 inc = {ns::lengthdir_vec(8.0, angle), 0.f};
  ns::vec3 p = laser_offset + inc;
  rect_inc *= 2;
  inc *= 2;
  int i = 0;
  for (; (i + 1) * 16.f < laser_inf_current_length; i++) {
    if (-p2.x * 0.5 <= rect_pos.x && rect_pos.x <= p2.x * 0.5 &&
        -p2.y * 0.5 <= rect_pos.y && rect_pos.y <= p2.y * 0.5) {
      canceled_node[i] = 1;
      gen_items_from_et_cancel(p, item);
      if (bullet_type < 0x12 || bullet_type == 0x22 || bullet_type == 0x26) {
        BULLET_MANAGER_PTR->bullet_anm->createEffectPos(inner.color * 2 + 0xd1,
                                                        0, p);
        // put in EffectManager
      } else if (bullet_type < 0x20) {
        BULLET_MANAGER_PTR->bullet_anm->createEffectPos(inner.color * 2 + 0x101,
                                                        0, p);
        // put in EffectManager
      } else if (bullet_type < 0x22) {
        BULLET_MANAGER_PTR->bullet_anm->createEffectPos(inner.color * 2 + 0x119,
                                                        0, p);
        // put in EffectManager
      }
    }
    p += inc;
    rect_pos += rect_inc;
  }
  if (i != 0) {
    if (0 < i) {
      int j = 0;
      do {
        if (canceled_node[j] == 0)
          break;
        j++;
      } while (j < i);
      if (j != 0) {
        laser_inf_current_length = 0.f;
        while (j < i) {
          if (canceled_node[j] == 0) {
            float len = 0.0;
            do {
              if (canceled_node[j])
                break;
              j++;
              len++;
            } while (j < i);
            LaserLineInner_t inner_;
            inner_.start_pos = laser_offset + inc * (float)j;
            inner_.ang_aim = angle;
            inner_.laser_new_arg_2 = len * 16.f;
            inner_.laser_new_arg_1 = len * 16.f;
            inner_.laser_new_arg_3 = inner.laser_new_arg2 - j * 16.f;
            inner_.laser_new_arg_4 = laser_st_width;
            inner_.spd_1 = 8.f;
            inner_.bullet_type = inner.type;
            inner_.bullet_color = inner.color;
            inner_.flags =
                (inner_.flags ^ ((uint)inner.flags >> 1 ^ inner_.flags)) & 1;
            allocate_new_laser(0, &inner_);
          } else {
            j++;
          }
        }
        return 1;
      }
    }
  }
  return 1;
}
