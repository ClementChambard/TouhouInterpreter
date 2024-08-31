#include "./Gui.hpp"
#include "./Anm/AnmManager.h"
#include "./AsciiManager.hpp"
#include "./Ecl/EnemyManager.h"
#include "./GlobalData.h"
#include "./Hardcoded.h"
#include "./Player.h"
#include "./Spellcard.h"

#include <logger.h>
#include <platform/filesystem.h>
#include <cstdlib>
#include <cstring>
#include <memory.h>

Gui *GUI_PTR = nullptr;

uint8_t* DAT_0052a320 = nullptr;

static usize msg_file_data_size = 0;

Gui::Gui() {
  GUI_PTR = this;
  on_tick = new UpdateFunc([this]() { return this->f_on_tick(); });
  UPDATE_FUNC_REGISTRY.register_on_tick(on_tick, 10);
  on_draw_49 = new UpdateFunc([this]() { return this->f_on_draw(); });
  UPDATE_FUNC_REGISTRY.register_on_draw(on_draw_49, 49);
  on_draw_52 = new UpdateFunc([]() { return 1; });
  UPDATE_FUNC_REGISTRY.register_on_draw(on_draw_52, 52);
  front_anm = anm::loadFile(5, "front.anm");
  cstr stage_logo_filename =
      STAGE_DATA_TABLE[GLOBALS.inner.STAGE_NUM]["stage_logo_anm_filename"]
          .asCString();
  stage_logo_anmloaded = anm::loadFile(6, stage_logo_filename);

  if (DAT_0052a320 == NULL) {
    auto fname = STAGE_DATA_TABLE[GLOBALS.inner.STAGE_NUM]["msg_filenames"]
      [PLAYERS[GLOBALS.inner.CHARACTER]["shottypes"][GLOBALS.inner.SHOTTYPE]["msg"].asInt()].asString();
    ns::fs::File f;
    ns::fs::open(fname.c_str(), ns::fs::Mode::READ, true, &f);
    ns::fs::fsize(&f, &msg_file_data_size);
    msg_file_data = ns::alloc_n<byte>(msg_file_data_size, ns::MemTag::GAME);
    ns::fs::read_all_bytes(&f, msg_file_data, &msg_file_data_size);
    ns::fs::close(&f);
    NS_INFO("opened %s: %lld bytes", fname.c_str(), msg_file_data_size);
  } else {
    msg_file_data = DAT_0052a320;
    DAT_0052a320 = NULL;
  }

  current_score = GLOBALS.inner.CURRENT_SCORE;
  remaining_spell_time_seconds = -1;
  old_remaining_spell_time_seconds = -1;


  FUN_0042a400();
}

