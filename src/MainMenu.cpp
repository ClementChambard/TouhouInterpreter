#include "./MainMenu.hpp"
#include "Anm/AnmManager.h"
#include "AsciiManager.hpp"
#include "GlobalData.h"

MainMenu* MAIN_MENU_PTR = nullptr;

MainMenu::MainMenu() {
  on_tick = new UpdateFunc([this]() { return this->f_on_tick(); });
  UPDATE_FUNC_REGISTRY.register_on_tick(on_tick, 7); 
  on_draw = new UpdateFunc([this]() { return this->f_on_draw(); });
  UPDATE_FUNC_REGISTRY.register_on_draw(on_draw, 70);

  title_anm = anm::loadFile(0x10, "title.anm");
  title_v_anm = anm::loadFile(0x11, "title_v.anm");
  // if (!title_anm || !title_v_anm) {
  //   sub_4025a0_logs_debug_message(this,0x4b3910,"データが壊れています\r\n");
  //   SUPERVISOR.gamemode_to_switch_to = ~((uint)SUPERVISOR.flags >> 0xd) & 1 | 2;
  //   return (zMainMenu *)0x0;
  // }

  menu_helper_0x24.option_wrapping = 1;
  // if ((SUPERVISOR.flags & 0x400U) != 0) {
  //   Supervisor::sub_463c90_inputRelated();
  // }
  // IS_ACHIEVEMENT_MODE = -1;
  // if (LOADING_THREAD_PTR != (zLoadingThread *)0x0) {
  //   iVar2 = LOADING_THREAD_PTR->field9_0x63c;
  //   while ((iVar2 < 0xb4 && ((SUPERVISOR.flags & 0x180U) == 0))) {
  //     Sleep(0x10);
  //     iVar2 = LOADING_THREAD_PTR->field9_0x63c;
  //   }
  //   AnmManager::_unload_file(ANM_MANAGER_PTR,1);
  // }

}

MainMenu::~MainMenu() {

}

