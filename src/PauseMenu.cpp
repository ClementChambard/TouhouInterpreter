#include "./PauseMenu.hpp"
#include "./AsciiManager.hpp"
#include "AnmOpener/AnmManager.h"
#include "./GlobalData.h"
#include "./Supervisor.h"
#include "./Gui.hpp"
#include "AnmOpener/CopyTextures.hpp"
#include "Hardcoded.h"
#include <InputManager.h>
#include <TextureManager.h>
#include <cstdio>

PauseMenu* PAUSE_MENU_PTR = nullptr;

PauseMenu::PauseMenu() {
    PAUSE_MENU_PTR = this;
    flags_0 |= 2;
    on_tick = new UpdateFunc([this](){ return this->f_on_tick(); });
    on_draw = new UpdateFunc([this](){ return this->f_on_draw(); });
    UPDATE_FUNC_REGISTRY->register_on_tick(on_tick, 11);
    UPDATE_FUNC_REGISTRY->register_on_draw(on_draw, 75);
    timer4_0xc.reset();
    timer5_0x20.reset();
}

PauseMenu::~PauseMenu() {
    if (on_tick) UPDATE_FUNC_REGISTRY->unregister(on_tick);
    if (on_draw) UPDATE_FUNC_REGISTRY->unregister(on_draw);
    // ppzVar3 = this->field70_0x20c;
    // pzVar3 = 0x19;
    // do {
    //   ptr = *ppzVar3;
    //   if (ptr != NULL) {
    //     ReplayManager::destructor(ptr);
    //     ::operator_delete(ptr);
    //   }
    //   ppzVar3 = ppzVar3 + 1;
    //   pzVar3 = (int)&((zPauseMenu *)(pzVar3 + -0x3f8))->anmloaded_0x3f4 + 3;
    // } while ((zPauseMenu *)pzVar3 != NULL);
    PAUSE_MENU_PTR = nullptr;
}

static const char KB_CHARS[] =
    "ABCDEFGHIJKLM"
    "NOPQRSTUVWXYZ"
    "abcdefghijklm"
    "nopqrstuvwxyz"
    "0123456789+-="
    ".,!?@:;[]()_/"
    "{}|~^#$%&*   ";

void PauseMenu::draw_keyboard(glm::vec3 const& entry_pos) {
    ASCII_MANAGER_PTR->create_string_f(entry_pos, "%s", keyboard_entry);
    glm::vec3 pos = entry_pos;
    if (keyboard_entry_size != 8)
        pos.x += keyboard_entry_size * 9;
    ASCII_MANAGER_PTR->color = c_yellow;
    ASCII_MANAGER_PTR->create_string(pos, "_");
    ASCII_MANAGER_PTR->color = c_white;
    pos = {112.f, 320.f, 0.f};
    for (int i = 0; i < 91; i++) {
        if (keyboard_menu_helper.selection == i) {
            ASCII_MANAGER_PTR->color = c_yellow;
        } else {
            ASCII_MANAGER_PTR->color = c_hgray;
        }
        char c;
        if (i < 0x58) {
            c = KB_CHARS[i];
        } else if (i == 0x58) {
            c = 0x81;
        } else {
            c = (i != 0x59) + 0x7f;
        }
        ASCII_MANAGER_PTR->create_string_f(pos, "%c", c);
        if (i % 13 == 12) {
            pos.x = 112.f;
            pos.y += 16.f;
        } else {
            pos.x += 18.f;
        }
    }
    ASCII_MANAGER_PTR->color = c_white;
}

void PauseMenu::draw_save_replay() { /*
  int iVar1;
  D3DCOLOR *pDVar2;
  D3DCOLOR *pDVar3;
  D3DCOLOR *pDVar4;
  tm *ptVar5;
  D3DCOLOR DVar6;
  int *piVar7;
  zFloat3 local_18;

  piVar7 = &this->field70_0x20c;
  local_18.x = 48.0;
  local_18.z = 0.0;
  for (int i = 1; i < 26; i++) {
    ASCII_MANAGER_PTR->color = {128,128,128,255};
    if (menu_helper.next_selection == i + -1) {
      ASCII_MANAGER_PTR->color = {255,255,0,255};
    }
    local_18.y = 64.0 + 15.0 * i;
    if (field70_0x20c[i] == 0) {
      AsciiManager::create_string_f
                (local_18,"No.%.2d -------- --/--/-- ------- -- St-",i);
    } else {
      iVar1 = *(int *)(field70_0x20c[i] + 0x1c);
      ptVar5 = __localtime64((__time64_t *)(iVar1 + 0x10));
      if ((*(byte *)(iVar1 + 10) & 2) == 0) {
        AsciiManager::create_string_f
                  (local_18,"No.%.2d %s %.2d/%.2d/%.2d %s %s %s",i,iVar1,
                   ptVar5->tm_year % 100,ptVar5->tm_mon + 1,ptVar5->tm_mday,
                   (&CHAR_NAMES)[*(int *)(iVar1 + 0x88) * 3 + *(int *)(iVar1 + 0x8c)],
                   (&DIFF_ABREV)[*(int *)(iVar1 + 0x90)],
                   (&PTR_s_tst_004a0eb0)[*(int *)(iVar1 + 0x94)]);
      } else {
        AsciiManager::create_string_f
                  (ASCII_MANAGER_PTR,&local_18,"No.%.2d %s %.2d/%.2d/%.2d %s Sp %3d",i,iVar1,
                   ptVar5->tm_year % 100,ptVar5->tm_mon + 1,ptVar5->tm_mday,
                   (&CHAR_NAMES)[*(int *)(iVar1 + 0x88) * 3 + *(int *)(iVar1 + 0x8c)],
                   *(int *)(iVar1 + 0x9c) + 1);
      }
    }
  }
  ASCII_MANAGER_PTR->color = c_white;
  return;
*/ }