Gui::~Gui() {
  for (int i = 0; i < 8; i++) {
    anm::delete_vm(life_meter_anmids[i]);
    anm::delete_vm(bomb_meter_anmids[i]);
  }
  anm::delete_vm(timer_digit_hi_anmid);
  anm::delete_vm(timer_digit_lo_anmid);
  anm::delete_vm(boss_marker_anmid);
  for (int i = 0; i < 10; i++) {
    anm::delete_vm(spellcard_score_anmids[i]);
    anm::delete_vm(stars_anmids[i]);
  }
  anm::delete_vm(some_anmid_0xc8);
  anm::delete_vm(some_anmid_0xcc);
  anm::delete_vm(boss_name_anmid);
  anm::delete_vm(difficuty_screen_anmid);
  anm::delete_vm(difficuty_side_anmid);
  anm::delete_vm(player_shottype_anmid);
  anm::delete_vm(anmid_0x110_msgRelated);
  anm::delete_vm(stage_clear_anmid);
  anm::delete_vm(some_anmid_0x11c);
  anm::delete_vm(anmid_0x120_spellTimeShowEnd);
  anm::delete_vm(th15_chapter_end_anmid);
  anm::delete_vm(frame_anmid);

  stage_logo_anmloaded->Cleanup();
  front_anm->Cleanup();

  if (msg_file_data) ns::free_n(msg_file_data, msg_file_data_size, ns::MemTag::GAME);
  msg_file_data_size = 0;

  if (on_tick)
    UPDATE_FUNC_REGISTRY.unregister(on_tick);
  if (on_draw_52)
    UPDATE_FUNC_REGISTRY.unregister(on_draw_52);
  if (on_draw_49)
    UPDATE_FUNC_REGISTRY.unregister(on_draw_49);
}

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
        th15_chapter_kill_pc = field_0x130;
        field_0x13c = 0;
        th15_chapter_total_bonus = field_0x134;
        some_flags_1a4 = (some_flags_1a4 & 0xfffff7ff) | 0x1000;
      } else {
        if (timer_chapter_end % 4 == 0) {
          // SoundManager::play_sound_centered(0x27);
        }
        th15_chapter_total_bonus += field_0x13c;
        th15_chapter_kill_pc -= 1;
      }
    }
    if (th15_some_time_value_1bc <= timer_chapter_end) {
      anm::interrupt_tree(th15_chapter_end_anmid, 1);
      timer_chapter_end = 0;
      some_flags_1a4 &= 0xffffe7ff;
    }
  }

  if (field_0x144_spellcard_related &&
      !anm::get_vm(anmid_0x120_spellTimeShowEnd)) {
    anmid_0x120_spellTimeShowEnd = 0;
    field_0x144_spellcard_related = 0;
  }

  if (!ENEMY_MANAGER_PTR || remaining_spell_time_seconds < 0 ||
      !ENEMY_MANAGER_PTR->EnmFind(ENEMY_MANAGER_PTR->boss_ids[0]) ||
      (ENEMY_MANAGER_PTR->flags & 1) || msg /*||
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
          // run
          vm_timer_digit_hi->update();
          vm_timer_digit_lo->update();
        } else {
          vm_timer_digit_hi->interrupt(2);
          vm_timer_digit_lo->interrupt(2);
          // run
          vm_timer_digit_hi->update();
          vm_timer_digit_lo->update();
        }
        vm_timer_digit_hi->interrupt(4);
        vm_timer_digit_lo->interrupt(4);
        // run
        vm_timer_digit_hi->update();
        vm_timer_digit_lo->update();
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
      anm::getLoaded(vm_timer_digit_hi->anm_loaded_index)
          ->setSprite(vm_timer_digit_hi,
                      remaining_spell_time_seconds / 10 + 239);
      anm::getLoaded(vm_timer_digit_lo->anm_loaded_index)
          ->setSprite(vm_timer_digit_lo,
                      remaining_spell_time_seconds % 10 + 239);
    }
    old_remaining_spell_time_seconds = remaining_spell_time_seconds;
  }

  if (ENEMY_MANAGER_PTR && !(ENEMY_MANAGER_PTR->flags & 1)) {
    for (int bbid = 0; bbid < 2; bbid++) {
      Enemy *boss =
          ENEMY_MANAGER_PTR->EnmFind(ENEMY_MANAGER_PTR->boss_ids[bbid]);
      if (!boss) {
        boss_bars[bbid].visual_bar_pc = 0;
        boss_bars[bbid].markers[0] = 0;
        boss_bars[bbid].markers[2] = 0;
        boss_bars[bbid].markers[4] = 0;
        boss_bars[bbid].markers[6] = 0;
        if (boss_bars[bbid].vms_created) {
          for (int i = 0; i < 7; i++) {
            anm::delete_vm(boss_bars[bbid].anm_ids[i]);
            boss_bars[bbid].anm_ids[i] = 0;
          }
          boss_bars[bbid].vms_created = false;
        }
        if (bbid == 0) {
          anm::delete_vm(boss_name_anmid);
          boss_name_anmid = 0;
        }
      } else {
        if (boss->getData()->life.current < 100000 &&
            !(boss->getData()->flags & 0x31) &&
            boss->getData()->invFrame < 1 && !msg) {
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
            int bbf = GUI_ANMS["boss_bar_first"].asInt();
            boss_bars[bbid].anm_ids[0] =
                front_anm->createEffect(bbf);
            boss_bars[bbid].anm_ids[1] =
                front_anm->createEffect(bbf + 1);
            boss_bars[bbid].anm_ids[2] =
                front_anm->createEffect(bbf + 2);
            boss_bars[bbid].anm_ids[3] =
                front_anm->createEffect(bbf + 3);
            boss_bars[bbid].anm_ids[4] =
                front_anm->createEffect(bbf + 3);
            boss_bars[bbid].anm_ids[5] =
                front_anm->createEffect(bbf + 3);
            boss_bars[bbid].anm_ids[6] =
                front_anm->createEffect(bbf + 3);
            boss_bars[bbid].vms_created = true;
          }
          create_boss_name();
          anm::VM *bbvm = anm::get_vm(boss_bars[bbid].anm_ids[0]);
          if (!bbvm) {
            boss_bars[bbid].anm_ids[0] = 0;
          }
          bbvm->bitflags.rotated = true;
          bbvm->rotation.x = boss_bars[bbid].visual_bar_pc * -ns::PI_2<f32>;
          bbvm->entity_pos = boss->getData()->final_pos.pos * 2.f;
          bbvm = anm::get_vm(boss_bars[bbid].anm_ids[1]);
          if (!bbvm) {
            boss_bars[bbid].anm_ids[1] = 0;
          }
          bbvm->entity_pos = boss->getData()->final_pos.pos * 2.f;
          bbvm = anm::get_vm(boss_bars[bbid].anm_ids[2]);
          if (!bbvm) {
            boss_bars[bbid].anm_ids[2] = 0;
          }
          bbvm->entity_pos = boss->getData()->final_pos.pos * 2.f;
          for (int i = 3; i < 7; i++) {
            float pc = boss_bars[bbid].markers[(i - 3) * 2];
            bbvm = anm::get_vm(boss_bars[bbid].anm_ids[i]);
            if (!bbvm) {
              boss_bars[bbid].anm_ids[i] = 0;
            }
            if ((pc == 0.0) || boss_bars[bbid].visual_bar_pc <= pc) {
              bbvm->clear_flag_1_rec();
            } else {
              bbvm->set_flag_1_rec();
              bbvm->bitflags.rotated = true;
              bbvm->rotation.z = -ns::PI<f32> - pc * ns::PI_2<f32>;
              math::angle_normalize(bbvm->rotation.z);
              bbvm->entity_pos =
                  boss->getData()->final_pos.pos * 2.f +
                  ns::vec3(math::lengthdir_vec(112.f, -ns::PI_1_2<f32> - pc * ns::PI_2<f32>), 0.f);
            }
          }
          if (!boss_bars[bbid].vms_hidden_by_player) {
            if (math::point_distance_sq(PLAYER_PTR->inner.pos,
                                        boss->getData()->final_pos.pos) <
                6400.0) {
              for (int i = 0; i < 7; i++) {
                anm::interrupt_tree(boss_bars[bbid].anm_ids[i], 3);
              }
              boss_bars[bbid].vms_hidden_by_player = 1;
            }
          } else {
            if (math::point_distance_sq(PLAYER_PTR->inner.pos,
                                        boss->getData()->final_pos.pos) >=
                9216.0) {
              for (int i = 0; i < 7; i++) {
                anm::interrupt_tree(boss_bars[bbid].anm_ids[i], 2);
              }
              boss_bars[bbid].vms_hidden_by_player = 0;
            }
          }
        } else {
          if (boss_bars[bbid].vms_created) {
            for (int i = 0; i < 7; i++) {
              anm::delete_vm(boss_bars[bbid].anm_ids[i]);
            }
            boss_bars[bbid].vms_created = 0;
          }
        }
      }
    }
  }

  int starfirst = GUI_ANMS["stars_first"].asInt();
  for (int i = 0; i < 10; i++) {
    if (i < stars_nb) {
      if (stars_anmids[i].val == 0) {
        stars_anmids[i] =
            front_anm->createEffect(i + starfirst);
      }
    } else {
      if (stars_anmids[i].val != 0) {
        anm::interrupt_tree(stars_anmids[i], 1);
        stars_anmids[i] = 0;
      }
    }
  }

  if (msg) {
    if (!msg->tick()) {
      msg->__timer_4.increment();
    } else {
      if (msg)
        delete msg;
      msg = nullptr;
    }
  }

  __timer_14c++;

  if (!ENEMY_MANAGER_PTR)
    return 1;

  Enemy *boss0 = ENEMY_MANAGER_PTR->EnmFind(ENEMY_MANAGER_PTR->boss_ids[0]);
  anm::VM *vmBossMarker = anm::get_vm(boss_marker_anmid);
  if (boss0 && (~(boss0->getData()->flags >> 5) & 1) &&
      (~boss0->getData()->flags & 1)) {
    vmBossMarker->set_flag_1_rec();
    if (SPELLCARD_PTR->flags & 1) {
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
    } else {
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
    }
    vmBossMarker->entity_pos.y = 960.0;
    vmBossMarker->entity_pos.x =
        2 * (boss0->getData()->final_pos.pos.x + 32.0 + 192.0);
    vmBossMarker->color_1.a =
        math::min(ns::abs(boss0->getData()->final_pos.pos.x - PLAYER_PTR->inner.pos.x) *
                     191.f / 64.f +
                 64.f,
             255.f);
    if (ns::abs(boss0->getData()->final_pos.pos.x) > 192.0) {
      vmBossMarker->color_1.a = 0;
    }
  } else {
    vmBossMarker->clear_flag_1_rec();
  }
  return 1;
}

int Gui::f_on_draw() {
  anm::VM *stage_clear_vm = anm::get_vm(stage_clear_anmid);
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

  anm::VM *th15_chapter_end_vm = anm::get_vm(th15_chapter_end_anmid);
  if (!th15_chapter_end_vm) {
    th15_chapter_end_anmid = 0;
  } else {
    if (th15_chapter_end_vm->bitflags.visible) { // shoudl be f530_1
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
    anm::VM *pzVar7 = anm::get_vm(anmid_0x120_spellTimeShowEnd);
    if (pzVar7 == NULL) {
      anmid_0x120_spellTimeShowEnd = 0;
    }
    if (SPELLCARD_PTR == NULL) {
      field_0x144_spellcard_related = 0;
      anm::delete_vm(anmid_0x120_spellTimeShowEnd);
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
      int smthg = 0; // fmni(smth, 999);
      ASCII_MANAGER_PTR->create_string_f({224.0, 144.0, 0.0}, "%3d.", smthg);

      ASCII_MANAGER_PTR->scale = {0.6, 0.6};
      ASCII_MANAGER_PTR->color = {0xff, 0xff, 0xff, pzVar7->color_1.a};
      ASCII_MANAGER_PTR->group = 2;
      ASCII_MANAGER_PTR->font_id = 4;
      ASCII_MANAGER_PTR->alignment_mode_h = 1;
      ASCII_MANAGER_PTR->alignment_mode_v = 1;
      ASCII_MANAGER_PTR->create_string_f({268.0, 150.0, 0.0}, "%.2ds", smthg);
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
      int iVar9 = smthg;
      ASCII_MANAGER_PTR->create_string_f({224.0, 160.0, 0.0}, "%3d.", iVar9);

      ASCII_MANAGER_PTR->scale = {0.6, 0.6};
      ASCII_MANAGER_PTR->color = {0x80, 0x80, 0x80, pzVar7->color_1.a};
      ASCII_MANAGER_PTR->group = 2;
      ASCII_MANAGER_PTR->font_id = 4;
      ASCII_MANAGER_PTR->alignment_mode_h = 1;
      ASCII_MANAGER_PTR->alignment_mode_v = 1;
      int iVar10 = smthg;
      ASCII_MANAGER_PTR->create_string_f({268.0, 166.0, 0.0}, "%.2ds", iVar10);
    }
  }

  anm::VM *vm = life_meter_vms[0];

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
  current_score = GLOBALS.inner.CURRENT_SCORE;
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
  ASCII_MANAGER_PTR->create_string_f({576.f - local_58, 120.0, 0.0}, "%3d", 0);
  ASCII_MANAGER_PTR->create_string_f({597.f - local_58, 120.0, 0.0}, "/%d",
                                     /*(&ITEM_ANM_SCRIPT_IDS[17].id_1)[*/
                                     3 /*GLOBALS.inner.lifepiece_related ]*/);
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
                                     GLOBALS.inner.MAXIMUM_POWER /
                                         GLOBALS.inner.POWER_PER_LEVEL);

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
  if (!ENEMY_MANAGER_PTR->EnmFind(ENEMY_MANAGER_PTR->boss_ids[0]))
    return 1;
  if (ENEMY_MANAGER_PTR->flags & 1)
    return 1;
  if (msg)
    return 1;
  // If (GAME_THREAD_PTR->flags & 0x10000U)
  //   return 1;

  ASCII_MANAGER_PTR->scale = {1.0, 1.0};
  ASCII_MANAGER_PTR->color = vm_timer_digit_hi->color_1;
  ASCII_MANAGER_PTR->group = 2;
  ASCII_MANAGER_PTR->font_id = 4;
  ASCII_MANAGER_PTR->alignment_mode_h = 1;
  ASCII_MANAGER_PTR->alignment_mode_v = 1;
  ASCII_MANAGER_PTR->create_string_f(
      {vm_timer_digit_hi->pos.x + 16.f, vm_timer_digit_hi->pos.y - 7.f, 0.f},
      ".");

  ASCII_MANAGER_PTR->scale = {0.6, 0.6};
  ASCII_MANAGER_PTR->color = vm_timer_digit_hi->color_1;
  ASCII_MANAGER_PTR->group = 2;
  ASCII_MANAGER_PTR->font_id = 4;
  ASCII_MANAGER_PTR->alignment_mode_h = 1;
  ASCII_MANAGER_PTR->alignment_mode_v = 1;
  ASCII_MANAGER_PTR->create_string_f(
      {vm_timer_digit_hi->pos.x + 24.f, vm_timer_digit_hi->pos.y - 1.f, 0.f},
      "%.2d", remaining_spell_time_centiseconds);

  ASCII_MANAGER_PTR->scale = {1, 1};
  ASCII_MANAGER_PTR->color = {255, 255, 255, 255};
  ASCII_MANAGER_PTR->group = 0;
  ASCII_MANAGER_PTR->font_id = 0;
  ASCII_MANAGER_PTR->alignment_mode_h = 1;
  ASCII_MANAGER_PTR->alignment_mode_v = 1;
  return 1;
}