int MainMenu::f_on_tick() {
//  int32_t *piVar1;
//  char cVar2;
//  char *pcVar3;
//  zRpyNameData *pzVar4;
//  zHelpManual *pzVar5;
//  zAnmManager *this_00;
//  zReplayManager *ptr_00;
//  int iVar6;
//  float *pfVar7;
//  zAnmId *pzVar8;
//  char *pcVar9;
//  uint uVar10;
//  zMainMenu *this_01;
//  int *piVar11;
//  bool bVar12;
//  float fVar13;
  
  // if (ENDING_PTR) {
  //   if ((ENDING_PTR->field_0x1c & 8) == 0) return 1;
  //   delete ENDING_PTR;
  //   song_related_442040(0,"th17_01");
  //   if ((SUPERVISOR.config._40_4_ & 0x10) != 0) SOUND_MANAGER->sub_4662e0(4, 0, "dummy");
  //   SOUND_MANAGER->sub_4662e0(2, 0, "dummy");
  //   SCOREFILE_PTR->field_0x2d16e = 1;
  //   return 1;
  // }
//   if (int_0x18_maybe_submenu == 1) {
//     GLOBALS.DEMOPLAY_TIMER = GLOBALS.DEMOPLAY_TIMER + 1;
//     if ((short)INPUT_STRUCT.hardware_input == 0) {
//       if (GLOBALS.DEMOPLAY_TIMER < 0x708) goto LAB_00451772;
//       GLOBALS.FLAGS = GLOBALS.FLAGS & 0xffffff7fU | 0x40;
//       pcVar3 = (&PTR_s_demo_demo1_rpy_004a2ccc)[GLOBALS.__field_18];
//       pcVar9 = pcVar3;
//       do {
//         cVar2 = *pcVar9;
//         pcVar9 = pcVar9 + 1;
//         pcVar9[0x4b76bf - (int)pcVar3] = cVar2;
//       } while (cVar2 != '\0');
//       ptr_00 = (zReplayManager *)FUN_0044e530(CURRENT_REPLAY_FILENAME);
//       GLOBALS.__field_18 = (GLOBALS.__field_18 + 1) % 3;
//       GLOBALS.inner.STAGE_NUM = 0;
//       piVar11 = &ptr_00->field50_0xc8[0].field4_0x10;
//       do {
//         if (*piVar11 != 0) break;
//         GLOBALS.inner.STAGE_NUM = GLOBALS.inner.STAGE_NUM + 1;
//         piVar11 = piVar11 + 10;
//       } while (GLOBALS.inner.STAGE_NUM < 8);
//       SUPERVISOR.gamemode_to_switch_to = 0xd;
//       pzVar4 = ptr_00->field7_0x1c;
//       CURRENT_STAGE_DATA = STAGE_DATA_TABLE + GLOBALS.inner.STAGE_NUM;
//       GLOBALS.inner.CHARACTER = pzVar4->character;
//       GLOBALS.inner.SHOTTYPE = pzVar4->shottype;
//       GLOBALS.inner.field43_0xec = GLOBALS.inner.DIFFICULTY;
//       GLOBALS.inner.DIFFICULTY = pzVar4->difficulty;
//       GLOBALS.inner.field1_0x4 = GLOBALS.inner.STAGE_NUM;
//       delete ptr_00;
//       DAT_004b77ec = 1;
//     }
//     GLOBALS.DEMOPLAY_TIMER = 0;
//   }
// LAB_00451772:
  // if ((this->flags_0x5cf4 & 1) != 0) {
  //   iVar6 = this->ctr_0x5cf0 + 1;
  //   bVar12 = DAT_004b77ec != 0;
  //   this->ctr_0x5cf0 = iVar6;
  //   if ((bVar12) || (0x3b < iVar6)) {
  //     SOUND_MANAGER->sub_4662e0(((uint)SUPERVISOR.config._40_4_ >> 4 & 1) + 3,0, "dummy");
  //     DAT_00526b08 = 0;
  //     song_related_442040(0,"th17_01");
  //     if ((SUPERVISOR.config._40_4_ & 0x10) != 0) {
  //       SOUND_MANAGER->sub_4662e0(4,0,"dummy");
  //     }
  //     SOUND_MANAGER->sub_4662e0(2,0,"dummy");
  //     SCOREFILE_PTR->field_0x2d16e = 1;
  //     this->flags_0x5cf4 = this->flags_0x5cf4 & 0xfffffffe;
  //     this->ctr_0x5cf0 = 0;
  //   }
  // }
//  this_00 = ANM_MANAGER_PTR;
//  switch(this->int_0x18_maybe_submenu) {
//  case 0:
//    // anm::remove_anm_of_file("front.anm");
//    // anm::remove_anm_of_file("bullet.anm");
//    // anm::remove_anm_of_file("ascii.anm");
//    // anm::remove_anm_of_file("text.anm");
//    // sub_41d3a0_stopNowLoading();
//    iVar6 = DAT_004b77ec;
//    if (DAT_004b77ec != 3) {
//      if ((GLOBALS.FLAGS & 0x40U) == 0) {
//        this->ctr_0x5cf0 = 0;
//        this->flags_0x5cf4 = this->flags_0x5cf4 | 1;
//      }
//      else {
//        this->flags_0x5cf4 = this->flags_0x5cf4 & 0xfffffffe;
//        GLOBALS.inner.DIFFICULTY = GLOBALS.inner.field43_0xec;
//      }
//      GLOBALS.FLAGS = GLOBALS.FLAGS & 0xffffffbf;
//      uVar10 = this->flags_0x5cf4 & 0xfffffffd;
//      if (iVar6 == 0) {
//        uVar10 = this->flags_0x5cf4 | 2;
//      }
//      this->flags_0x5cf4 = uVar10;
//      if (iVar6 != 0) {
//        if (iVar6 == 1) {
//          if (GLOBALS.inner.DIFFICULTY == 4) {
//            MenuHelper::select_option(&this->menu_helper_0x24,1);
//          }
//          sub_450c60_maybe_switch_submenu(this,1);
//          piVar1 = &this->on_draw->flags;
//          *piVar1 = *piVar1 | 2;
//          sub_451eb0(this_01);
//          break;
//        }
//        if (iVar6 == 2) {
//          Globals::sub_44f840_spell_practice_related(&GLOBALS,0);
//          (this->menu_helper_0x24).num_choices = 10;
//          MenuHelper::select_option(&this->menu_helper_0x24,4);
//          MenuHelper::push_selection(&this->menu_helper_0x24);
//          sub_450c60_maybe_switch_submenu(this,0xc);
//          DAT_004b77ec = 1;
//          piVar1 = &this->on_draw->flags;
//          *piVar1 = *piVar1 | 2;
//          goto switchD_00451825_caseD_c;
//        }
//        if (iVar6 == 6) {
//          Globals::sub_44f840_spell_practice_related(&GLOBALS,0);
//          (this->menu_helper_0x24).num_choices = 10;
//          MenuHelper::select_option(&this->menu_helper_0x24,5);
//          MenuHelper::push_selection(&this->menu_helper_0x24);
//          (this->menu_helper_0x24).num_choices = 10;
//          MenuHelper::select_option(&this->menu_helper_0x24,2);
//          MenuHelper::push_selection(&this->menu_helper_0x24);
//          ScreenEffect::operator_new(9,0x1e,0,0,0,0x55);
//          pzVar8 = AnmLoaded::create_effect(this->title_anm,86,-1,(zAnmVM **)0x0);
//          uVar10 = pzVar8->value;
//          (this->main_bg_anmid).value = uVar10;
//          AnmVm::interrupt_tree_run(uVar10,3);
//          sub_450c60_maybe_switch_submenu(this,0x17);
//          DAT_004b77ec = 1;
//          piVar1 = &this->on_draw->flags;
//          *piVar1 = *piVar1 | 2;
//          goto switchD_00451825_caseD_17;
//        }
//        if (iVar6 == 5) {
//          ScreenEffect::operator_new(9,0x1e,0,0,0,0x55);
//          pzVar8 = AnmLoaded::create_effect(this->title_anm,86,-1,(zAnmVM **)0x0);
//          uVar10 = pzVar8->value;
//          (this->main_bg_anmid).value = uVar10;
//          AnmVm::interrupt_tree_run(uVar10,3);
//          sub_450c60_maybe_switch_submenu(this,0x12);
//          DAT_004b77ec = 1;
//          piVar1 = &this->on_draw->flags;
//          *piVar1 = *piVar1 | 2;
//          goto switchD_00451825_caseD_12;
//        }
//        if (iVar6 == 4) {
//          ScreenEffect::operator_new(9,0x1e,0,0,0,0x55);
//          pzVar8 = AnmLoaded::create_effect(this->title_anm,0x56,-1,(zAnmVM **)0x0);
//          uVar10 = pzVar8->value;
//          (this->main_bg_anmid).value = uVar10;
//          AnmVm::interrupt_tree_run(uVar10,3);
//          sub_450c60_maybe_switch_submenu(this,5);
//          piVar1 = &this->on_draw->flags;
//          *piVar1 = *piVar1 | 2;
//          goto switchD_00451825_caseD_5;
//        }
//      }
//      else {
//        sub_450c60_maybe_switch_submenu(this,1);
//        DAT_004b77ec = 1;
//      }
//      goto switchD_00451825_caseD_1;
//    }
//    (this->menu_helper_0x24).num_choices = 10;
//    MenuHelper::select_option(&this->menu_helper_0x24,0);
//    MenuHelper::push_selection(&this->menu_helper_0x24);
//    sub_450c60_maybe_switch_submenu(this,0xf);
//    DAT_004b77ec = 1;
//    piVar1 = &this->on_draw->flags;
//    *piVar1 = *piVar1 | 2;
//  case 0xf:
//    sub_458fb0_submenu15(this);
//    break;
//  case 1:
//switchD_00451825_caseD_1:
//    piVar1 = &this->on_draw->flags;
//    *piVar1 = *piVar1 | 2;
//    sub_451eb0(this);
//    break;
//  case 2:
//    SUPERVISOR.gamemode_to_switch_to = ~((uint)SUPERVISOR.flags >> 0xd) & 1 | 2;
//  case 9:
//  case 0xd:
//    call_sound_func_442110();
//    break;
//  case 3:
//    sub_452d40_submenu3(this);
//    break;
//  case 4:
//    sub_4540e0_submenu4(this);
//    break;
//  case 5:
//switchD_00451825_caseD_5:
//    sub_455500_submenu5(this);
//    break;
//  case 6:
//    sub_4559b0_submenu6(this);
//    break;
//  case 7:
//    sub_456250_submenu7(this);
//    break;
//  case 8:
//    sub_456900_submenu8(this);
//    break;
//  case 10:
//    sub_457b40_submenu10(this);
//    break;
//  case 0xb:
//    sub_457e40_submenu11(this);
//    break;
//  case 0xc:
//switchD_00451825_caseD_c:
//    sub_456fa0_submenu12(this);
//    break;
//  case 0xe:
//    sub_45a320_submenu14(this);
//    break;
//  case 0x10:
//    sub_459900_submenu16(this);
//    break;
//  case 0x11:
//    if (this->int_0x20_maybeSelection == 0) {
//      if ((this->bg_paw_rain_anmid).value == 0) {
//        pzVar8 = AnmLoaded::create_effect(ASCII_MANAGER_PTR->ascii_anm,0x13,-1,(zAnmVM **)0x0);
//        (this->bg_paw_rain_anmid).value = pzVar8->value;
//      }
//      pzVar8 = AnmLoaded::create_effect(this->title_anm,103,-1,(zAnmVM **)0x0);
//      (this->top_manual_anmid).value = pzVar8->value;
//      HelpManual::operator_new();
//      this->int_0x20_maybeSelection = 1;
//      uVar10 = (this->timer_0x2ac).control;
//      if ((uVar10 & 1) == 0) {
//        (this->timer_0x2ac).__game_speed__disused = (float *)0x0;
//        (this->timer_0x2ac).control = uVar10 | 1;
//      }
//      pzVar5 = HELP_MANUAL_PTR;
//      (this->timer_0x2ac).current = 0;
//      (this->timer_0x2ac).current_f = 0.0;
//      pzVar5->float_0x128 = 128.0;
//    }
//    else if ((this->int_0x20_maybeSelection == 1) && (HELP_MANUAL_PTR->field73_0x124 != 0)) {
//      AnmManager::interrupt_tree((this->bg_paw_rain_anmid).value,1);
//      (this->bg_paw_rain_anmid).value = 0;
//      AnmManager::interrupt_tree((this->top_manual_anmid).value,1);
//      (this->top_manual_anmid).value = 0;
//      sub_450c60_maybe_switch_submenu(this,1);
//      if (HELP_MANUAL_PTR != (zHelpManual *)0x0) {
//        HelpManual::operator_delete(HELP_MANUAL_PTR);
//      }
//    }
//    break;
//  case 0x12:
//switchD_00451825_caseD_12:
//    sub_45afe0_submenu18(this);
//    break;
//  case 0x13:
//    sub_45b500_submenu19(this);
//    break;
//  case 0x14:
//    sub_45c3a0_submenu20(this);
//    break;
//  case 0x15:
//    sub_45b9b0_submenu21(this);
//    break;
//  case 0x17:
//switchD_00451825_caseD_17:
//    sub_45c960_submenu23(this);
//  }
//  pfVar7 = (this->timer_0x2ac).__game_speed__disused;
//  if (pfVar7 != (float *)0x0) {
//    (this->timer_0x2ac).__game_speed__disused = (float *)0x0;
//    pfVar7 = (float *)0x0;
//  }
//  pfVar7 = (float *)(&PTR_GAME_SPEED)[(int)pfVar7];
//  iVar6 = (this->timer_0x2ac).current;
//  (this->timer_0x2ac).previous = iVar6;
//  if ((pfVar7 != (float *)0x0) && ((fVar13 = *pfVar7, fVar13 <= 0.99 || (1.01 <= fVar13)))) {
//    fVar13 = fVar13 + (this->timer_0x2ac).current_f;
//    (this->timer_0x2ac).current_f = fVar13;
//    (this->timer_0x2ac).current = (int)fVar13;
//    return 1;
//  }
//  (this->timer_0x2ac).current_f = (this->timer_0x2ac).current_f + 1.0;
//  (this->timer_0x2ac).current = iVar6 + 1;
  return 1;
}

