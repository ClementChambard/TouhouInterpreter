#include "./Gui.hpp"
#include "./AnmOpener/AnmManager.h"
#include "./AsciiManager.hpp"
#include "./EnemyManager.h"
#include "./GlobalData.h"
#include "./Player.h"
#include "./Spellcard.h"

Gui *GUI_PTR = nullptr;

int Gui::f_on_tick() {
  if (some_flags_1a4 & 0x100) {
    timer_chapter_end++;
  }
  if (some_flags_1a4 & 0x1800) {
    timer_chapter_end++;
    if ((some_flags_1a4 & 0x1800) == 0x800 && timer_chapter_end >= 90) {
      if (th15_chapter_kill_pc <= 0.0) {
        if (timer_chapter_end != 90) {
          // SoundManager::play_sound_centered(0x2f);
        }
        // th15_chapter_kill_pc = field76_0x130;
        // field79_0x13c = 0;
        // th15_chapter_total_bonus = field77_0x134;
        some_flags_1a4 = (some_flags_1a4 & 0xfffff7ff) | 0x1000;
      } else {
        if (timer_chapter_end % 4 == 0) {
          // SoundManager::play_sound_centered(0x27);
        }
        // th15_chapter_total_bonus += field79_0x13c;
        th15_chapter_kill_pc -= 1;
      }
    }
    if (th15_some_time_value_1bc <= timer_chapter_end) {
      AnmManager::interrupt_tree(th15_chapter_end_anmid, 1);
      timer_chapter_end = 0;
      some_flags_1a4 &= 0xffffe7ff;
    }
  }

  if (field_0x144_spellcard_related &&
      !AnmManager::getVM(anmid_0x120_spellTimeShowEnd)) {
    anmid_0x120_spellTimeShowEnd = 0;
    field_0x144_spellcard_related = 0;
  }

  if (!ENEMY_MANAGER_PTR || remaining_spell_time_seconds < 0 ||
      !ENEMY_MANAGER_PTR->getBoss(0) /*||
      (ENEMY_MANAGER_PTR->flag_0_is_life_hide & 1) || msg ||
      (GAME_THREAD_PTR->flags & 0x10000U)*/) {
    vm_timer_digit_hi->clear_flag_1_rec();
    vm_timer_digit_lo->clear_flag_1_rec();
    some_flags_1a4 = (some_flags_1a4 & 0xfffffdff) | 0x400;
  } else {
    vm_timer_digit_hi->set_flag_1_rec();
    vm_timer_digit_lo->set_flag_1_rec();
    if (!(some_flags_1a4 & 0x600)) {
      if ((!(SPELLCARD_PTR->flags & 0x100) &&
           PLAYER_PTR->inner.pos.y < 128.0) ||
          ((SPELLCARD_PTR->flags & 0x100) && 320.0 < PLAYER_PTR->inner.pos.y)) {
        some_flags_1a4 = (some_flags_1a4 & 0xfffffbff) | 0x200;
        vm_timer_digit_hi->interrupt(5);
        vm_timer_digit_lo->interrupt(5);
      }
    } else {
      if ((some_flags_1a4 & 0x600) == 0x200) {
        if ((!(SPELLCARD_PTR->flags & 0x100) &&
             160.0 > PLAYER_PTR->inner.pos.y) ||
            ((SPELLCARD_PTR->flags & 0x100) &&
             PLAYER_PTR->inner.pos.y > 288.0)) {
          some_flags_1a4 &= 0xfffff9ff;
          vm_timer_digit_hi->interrupt(4);
          vm_timer_digit_lo->interrupt(4);
        }
      } else {
        if (!(SPELLCARD_PTR->flags & 1)) {
          vm_timer_digit_hi->interrupt(3);
          vm_timer_digit_lo->interrupt(3);
          vm_timer_digit_hi->run();
          vm_timer_digit_lo->run();
        } else {
          vm_timer_digit_hi->interrupt(2);
          vm_timer_digit_lo->interrupt(2);
          vm_timer_digit_hi->run();
          vm_timer_digit_lo->run();
        }
        vm_timer_digit_hi->interrupt(4);
        vm_timer_digit_lo->interrupt(4);
        vm_timer_digit_hi->run();
        vm_timer_digit_lo->run();
        some_flags_1a4 &= 0xfffff9ff;
      }
    }
    if (remaining_spell_time_seconds < old_remaining_spell_time_seconds) {
      if (remaining_spell_time_seconds < 2) {
        vm_timer_digit_hi->interrupt(9);
        vm_timer_digit_lo->interrupt(9);
        // SoundManager::play_sound_centered(0xc);
      } else if (remaining_spell_time_seconds < 5) {
        vm_timer_digit_hi->interrupt(8);
        vm_timer_digit_lo->interrupt(8);
        // SoundManager::play_sound_centered(0xb);
      }
    } else if (old_remaining_spell_time_seconds <
               remaining_spell_time_seconds) {
      vm_timer_digit_hi->interrupt(7);
      vm_timer_digit_lo->interrupt(7);
    }
    if (remaining_spell_time_seconds != old_remaining_spell_time_seconds) {
      AnmManager::getLoaded(vm_timer_digit_hi->anm_loaded_index)
          ->setSprite(vm_timer_digit_hi,
                      remaining_spell_time_seconds / 10 + 239);
      AnmManager::getLoaded(vm_timer_digit_lo->anm_loaded_index)
          ->setSprite(vm_timer_digit_lo,
                      remaining_spell_time_seconds % 10 + 239);
    }
    old_remaining_spell_time_seconds = remaining_spell_time_seconds;
  }

  if (ENEMY_MANAGER_PTR /*&& !(ENEMY_MANAGER_PTR->flag_0_is_life_hide & 1)*/) {
    for (int bbid = 0; bbid < 2; bbid++) {
      Enemy *boss =
          ENEMY_MANAGER_PTR->EnmFind(ENEMY_MANAGER_PTR->getBoss(bbid));
      if (!boss) {
        boss_bars[bbid].visual_bar_pc = 0;
        boss_bars[bbid].markers[0] = 0;
        boss_bars[bbid].markers[2] = 0;
        boss_bars[bbid].markers[4] = 0;
        boss_bars[bbid].markers[6] = 0;
        if (boss_bars[bbid].vms_created) {
          for (int i = 0; i < 7; i++) {
            AnmManager::deleteVM(boss_bars[bbid].anm_ids[i]);
            boss_bars[bbid].anm_ids[i] = 0;
          }
          boss_bars[bbid].vms_created = false;
        }
        if (bbid == 0) {
          // AnmManager::deleteVM(anmid_front_ename_0xd8);
          // anmid_front_ename_0xd8 = 0;
        }
      } else {
        if (boss->getData()->life.current < 100000 &&
            !(boss->getData()->flags & 0x31) /* &&
            boss->getData()->invuln_timer < 1 && !msg*/) {
          boss_bars[bbid].current_life = boss->getData()->life.current;
          boss_bars[bbid].actual_bar_pc =
              static_cast<float>(boss->getData()->life.current) /
              static_cast<float>(boss->getData()->life.max);
          if (boss_bars[bbid].visual_bar_pc < boss_bars[bbid].actual_bar_pc) {
            boss_bars[bbid].visual_bar_pc += 0.025;
          }
          if (boss_bars[bbid].actual_bar_pc < boss_bars[bbid].visual_bar_pc) {
            boss_bars[bbid].visual_bar_pc = boss_bars[bbid].actual_bar_pc;
          }
          if (!boss_bars[bbid].vms_created) {
            boss_bars[bbid].anm_ids[0] =
                AnmManager::SpawnVM(front_anm->getSlot(), 0x187);
            boss_bars[bbid].anm_ids[1] =
                AnmManager::SpawnVM(front_anm->getSlot(), 0x188);
            boss_bars[bbid].anm_ids[2] =
                AnmManager::SpawnVM(front_anm->getSlot(), 0x189);
            boss_bars[bbid].anm_ids[3] =
                AnmManager::SpawnVM(front_anm->getSlot(), 0x18a);
            boss_bars[bbid].anm_ids[4] =
                AnmManager::SpawnVM(front_anm->getSlot(), 0x18a);
            boss_bars[bbid].anm_ids[5] =
                AnmManager::SpawnVM(front_anm->getSlot(), 0x18a);
            boss_bars[bbid].anm_ids[6] =
                AnmManager::SpawnVM(front_anm->getSlot(), 0x18a);
            boss_bars[bbid].vms_created = true;
          }
          // sub_42fe40_front_ename(this);
          AnmVM *bbvm = AnmManager::getVM(boss_bars[bbid].anm_ids[0]);
          if (!bbvm) {
            boss_bars[bbid].anm_ids[0] = 0;
          }
          bbvm->bitflags.rotated = true;
          bbvm->rotation.x = boss_bars[bbid].visual_bar_pc * -PI2;
          bbvm->entity_pos = boss->getData()->final_pos.pos;
          bbvm = AnmManager::getVM(boss_bars[bbid].anm_ids[1]);
          if (!bbvm) {
            boss_bars[bbid].anm_ids[1] = 0;
          }
          bbvm->entity_pos = boss->getData()->final_pos.pos * 2.f;
          bbvm = AnmManager::getVM(boss_bars[bbid].anm_ids[2]);
          if (!bbvm) {
            boss_bars[bbid].anm_ids[2] = 0;
          }
          bbvm->entity_pos = boss->getData()->final_pos.pos * 2.f;
          for (int i = 3; i < 7; i++) {
            float pc = boss_bars[bbid].markers[(i - 3) * 2];
            bbvm = AnmManager::getVM(boss_bars[bbid].anm_ids[i]);
            if (!bbvm) {
              boss_bars[bbid].anm_ids[i] = 0;
            }
            if ((pc == 0.0) || boss_bars[bbid].visual_bar_pc <= pc) {
              bbvm->clear_flag_1_rec();
            } else {
              bbvm->set_flag_1_rec();
              bbvm->bitflags.rotated = true;
              bbvm->rotation.z = -PI - pc * PI2;
              math::angle_normalize(bbvm->rotation.z);
              bbvm->entity_pos =
                  boss->getData()->final_pos.pos * 2.f +
                  glm::vec3(math::lengthdir_vec(112.0, bbvm->rotation.z), 0.f);
            }
          }
          if (!boss_bars[bbid].vms_hidden_by_player) {
            if (math::point_distance_sq(PLAYER_PTR->inner.pos,
                                        boss->getData()->final_pos.pos) <
                6400.0) {
              for (int i = 0; i < 7; i++) {
                AnmManager::interrupt_tree(boss_bars[bbid].anm_ids[i], 3);
              }
              boss_bars[bbid].vms_hidden_by_player = 1;
            }
          } else {
            if (math::point_distance_sq(PLAYER_PTR->inner.pos,
                                        boss->getData()->final_pos.pos) >=
                9216.0) {
              for (int i = 0; i < 7; i++) {
                AnmManager::interrupt_tree(boss_bars[bbid].anm_ids[i], 2);
              }
              boss_bars[bbid].vms_hidden_by_player = 0;
            }
          }
        } else {
          if (boss_bars[bbid].vms_created) {
            for (int i = 0; i < 7; i++) {
              AnmManager::deleteVM(boss_bars[bbid].anm_ids[i]);
            }
            boss_bars[bbid].vms_created = 0;
          }
        }
      }
    }
  }

  for (int i = 0; i < 10; i++) {
    if (i < stars_nb) {
      if (stars_anmids[i].val == 0) {
        stars_anmids[i] = AnmManager::SpawnVM(front_anm->getSlot(), i + 0x46);
      }
    } else {
      if (stars_anmids[i].val != 0) {
        AnmManager::interrupt_tree(stars_anmids[i], 1);
        stars_anmids[i] = 0;
      }
    }
  }

  // if (msg) {
  //   if (!msg->tick()) {
  //     msg->__timer_4.increment();
  //   } else {
  //     if (msg)
  //       delete msg;
  //     msg = nullptr;
  //   }
  // }

  if (!ENEMY_MANAGER_PTR) {
    __timer_14c++;
    return 1;
  }

  Enemy *boss0 = ENEMY_MANAGER_PTR->EnmFind(ENEMY_MANAGER_PTR->getBoss(0));
  AnmVM *vmBossMarker = AnmManager::getVM(boss_marker_anmid);
  if (boss0 && (~(boss0->getData()->flags >> 5) & 1) &&
      (~boss0->getData()->flags & 1)) {
    vmBossMarker->set_flag_1_rec();
    if (!(SPELLCARD_PTR->flags & 1)) {
      if ((some_flags_1a4 & 6) == 0) {
        if (boss0->getData()->life.curAtk < 700) {
          vmBossMarker->interrupt(7);
          some_flags_1a4 |= 2;
        }
      } else if ((some_flags_1a4 & 6) == 2) {
        if (boss0->getData()->life.curAtk < 400) {
          vmBossMarker->interrupt(8);
          some_flags_1a4 = (some_flags_1a4 & 0xfffffffd) | 4;
        }
      } else if ((some_flags_1a4 & 6) == 4) {
        if (boss0->getData()->life.curAtk < 200) {
          vmBossMarker->interrupt(9);
          some_flags_1a4 |= 6;
        }
      } else if ((some_flags_1a4 & 6) == 6) {
        if (boss0->getData()->life.curAtk > 200) {
          vmBossMarker->interrupt(10);
          some_flags_1a4 &= 0xfffffff9;
        }
      }
    } else {
      if ((some_flags_1a4 & 6) == 0) {
        if (boss0->getData()->life.curAtk < 2000) {
          vmBossMarker->interrupt(7);
          some_flags_1a4 |= 2;
        }
      } else if ((some_flags_1a4 & 6) == 2) {
        if (boss0->getData()->life.curAtk < 1000) {
          vmBossMarker->interrupt(8);
          some_flags_1a4 = (some_flags_1a4 & 0xfffffffd) | 4;
        }
      } else if ((some_flags_1a4 & 6) == 4) {
        if (boss0->getData()->life.curAtk < 400) {
          vmBossMarker->interrupt(9);
          some_flags_1a4 |= 6;
        }
      } else if ((some_flags_1a4 & 6) == 6) {
        if (boss0->getData()->life.curAtk > 400) {
          vmBossMarker->interrupt(10);
          some_flags_1a4 &= 0xfffffff9;
        }
      }
    }
    vmBossMarker->entity_pos.y = 960.0;
    vmBossMarker->entity_pos.x =
        2 * (boss0->getData()->final_pos.pos.x + 32.0 + 192.0);
    vmBossMarker->color_1.a =
        fmax(abs(boss0->getData()->final_pos.pos.x - PLAYER_PTR->inner.pos.x) *
                     191.0 / 64.0 +
                 64,
             0xff);
    if (abs(boss0->getData()->final_pos.pos.x) > 192.0) {
      vmBossMarker->color_1.a = 0;
    }
  } else {
    vmBossMarker->clear_flag_1_rec();
  }
  __timer_14c++;
  return 1;
}