void Gui::midScreenInfo(int bonus, int type) {
  switch (type) {
  case 0: {
    anm::delete_vm(some_anmid_0xc8);
    some_anmid_0xc8 =
        front_anm->createEffect(GUI_ANMS["get_spell_card_bonus"].asInt());
    bool fstnumfound = false;
    int pow10 = 10000000;
    int curbonus = bonus;
    anm::VM *vm;
    for (int i = 0; i < 8; i++) {
      anm::delete_vm(spellcard_score_anmids[i]);

      spellcard_score_anmids[i] =
          ASCII_MANAGER_PTR->ascii_anm->createEffect(i + 4, -1, &vm);
      if (curbonus / pow10 != 0) {
        fstnumfound = true;
      }

      anm::getLoaded(vm->anm_loaded_index)
          ->setSprite(vm, 0xef + curbonus / pow10);
      if (!fstnumfound) {
        vm->clear_flag_1_rec();
      } else {
        vm->set_flag_1_rec();
      }
      curbonus %= pow10;
      pow10 /= 10;
    }

    anm::delete_vm(spellcard_score_anmids[8]);
    spellcard_score_anmids[8] = 0;
    if (999999 < bonus) {
      spellcard_score_anmids[8] =
          ASCII_MANAGER_PTR->ascii_anm->createEffect(0xc, -1, &vm);
      anm::getLoaded(vm->anm_loaded_index)->setSprite(vm, 0xfd);
    }
    anm::delete_vm(spellcard_score_anmids[9]);
    spellcard_score_anmids[9] = 0;
    if (999 < bonus) {
      spellcard_score_anmids[9] =
          ASCII_MANAGER_PTR->ascii_anm->createEffect(0xd, -1, &vm);
      anm::getLoaded(vm->anm_loaded_index)->setSprite(vm, 0xfd);
    }
    field_0x144_spellcard_related = 1;
    anmid_0x120_spellTimeShowEnd =
        front_anm->createEffect(GUI_ANMS["spellcard_endtime"].asInt());
  } break;
  case 1:
    anm::delete_vm(some_anmid_0xc8);
    some_anmid_0xc8 = front_anm->createEffect(GUI_ANMS["bonus_failed"].asInt());
    field_0x144_spellcard_related = 1;
    anmid_0x120_spellTimeShowEnd =
        front_anm->createEffect(GUI_ANMS["spellcard_endtime"].asInt());
    break;
  case 2:
    anm::delete_vm(some_anmid_0xcc);
    some_anmid_0xcc = front_anm->createEffect(GUI_ANMS["full_power"].asInt());
    break;
  case 3:
    anm::delete_vm(some_anmid_0xcc);
    some_anmid_0xcc = front_anm->createEffect(GUI_ANMS["high_score"].asInt());
    break;
  case 4:
    anm::delete_vm(some_anmid_0xcc);
    some_anmid_0xcc = front_anm->createEffect(GUI_ANMS["extend"].asInt());
    break;
  case 6:
    anm::delete_vm(some_anmid_0xc8);
    some_anmid_0xc8 =
        front_anm->createEffect(GUI_ANMS["challenge_next_stage"].asInt());
    break;
  }
}