void MainMenu::sub_456cd0() {
  if (int_0x20_maybe_selection > 1 && int_0x20_maybe_selection < 4) {
    ASCII_MANAGER_PTR->draw_shadows = 1;
    if ((9 < (timer_0x2ac).current) || (int_0x20_maybe_selection == 3)) {
      for (int i = 1; i < 7; i++) {
        if (menu_helper_0x24.selection != i + -1) {
          ASCII_MANAGER_PTR->color = {128, 128, 128, 255};
        } else if (false
          // (&SCOREFILE_PTR->shottype[GLOBALS.inner.SHOTTYPE + GLOBALS.inner.CHARACTER * 3].field_0x46a5)
          // [GLOBALS.inner.DIFFICULTY * 0x40 + i * 8] == '\0'
        ) {
          ASCII_MANAGER_PTR->color = {0xdf, 0xdf, 0xdf, 255};
        } else if (int_0x20_maybe_selection == 3 && 1 < timer_0x2ac.current % 4) {
          ASCII_MANAGER_PTR->color = c_black;
        } else {
          ASCII_MANAGER_PTR->color = c_yellow;
        }
        // int chara = GLOBALS.inner.SHOTTYPE + GLOBALS.inner.CHARACTER * 3;
        if (true
          //(&SCOREFILE_PTR->shottype[chara].field_0x46a5)[GLOBALS.inner.DIFFICULTY * 0x40 + i * 8] == '\0'
          ) {
          ASCII_MANAGER_PTR->create_string_f({240.f, 174.f + i * 18.f, 0.f},"%s  ---------", "WTA" /*WENT_TO_ARR[i]*/);
        }
        else {
          ASCII_MANAGER_PTR->create_string_f({240.f, 174.f + i * 18.f, 0.f},"%s  %.8d0", "WTA" /*WENT_TO_ARR[i]*/,
                     0/**(undefined4 *) (&SCOREFILE_PTR->shottype[chara].field_0x46a0 + GLOBALS.inner.DIFFICULTY * 0x40 + i * 8)*/);
        }
      }
    }
    ASCII_MANAGER_PTR->color = c_white;
    ASCII_MANAGER_PTR->draw_shadows = 0;
  }
}