int Gui::f_on_draw() {
  AnmVM *stage_clear_vm = AnmManager::getVM(stage_clear_anmid);
  if (!stage_clear_vm) {
    stage_clear_anmid = 0;
  } else {
    ASCII_MANAGER_PTR->scale = {1.0, 1.0};
    ASCII_MANAGER_PTR->color = {0xff, 0xff, 0xff, stage_clear_vm->color_1.a};
    ASCII_MANAGER_PTR->group = 2;
    ASCII_MANAGER_PTR->font_id = 4;
    ASCII_MANAGER_PTR->alignment_mode_h = 0;
    ASCII_MANAGER_PTR->alignment_mode_v = 0;
    ASCII_MANAGER_PTR->comma_separated_number({224.0, 200.0, 0.0},
                                              stage_clear_bonus);
  }

  AnmVM *th15_chapter_end_vm = AnmManager::getVM(th15_chapter_end_anmid);
  if (!th15_chapter_end_vm) {
    th15_chapter_end_anmid = 0;
  } else {
    if (th15_chapter_end_vm->bitflags.f530_1) {
      ASCII_MANAGER_PTR->scale = {1.0, 1.0};
      ASCII_MANAGER_PTR->color = {0xff, 0xff, 0xff,
                                  th15_chapter_end_vm->color_1.a};
      ASCII_MANAGER_PTR->group = 2;
      ASCII_MANAGER_PTR->font_id = 2;
      ASCII_MANAGER_PTR->alignment_mode_h = 2;
      ASCII_MANAGER_PTR->alignment_mode_v = 0;
      ASCII_MANAGER_PTR->create_string_f({300.0, 226.0, 0.0}, "%d",
                                         th15_chapter_graze);
      ASCII_MANAGER_PTR->create_string_f({308.0, 246.0, 0.0}, "%3.1f%%",
                                         th15_chapter_kill_pc);
      ASCII_MANAGER_PTR->create_string_f({300.0, 266.0, 0.0}, "%3d",
                                         th15_chapter_retry_count);
      ASCII_MANAGER_PTR->create_string_f({300.0, 286.0, 0.0}, "%6d",
                                         th15_chapter_total_bonus);

      ASCII_MANAGER_PTR->scale = {1.0, 1.0};
      ASCII_MANAGER_PTR->color = {0x80, 0x80, 0xff, 0xff};
      ASCII_MANAGER_PTR->group = 2;
      ASCII_MANAGER_PTR->font_id = 2;
      ASCII_MANAGER_PTR->alignment_mode_h = 2;
      ASCII_MANAGER_PTR->alignment_mode_v = 0;
      ASCII_MANAGER_PTR->create_string_f({300.0, 296.0, 0.0}, "  +%d",
                                         (th15_chapter_total_bonus / 50000) *
                                             10);
    }
  }

  if (field_0x144_spellcard_related != 0) {
    AnmVM *pzVar7 = AnmManager::getVM(anmid_0x120_spellTimeShowEnd);
    if (pzVar7 == NULL) {
      anmid_0x120_spellTimeShowEnd = 0;
    }
    if (SPELLCARD_PTR == NULL) {
      field_0x144_spellcard_related = 0;
      AnmManager::deleteVM(anmid_0x120_spellTimeShowEnd);
      anmid_0x120_spellTimeShowEnd = 0;
    } else if (pzVar7 == NULL) {
      field_0x144_spellcard_related = 0;
    } else {
      ASCII_MANAGER_PTR->scale = {1.0, 1.0};
      ASCII_MANAGER_PTR->color = {0xff, 0xff, 0xff, pzVar7->color_1.a};
      ASCII_MANAGER_PTR->group = 2;
      ASCII_MANAGER_PTR->font_id = 4;
      ASCII_MANAGER_PTR->alignment_mode_h = 1;
      ASCII_MANAGER_PTR->alignment_mode_v = 1;
      // ASCII_MANAGER_PTR->create_string_f({224.0, 144.0, 0.0}, "%3d.",
      //                                    min(smthg, 999));

      ASCII_MANAGER_PTR->scale = {0.6, 0.6};
      ASCII_MANAGER_PTR->color = {0xff, 0xff, 0xff, pzVar7->color_1.a};
      ASCII_MANAGER_PTR->group = 2;
      ASCII_MANAGER_PTR->font_id = 4;
      ASCII_MANAGER_PTR->alignment_mode_h = 1;
      ASCII_MANAGER_PTR->alignment_mode_v = 1;
      // ASCII_MANAGER_PTR->create_string_f(
      //     {268.0, 150.0, 0.0}, "%.2ds",
      //     ((SPELLCARD_PTR->field32_0x90 % 0x3c) * 100) / 60);

      // iVar10 = (SPELLCARD_PTR->field38_0xa8 % 100 + 67) % 100;
      // iVar9 = ((SPELLCARD_PTR->field38_0xa8 % 100 + 67) % 100) +
      //         ((SPELLCARD_PTR->field38_0xa8 / 100) % 1000 + 0x3a6) % 1000;
      // if (SPELLCARD_PTR->field38_0xa8 / 100000 + -22 != iVar9) {
      //   iVar10 = 99;
      // }
      ASCII_MANAGER_PTR->scale = {1.0, 1.0};
      ASCII_MANAGER_PTR->color = {0x80, 0x80, 0x80, pzVar7->color_1.a};
      ASCII_MANAGER_PTR->group = 2;
      ASCII_MANAGER_PTR->font_id = 4;
      ASCII_MANAGER_PTR->alignment_mode_h = 1;
      ASCII_MANAGER_PTR->alignment_mode_v = 1;
      // ASCII_MANAGER_PTR->create_string_f({224.0, 160.0, 0.0}, "%3d.", iVar9);

      ASCII_MANAGER_PTR->scale = {0.6, 0.6};
      ASCII_MANAGER_PTR->color = {0x80, 0x80, 0x80, pzVar7->color_1.a};
      ASCII_MANAGER_PTR->group = 2;
      ASCII_MANAGER_PTR->font_id = 4;
      ASCII_MANAGER_PTR->alignment_mode_h = 1;
      ASCII_MANAGER_PTR->alignment_mode_v = 1;
      // ASCII_MANAGER_PTR->create_string_f({268.0, 166.0, 0.0}, "%.2ds",
      // iVar10);
    }
  }

  AnmVM *vm = life_meter_vms[0];

  ASCII_MANAGER_PTR->scale = {1.0, 1.0};
  ASCII_MANAGER_PTR->color = {0x80, 0x80, 0x80, vm->color_1.a};
  ASCII_MANAGER_PTR->group = 0;
  ASCII_MANAGER_PTR->font_id = 4;
  ASCII_MANAGER_PTR->alignment_mode_h = 2;
  ASCII_MANAGER_PTR->alignment_mode_v = 1;
  ASCII_MANAGER_PTR->draw_score({620.0, 42.0, 0.0}, GLOBALS.HIGH_SCORE,
                                GLOBALS.__field_4);

  ASCII_MANAGER_PTR->scale = {1.0, 1.0};
  ASCII_MANAGER_PTR->color = {0xff, 0xff, 0xff, vm->color_1.a};
  ASCII_MANAGER_PTR->group = 0;
  ASCII_MANAGER_PTR->font_id = 4;
  ASCII_MANAGER_PTR->alignment_mode_h = 2;
  ASCII_MANAGER_PTR->alignment_mode_v = 1;
  // continue used ?
  ASCII_MANAGER_PTR->draw_score({620.0, 64.0, 0.0}, current_score, 0);

  float local_58 = 0.0;
  // if (99 < (&ITEM_ANM_SCRIPT_IDS[17].id_1)[GLOBALS.inner.field27_0x6c]) {
  //   local_58 = 7.0;
  // }
  ASCII_MANAGER_PTR->scale = {0.6, 0.6};
  ASCII_MANAGER_PTR->color = {0xff, 0xff, 0xff, vm->color_1.a};
  ASCII_MANAGER_PTR->group = 0;
  ASCII_MANAGER_PTR->font_id = 4;
  ASCII_MANAGER_PTR->alignment_mode_h = 1;
  ASCII_MANAGER_PTR->alignment_mode_v = 1;
  // if ((&ITEM_ANM_SCRIPT_IDS[17].id_1)[GLOBALS.inner.lifepiece_related] <
  // 999999) {
  ASCII_MANAGER_PTR->create_string_f({576.0 - local_58, 120.0, 0.0}, "%3d");
  ASCII_MANAGER_PTR->create_string_f({597.0 - local_58, 120.0, 0.0}, "/%d",
                                     /*(&ITEM_ANM_SCRIPT_IDS[17].id_1)[*/
                                     GLOBALS.inner.lifepiece_related /*]*/);
  //} else {
  // ASCII_MANAGER_PTR->create_string_f({576.0 - local_58, 120.0, 0.0},
  //"---/---");
  //}

  ASCII_MANAGER_PTR->scale = {0.6, 0.6};
  ASCII_MANAGER_PTR->color = {0xff, 0xff, 0xff, vm->color_1.a};
  ASCII_MANAGER_PTR->group = 0;
  ASCII_MANAGER_PTR->font_id = 4;
  ASCII_MANAGER_PTR->alignment_mode_h = 1;
  ASCII_MANAGER_PTR->alignment_mode_v = 1;
  ASCII_MANAGER_PTR->create_string_f({576.0, 158.0, 0.0}, "%3d",
                                     GLOBALS.inner.bombpiece_related);
  ASCII_MANAGER_PTR->create_string_f({597.0, 158.0, 0.0}, "/3");

  ASCII_MANAGER_PTR->scale = {1.0, 1.0};
  ASCII_MANAGER_PTR->color = {0xff, 0x80, 0, vm->color_1.a};
  ASCII_MANAGER_PTR->group = 0;
  ASCII_MANAGER_PTR->font_id = 4;
  ASCII_MANAGER_PTR->alignment_mode_h = 1;
  ASCII_MANAGER_PTR->alignment_mode_v = 1;
  ASCII_MANAGER_PTR->create_string_f({540.0, 182.0, 0.0}, "%d.",
                                     GLOBALS.inner.CURRENT_POWER /
                                         GLOBALS.inner.POWER_PER_LEVEL);

  ASCII_MANAGER_PTR->scale = {0.6, 0.6};
  ASCII_MANAGER_PTR->color = {0xff, 0x80, 0, vm->color_1.a};
  ASCII_MANAGER_PTR->group = 0;
  ASCII_MANAGER_PTR->font_id = 4;
  ASCII_MANAGER_PTR->alignment_mode_h = 1;
  ASCII_MANAGER_PTR->alignment_mode_v = 1;
  ASCII_MANAGER_PTR->create_string_f(
      {560.0, 189.0, 0.0}, "%.2d",
      ((GLOBALS.inner.CURRENT_POWER % GLOBALS.inner.POWER_PER_LEVEL) * 100) /
          GLOBALS.inner.POWER_PER_LEVEL);

  ASCII_MANAGER_PTR->scale = {1.0, 1.0};
  ASCII_MANAGER_PTR->color = {0xff, 0x80, 0, vm->color_1.a};
  ASCII_MANAGER_PTR->group = 0;
  ASCII_MANAGER_PTR->font_id = 4;
  ASCII_MANAGER_PTR->alignment_mode_h = 1;
  ASCII_MANAGER_PTR->alignment_mode_v = 1;
  ASCII_MANAGER_PTR->create_string_f({574.0, 182.0, 0.0}, "/%d.",
                                     GLOBALS.inner.MAXIMUM_POWER);

  ASCII_MANAGER_PTR->scale = {0.6, 0.6};
  ASCII_MANAGER_PTR->color = {0xff, 0x80, 0, vm->color_1.a};
  ASCII_MANAGER_PTR->group = 0;
  ASCII_MANAGER_PTR->font_id = 4;
  ASCII_MANAGER_PTR->alignment_mode_h = 1;
  ASCII_MANAGER_PTR->alignment_mode_v = 1;
  ASCII_MANAGER_PTR->create_string_f({606.0, 189.0, 0.0}, "00");

  ASCII_MANAGER_PTR->scale = {1.0, 1.0};
  ASCII_MANAGER_PTR->color = {0x40, 0xc0, 0xff, vm->color_1.a};
  ASCII_MANAGER_PTR->group = 0;
  ASCII_MANAGER_PTR->font_id = 4;
  ASCII_MANAGER_PTR->alignment_mode_h = 2;
  ASCII_MANAGER_PTR->alignment_mode_v = 1;
  ASCII_MANAGER_PTR->comma_separated_number(
      {620.0, 204.0, 0.0},
      GLOBALS.inner.CURRENT_PIV / 100 - (GLOBALS.inner.CURRENT_PIV / 100) % 10);

  ASCII_MANAGER_PTR->scale = {1.0, 1.0};
  ASCII_MANAGER_PTR->color = {0xff, 0xff, 0xff, vm->color_1.a};
  ASCII_MANAGER_PTR->group = 0;
  ASCII_MANAGER_PTR->font_id = 4;
  ASCII_MANAGER_PTR->alignment_mode_h = 2;
  ASCII_MANAGER_PTR->alignment_mode_v = 1;
  ASCII_MANAGER_PTR->comma_separated_number({620.0, 226.0, 0.0},
                                            GLOBALS.inner.GRAZE);

  if (!ENEMY_MANAGER_PTR)
    return 1;
  if (remaining_spell_time_seconds < 0)
    return 1;
  if (!ENEMY_MANAGER_PTR->getBoss(0))
    return 1;
  // if (ENEMY_MANAGER_PTR->flag_0_is_life_hide & 1)
  //   return 1;
  // if (msg)
  //   return 1;
  // if (GAME_THREAD_PTR->flags & 0x10000U)
  //   return 1;

  ASCII_MANAGER_PTR->scale = {1.0, 1.0};
  ASCII_MANAGER_PTR->color = vm_timer_digit_hi->color_1;
  ASCII_MANAGER_PTR->group = 2;
  ASCII_MANAGER_PTR->font_id = 4;
  ASCII_MANAGER_PTR->alignment_mode_h = 1;
  ASCII_MANAGER_PTR->alignment_mode_v = 1;
  ASCII_MANAGER_PTR->create_string_f(
      {vm_timer_digit_hi->pos.x + 16.0, vm_timer_digit_hi->pos.y - 7.0, 0.0},
      ".");

  ASCII_MANAGER_PTR->scale = {0.6, 0.6};
  ASCII_MANAGER_PTR->color = vm_timer_digit_hi->color_1;
  ASCII_MANAGER_PTR->group = 2;
  ASCII_MANAGER_PTR->font_id = 4;
  ASCII_MANAGER_PTR->alignment_mode_h = 1;
  ASCII_MANAGER_PTR->alignment_mode_v = 1;
  ASCII_MANAGER_PTR->create_string_f(
      {vm_timer_digit_hi->pos.x + 24.0, vm_timer_digit_hi->pos.y - 1.0, 0.0},
      "%.2d", remaining_spell_time_centiseconds);

  return 1;
}