void Gui::FUN_0042a400() {
  if (on_tick)
    on_tick->flags |= 2;
  if (on_draw_52)
    on_draw_52->flags |= 2;
  if (on_draw_49)
    on_draw_49->flags |= 2;
  if (frame_anmid.val == 0) {
    frame_anmid = front_anm->new_vm_ui_back(0);
  }

  if (!life_meter_vms[0]) {
    for (int i = 0; i < 8; i++) {
      life_meter_anmids[i] =
          front_anm->new_vm_ui_back(i + 0x1e, &life_meter_vms[i]);
      bomb_meter_anmids[i] =
          front_anm->new_vm_ui_back(i + 0x26, &bomb_meter_vms[i]);
    }

    timer_digit_hi_anmid =
        ASCII_MANAGER_PTR->ascii_anm->createEffect(2, -1, &vm_timer_digit_hi);
    timer_digit_lo_anmid =
        ASCII_MANAGER_PTR->ascii_anm->createEffect(3, -1, &vm_timer_digit_lo);
    vm_timer_digit_hi->clear_flag_1_rec();
    vm_timer_digit_lo->clear_flag_1_rec();
  }

  set_life_meter(GLOBALS.inner.CURRENT_LIVES,
                 GLOBALS.inner.CURRENT_LIFE_PIECES);
  set_bomb_meter(GLOBALS.inner.CURRENT_BOMBS,
                 GLOBALS.inner.CURRENT_BOMB_PIECES);

  if ((/*(SUPERVISOR.gamemode_to_switch_to != 8) &&*/
       ((GLOBALS.FLAGS & 0x40) == 0)) &&
      ((GLOBALS.FLAGS & 0x30) != 0x20)) {
    // Stage bgm anm
    stage_logo_anmloaded->createEffect(1);
  }

  if (GLOBALS.FLAGS & 0x40) {
    // Demo play
    front_anm->createEffect(GUI_ANMS["demo_play"].asInt());
  }

  if (boss_marker_anmid.val == 0) {
    boss_marker_anmid =
        front_anm->createEffect(GUI_ANMS["boss_indicator"].asInt());
  }

  // Item get border line
  if (GLOBALS.inner.STAGE_NUM == 1 &&
      /* GAME_THREAD_PTR->field18_0xa8 == 0 && */
      GLOBALS.inner.CONTINUES_USED == 0) {
    anm::VM *vm;
    front_anm->createEffect(GUI_ANMS["item_get_border_line"].asInt(), -1, &vm);
    if (vm)
      vm->entity_pos.y = (GLOBALS.inner.CHARACTER == 1) ? 0x94 : 0x80;
    // /\ not Hardcoded
  }

  // if (SUPERVISOR.field61_0x700) {
  difficuty_screen_anmid = front_anm->createEffect(
      GLOBALS.inner.DIFFICULTY + GUI_ANMS["difficulty_top_first"].asInt());
  anm::interrupt_tree(difficuty_screen_anmid, 3);
  // }

  difficuty_side_anmid = front_anm->createEffect(
      GLOBALS.inner.DIFFICULTY + GUI_ANMS["difficulty_side_first"].asInt());
  int shottype_side_fst = GUI_ANMS["shottype_side_first"].asInt();
  if (shottype_side_fst > 0)
    player_shottype_anmid = front_anm->createEffect(
        GLOBALS.inner.SHOTTYPE + GLOBALS.inner.CHARACTER * 3 +
        shottype_side_fst);
  boss_bars[0].vms_created = 0;
  boss_bars[1].vms_created = 0;
  boss_bars[2].vms_created = 0;
  stars_nb = 0;
  if (anmid_0x110_msgRelated) {
    anm::interrupt_tree(anmid_0x110_msgRelated, 1);
    anmid_0x110_msgRelated = 0;
  }
  return;
}

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

void Gui::create_boss_name() {
  auto vm = anm::get_vm(boss_name_anmid);
  if (!vm) {
    boss_name_anmid = 0;
    int id;
    if (GLOBALS.inner.CURRENT_CHAPTER < 41) {
      id = STAGE_DATA_TABLE[GLOBALS.inner.STAGE_NUM]["boss_data"][1]
        ["front_ename_anm_script"].asInt();
    } else {
      id = STAGE_DATA_TABLE[GLOBALS.inner.STAGE_NUM]["boss_data"][0]
        ["front_ename_anm_script"].asInt();
    }
    if (id >= 0 && (-1 < id + 167)) {
      boss_name_anmid = front_anm->createEffect(id + 167);
    }
  }
}