void PauseMenu::draw_score_rankings() { /*
  ASCII_MANAGER_PTR->create_string_f({48.0, 64.0, 0.0},
                          "            Score Ranking!!");
  int actualSelection = menu_helper.next_selection;
  if (field67_0x200 == 0) {
    draw_keyboard({75.0, menu_helper.next_selection * 18.0 + 96.0, 0.0});
  } else {
    actualSelection = -1;
  }
  for (int i = 0; i < 10; i++) {
    if (i == actualSelection) {
      ASCII_MANAGER_PTR->color = {255, 255, 0, 255};
    } else {
      ASCII_MANAGER_PTR->color = {128, 128, 128, 255};
    }
    iVar1 = GLOBALS.inner.SHOTTYPE + GLOBALS.inner.CHARACTER * 3;
    local_10 = (__time64_t *) (&SCOREFILE_PTR->shottype[iVar1].field10_0x28 + i * 8 + GLOBALS.inner.DIFFICULTY * 0x50);
    if ((*(uint *)local_10 | (&SCOREFILE_PTR->shottype[iVar1].field11_0x2c)[i * 8 + GLOBALS.inner.DIFFICULTY * 0x50]) == 0) {
      ASCII_MANAGER_PTR->create_string_f
                ({48.0, 96.0 + i * 18.0, 0.0},
                 "%2d %s %9ld%d --/--/-- Stage -",
                 i + 1,
                 (int)&SCOREFILE_PTR->shottype[iVar1].field7_0x1c + GLOBALS.inner.DIFFICULTY * 0x140 + i * 0x20 + 2,
                 (&SCOREFILE_PTR->shottype[iVar1].field6_0x18)[i * 8 + GLOBALS.inner.DIFFICULTY * 0x50],
                 (int)*(char *)((int)&SCOREFILE_PTR->shottype[iVar1].field7_0x1c + GLOBALS.inner.DIFFICULTY * 0x140 + i * 0x20 + 1));
    } else {
      ptVar12 = __localtime64(local_10);
      ASCII_MANAGER_PTR->create_string_f
                ({48.0, 96.0 + i * 18.0, 0.0},
                 "%2d %s %9ld%d %.2d/%.2d/%.2d %s",
                 i + 1,
                 (int)&SCOREFILE_PTR->shottype[iVar1].field7_0x1c + GLOBALS.inner.DIFFICULTY * 0x140 + i * 0x20 + 2,
                 (&SCOREFILE_PTR->shottype[iVar1].field6_0x18)[i * 8 + GLOBALS.inner.DIFFICULTY * 0x50],
                 (int)*(char *)((int)&SCOREFILE_PTR->shottype[iVar1].field7_0x1c + GLOBALS.inner.DIFFICULTY * 0x140 + i * 0x20 + 1),
                 ptVar12->tm_year % 100,
                 ptVar12->tm_mon + 1,
                 ptVar12->tm_mday,
                 (&PTR_s_test_004a0ef0)[*(char *)(&SCOREFILE_PTR->shottype[iVar1].field7_0x1c + i * 8 + GLOBALS.inner.DIFFICULTY * 0x50)]));
    }
  }
  ASCII_MANAGER_PTR->color = c_white;
*/ }

void PauseMenu::draw_thing_w_keyboard() { /*
  glm::vec3 zVar4;
  zVar4.x = 102.0;
  zVar4.z = 0.0;
  if (timer4_0xc.current < 10) {
    auto f = menu_helper.next_selection * 15.0 + 64.0;
    zVar4.y = ((224.0 - f) * timer4_0xc.current) / 10.0 + f;
  } else {
    zVar4.y = 224.0;
  }
  zVar4.y = fStack_c;
  draw_keyboard(zVar4);
  zVar4.x = 48.0;
  *REPLAY_MANAGER_PTR->field7_0x1c = 0x2020202020202020;
  *(undefined *)(REPLAY_MANAGER_PTR->field7_0x1c + 1) = 0;
  tm *ptVar1 = __localtime64((__time64_t *)(REPLAY_MANAGER_PTR->field7_0x1c + 0x10));
  if ((*(byte *)(REPLAY_MANAGER_PTR->field7_0x1c + 10) & 2) == 0) {
    AsciiManager::create_string_f
              (ASCII_MANAGER_PTR,zVar4,"No.%.2d %s %.2d/%.2d/%.2d %s %s %s",menu_helper.selection + 1,REPLAY_MANAGER_PTR->field7_0x1c,
               ptVar1->tm_year % 100,ptVar1->tm_mon + 1,ptVar1->tm_mday,
               (&CHAR_NAMES)[*(int *)(REPLAY_MANAGER_PTR->field7_0x1c + 0x88) * 3 + *(int *)(REPLAY_MANAGER_PTR->field7_0x1c + 0x8c)],
               (&DIFF_ABREV)[*(int *)(REPLAY_MANAGER_PTR->field7_0x1c + 0x90)],
               (&PTR_s_tst_004a0eb0)[*(int *)(REPLAY_MANAGER_PTR->field7_0x1c + 0x94)]);
  } else {
    AsciiManager::create_string_f
              (ASCII_MANAGER_PTR,zVar4,"No.%.2d %s %.2d/%.2d/%.2d %s Sp %3d",menu_helper.selection + 1,REPLAY_MANAGER_PTR->field7_0x1c,
               ptVar1->tm_year % 100,ptVar1->tm_mon + 1,ptVar1->tm_mday,
               (&CHAR_NAMES)[*(int *)(REPLAY_MANAGER_PTR->field7_0x1c + 0x88) * 3 + *(int *)(REPLAY_MANAGER_PTR->field7_0x1c + 0x8c)],
               *(int *)(REPLAY_MANAGER_PTR->field7_0x1c + 0x9c) + 1);
  }
*/ }

static bool check_input(int i) {
    switch (i) {
        case 0x10:
            return Inputs::Keyboard().Pressed(NSK_up);
        case 0x20:
            return Inputs::Keyboard().Pressed(NSK_down);
        case 0x40:
            return Inputs::Keyboard().Pressed(NSK_left);
        case 0x80:
            return Inputs::Keyboard().Pressed(NSK_right);
    }
    return false;
}

int PauseMenu::f_on_draw() {
  ASCII_MANAGER_PTR->draw_shadows = 1;
  auto vm = AnmManager::getVM(pause_blur_anmid);
  if (!vm) {
    pause_blur_anmid = 0;
  } else {
    int a = 57;
    if (TOUHOU_VERSION == 18) a = 64;
    vm = vm->search_children(a, 0);
    if (vm) {
      SUPERVISOR.arcade_vm_2__handles_upscaling->color_1 = vm->color_1;
      SUPERVISOR.arcade_vm_2__handles_upscaling->color_1.a = 255;
    }
  }
  if (field_1ec == 1 || field_1ec == 3) {
    if ((flags_3f0 & 3) == 2) {
      draw_thing_w_keyboard();
    } else if ((flags_3f0 & 3) == 1) {
      draw_save_replay();
    }
  } else if (field_1ec == 2) {
    if (submenu == 15) {
      draw_score_rankings();
    }
    if ((flags_3f0 & 3) == 1) {
      draw_save_replay();
    } else if ((flags_3f0 & 3) == 2) {
      draw_thing_w_keyboard();
    } else if (submenu != 14 && (GLOBALS.FLAGS & 0x30) != 0x20) {
      ASCII_MANAGER_PTR->create_string_f({184.0, 448.0, 0.0},
                            "Credit %d", GLOBALS.CREDIT);
    }
  }
  ASCII_MANAGER_PTR->draw_shadows = 0;
  return 1;
}