void MainMenu::sub_458bf0() {
  if (int_0x20_maybe_selection != 2) return;
  // if (field5605_0x5df0 == 0 && (ASCII_MANAGER_PTR->draw_shadows = 1, menu_helper_0x1d4.selection == 0)) {
  //   for (int i = 0; i < 10; i++) {
  //     ASCII_MANAGER_PTR->color = (~(i * 0x10) | 0xffffff00);
  //     auto t = &SCOREFILE_PTR->shottype[menu_helper_0x24.selection].highscores[0][menu_helper_0xfc.selection * 10 + i].date;
  //     if ((*(uint *)t | *(uint *)((int)t + 4)) == 0) {
  //       ASCII_MANAGER_PTR->create_string_f
  //                 ({48.0, 160.0 + i * 18.0, 0.0},"%2d  %s  %9ld%d  ----/--/-- --:--  Stage -  ---%%", i + 1,
  //                  SCOREFILE_PTR->shottype[menu_helper_0x24.selection].highscores[0][menu_helper_0xfc.selection * 10 + i].name,
  //                  SCOREFILE_PTR->shottype[menu_helper_0x24.selection].highscores[0][menu_helper_0xfc.selection * 10 + i].score,
  //                  SCOREFILE_PTR->shottype[menu_helper_0x24.selection].highscores[0][menu_helper_0xfc.selection * 10 + i].score_last_digit,
  //                  SCOREFILE_PTR->shottype[menu_helper_0x24.selection].highscores[0][menu_helper_0xfc.selection * 10 + i].field7_0x18);
  //     } else {
  //       auto lt = __localtime64(t);
  //       ASCII_MANAGER_PTR->create_string_f
  //                 ({48.0, 160.0 + i * 18.0, 0.0},
  //                  "%2d  %s  %9ld%d  %.4d/%.2d/%.2d %.2d:%.2d  %s  %2.1f%%",i + 1,
  //                  SCOREFILE_PTR->shottype[menu_helper_0x24.selection].highscores[0][menu_helper_0xfc.selection * 10 + i].name,
  //                  SCOREFILE_PTR->shottype[menu_helper_0x24.selection].highscores[0][menu_helper_0xfc.selection * 10 + i].score,
  //                  SCOREFILE_PTR->shottype[menu_helper_0x24.selection].highscores[0][menu_helper_0xfc.selection * 10 + i].score_last_digit,
  //                  lt->tm_year + 0x76c, lt->tm_mon + 1, lt->tm_mday, lt->tm_hour, lt->tm_min,
  //                  WENT_TO_ARR[SCOREFILE_PTR->shottype[menu_helper_0x24.selection].highscores[0][menu_helper_0xfc.selection * 10 + i].went_to],
  //                  SCOREFILE_PTR->shottype[menu_helper_0x24.selection].highscores[0][menu_helper_0xfc.selection * 10 + i].field7_0x18);
  //     }
  //   }
  // }
  ASCII_MANAGER_PTR->color = c_white;
  ASCII_MANAGER_PTR->create_string_f({328.0, 378.0, 0.0},"    %5d", 0); // TODO: findout what it prints
  // uVar12 = SCOREFILE_PTR->shottype[menu_helper_0x24.selection].field_uint_0x4668;
  // uVar16 = SCOREFILE_PTR->shottype[menu_helper_0x24.selection].field_uint_0x466c;
  // lVar2 = (ulonglong)uVar16 * 0xe147ae15 + ((ulonglong)uVar12 * 0xe147ae15 >> 0x20);
  // uVar15 = (uint)((ulonglong)lVar2 >> 0x20);
  // uVar11 = (int)((ulonglong)uVar12 * 0x47ae147a >> 0x20) +
  //          (uint)CARRY4((uint)((ulonglong)uVar12 * 0x47ae147a),(uint)lVar2);
  // lVar2 = (ulonglong)uVar16 * 0x47ae147a +
  //         (ulonglong)CONCAT14(CARRY4(uVar15,uVar11),uVar15 + uVar11);
  // uVar16 = (uVar16 - (int)((ulonglong)lVar2 >> 0x20)) - (uint)(uVar12 < (uint)lVar2);
  // lVar2 = lVar2 + CONCAT44(uVar16 >> 1,uVar12 - (uint)lVar2 >> 1 | uVar16 * -0x80000000);
  // local_8 = (uint)((ulonglong)lVar2 >> 0x20);
  // uVar17 = (uint)lVar2 >> 6 | local_8 * 0x4000000;
  // local_8 = local_8 >> 6;
  // lVar2 = (ulonglong)local_8 * 0x88888889 + ((ulonglong)uVar17 * 0x88888889 >> 0x20);
  // uVar16 = (uint)((ulonglong)lVar2 >> 0x20);
  // uVar12 = (int)((ulonglong)uVar17 * 0x88888888 >> 0x20) +
  //          (uint)CARRY4((uint)((ulonglong)uVar17 * 0x88888888),(uint)lVar2);
  // lVar2 = (ulonglong)local_8 * 0x88888888 +
  //         (ulonglong)CONCAT14(CARRY4(uVar16,uVar12),uVar16 + uVar12);
  // uVar16 = (uint)((ulonglong)lVar2 >> 0x20);
  // uVar12 = uVar16 * 0x8000000;
  // local_10 = (uint)lVar2 >> 5 | uVar12;
  // local_14 = (float)(uVar16 >> 5);
  // local_24 = (undefined4)((ulonglong)local_10 * 0x88888889);
  // local_1c = (undefined4)((ulonglong)local_10 * 0x88888888 >> 0x20);
  // local_20 = (undefined4)((ulonglong)(uint)local_14 * 0x88888888);
  // uVar3 = (ulonglong)(uint)local_14 * 0x88888889 + ((ulonglong)local_10 * 0x88888889 >> 0x20);
  // uVar11 = (uint)(uVar3 >> 0x20);
  // lVar2 = (ulonglong)local_10 * 0x88888888 + (uVar3 & 0xffffffff);
  // local_34 = (undefined4)lVar2;
  // uVar16 = (uint)((ulonglong)lVar2 >> 0x20);
  // lVar2 = (ulonglong)(uint)local_14 * 0x88888888 +
  //         (ulonglong)CONCAT14(CARRY4(uVar11,uVar16),uVar11 + uVar16);
  // uVar11 = (uint)((ulonglong)lVar2 >> 0x20);
  // uVar16 = uVar11 * 0x8000000;
  // uVar15 = (uint)lVar2 >> 5 | uVar16;
  // uVar11 = uVar11 >> 5;
  // local_c = uVar17 + local_10 * -0x3c;
  // ASCII_MANAGER_PTR->create_string_f
  //           ({328.0, 396.0, 0.0},"%3lld:%.2lld:%.2lld",uVar15,uVar11,
  //            local_10 + uVar15 * -0x3c,
  //            ((int)local_14 -
  //            ((((uVar11 << 4 | uVar16 >> 0x1c) - uVar11) - (uint)(uVar15 * 0x10 < uVar15)) * 4 |
  //            uVar15 * 0xf >> 0x1e)) - (uint)(local_10 < uVar15 * 0x3c),local_c,
  //            (local_8 -
  //            (((((int)local_14 << 4 | uVar12 >> 0x1c) - (int)local_14) -
  //             (uint)(local_10 * 0x10 < local_10)) * 4 | local_10 * 0xf >> 0x1e)) -
  //            (uint)(uVar17 < local_10 * 0x3c));
  ASCII_MANAGER_PTR->create_string_f({328.0, 414.0, 0.0},"    %5d", 0); // TODO: findout what it prints
  ASCII_MANAGER_PTR->color = c_white;
  ASCII_MANAGER_PTR->draw_shadows = 0;
}