void Gui::midScreenInfo(int bonus, int type) {

  switch (type) {
  case 0: {
    AnmManager::deleteVM(some_anmid_0xc8);
    some_anmid_0xc8 = AnmManager::SpawnVM(front_anm->getSlot(), 0x3d);
    bool fstnumfound = false;
    int pow10 = 10000000;
    int curbonus = bonus;
    for (int i = 0; i < 8; i++) {
      AnmManager::deleteVM(spellcard_score_anmids[i]);

      spellcard_score_anmids[i] =
          AnmManager::SpawnVM(ASCII_MANAGER_PTR->ascii_anm->getSlot(), i + 4);
      if (curbonus / pow10 != 0) {
        fstnumfound = true;
      }
      if (auto vm = AnmManager::getVM(spellcard_score_anmids[i]); vm) {
        AnmManager::getLoaded(vm->anm_loaded_index)
            ->setSprite(vm, 0xef + curbonus / pow10);
        if (!fstnumfound) {
          vm->clear_flag_1_rec();
        } else {
          vm->set_flag_1_rec();
        }
      }
      curbonus %= pow10;
      pow10 /= 10;
    }

    AnmManager::deleteVM(spellcard_score_anmids[8]);
    spellcard_score_anmids[8] = 0;
    if (999999 < bonus) {
      spellcard_score_anmids[8] =
          AnmManager::SpawnVM(ASCII_MANAGER_PTR->ascii_anm->getSlot(), 0xc);
      if (auto vm = AnmManager::getVM(spellcard_score_anmids[8]); vm) {
        AnmManager::getLoaded(vm->anm_loaded_index)->setSprite(vm, 0xfd);
      }
    }
    AnmManager::deleteVM(spellcard_score_anmids[9]);
    spellcard_score_anmids[9] = 0;
    if (999 < bonus) {
      spellcard_score_anmids[9] =
          AnmManager::SpawnVM(ASCII_MANAGER_PTR->ascii_anm->getSlot(), 0xd);
      if (auto vm = AnmManager::getVM(spellcard_score_anmids[9]); vm) {
        AnmManager::getLoaded(vm->anm_loaded_index)->setSprite(vm, 0xfd);
      }
    }
    field_0x144_spellcard_related = 1;
    anmid_0x120_spellTimeShowEnd =
        AnmManager::SpawnVM(front_anm->getSlot(), 0x60);
  } break;
  case 1:
    AnmManager::deleteVM(some_anmid_0xc8);
    some_anmid_0xc8 = AnmManager::SpawnVM(front_anm->getSlot(), 0x3e);
    field_0x144_spellcard_related = 1;
    anmid_0x120_spellTimeShowEnd =
        AnmManager::SpawnVM(front_anm->getSlot(), 0x60);
    break;
  case 2:
    AnmManager::deleteVM(some_anmid_0xcc);
    some_anmid_0xcc = AnmManager::SpawnVM(front_anm->getSlot(), 0x3f);
    break;
  case 3:
    AnmManager::deleteVM(some_anmid_0xcc);
    some_anmid_0xcc = AnmManager::SpawnVM(front_anm->getSlot(), 0x40);
    break;
  case 4:
    AnmManager::deleteVM(some_anmid_0xcc);
    some_anmid_0xcc = AnmManager::SpawnVM(front_anm->getSlot(), 0x41);
    break;
  case 6:
    AnmManager::deleteVM(some_anmid_0xc8);
    some_anmid_0xc8 = AnmManager::SpawnVM(front_anm->getSlot(), 0x42);
    break;
  }
}