int PauseMenu::f_on_tick() {
    if (field_1ec != 0) {
        if (field_1ec - 1 < 3) {
            update_submenus();
        }
    } else if (false /*IS_ACHIEVEMENT_MODE >= 0*/) {
        // if ((INPUT_STRUCT.hardware_input_rising_edge & 0x100U) != 0) {
        //     IS_ACHIEVEMENT_MODE = -1;
        //     SUPERVISOR.gamemode_to_switch_to =
        //     (-(uint)((SUPERVISOR.flags & 0x2000U) != 0) & 0xfffffffe) + 4 ;
        // } else {
        //     timer4_0xc++;
        //     timer5_0x20++;
        // }
        return 1;
    } else if (!GAME_PAUSED
               /*!(GLOBALS.FLAGS & 0x40U) &&
               !(GAME_THREAD_PTR->flags & 0x10000U)*/
              ) {
        if (Inputs::Keyboard().Pressed(NSK_escape)
          /*(((INPUT_STRUCT.hardware_input_rising_edge & 0x100U) ||
             (SUPERVISOR.flags & 0x10)) && GAME_THREAD_PTR->on_tick) &&
           (GAME_THREAD_PTR->on_tick->flags & 2) &&
            0x1d < GAME_THREAD_PTR->field4_0x10*/
           ) {
          // run_time_related_431800(&GLOBALS);
          GAME_PAUSED = true;
          field_1f0_old_1ec = field_1ec;
          field_1ec = 1;
          submenu = 0;
          timer4_0xc = 0;
          timer5_0x20 = 0;
          // GAME_THREAD_PTR->flags |= 0x10;
          front_anm = GUI_PTR->front_anm;
          AnmManager::deleteVM(menu_anmid);
          // if (GAME_THREAD_PTR->field18_0xa8 == 0) {
          menu_anmid = front_anm->new_vm_ui_back(0x9f);
          // } else {
          //   menu_anmid = front_anm->new_vm_ui_back(0xa1);
          // }
          AnmManager::interrupt_tree(menu_anmid, 3);

          // FUN_00432660(extraout_ECX);

          // SoundManager::play_sound_centered(14);
          // if (((byte)GLOBALS.FLAGS & 0x30) != 0x20) {
          //   SoundManager::sub_4662e0(SOUND_MANAGER,6,0,"Pause");
          // }
          // while (sound_related_465660());

          init_pause_blur_effect();

          // saved_game_speed = GAME_SPEED;
          // GAME_SPEED = 1.0;
          // saved_dat_524778 = DAT_00524778;
          // DAT_00524778 = 0;
          // if (GUI_PTR->msg) GUI_PTR->msg->hide_anms();

          auto vm = AnmManager::getVM(GUI_PTR->th15_chapter_end_anmid);
          if (vm) vm->clear_flag_1_rec();
          flags_3f0 &= 0xfffffffb;
        }
    }
    timer4_0xc++;
    timer5_0x20++;
    return 1;
}

extern float SURF_ORIGIN_ECL_FULL_X;
extern float SURF_ORIGIN_ECL_FULL_Y;

void PauseMenu::init_pause_blur_effect() {
  AnmManager::deleteVM(pause_blur_anmid);
  AnmVM* vm;
  int a = 52;
  if (TOUHOU_VERSION == 18) a = 59;
  pause_blur_anmid = SUPERVISOR.text_anm->new_vm_ui_back(a, &vm);
  auto srcTexId = NSEngine::TextureManager::
    GetTextureNSIDByOpengl(SUPERVISOR.surface_atR_1->getColorTexture());
  auto texsiz = NSEngine::TextureManager::GetTexture(srcTexId)->getSize();
  auto spr = vm->getSprite();
  AnmManager::flush_vbos();
  CopyTextures::doCopy(srcTexId, spr.opengl_texid, {
      (SURF_ORIGIN_ECL_FULL_X - RESOLUTION_MULT * 384.0 * 0.5)       / texsiz.x,
      (SURF_ORIGIN_ECL_FULL_Y)                                       / texsiz.y,
      (SURF_ORIGIN_ECL_FULL_X + RESOLUTION_MULT * 384.0 * 0.5 - 1.0) / texsiz.x,
      (SURF_ORIGIN_ECL_FULL_Y + RESOLUTION_MULT * 448.0 - 1.0)       / texsiz.y
      }, { spr.u1, spr.v1, spr.u2, spr.v2 });
}

static bool menu_confirm() {
    // (INPUT_STRUCT.hardware_input_rising_edge & 0x80001U) != 0
    return Inputs::Keyboard().Pressed(NSK_w) || Inputs::Keyboard().Pressed(NSK_return);
}