void MainMenu::sub_457590() {
//   int iVar1;
//   int iVar2;
//   D3DCOLOR *pDVar3;
//   D3DCOLOR *pDVar4;
//   D3DCOLOR *pDVar5;
//   tm *ptVar6;
//   undefined *puVar7;
//   D3DCOLOR DVar8;
//   D3DCOLOR DVar9;
//   int iVar10;
//   undefined *puVar11;
//   int iVar12;
//   zAsciiManager *ASCII_MANAGER_PTR;
//   double dVar14;
//   char *pcVar15;
//   undefined *puVar16;
//   int iVar17;
//   int iVar18;
//   char *pcVar19;
//   char *pcVar20;
//   undefined8 uVar21;
//   zFloat3 local_1c;
//   float local_10;
//   int *local_c;
//   undefined *local_8;
//   
//   ASCII_MANAGER_PTR = ASCII_MANAGER_PTR;
//   if (param_1->int_0x20_maybeSelection == 2) {
//     local_1c.x = 58.0;
//     local_10 = 80.0;
//     ASCII_MANAGER_PTR->draw_shadows = 1;
//     iVar10 = (param_1->menu_helper_0x1d4).selection;
//     puVar11 = (undefined *)(iVar10 * 0x19);
//     local_1c.z = 0.0;
//     if ((int)puVar11 < (int)(puVar11 + 0x19)) {
//       local_c = param_1->array_0x5b5c + iVar10 * 0x19;
//       local_8 = puVar11;
//       do {
//         local_8 = local_8 + 1;
//         DVar8.b = 0x80;
//         DVar8.g = 0x80;
//         DVar8.r = 0x80;
//         DVar8.a = 0xff;
//         if ((param_1->menu_helper_0x24).selection == (int)puVar11 % 0x19) {
//           DVar8.b = '\0';
//           DVar8.g = 0xff;
//           DVar8.r = 0xff;
//           DVar8.a = 0xff;
//         }
//         ASCII_MANAGER_PTR->color = DVar8;
//         local_1c.y = local_10;
//         if (*local_c == 0) {
//           AsciiManager::create_string_f
//                     (ASCII_MANAGER_PTR,&local_1c,
//                      (&PTR_s_No.%.2d_--------_--/--/--_--:--_-_004a2cb4)
//                      [(uint)((param_1->menu_helper_0x1d4).selection != 0) * 2]);
//         }
//         else {
//           iVar10 = *(int *)(*local_c + 0x1c);
//           ptVar6 = __localtime64((__time64_t *)(iVar10 + 0x10));
//           if ((param_1->menu_helper_0x1d4).selection == 0) {
//             dVar14 = (double)*(float *)(iVar10 + 0x80);
//             if ((*(byte *)(iVar10 + 10) & 2) != 0) {
//               AsciiManager::create_string_f
//                         (ASCII_MANAGER_PTR,&local_1c,
//                          "No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %s SpellPr %3d %2.1f%%",local_8,iVar10
//                          ,ptVar6->tm_year % 100,ptVar6->tm_mon + 1,ptVar6->tm_mday,ptVar6->tm_hour,
//                          ptVar6->tm_min,
//                          CHARA_NAMES[*(int *)(iVar10 + 0x88) * 3 + *(int *)(iVar10 + 0x8c)],
//                          *(int *)(iVar10 + 0x9c) + 1,dVar14);
//               goto LAB_00457ae7;
//             }
//             pcVar20 = STAGENUM_ABBREV[*(int *)(iVar10 + 0x94)];
//             puVar7 = (&DIFF_NAMES)[*(int *)(iVar10 + 0x90)];
//             pcVar19 = CHARA_NAMES[*(int *)(iVar10 + 0x88) * 3 + *(int *)(iVar10 + 0x8c)];
//             iVar1 = ptVar6->tm_mon;
//             iVar12 = ptVar6->tm_min;
//             iVar18 = ptVar6->tm_hour;
//             iVar17 = ptVar6->tm_mday;
//             iVar2 = ptVar6->tm_year;
//             pcVar15 = "No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%";
//             puVar16 = local_8;
//           }
//           else {
//             _DAT_0052a914 = *(undefined4 *)(*local_c + 0x227);
//             DAT_0052a918 = 0;
//             dVar14 = (double)*(float *)(iVar10 + 0x80);
//             pcVar20 = STAGENUM_ABBREV[*(int *)(iVar10 + 0x94)];
//             puVar7 = (&DIFF_NAMES)[*(int *)(iVar10 + 0x90)];
//             pcVar19 = CHARA_NAMES[*(int *)(iVar10 + 0x88) * 3 + *(int *)(iVar10 + 0x8c)];
//             iVar1 = ptVar6->tm_mon;
//             iVar12 = ptVar6->tm_min;
//             iVar18 = ptVar6->tm_hour;
//             iVar17 = ptVar6->tm_mday;
//             iVar2 = ptVar6->tm_year;
//             puVar16 = &DAT_0052a914;
//             pcVar15 = "%s  %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %s %2.1f%%";
//           }
//           AsciiManager::create_string_f
//                     (ASCII_MANAGER_PTR,&local_1c,pcVar15,puVar16,iVar10,iVar2 % 100,iVar1 + 1,iVar17
//                      ,iVar18,iVar12,pcVar19,puVar7,pcVar20,dVar14);
//         }
// LAB_00457ae7:
//         puVar11 = puVar11 + 1;
//         local_10 = local_10 + 15.0;
//         local_c = local_c + 1;
//         ASCII_MANAGER_PTR = ASCII_MANAGER_PTR;
//       } while ((int)puVar11 < ((param_1->menu_helper_0x1d4).selection + 1) * 0x19);
//     }
//     (ASCII_MANAGER_PTR->color).b = 0xff;
//     (ASCII_MANAGER_PTR->color).g = 0xff;
//     (ASCII_MANAGER_PTR->color).r = 0xff;
//     (ASCII_MANAGER_PTR->color).a = 0xff;
//     ASCII_MANAGER_PTR->draw_shadows = 0;
//     return 1;
//   }
//   local_1c.z = (float)(param_1->int_0x20_maybeSelection + -4);
//   if (local_1c.z != 0.0) {
//     return 1;
//   }
//   local_1c.x = 58.0;
//   local_1c.y = 80.0;
//   iVar10 = *(int *)(param_1->array_0x5b5c[param_1->field5590_0x5b54] + 0x1c);
//   if ((param_1->timer_0x2ac).current < 10) {
//     local_1c.y = ((10.0 - (param_1->timer_0x2ac).current_f) *
//                  (float)(((int)param_1->field5590_0x5b54 % 0x19) * 0xf)) / 10.0 + 80.0;
//   }
//   ASCII_MANAGER_PTR->draw_shadows = 1;
//   local_10 = (float)iVar10;
//   ptVar6 = __localtime64((__time64_t *)(iVar10 + 0x10));
//   if (menu_helper_0x1d4.selection == 0) {
//     if (((iVar10 + 10) & 2) != 0) {
//       AsciiManager::create_string_f
//                 (ASCII_MANAGER_PTR,&local_1c,
//                  "No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %s SpellPr %3d %2.1f%%",
//                  param_1->field5590_0x5b54 + 1,iVar10,
//                  ptVar6->tm_year % 100,ptVar6->tm_mon + 1, ptVar6->tm_mday,ptVar6->tm_hour,ptVar6->tm_min,
//                  CHARA_NAMES[*(int *)(iVar10 + 0x88) * 3 + *(int *)(iVar10 + 0x8c)],
//                  *(int *)(iVar10 + 0x9c) + 1, (double)*(float *)(iVar10 + 0x80));
//     } else {
//       ASCII_MANAGER_PTR->create_string_f(&local_1c,"No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%",
//             (undefined *)(param_1->field5590_0x5b54 + 1),
//             iVar10,
//             ptVar6->tm_year % 100, ptVar6->tm_mon + 1, ptVar6->tm_mday, ptVar6->tm_hour, ptVar6->tm_min,
//             CHARA_NAMES[*(int *)(iVar10 + 0x88) * 3 + *(int *)(iVar10 + 0x8c)],
//             (&DIFF_NAMES)[*(int *)(iVar10 + 0x90)],
//             STAGENUM_ABBREV[*(int *)(iVar10 + 0x94)], (double)*(float *)(iVar10 + 0x80));
//     }
//   }
//   else {
//     _DAT_0052a914 = *(undefined4 *)(param_1->array_0x5b5c[param_1->field5590_0x5b54] + 0x227);
//     DAT_0052a918 = 0;
//     dVar14 = (double)*(float *)(iVar10 + 0x80);
//     pcVar20 = STAGENUM_ABBREV[*(int *)(iVar10 + 0x94)];
//     puVar11 = (&DIFF_NAMES)[*(int *)(iVar10 + 0x90)];
//     pcVar19 = CHARA_NAMES[*(int *)(iVar10 + 0x88) * 3 + *(int *)(iVar10 + 0x8c)];
//     iVar1 = ptVar6->tm_mon;
//     iVar12 = ptVar6->tm_min;
//     iVar18 = ptVar6->tm_hour;
//     iVar17 = ptVar6->tm_mday;
//     iVar2 = ptVar6->tm_year;
//     puVar7 = &DAT_0052a914;
//     pcVar15 = "%s  %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %s %2.1f%%";
//     ASCII_MANAGER_PTR->create_string_f(&local_1c,pcVar15,puVar7,iVar10,iVar2 % 100,iVar1 + 1,iVar17,iVar18, iVar12,pcVar19,puVar11,pcVar20,dVar14);
//   }
// LAB_004577b2:
//   if (9 < timer_0x2ac.current) {
//     for (int i = 1; i < 8; i++) {
//       ASCII_MANAGER_PTR->color = {128, 128, 128, 255};
//       if (menu_helper_0x24.selection == i - 1) {
//         ASCII_MANAGER_PTR->color = c_yellow;
//       }
//       if (*(int *)(this->array_0x5b5c[param_1->field5590_0x5b54] + 0xd8 + i * 0x28) == 0) {
//         ASCII_MANAGER_PTR->create_string_f({220.f, 110.f + i * 18.f, 0.f},"%s  ---------", "A");
//       } else if ((i < 6) && (iVar18 = *(int *)(this->array_0x5b5c[this->field5590_0x5b54] + 0x100 + i * 0x28), iVar18 != 0)) {
//         ASCII_MANAGER_PTR->create_string_f({220.f, 110.f + i * 18.f, 0.f}, "%s  %.8d%d", WENT_TO_ARR[i], CONCAT44(*(undefined4 *)(iVar18 + 0x3c),*(undefined4 *)(iVar18 + 0x34)), 0);
//       } else {
//         ASCII_MANAGER_PTR->create_string_f({220.f, 110.f + i * 18.f, 0.f}, "%s  %.8d%d", WENT_TO_ARR[i], CONCAT44(*(undefined4 *)((int)local_10 + 0x98), *(undefined4 *)((int)local_10 + 0x18)), 0);
//       }
//     }
//   }
//   ASCII_MANAGER_PTR->color = c_white;
//   ASCII_MANAGER_PTR->draw_shadows = 0;
}