// void FUN_0042a400(void) {
//   int32_t *piVar1;
//   undefined *puVar2;
//   zAnmLoaded *pzVar3;
//   undefined4 uVar4;
//   zAnmVM *pzVar5;
//   int32_t iVar6;
//   uint uVar7;
//   undefined4 *puVar8;
//   zAnmVM *pzVar9;
//   zAnmVM **ppzVar10;
//   zAnmId *pzVar11;
//   zLinkedList<zAnmVM> *pzVar12;
//   int iVar13;
//   zAnmVM **local_50;
//   zAnmVM **local_4c;
//   zAnmId local_3c;
//   zAnmId zStack_38;
//   zAnmId zStack_34;
//   zAnmId zStack_30;
//   undefined4 local_2c;
//   float fStack_28;
//   float local_24;
//   undefined local_20[16];
//
//   if (GUI_PTR->on_tick)
//     GUI_PTR->on_tick->flags |= 2;
//   if (GUI_PTR->on_draw_52)
//     GUI_PTR->on_draw_52->flags |= 2;
//   if (GUI_PTR->on_draw_49)
//     GUI_PTR->on_draw_49->flags |= 2;
//   if (GUI_PTR->frame_anmid == 0) {
//     GUI_PTR->frame_anmid = GUI_PTR->front_anm->new_vm_ui_back(0);
//   }
//
//   if (!GUI_PTR->life_meter_vms[0]) {
//     for (int i = 0; i < 8; i++) {
//       GUI_PTR->life_meter_anmids[i] =
//           new_vm_ui_back(GUI_PTR->front_anm, i + 0x1e);
//       GUI_PTR->life_meter_vms[i] =
//           get_vm_with_id(GUI_PTR->bomb_meter_anmids[i]);
//     }
//
//     for (int i = 0; i < 8; i++) {
//       GUI_PTR->bomb_meter_anmids[i] =
//           new_vm_ui_back(GUI_PTR->front_anm, i + 0x26);
//       GUI_PTR->bomb_meter_vms[i] =
//           get_vm_with_id(GUI_PTR->bomb_meter_anmids[i]);
//     }
//
//     GUI_PTR->timer_digit_hi_anmid =
//         anm_new_mocc_0_wlb(ASCII_MANAGER_PTR->ascii_anm, 2);
//     GUI_PTR->vm_timer_digit_hi =
//         ANM_MANAGER_PTR->get_vm_with_id(GUI_PTR->timer_digit_hi_anmid);
//     GUI_PTR->vm_timer_digit_hi->prefix.field_0x534 &= 0xfff3ffff;
//     GUI_PTR->vm_timer_digit_hi->clear_flag_1_recursive();
//
//     GUI_PTR->timer_digit_lo_anmid =
//         anm_new_mocc_0_wlb(ASCII_MANAGER_PTR->ascii_anm, 3);
//     GUI_PTR->vm_timer_digit_lo =
//         ANM_MANAGER_PTR->get_vm_with_id(GUI_PTR->timer_digit_lo_anmid);
//     GUI_PTR->vm_timer_digit_lo->prefix.field_0x534 &= 0xfff3ffff;
//     GUI_PTR->vm_timer_digit_lo->clear_flag_1_recursive();
//   }
//
//   GUI_PTR->sub_42fc60(GLOBALS.inner.CURRENT_LIVES,
//   GLOBALS.inner.field26_0x68);
//   GUI_PTR->sub_42fd50(GLOBALS.inner.CURRENT_BOMBS,
//                       GLOBALS.inner.CURRENT_BOMB_PIECES);
//
//   if (((SUPERVISOR.gamemode_to_switch_to != 8) &&
//        ((GLOBALS.FLAGS & 0x40U) == 0)) &&
//       (((byte)GLOBALS.FLAGS & 0x30) != 0x20)) {
//     // Stage bgm anm
//     anm_new_mocc_0_wlb(GUI_PTR->stage_logo_anmloaded, 1);
//   }
//
//   if (GLOBALS.FLAGS & 0x40) {
//     // Demo play ?
//     anm_new_mocc_0_wlb(GUI_PTR->front_anm, 0x7a);
//   }
//
//   if (GUI_PTR->boss_marker_anmid == 0) {
//     GUI_PTR->boss_marker_anmid = anm_new_mocc_0_wlb(GUI_PTR->front_anm,
//     0x70);
//   }
//
//   // Item get border line
//   if (((GLOBALS.inner.STAGE_NUM == 1) &&
//        (GAME_THREAD_PTR->field18_0xa8 == 0)) &&
//       (GLOBALS.inner.CONTINUES_USED == 0)) {
//     auto vm = ANM_MANAGER_PTR->get_vm_with_id(
//         anm_new_mocc_0_wlb(GUI_PTR->front_anm, 0x45));
//     if (vm)
//       vm->suffix.entity_pos.y = (GLOBALS.inner.CHARACTER == 1) ? 0x94 : 0x80;
//   }
//
//   if (SUPERVISOR.field61_0x700) {
//     GUI_PTR->difficulty_screen_anmid =
//         anm_new_mocc_0_wlb(GUI_PTR->front_anm, GLOBALS.inner.DIFFICULTY +
//         0x51);
//     ANM_MANAGER_PTR->interrupt_tree(GUI_PTR->some_anmid_0x104, 3);
//   }
//
//   GUI_PTR->difficulty_side_anmid =
//       anm_new_mocc_0_wlb(GUI_PTR->front_anm, GLOBALS.inner.DIFFICULTY +
//       0x57);
//   GUI_PTR->player_shottype_anmid = anm_new_mocc_0_wlb(
//       GUI_PTR->front_anm,
//       GLOBALS.inner.SHOTTYPE + GLOBALS.inner.CHARACTER * 3 + 0x71);
//   GUI_PTR->boss_bars[0].vms_created = 0;
//   GUI_PTR->boss_bars[1].vms_created = 0;
//   GUI_PTR->field111_0x2c8 = 0;
//   GUI_PTR->stars_nb = 0;
//   if (GUI_PTR->field50_0x110) {
//     ANM_MANAGER_PTR->interrupt_tree(GUI_PTR->field50_0x110, 1);
//     GUI_PTR->field50_0x110 = 0;
//   }
//   return;
// }

void Gui::set_bomb_meter(int bombs, int bomb_pieces) {
  if (!this->bomb_meter_vms[0])
    return;

  if (bombs < 0)
    bombs = 0;

  for (int i = 0; i < bombs; i++)
    this->bomb_meter_vms[i]->interrupt(2);

  if (7 < bombs)
    return;

  static const int interuptsByPieceCount[] = {0, 2, 4};

  this->bomb_meter_vms[bombs]->interrupt(interuptsByPieceCount[bomb_pieces] +
                                         7);

  for (int i = bombs + 1; i < 8; i++)
    this->bomb_meter_vms[i]->interrupt(3);
  return;
}

void Gui::set_life_meter(int lives, int life_pieces) {
  if (!this->life_meter_vms[0])
    return;

  if (lives < 0)
    lives = 0;

  for (int i = 0; i < lives; i++)
    this->life_meter_vms[i]->interrupt(2);

  if (7 < lives)
    return;

  static const int interuptsByPieceCount[] = {0, 2, 3};

  this->life_meter_vms[lives]->interrupt(interuptsByPieceCount[life_pieces] +
                                         7);

  for (int i = lives + 1; i < 8; i++)
    this->life_meter_vms[i]->interrupt(3);
  return;
}