#define child_interrupt(cid, inte) vm = AnmManager::getVM(menu_anmid)->search_children(cid, 0); if (vm) vm->interrupt(inte);
void PauseMenu::update_submenus() {
    AnmVM* vm;
    if (timer4_0xc.current % 60 == 0 && (SUPERVISOR.flags & 0x400U)) {
      // Supervisor::sub_463c90_inputRelated();
    }
    switch (submenu) {
    case 0:
      if (timer4_0xc < 10) break;
      go_to_submenu(6);
      menu_helper.num_choices = 5;
      // if (GAME_THREAD_PTR->field18_0xa8 != 0) {
      //   menu_helper.disable_option(2);
      //   menu_helper.disable_option(3);
      // }
      if (0 < GLOBALS.inner.CONTINUES_USED) {
        menu_helper.disable_option(2);
        AnmManager::interrupt_tree(menu_anmid, menu_helper.selection + 7);
        child_interrupt(0x85, 5)
        child_interrupt(0x8e, 5)
        child_interrupt(0x9a, 5)
      }
      menu_helper.option_wrapping = 1;
      menu_helper.select(0);
      AnmManager::interrupt_tree(menu_anmid, menu_helper.selection + 7);
      prev_submenu = 0;
      return;
    case 1:
      if (timer4_0xc < 10) break;
      go_to_submenu(6);
      menu_helper.num_choices = 5;
      menu_helper.disable_option(3);
      menu_helper.disable_option(2);
      menu_helper.disable_option(0);
      menu_helper.option_wrapping = 1;
      menu_helper.select(1);
      AnmManager::interrupt_tree(menu_anmid, menu_helper.selection + 7);
      prev_submenu = 1;
      return;
    case 2:
    case 3:
    case 4:
    case 5:
      if (timer4_0xc < 10) break;
      GUI_PTR->current_score = GLOBALS.inner.CURRENT_SCORE;
      if (GLOBALS.HIGH_SCORE < GLOBALS.inner.CURRENT_SCORE) {
        GLOBALS.HIGH_SCORE = GLOBALS.inner.CURRENT_SCORE;
      }
      if ((GLOBALS.FLAGS >> 4 & 3) != 2) {
        if ((GLOBALS.FLAGS >> 4 & 3) == 0) {
          // if ((GLOBALS.inner.STAGE_NUM == 7) && (this->field13_0x1fc != 0)) {
          //   GLOBALS.inner.STAGE_NUM = 9;
          // }
          // uVar24 = FUN_00443840((int)&SCOREFILE_PTR->shottype
          //                             [GLOBALS.inner.SHOTTYPE + GLOBALS.inner.CHARACTER * 3].
          //                             field2_0x8);
          // if ((GLOBALS.inner.STAGE_NUM == 9) && (this->field13_0x1fc != 0)) {
          //   GLOBALS.inner.STAGE_NUM = 7;
          // }
          // if (-1 < uVar24) {
          //   menu_helper.num_choices = 0x19;
          //   menu_helper.option_wrapping = 1;
          //   menu_helper.select(uVar24);
          //   keyboard_menu_helper.select(0);
          //   pcVar18 = keyboard_entry;
          //   puVar16 = &SCOREFILE_PTR->field_0x2d154;
          //   keyboard_menu_helper.num_choices = 0x5b;
          //   keyboard_menu_helper.option_wrapping = 1;
          //   pcVar17 = puVar16;
          //   do {
          //     cVar2 = *pcVar17;
          //     pcVar17 = pcVar17 + 1;
          //     pcVar17[(int)(pcVar18 + (-1 - (int)puVar16))] = cVar2;
          //   } while (cVar2 != '\0');
          //   keyboard_entry_size = 0;
          //   pcVar17 = "        ";
          //   uVar13 = 0;
          //   do {
          //     bVar3 = *pcVar18;
          //     bVar23 = bVar3 < (byte)*pcVar17;
          //     if (bVar3 != *pcVar17) {
          //       uVar13 = -(uint)bVar23 | 1;
          //       break;
          //     }
          //     if (bVar3 == 0) break;
          //     bVar3 = pcVar18[1];
          //     bVar23 = bVar3 < ((byte *)pcVar17)[1];
          //     if (bVar3 != ((byte *)pcVar17)[1]) {
          //       uVar13 = -(uint)bVar23 | 1;
          //       break;
          //     }
          //     pcVar18 = pcVar18 + 2;
          //     pcVar17 = (char *)((byte *)pcVar17 + 2);
          //   } while (bVar3 != 0);
          //   if (uVar13 != 0) {
          //     keyboard_menu_helper.select_rel(-1);
          //   }
          //   iVar15 = 8;
          //   do {
          //     if (keyboard_entry[iVar15 + -1] != ' ') break;
          //     iVar15 += -1;
          //   } while (0 < iVar15);
          //   keyboard_entry_size = iVar15;
          //   field_200 = 0;
          //   go_to_submenu(15);
          //   AnmManager::getVM(menu_anmid)->clear_flag_1();
          //   return;
          // }
        } else {
          // iVar15 = GLOBALS.inner.SHOTTYPE + GLOBALS.inner.CHARACTER * 3;
          // if ((uint)(&SCOREFILE_PTR->shottype[iVar15].field9570_0x46a0)
          //           [GLOBALS.inner.STAGE_NUM * 2 + GLOBALS.inner.DIFFICULTY * 0x10] <
          //     (uint)GLOBALS.inner.CURRENT_SCORE) {
          //   (&SCOREFILE_PTR->shottype[iVar15].field9570_0x46a0)
          //   [GLOBALS.inner.STAGE_NUM * 2 + GLOBALS.inner.DIFFICULTY * 0x10] =
          //        GLOBALS.inner.CURRENT_SCORE;
          // }
        }
      }
      field_200 = 1;
      go_to_submenu(6);
      menu_helper.num_choices = 5;
      menu_helper.option_wrapping = 1;
      if (!(flags_3f0 & 4) && !(GLOBALS.FLAGS & 0x30U)) {
        menu_anmid = front_anm->new_vm_ui_back(0xa3);
        if (GLOBALS.inner.CONTINUES_USED > 0) menu_helper.disable_option(2);
        if (GLOBALS.CREDIT < 1) menu_helper.disable_option(0);
      } else {
        menu_anmid = front_anm->new_vm_ui_back(0xa4);
        menu_helper.disable_option(0);
        menu_helper.disable_option(3);
        menu_helper.select(0);
        if (flags_3f0 & 4) {
            AnmManager::interrupt_tree_run(menu_anmid, 3);
            AnmManager::interrupt_tree(menu_anmid, menu_helper.selection + 7);
            return;
        }
      }
      menu_helper.select(4);
      AnmManager::interrupt_tree_run(menu_anmid, 3);
      AnmManager::interrupt_tree(menu_anmid, menu_helper.selection + 7);
      return;
    case 6:
      menu_helper.current_selection = menu_helper.selection;
      if (check_input(0x10)) {
        menu_helper.select_rel(-1);
      }
      if (check_input(0x20)) {
        menu_helper.select_rel(1);
      }
      if (menu_helper.current_selection != menu_helper.selection) {
        AnmManager::interrupt_tree(menu_anmid, menu_helper.selection + 7);
        // SoundManager::play_sound_centered(10);
      }
      if (menu_confirm()) {
        // SoundManager::play_sound_centered(7);
        switch (menu_helper.selection) {
        case 0:
          AnmManager::interrupt_tree(pause_blur_anmid, 1);
          AnmManager::interrupt_tree(menu_anmid, 1);
          go_to_submenu(16);
          break;
        case 1:
          child_interrupt(0x84, 6)
          child_interrupt(0x8d, 6)
          child_interrupt(0x95, 6)
          child_interrupt(0x97, 6)
          child_interrupt(0x99, 6)
          if (/*GAME_THREAD_PTR->field18_0xa8 ||*/ field_1ec != 1) {
            go_to_submenu(16);
          } else {
            go_to_submenu(7);
          }
          break;
        case 2:
          child_interrupt(0x85, 6)
          child_interrupt(0x8e, 6)
          child_interrupt(0x9a, 6)
          go_to_submenu((field_1ec != 1) + 9);
          break;
        case 3:
          child_interrupt(0x8f, 6)
          child_interrupt(0x86, 6)
          go_to_submenu(14);
          break;
        case 4:
          child_interrupt(0x87, 6)
          child_interrupt(0x90, 6)
          child_interrupt(0x96, 6)
          child_interrupt(0x98, 6)
          child_interrupt(0x9b, 6)
          if (/*GAME_THREAD_PTR->field18_0xa8 ||*/ field_1ec != 1) {
            go_to_submenu(16);
          } else {
            go_to_submenu(7);
          }
          return;
        }
        timer4_0xc = 0;
      }
      if (prev_submenu == 0) {
        // if (((INPUT_STRUCT.hardware_input_rising_edge & 0x200000U) != 0) &&
        //    (0 < menu_helper.disabled_option_count)) {
        //   int i = 0;
        //   do {
        //     if (menu_helper.disabled_options[i] == menu_helper.selection) {
        //       child_interrupt(0x87, 6)
        //       child_interrupt(0x90, 6)
        //       child_interrupt(0x96, 6)
        //       child_interrupt(0x98, 6)
        //       child_interrupt(0x9b, 6)
        //       if (/*GAME_THREAD_PTR->field18_0xa8 ||*/ field_1ec != 1) {
        //         go_to_submenu(16);
        //       } else {
        //         go_to_submenu(7);
        //       }
        //       return;
        //     }
        //   } while (i < menu_helper.disabled_option_count);
        // }
        if (Inputs::Keyboard().Pressed(NSK_escape)) {
          AnmManager::interrupt_tree(pause_blur_anmid, 1);
          AnmManager::interrupt_tree(menu_anmid, 1);
          menu_helper.select(0);
          go_to_submenu(16);
          return;
        }
      }
      if (!Inputs::Keyboard().Pressed(NSK_q)) break;
      // SoundManager::play_sound_centered(7);
      child_interrupt(0x84, 6)
      menu_helper.select(1);
      go_to_submenu(16);
      return;
    case 7:
    case 9:
      if (timer4_0xc < 0x14) break;
      if (timer4_0xc == 0x14) {
        menu_helper.push_selection();
        menu_helper.num_choices = 2;
        menu_helper.option_wrapping = 1;
        menu_helper.select(1);
        AnmManager::interrupt_tree(menu_anmid, 14);
      }
      if (timer4_0xc < 0x1e) break;
      if (timer4_0xc == 0x1e) {
        AnmManager::interrupt_tree(menu_anmid, menu_helper.selection + 15);
      }
      menu_helper.current_selection = menu_helper.selection;
      if (check_input(0x10) != 0) {
        menu_helper.select_rel(-1);
      }
      if (check_input(0x20) != 0) {
        menu_helper.select_rel(1);
      }
      if (menu_helper.current_selection != menu_helper.selection) {
        AnmManager::interrupt_tree(menu_anmid, menu_helper.selection + 15);
        // SoundManager::play_sound_centered(10);
      }
      if (menu_confirm()) {
        if (menu_helper.selection == 0) {
          child_interrupt(0x9d, 6)
          go_to_submenu((submenu == 9) * 2 + 8);
          // SoundManager::play_sound_centered(7);
        } else if (menu_helper.selection == 1) {
          child_interrupt(0x9e, 6)
          go_to_submenu(8);
          // SoundManager::play_sound_centered(9);
        }
      }
      if (Inputs::Keyboard().Pressed(NSK_escape) || Inputs::Keyboard().Pressed(NSK_x)) {
        // SoundManager::play_sound_centered(9);
        if (menu_helper.selection == 0) {
          menu_helper.select(1);
          AnmManager::interrupt_tree(menu_anmid, menu_helper.selection + 15);
        } else if (menu_helper.selection == 1) {
          child_interrupt(0x9e, 6)
          go_to_submenu(8);
        }
      }
      if (!Inputs::Keyboard().Pressed(NSK_escape) || (field_1ec != 1))
        break;
      AnmManager::interrupt_tree(pause_blur_anmid, 1);
      AnmManager::interrupt_tree(menu_anmid, 1);
      menu_helper.select(0);
      go_to_submenu(16);
      return;
    case 8:
      if (timer4_0xc < 0x14) break;
      if (menu_helper.selection != 0) {
        if (menu_helper.selection != 1) break;
        menu_helper.pop_selection();
        if (0 < GLOBALS.inner.CONTINUES_USED) menu_helper.disable_option(2);
        AnmManager::interrupt_tree(menu_anmid, menu_helper.selection + 7);
        go_to_submenu(6);
        return;
      }
      AnmManager::interrupt_tree(menu_anmid, 1);
      menu_helper.pop_selection();
      go_to_submenu(16);
      return;
    case 10:
      if (timer4_0xc < 0x14) break;
      flags_3f0 = (flags_3f0 & 0xfffffffd) | 1;
      go_to_submenu(11);
      AnmManager::getVM(menu_anmid)->clear_flag_1_rec();
      menu_helper.push_selection();
      menu_helper.num_choices = 0x19;
      menu_helper.option_wrapping = 1;
      menu_helper.select(0);
      // iVar15 = 1;
      // ppvVar22 = (void **)&this->field17_0x20c;
      // do {
      //   sprintf_related_404cd0(local_50,"th17_%.2d.rpy",iVar15);
      //   pvVar11 = FUN_0044e530(local_50);
      //   iVar15 += 1;
      //   *ppvVar22 = pvVar11;
      //   ppvVar22 = ppvVar22 + 1;
      // } while (iVar15 < 0x1a);
      return;
    case 11:
      if ((this->timer4_0xc).current < 10) break;
      (this->menu_helper).current_selection = (this->menu_helper).selection;
      if (check_input(0x10)) {
        menu_helper.select_rel(-1);
      }
      if (check_input(0x20) != 0) {
        menu_helper.select_rel(1);
      }
      if (menu_helper.current_selection != menu_helper.selection) {
        // SoundManager::play_sound_centered(10);
      }
      if (!menu_confirm()) {
        if (Inputs::Keyboard().Pressed(NSK_escape) || Inputs::Keyboard().Pressed(NSK_x)) {
          flags_3f0 &= 0xfffffffc;
          menu_helper.pop_selection();
          menu_helper.num_choices = 5;
          menu_helper.option_wrapping = 1;
          AnmManager::interrupt_tree(menu_anmid, menu_helper.selection + 7);
          // ppzVar21 = (zReplayManager **)&this->field17_0x20c;
          // local_54 = 0x19;
          // do {
          //   local_58 = *ppzVar21;
          //   if (local_58 != NULL) {
          //     ReplayManager::destructor(local_58);
          //     ::operator_delete(local_58);
          //   }
          //   *ppzVar21 = NULL;
          //   ppzVar21 = ppzVar21 + 1;
          //   local_54 += -1;
          // } while (local_54 != 0);
          if (field_1ec == 1) {
            go_to_submenu(16);
            menu_helper.select(1);
            // SoundManager::play_sound_centered(9);
            return;
          }
          go_to_submenu(6);
          AnmManager::getVM(menu_anmid)->set_flag_1_rec();
          if (GLOBALS.FLAGS & 0x30U) {
            menu_helper.disable_option(0);
            menu_helper.disable_option(3);
          }
          // SoundManager::play_sound_centered(9);
          return;
        }
        break;
      }
      flags_3f0 = (flags_3f0 & 0xfffffffe) | 2;
      go_to_submenu(12);
      keyboard_menu_helper.select(0);
      keyboard_menu_helper.num_choices = 0x5b;
      keyboard_menu_helper.option_wrapping = 1;
      // iVar15 = this->field13_0x1fc;
      // pzVar8 = REPLAY_MANAGER_PTR;
      // if ((iVar15 == 0) || ((GLOBALS.FLAGS & 0x30U) != 0)) {
      //   FID_conflict:__time64((__time64_t *)(REPLAY_MANAGER_PTR->field7_0x1c + 0x10));
      //   *(int32_t *)(pzVar8->field7_0x1c + 0x94) = GLOBALS.inner.STAGE_NUM;
      // } else {
      //   FUN_0044e6a0(1);
      // }
      // pcVar17 = this->keyboard_entry_0x2d4;
      // pcVar18 = &SCOREFILE_PTR->field_0x2d154;
      // iVar15 = (int)pcVar17 - (int)pcVar18;
      // do {
      //   cVar2 = *pcVar18;
      //   pcVar18[iVar15] = cVar2;
      //   pcVar18 = pcVar18 + 1;
      // } while (cVar2 != '\0');
      // keyboard_entry_size = 0;
      // pcVar18 = "        ";
      // uVar13 = 0;
      // do {
      //   bVar3 = *pcVar17;
      //   bVar23 = bVar3 < (byte)*pcVar18;
      //   if (bVar3 != *pcVar18) {
      //     uVar13 = -(uint)bVar23 | 1;
      //     break;
      //   }
      //   if (bVar3 == 0) break;
      //   bVar3 = pcVar17[1];
      //   bVar23 = bVar3 < ((byte *)pcVar18)[1];
      //   if (bVar3 != ((byte *)pcVar18)[1]) {
      //     uVar13 = -(uint)bVar23 | 1;
      //     break;
      //   }
      //   pcVar17 = pcVar17 + 2;
      //   pcVar18 = (char *)((byte *)pcVar18 + 2);
      // } while (bVar3 != 0);
      // if (uVar13 != 0) {
      //   keyboard_menu_helper.select_rel(-1);
      // }
      // iVar15 = 8;
      // do {
      //   if (keyboard_entry[iVar15 + -1] != ' ') break;
      //   iVar15 += -1;
      // } while (0 < iVar15);
      // keyboard_entry_size = iVar15;
      // SoundManager::play_sound_centered(7);
      return;
    case 0xc:
    case 0xf:
      if (timer4_0xc < 10) break;
      keyboard_menu_helper.current_selection = keyboard_menu_helper.selection;
      if (check_input(0x10)) {
        keyboard_menu_helper.select_rel(-13);
      }
      if (check_input(0x20)) {
        keyboard_menu_helper.select_rel(13);
      }
      if (check_input(0x40)) {
        if (keyboard_menu_helper.selection % 13 == 0) {
          keyboard_menu_helper.select_rel(12);
        } else {
          keyboard_menu_helper.select_rel(-1);
        }
      }
      if (check_input(0x80)) {
        if (keyboard_menu_helper.selection % 13 == 12) {
          keyboard_menu_helper.select_rel(-12);
        } else {
          keyboard_menu_helper.select_rel(1);
        }
      }
      if (keyboard_menu_helper.current_selection != keyboard_menu_helper.selection) {
        // SoundManager::play_sound_centered(10);
      }
      if (!menu_confirm()) {
        if (Inputs::Keyboard().Pressed(NSK_escape) || Inputs::Keyboard().Pressed(NSK_x)) {
          // SoundManager::play_sound_centered(9);
          if (keyboard_entry_size != 0) {
            keyboard_entry_size--;
            keyboard_entry[keyboard_entry_size - 1] = ' ';
            return;
          }
          if (submenu == 12) {
            flags_3f0 = (flags_3f0 & 0xfffffffd) | 1;
            go_to_submenu(11);
            return;
          }
        }
        break;
      }
      if (keyboard_menu_helper.selection < 0x58) {
        if (7 < keyboard_entry_size) {
          keyboard_entry[keyboard_entry_size - 1] = KB_CHARS[keyboard_menu_helper.selection];
          // SoundManager::play_sound_centered(7);
          return;
        }
        keyboard_entry[keyboard_entry_size] = KB_CHARS[keyboard_menu_helper.selection];
      } else {
        if (keyboard_menu_helper.selection != 0x58) {
          if (keyboard_menu_helper.selection == 0x59) {
            if (keyboard_entry_size != 0) {
              keyboard_entry_size--;
              keyboard_entry[keyboard_entry_size - 1] = ' ';
              // SoundManager::play_sound_centered(9);
              return;
            }
            break;
          }
          if (keyboard_menu_helper.selection != 0x5a) {
              // SoundManager::play_sound_centered(7);
              return;
          }
          if (submenu == 12) {
            flags_3f0 = (flags_3f0 & 0xfffffffd) | 1;
            // SoundManager::play_sound_centered(0x11);
            // sprintf(local_50, "th17_%.2d.rpy", menu_helper.selection + 1);
            // ReplayManager::operator_delete ((void *)(&this->field17_0x20c)[(this->menu_helper).selection]);
            // FUN_0044e6f0((DWORD)local_50,this->keyboard_entry_0x2d4,0,1);
            // iVar15 = (this->menu_helper).selection;
            // pvVar11 = FUN_0044e530(local_50);
            // (&this->field17_0x20c)[iVar15] = pvVar11;
            go_to_submenu(11);
            // pzVar9 = SCOREFILE_PTR;
            // pcVar17 = this->keyboard_entry_0x2d4;
            // do {
            //   cVar2 = *pcVar17;
            //   pcVar17[(int)pzVar9 + (0x2d154 - (int)this->keyboard_entry_0x2d4)] = cVar2;
            //   pcVar17 = pcVar17 + 1;
            // } while (cVar2 != '\0');
            // SoundManager::play_sound_centered(7);
            return;
          }
          // SoundManager::play_sound_centered(7);
          // pcVar1 = keyboard_entry;
          // pcVar18 = (char *)((int)&SCOREFILE_PTR->shottype
          //                          [GLOBALS.inner.SHOTTYPE + GLOBALS.inner.CHARACTER * 3].field7_0x1c
          //                   + (this->menu_helper).selection * 0x20 + GLOBALS.inner.DIFFICULTY * 0x140
          //                     + 2);
          // pcVar17 = pcVar1;
          // do {
          //   cVar2 = *pcVar17;
          //   pcVar17 = pcVar17 + 1;
          //   *pcVar18 = cVar2;
          //   pzVar9 = SCOREFILE_PTR;
          //   pcVar18 = pcVar18 + 1;
          // } while (cVar2 != '\0');
          // pcVar17 = pcVar1;
          // do {
          //   cVar2 = *pcVar17;
          //   pcVar17 = pcVar17 + 1;
          //   pcVar17[(int)pzVar9 + (0x2d153 - (int)pcVar1)] = cVar2;
          // } while (cVar2 != '\0');
          go_to_submenu(6);
          menu_helper.num_choices = 5;
          menu_helper.option_wrapping = 1;
          if (!(flags_3f0 & 4) && !(GLOBALS.FLAGS & 0x30U)) {
            menu_anmid = front_anm->new_vm_ui_back(0xa3);
            if (GLOBALS.inner.CONTINUES_USED > 0) menu_helper.disable_option(2);
            if (GLOBALS.CREDIT < 1) menu_helper.disable_option(0);
          } else {
            menu_anmid = front_anm->new_vm_ui_back(0xa4);
            menu_helper.disable_option(0);
            menu_helper.disable_option(3);
            menu_helper.select(0);
            if (flags_3f0 & 4) {
                AnmManager::interrupt_tree_run(menu_anmid, 3);
                AnmManager::interrupt_tree(menu_anmid, menu_helper.selection + 7);
                return;
            }
          }
          menu_helper.select(4);
          AnmManager::interrupt_tree_run(menu_anmid, 3);
          AnmManager::interrupt_tree(menu_anmid, menu_helper.selection + 7);
          return;
        }
        if (7 < keyboard_entry_size) {
          keyboard_entry[keyboard_entry_size - 1] = ' ';
          // SoundManager::play_sound_centered(7);
          return;
        }
        keyboard_entry[keyboard_entry_size] = ' ';
      }
      keyboard_entry_size++;
      if (7 < keyboard_entry_size) {
        keyboard_menu_helper.select(0x5a);
      }
      // SoundManager::play_sound_centered(7);
      return;
    case 0xe:
      if (timer4_0xc == 0x14) {
        AnmManager::getVM(menu_anmid)->clear_flag_1_rec();
        // HelpManual::operator_new();
        // HELP_MANUAL_PTR->field74_0x128 = 0x42000000;
      // }
      // if (HELP_MANUAL_PTR && (HELP_MANUAL_PTR->field73_0x124 != 0)) {
      //   delete HELP_MANUAL_PTR;
        go_to_submenu(6);
        AnmManager::getVM(menu_anmid)->set_flag_1_rec();
        return;
      }
      break;
    case 0x10:
      if (timer4_0xc < 12) break;
      if (field_1ec == 1) {
        // if (GAME_THREAD_PTR->field18_0xa8 == 0) {
        //   GLOBALS.inner.field49_0xf4 = sub_462710_get_runtime();
        // }
        // GAME_THREAD_PTR->flags = GAME_THREAD_PTR->flags & 0xffffffef;
        GAME_PAUSED = false;
        if (vm = AnmManager::getVM(GUI_PTR->th15_chapter_end_anmid); vm)
            vm->set_flag_1_rec();
        // if (GUI_PTR->msg) GUI_PTR->msg->unhide_anms();
        // GAME_SPEED = saved_game_speed;
        // DAT_00524778 = saved_dat_524778;
      } else if (field_1ec == 2 || field_1ec == 3) {
        // if (GAME_THREAD_PTR->field18_0xa8 == 0) {
        //   GLOBALS.inner.field49_0xf4 = sub_462710_get_runtime();
        // }
        AnmManager::interrupt_tree(pause_blur_anmid, 1);
        AnmManager::interrupt_tree(menu_anmid, 1);
        // GAME_SPEED = saved_game_speed;
      }
      if (menu_helper.selection == 0) {
        if (field_1ec == 1) {
          // FUN_00446120();
          // SoundManager::sub_4662e0((zSoundManager *)&SOUND_MANAGER,7,0,"UnPause");
        } else if (field_1ec == 2) {
          if (field_1fc != 0) {
            SUPERVISOR.gamemode_to_switch_to = 10;
            field_1f0_old_1ec = field_1ec;
            field_1ec = 0;
            submenu = 0;
            timer4_0xc = 0;
            timer5_0x20 = 0;
            return;
          }
          if (GLOBALS.inner.STAGE_NUM == 7) {
            SUPERVISOR.gamemode_to_switch_to = 0xe;
          } else {
            GLOBALS.inner.CURRENT_LIVES = 2;
            GLOBALS.inner.CURRENT_LIFE_PIECES = 0;
            GLOBALS.inner.CURRENT_BOMBS = 2;
            GLOBALS.inner.CURRENT_POWER = 0;
            GLOBALS.inner.CURRENT_BOMB_PIECES = 0;
            if (GLOBALS.inner.MAXIMUM_POWER < 0) {
              GLOBALS.inner.CURRENT_POWER = GLOBALS.inner.MAXIMUM_POWER;
            } else if (0 < GLOBALS.inner.POWER_PER_LEVEL) {
              GLOBALS.inner.CURRENT_POWER = GLOBALS.inner.POWER_PER_LEVEL;
            }
            // GlobalsInner::add_power(&GLOBALS.inner,GLOBALS.inner.POWER_PER_LEVEL * 4);
            // PlayerInner::sub_449630(&PLAYER_PTR->inner);
            GUI_PTR->set_life_meter(GLOBALS.inner.CURRENT_LIVES, GLOBALS.inner.CURRENT_LIFE_PIECES);
            GUI_PTR->set_bomb_meter(GLOBALS.inner.CURRENT_BOMBS, GLOBALS.inner.CURRENT_BOMB_PIECES);
            GLOBALS.inner.CONTINUES_USED += 1;
            GLOBALS.inner.CURRENT_SCORE = 0;
            if (9 < GLOBALS.inner.CONTINUES_USED) {
              GLOBALS.inner.CONTINUES_USED = 9;
            }
            GLOBALS.CREDIT--;
            GAME_PAUSED = false;
            // GAME_THREAD_PTR->flags = GAME_THREAD_PTR->flags & 0xffffffef;
            // FUN_00446120();
            // SoundManager::sub_4662e0
            //           (SOUND_MANAGER,2,0xffffffff,&field73_0x2f0);
            // while (sound_related_465660());
            // FUN_00478610(DAT_00529e88);
            // if (GUI_PTR->msg) GUI_PTR->msg->unhide_anms();
            // Gui::activate_chapter_end_vm();
            // GAME_SPEED = saved_game_speed;
            // DAT_00524778 = saved_dat_524778;
          }
        }
      } else if (menu_helper.selection == 1) {
        AnmManager::interrupt_tree(pause_blur_anmid, 1);
        AnmManager::interrupt_tree(menu_anmid, 1);
        // SUPERVISOR.gamemode_to_switch_to =
        // (-(uint)((SUPERVISOR.flags & 0x2000U) != 0) & 0xfffffffe) + 4;
      } else if (menu_helper.selection == 4) {
        AnmManager::deleteVM(pause_blur_anmid);
        AnmManager::deleteVM(menu_anmid);
        pause_blur_anmid = 0;
        menu_anmid = 0;
        // if (GAME_THREAD_PTR->field18_0xa8 == 0) {
        //   SUPERVISOR.gamemode_to_switch_to = 10;
        // } else {
        SUPERVISOR.gamemode_to_switch_to = 0xb;
        //}
      }
      field_1f0_old_1ec = field_1ec;
      field_1ec = 0;
      submenu = 0;
      timer4_0xc = 0;
      timer5_0x20 = 0;
    }
}