void MainMenu::sub_4595b0() {
//  int iVar1;
//  D3DCOLOR *pDVar2;
//  D3DCOLOR *pDVar3;
//  D3DCOLOR *pDVar4;
//  int32_t iVar5;
//  zScoreFile *pzVar6;
//  int iVar7;
//  D3DCOLOR DVar9;
//  tm *ptVar10;
//  D3DCOLOR DVar11;
//  __time64_t *_Time;
//  int iVar13;
//  uint uVar14;
//  zFloat3 local_20;
//  int local_14;
//  uint local_c;
//  float local_8;
//  
//  if (int_0x20_maybe_selection != 2) return;
//  iVar5 = GLOBALS.inner.DIFFICULTY;
//  iVar13 = 0;
//  local_8 = 160.0;
//  ASCII_MANAGER_PTR->draw_shadows = 1;
//  local_14 = iVar5 * 10;
//  local_c = 0;
//  local_20.x = 22.0;
//  local_20.z = 0.0;
//  do {
//    iVar7 = local_14;
//    if (this->field5584_0x5a64 == 0) {
//      DVar9.b = 0xff;
//      DVar9.g = 0xff;
//      DVar9.r = 0xff;
//      DVar9.a = 0xff;
//      if (menu_helper_0x24.selection != iVar13) {
//        DVar9.b = '@';
//        DVar9.g = 0x80;
//        DVar9.r = 0x80;
//        DVar9.a = 0xff;
//      }
//    }
//    else {
//      DVar9 = (D3DCOLOR)(~local_c | 0xffffff00);
//    }
//    ASCII_MANAGER_PTR->color = DVar9;
//    pzVar6 = SCOREFILE_PTR;
//    iVar1 = GLOBALS.inner.SHOTTYPE + GLOBALS.inner.CHARACTER * 3;
//    _Time = &SCOREFILE_PTR->shottype[iVar1].highscores[0][local_14 + iVar13].date;
//    local_20.y = local_8;
//    if ((*(uint *)_Time | *(uint *)((int)_Time + 4)) == 0) {
//      AsciiManager::create_string_f
//                (&local_20,"%2d  %s  %9ld%d  ----/--/-- --:--  Stage -  ---%%",iVar13 + 1,
//                 SCOREFILE_PTR->shottype[iVar1].highscores[0][local_14 + iVar13].name,
//                 SCOREFILE_PTR->shottype[iVar1].highscores[0][local_14 + iVar13].score,
//                 (int)SCOREFILE_PTR->shottype[iVar1].highscores[0][local_14 + iVar13].
//                      score_last_digit,
//                 (double)(float)SCOREFILE_PTR->shottype[iVar1].highscores[0][local_14 + iVar13].
//                                field7_0x18);
//    }
//    else {
//      ptVar10 = __localtime64(_Time);
//      AsciiManager::create_string_f
//                (ASCII_MANAGER_PTR,&local_20,
//                 "%2d  %s  %9ld%d  %.4d/%.2d/%.2d %.2d:%.2d  %s  %2.1f%%",iVar13 + 1,
//                 pzVar6->shottype[iVar1].highscores[0][iVar7 + iVar13].name,
//                 pzVar6->shottype[iVar1].highscores[0][iVar7 + iVar13].score,
//                 (int)pzVar6->shottype[iVar1].highscores[0][iVar7 + iVar13].score_last_digit,
//                 ptVar10->tm_year + 0x76c,ptVar10->tm_mon + 1,ptVar10->tm_mday,ptVar10->tm_hour,
//                 ptVar10->tm_min,
//                 WENT_TO_ARR[pzVar6->shottype[iVar1].highscores[0][iVar7 + iVar13].went_to],
//                 (double)(float)pzVar6->shottype[iVar1].highscores[0][iVar7 + iVar13].field7_0x18)
//      ;
//    }
//    pzVar8 = this;
//    iVar13 = iVar13 + 1;
//    local_8 = local_8 + 18.0;
//    local_c = local_c + 0x10;
//  } while ((int)local_c < 0xa0);
//  if (this->field5584_0x5a64 != 0) return;
//  ASCII_MANAGER_PTR.color = c_white;
//  ASCII_MANAGER_PTR->create_string_f({58.f, menu_helper_0x24.selection * 18.f + 160.f, 0.f},"%s", "test");
//  ASCII_MANAGER_PTR->color = c_yellow;
//  ASCII_MANAGER_PTR->create_string_f({this->field5583_0x5a60 * 9.f + 58.f + (this->field5583_0x5a60 == 8 ? -9.f : 0.f), menu_helper_0x24.selection * 18.f + 160.f, 0.f}, "_");
//  ns::vec3 pos = {212.f, 360.f, 0.f};
//  for (int i = 0; i < 0x5b; i++) {
//    if (this->menu_helper_0x5a68.selection == i) {
//      ASCII_MANAGER_PTR->color = c_yellow;
//    } else {
//      ASCII_MANAGER_PTR->color = {128, 128, 128, 255}
//    }
//    ASCII_MANAGER_PTR->create_string_f(pos, "%c", 't');
//    if (i % 0xd == 0xc) {
//      pos.x = 212.0;
//      pos.y += 16.0;
//    } else {
//      pos.x += 18.0;
//    }
//  } 
//  ASCII_MANAGER_PTR->color = c_white;
}

void MainMenu::sub_459f20() {
//  zRpyNameData *pzVar1;
//  int iVar2;
//  D3DCOLOR *pDVar3;
//  D3DCOLOR *pDVar4;
//  D3DCOLOR *pDVar5;
//  tm *ptVar6;
//  D3DCOLOR DVar7;
//  D3DCOLOR DVar8;
//  zAsciiManager *pzVar9;
//  uint uVar10;
//  int iVar11;
//  zFloat3 local_18;
//  float local_c;
//  int *local_8;
//  
//  pzVar9 = ;
//  if (param_1->int_0x20_maybeSelection == 2) {
//    local_8 = param_1->array_0x5b5c;
//    iVar11 = 0;
//    local_18.x = 58.0;
//    local_c = 80.0;
//    local_18.z = 0.0;
//    ASCII_MANAGER_PTR->draw_shadows = 1;
//    do {
//      DVar8.b = 0x80;
//      DVar8.g = 0x80;
//      DVar8.r = 0x80;
//      DVar8.a = 0xff;
//      if ((param_1->menu_helper_0x24).selection == iVar11) {
//        DVar8.b = '\0';
//        DVar8.g = 0xff;
//        DVar8.r = 0xff;
//        DVar8.a = 0xff;
//      }
//      ASCII_MANAGER_PTR->color = DVar8;
//      local_18.y = local_c;
//      if (*local_8 == 0) {
//        AsciiManager::create_string_f
//                  (&local_18,"No.%.2d -------- --/--/-- --:-- ------- ------- --- ---%%");
//      }
//      else {
//        iVar2 = *(int *)(*local_8 + 0x1c);
//        ptVar6 = __localtime64((__time64_t *)(iVar2 + 0x10));
//        AsciiManager::create_string_f
//                  (ASCII_MANAGER_PTR,&local_18,
//                   "No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%",iVar11 + 1,iVar2,
//                   ptVar6->tm_year % 100,ptVar6->tm_mon + 1,ptVar6->tm_mday,ptVar6->tm_hour,
//                   ptVar6->tm_min,CHARA_NAMES[*(int *)(iVar2 + 0x88) * 3 + *(int *)(iVar2 + 0x8c)],
//                   (&DIFF_NAMES)[*(int *)(iVar2 + 0x90)],STAGENUM_ABBREV[*(int *)(iVar2 + 0x94)],
//                   (double)*(float *)(iVar2 + 0x80));
//      }
//      iVar11 = iVar11 + 1;
//      local_c = local_c + 15.0;
//      local_8 = local_8 + 1;
//    } while (iVar11 < 0x19);
//  }
//  else {
//    local_18.z = (float)(param_1->int_0x20_maybeSelection + -3);
//    if (local_18.z != 0.0) {
//      return 1;
//    }
//    iVar11 = param_1->field5590_0x5b54;
//    local_18.x = 58.0;
//    local_18.y = 240.0;
//    if ((param_1->timer_0x2ac).current < 10) {
//      local_18.y = ((10.0 - (param_1->timer_0x2ac).current_f) *
//                   ((float)(iVar11 * 0xf + 0x50) - 240.0)) / 10.0 + 240.0;
//    }
//    pzVar1 = REPLAY_MANAGER_PTR->field7_0x1c;
//    ptVar6 = __localtime64(&pzVar1->time);
//    AsciiManager::create_string_f
//              (ASCII_MANAGER_PTR,&local_18,"No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%",
//               iVar11 + 1,"        ",ptVar6->tm_year % 100,ptVar6->tm_mon + 1,ptVar6->tm_mday,
//               ptVar6->tm_hour,ptVar6->tm_min,CHARA_NAMES[pzVar1->character * 3 + pzVar1->shottype],
//               (&DIFF_NAMES)[pzVar1->difficulty],"All",(double)*(float *)&pzVar1->field_0x80);
//    if (9 < (param_1->timer_0x2ac).current) {
//      local_18.x = 112.0;
//      local_18.y = 240.0;
//      local_18.z = 0.0;
//      pDVar3 = &ASCII_MANAGER_PTR->color;
//      pDVar4 = &ASCII_MANAGER_PTR->color;
//      pDVar5 = &ASCII_MANAGER_PTR->color;
//      (ASCII_MANAGER_PTR->color).b = 0xff;
//      pDVar3->g = 0xff;
//      pDVar4->r = 0xff;
//      pDVar5->a = 0xff;
//      AsciiManager::create_string_f(&local_18,"%s");
//      local_18.x = (float)(param_1->field5583_0x5a60 * 9) + 112.0;
//      if (param_1->field5583_0x5a60 == 8) {
//        local_18.x = local_18.x - 9.0;
//      }
//      pDVar3 = &ASCII_MANAGER_PTR->color;
//      pDVar4 = &ASCII_MANAGER_PTR->color;
//      pDVar5 = &ASCII_MANAGER_PTR->color;
//      (ASCII_MANAGER_PTR->color).b = '\0';
//      pDVar3->g = 0xff;
//      pDVar4->r = 0xff;
//      pDVar5->a = 0xff;
//      AsciiManager::create_string_f(&local_18,"_");
//      uVar10 = 0;
//      local_8 = (int *)0x43540000;
//      pDVar3 = &ASCII_MANAGER_PTR->color;
//      pDVar4 = &ASCII_MANAGER_PTR->color;
//      pDVar5 = &ASCII_MANAGER_PTR->color;
//      (ASCII_MANAGER_PTR->color).b = 0xff;
//      pDVar3->g = 0xff;
//      pDVar4->r = 0xff;
//      pDVar5->a = 0xff;
//      local_c = 360.0;
//      local_18.y = 360.0;
//      local_18.z = 0.0;
//      do {
//        DVar7.b = 0x80;
//        DVar7.g = 0x80;
//        DVar7.r = 0x80;
//        DVar7.a = 0xff;
//        if ((param_1->menu_helper_0x5a68).selection == uVar10) {
//          DVar7.b = '\0';
//          DVar7.g = 0xff;
//          DVar7.r = 0xff;
//          DVar7.a = 0xff;
//        }
//        ASCII_MANAGER_PTR->color = DVar7;
//        local_18.x = (float)local_8;
//        AsciiManager::create_string_f(&local_18,"%c");
//        if (uVar10 % 0xd == 0xc) {
//          local_8 = (int *)0x43540000;
//          local_18.y = local_c + 16.0;
//          local_c = local_18.y;
//        }
//        else {
//          local_8 = (int *)((float)local_8 + 18.0);
//        }
//        uVar10 = uVar10 + 1;
//      } while ((int)uVar10 < 0x5b);
//      ASCII_MANAGER_PTR->color = c_white;
//    }
//  }
//  ASCII_MANAGER_PTR->color = c_white;
//  ASCII_MANAGER_PTR->draw_shadows = 0;
}