void PauseMenu::open_submenu1_finish_replay() {
  field_1f0_old_1ec = field_1ec;
  field_1ec = 1;
  timer4_0xc = 0;
  timer5_0x20 = 0;
  submenu = 1;
  GAME_PAUSED = true;
  // GAME_THREAD_PTR->flags |= 0x10;
  init_pause_blur_effect();
  front_anm = GUI_PTR->front_anm;
  AnmManager::deleteVM(menu_anmid);
  menu_anmid = front_anm->new_vm_ui_back(0xa2);
  AnmManager::interrupt_tree(menu_anmid, 3);
  // FUN_00432660(extraout_ECX);
  // SoundManager::sub_4662e0((zSoundManager *)&SOUND_MANAGER,6,0,"Pause");
  // saved_game_speed = GAME_SPEED;
  // GAME_SPEED = 1.0;
  // saved_dat_524778 = DAT_00524778;
  // DAT_00524778 = 0;
  flags_3f0 &= 0xfffffffb;
  return;
}

void PauseMenu::open_submenu2() {
  // run_time_related_431800(&GLOBALS);
  // if (GAME_THREAD_PTR->field18_0xa8 == 1) {
  //   SUPERVISOR.gamemode_to_switch_to = (-(uint)((SUPERVISOR.flags & 0x2000U) != 0) & 0xfffffffe) + 4 ;
  //   return;
  // }
  field_1f0_old_1ec = field_1ec;
  field_1ec = 2;
  timer4_0xc = 0;
  timer5_0x20 = 0;
  submenu = 2;
  GAME_PAUSED = true;
  // GAME_THREAD_PTR->flags |= 0x10;
  // FUN_00432660(extraout_ECX);
  // SoundManager::play_sound_centered(0xe);
  // if ((GLOBALS.FLAGS & 0x30) != 0x20) {
  //   SoundManager::sub_4662e0((zSoundManager *)&SOUND_MANAGER,6,0,"Pause");
  // }
  // while (sound_related_465660());
  init_pause_blur_effect();
  front_anm = GUI_PTR->front_anm;
  // if ((GLOBALS.FLAGS & 0x30) != 0x20) {
  //   pcVar4 = &DAT_00526b08;
  //   do {
  //     cVar1 = *pcVar4;
  //     pcVar4 = pcVar4 + 1;
  //     ((undefined *)((int)this[-0x14c4].keyboard_menu_helper.disabled_options + 0x2b))[(int)pcVar4]
  //          = cVar1;
  //   } while (cVar1 != '\0');
  //   dVar5 = FUN_00478550(DAT_00529e88);
  //   *(double *)&this->field46_0x2e8 = dVar5;
  //   song_related_442040(0,"th128_08");
  //   if ((SUPERVISOR.config._40_1_ & 0x10) != 0) {
  //     SoundManager::sub_4662e0((zSoundManager *)&SOUND_MANAGER,4,0,"dummy");
  //   }
  //   SoundManager::sub_4662e0((zSoundManager *)&SOUND_MANAGER,2,0,"dummy");
  //   SCOREFILE_PTR->field_0x2d16e = 1;
  // }
  field_1fc = 0;
  // saved_game_speed = GAME_SPEED;
  // GAME_SPEED = 1.0;
  // this->saved_dat_524778 = DAT_00524778;
  // DAT_00524778 = 1;
  flags_3f0 &= 0xfffffffb;
  return;
}