void MainMenu::sub_45c700() {
//  if (0 >= int_0x20_maybe_selection || !(int_0x20_maybe_selection < 3 || (int_0x20_maybe_selection == 3 && int_0x18_maybe_submenu != 0x14))) return;
//  ASCII_MANAGER_PTR->font_id = 2;
//  ASCII_MANAGER_PTR->draw_shadows = 1;
//  for (int i = 0; i < 5; i++) {
//    int a = this->field5600_0x5ddc[i];
//    if (a < -1) continue;
//    if (int_0x18_maybe_submenu == 0x14 && i == menu_helper_0x24.selection) {
//      if (*(int *)(&SCOREFILE_PTR->shottype[this->menu_helper_0x5cf8.selection].field_0x95c + a * 0x9c) != 0) {
//        ASCII_MANAGER_PTR->color = {0x90, 0xd0, 0xff, 0xff};
//      } else {
//        ASCII_MANAGER_PTR->color = {0xb0, 0xb0, 0xb0, 0xff};
//      }
//    } else {
//      if (*(int *)(&SCOREFILE_PTR->shottype[this->menu_helper_0x5cf8.selection].field_0x95c + a * 0x9c) != 0) {
//        ASCII_MANAGER_PTR->color = {96, 0xa0, 0xc0, 0xff};
//      } else {
//        ASCII_MANAGER_PTR->color = {64, 64, 64, 255};
//      }
//    }
//    if ((*(int *)(&SCOREFILE_PTR->field_0x29280 + a * 0x9c) == 0) && (*(int *)(&SCOREFILE_PTR->field_0x29284 + a * 0x9c) == 0)) {
//      ASCII_MANAGER_PTR->create_string_f({330.0, 191.0 + 44.0 * i, 0.0},"SCORE        00  ----/----");
//    } else {
//      ASCII_MANAGER_PTR->create_string_f({330.0, 191.0 + 44.0 * i, 0.0}, "SCORE %8d0  %4d/%4d",
//                 *(undefined4 *)(&SCOREFILE_PTR->shottype[this->menu_helper_0x5cf8.selection].field_0x970 + a * 0x9c),
//                 *(undefined4 *)(&SCOREFILE_PTR->shottype[this->menu_helper_0x5cf8.selection].field_0x95c + a * 0x9c),
//                 *(undefined4 *)(&SCOREFILE_PTR->shottype[this->menu_helper_0x5cf8.selection].field_0x964 + a * 0x9c));
//      if (*(int *)(&SCOREFILE_PTR->shottype[this->menu_helper_0x5cf8.selection].field_0x958 + iVar6) != 0) {
//        ASCII_MANAGER_PTR->color = {32, 96, 96, 255};
//      } else {
//        ASCII_MANAGER_PTR->color = {64, 64, 64, 255};
//      }
//      if ((char)(&DAT_004a0ca0)[a] < '\x05') {
//        ASCII_MANAGER_PTR->create_string_f({330.0, 201.0 + 44.0 * i, 0.0}, "GAME MODE        %4d/%4d",
//                   *(undefined4 *)(&SCOREFILE_PTR->shottype[this->menu_helper_0x5cf8.selection].field_0x958 + a * 0x9c),
//                   *(undefined4 *)(&SCOREFILE_PTR->shottype[this->menu_helper_0x5cf8.selection].field_0x960 + a * 0x9c));
//      }
//    }
//  }
//  ASCII_MANAGER_PTR->font_id = 0;
//  ASCII_MANAGER_PTR->draw_shadows = 0;
//  ASCII_MANAGER_PTR->color = c_white;
}

void MainMenu::sub_45d2a0() {
  if (int_0x20_maybe_selection != 1) return;
  ASCII_MANAGER_PTR->font_id = 4;
  ASCII_MANAGER_PTR->draw_shadows = 1;
  ns::vec3 pos = {130.0, 100.0, 0.0};
  for (int i = 0; i < 0x28; i++) {
    if (menu_helper_0x24.selection == i) {
      ASCII_MANAGER_PTR->color = c_white;
    } else {
      ASCII_MANAGER_PTR->color = {112, 112, 112, 255};
    }
    if (false) { // (&SCOREFILE_PTR->field_0x2d198)[i] == '\0') {
      ASCII_MANAGER_PTR->create_string_f(pos, "--");
    } else {
      ASCII_MANAGER_PTR->create_string_f(pos, "%2d", i + 1);
    }
    if (i % 10 == 9) {
      pos.x = 130.0;
      pos.y += 34.0;
    } else {
      pos.x += 38.0;
    }
  }
  ASCII_MANAGER_PTR->font_id = 0;
  ASCII_MANAGER_PTR->draw_shadows = 0;
}

int MainMenu::f_on_draw() {
  // if (ENDING_PTR != 0x0) return 1;
  switch(this->int_0x18_maybe_submenu) {
  case 8:
    sub_456cd0();
    break;
  case 0xb:
    sub_458bf0();
    break;
  case 0xc:
    sub_457590();
    break;
  case 0xf:
    sub_4595b0();
    break;
  case 0x10:
    sub_459f20();
    break;
  case 0x13:
  case 0x14:
    sub_45c700();
    break;
  case 0x17:
    sub_45d2a0();
    break;
  }

  return 1;
}