void PauseMenu::open_submenu3() {
  
  // run_time_related_431800(&GLOBALS);
  // if (GAME_THREAD_PTR->field18_0xa8 == 1) {
  //   SUPERVISOR.gamemode_to_switch_to = (-(uint)((SUPERVISOR.flags & 0x2000U) != 0) & 0xfffffffe) + 4 ;
  //   return;
  // }
  GAME_PAUSED = true;
  // GAME_THREAD_PTR->flags |= 0x10;
  field_1f0_old_1ec = field_1ec;
  field_1ec = 3;
  timer5_0x20 = 0;
  timer4_0xc = 0;
  submenu = 3;
  if ((GLOBALS.FLAGS & 0x30) == 0x20) {
    submenu = 5;
  }
  // if (!(GLOBALS.FLAGS & 0x30)) {
  //   p_vm_0x34_id = AnmLoaded::new_vm_ui_back(SUPERVISOR.text_anm,0x34);
  //   vm_0x34_id = p_vm_0x34_id->value;
  //   (this->text_vm_52_0x34_id_pause_effect).value = vm_0x34_id;
  //   fVar7 = RESOLUTION_MULT * 448.0;
  //   fVar8 = RESOLUTION_MULT * 384.0;
  //   scaled_32 = RESOLUTION_MULT * 32.0;
  //   scaled_16 = RESOLUTION_MULT * 16.0;
  //   pzVar6 = AnmManager::get_vm_with_id(vm_0x34_id);
  //   iVar4 = (pzVar6->prefix).anm_loaded_index;
  //   sprite_id = (pzVar6->prefix).sprite_id;
  //   sprites = *(zAnmLoadedSprite **)(*(int *)((int)&anm_mgr->preloaded + iVar4 * 4) + 0x11c);
  //   iVar5 = sprites[sprite_id].image_file_num_in_anm;
  //   fVar1 = sprites[sprite_id].sprite_height;
  //   fVar2 = sprites[sprite_id].sprite_width;
  //   fVar3 = sprites[sprite_id].start_pixel_inclusive.y;
  //   s_startx = sprites[sprite_id].start_pixel_inclusive.x;
  //   i = 0;
  //   val = AnmManager::pause_related;
  //   do {
  //     if (val->anm_loaded < 0) {
  //       anm_mgr->pause_related[i].anm_loaded = iVar4;
  //       anm_mgr->pause_related[i].image_id = iVar5;
  //       anm_mgr->pause_related[i].sprite_begin.x = (int)s_startx;
  //       anm_mgr->pause_related[i].r_32x16.x = (int)scaled_32;
  //       anm_mgr->pause_related[i].r_32x16.y = (int)scaled_16;
  //       anm_mgr->pause_related[i].field_size.x = (int)fVar8;
  //       anm_mgr->pause_related[i].field_size.y = (int)fVar7;
  //       anm_mgr->pause_related[i].sprite_begin.y = (int)fVar3;
  //       anm_mgr->pause_related[i].sprite_size.x = (int)fVar2;
  //       anm_mgr->pause_related[i].sprite_size.y = (int)fVar1;
  //       break;
  //     }
  //     i += 1;
  //     val = val + 1;
  //   } while (i < 4);
  // }
  front_anm = GUI_PTR->front_anm;
  field_1fc = 1;
  // saved_game_speed = GAME_SPEED;
  // GAME_SPEED = 1.0;
  // saved_dat_524778 = DAT_00524778;
  // DAT_00524778 = 1;
  // if (GUI_PTR->msg) GUI_PTR->msg->hide_anms();
  // if (auto vm = AnmManager::getVM(GUI_PTR->th15_chapter_end_anmid); vm) vm->clear_flag_1_rec();
  flags_3f0 |= 4;
  return;
}
