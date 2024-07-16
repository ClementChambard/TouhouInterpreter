#include "./GuiMsgVm.hpp"
#include "Anm/AnmManager.h"
#include "Bullet/BulletManager.h"
#include "Ecl/EnemyManager.h"
#include "GlobalData.h"
#include "Gui.hpp"
#include "Hardcoded.h"
#include "Laser/LaserManager.h"
#include "Player.h"
#include "Supervisor.h"
#include "shiftjis.h"
#include "Anm/Text.hpp"

#include <input.hpp>
#include <cstring>

GuiMsgVm_t::GuiMsgVm_t(MsgIns_t* instr) {
    current_instr = instr;
    anm_id_text_line_1 = SUPERVISOR.text_anm->createEffect(0);
    anm_id_text_line_2 = SUPERVISOR.text_anm->createEffect(0);
    auto anm_text_line_1 = anm::getVM(anm_id_text_line_1);
    auto anm_text_line_2 = anm::getVM(anm_id_text_line_2);
    anm_text_line_1->font_dims[0] = 21;
    anm_text_line_1->font_dims[1] = 21;
    anm_text_line_1->bitflags.f534_12 = 1;
    anm_text_line_1->index_of_on_draw = 5;
    anm_text_line_2->interruptRecRun(7);
    anm_text_line_2->font_dims[0] = 21;
    anm_text_line_2->font_dims[1] = 21;
    anm_text_line_2->bitflags.f534_12 = 1;
    anm_text_line_2->index_of_on_draw = 5;
    anm_id_furigana_1 = SUPERVISOR.text_anm->createEffect(1);
    anm_id_furigana_2 = SUPERVISOR.text_anm->createEffect(1);
    auto anm_furigana_1 = anm::getVM(anm_id_furigana_1);
    auto anm_furigana_2 = anm::getVM(anm_id_furigana_2);
    anm_furigana_1->font_dims[0] = 21;
    anm_furigana_1->font_dims[1] = 21;
    anm_furigana_1->bitflags.f534_12 = 1;
    anm_furigana_1->index_of_on_draw = 5;
    anm_furigana_2->interruptRecRun(7);
    anm_furigana_2->font_dims[0] = 21;
    anm_furigana_2->font_dims[1] = 21;
    anm_furigana_2->bitflags.f534_12 = 1;
    anm_furigana_2->index_of_on_draw = 5;
    BulletManager::GetInstance()->cancel_all(false);
    LASER_MANAGER_PTR->cancel_all(false);
    ENEMY_MANAGER_PTR->EnmKillAll(nullptr, true);
}

GuiMsgVm_t::~GuiMsgVm_t() {
    anm::deleteVM(anm_id_player_face);
    anm::deleteVM(anm_id_enemy_face[0]);
    anm::deleteVM(anm_id_enemy_face[1]);
    anm::deleteVM(anm_id_enemy_face[2]);
    anm::deleteVM(anm_id_enemy_face[3]);
    anm::deleteVM(__anm_id_idfk);
    anm::deleteVM(anm_id_text_line_1);
    anm::deleteVM(anm_id_text_line_2);
    anm::deleteVM(anm_id_furigana_1);
    anm::deleteVM(anm_id_furigana_2);
    anm::deleteVM(anm_id_intro);
    anm::deleteVM(anm_id_speechBubble);
}

void GuiMsgVm_t::hideVms() {
    auto vm = anm::getVM(anm_id_player_face);
    if (vm) vm->clear_flag_1_rec();
    for (int i = 0; i < 4; i++) {
        vm = anm::getVM(anm_id_enemy_face[i]);
        if (vm) vm->clear_flag_1_rec();
    }
    vm = anm::getVM(__anm_id_idfk);
    if (vm) vm->clear_flag_1_rec();
    vm = anm::getVM(anm_id_text_line_1);
    if (vm) vm->clear_flag_1_rec();
    vm = anm::getVM(anm_id_text_line_2);
    if (vm) vm->clear_flag_1_rec();
    vm = anm::getVM(anm_id_furigana_1);
    if (vm) vm->clear_flag_1_rec();
    vm = anm::getVM(anm_id_furigana_2);
    if (vm) vm->clear_flag_1_rec();
    vm = anm::getVM(anm_id_intro);
    if (vm) vm->clear_flag_1_rec();
    vm = anm::getVM(__anm_id_70);
    if (vm) vm->clear_flag_1_rec();
    vm = anm::getVM(anm_id_speechBubble);
    if (vm) vm->clear_flag_1_rec();
}

void GuiMsgVm_t::unhideVms() {
    auto vm = anm::getVM(anm_id_player_face);
    if (vm) vm->set_flag_1_rec();
    for (int i = 0; i < 4; i++) {
        vm = anm::getVM(anm_id_enemy_face[i]);
        if (vm) vm->set_flag_1_rec();
    }
    vm = anm::getVM(__anm_id_idfk);
    if (vm) vm->set_flag_1_rec();
    vm = anm::getVM(anm_id_text_line_1);
    if (vm) vm->set_flag_1_rec();
    vm = anm::getVM(anm_id_text_line_2);
    if (vm) vm->set_flag_1_rec();
    vm = anm::getVM(anm_id_furigana_1);
    if (vm) vm->set_flag_1_rec();
    vm = anm::getVM(anm_id_furigana_2);
    if (vm) vm->set_flag_1_rec();
    vm = anm::getVM(anm_id_intro);
    if (vm) vm->set_flag_1_rec();
    vm = anm::getVM(__anm_id_70);
    if (vm) vm->set_flag_1_rec();
    vm = anm::getVM(anm_id_speechBubble);
    if (vm) vm->set_flag_1_rec();
}

void GuiMsgVm_t::setBubbleLength(float l) {
  auto vm = anm::getVM(anm_id_speechBubble);
  if (!vm) return;
  int i = 0;
  anm::VM* cvm = nullptr;
  do {
    cvm = vm->search_children(-1, i);
    if (cvm)
      cvm->float_script_vars[0] = l + 16.f;
    i++;
  } while (cvm);
}

void GuiMsgVm_t::OpenSpeechBubble(int bubble,
                                  ns::vec3 const& pos, float length) {
    anm::deleteVM(anm_id_speechBubble);
    anm_id_speechBubble =
        GUI_PTR->front_anm->createEffectPos(bubble + GUI_ANMS["speech_bubble"].asInt(), 0, pos);
    setBubbleLength(length);
    speechBubbleType = bubble;
}

#define CURRENT_STAGE_DATA STAGE_DATA_TABLE[GLOBALS.inner.STAGE_NUM]

int GuiMsgVm_t::tick() {
  if (__dword_incremented_by_enemyAppear > 0)
    __dword_incremented_by_enemyAppear--;
  // if ((INPUT_STRUCT.rising_edge & 0x201U) != 0) flags |= 0x40;
  // is inputstruct 200 the ctrl key ?
  // if (((flags & 0x41) == 0x41)
  //    ((((INPUT_STRUCT.input & 0x200U) != 0 && (0x13 < (uint)INPUT_STRUCT.field8_0x194[9])) ||
  //     (((INPUT_STRUCT.input & 1U) != 0 && (0x13 < (uint)INPUT_STRUCT.field8_0x194[0])))))) {
  //   time_in_script = current_instr->time;
  // }
  int arg0;
  int arg1;
  anm::VM* vm;
  while (current_instr->time <= time_in_script) {
      switch (current_instr->opcode) {
      case 0:
        return -1;
      case 1:
        arg0 = *reinterpret_cast<int32_t*>(current_instr->data);
        if (arg0 == 0) {
          anm_id_player_face =
            PLAYER_PTR->playerAnm->createEffect(
                PLAYERS[GLOBALS.inner.CHARACTER]["msg_face_anim_script"].asInt());
        } else {
          anm_id_player_face =
            ENEMY_MANAGER_PTR->loadedAnms[5]->createEffect(0xb);
        }
        break;
      case 2:
        arg0 = *reinterpret_cast<int32_t*>(current_instr->data);
        anm_id_enemy_face[arg0] =
            ENEMY_MANAGER_PTR->loadedAnms[CURRENT_STAGE_DATA["boss_data"][arg0]["msg_face_anim_index"].asInt()]
            ->createEffect(CURRENT_STAGE_DATA["boss_data"][arg0]["msg_face_anm_script"].asInt());
        field_0x1c0 = 0;
        break;
      case 4:
        anm::interrupt_tree(anm_id_player_face, 1);
        anm_id_player_face = 0;
        break;
      case 5:
        arg0 = *reinterpret_cast<int32_t*>(current_instr->data);
        anm::interrupt_tree(anm_id_enemy_face[arg0], 1);
        anm_id_enemy_face[arg0] = 0;
        anm::interrupt_tree(anm_id_intro, 1);
        break;
      case 6:
        anm::interrupt_tree(anm_id_text_line_1, 1);
        anm::interrupt_tree(anm_id_text_line_2, 1);
        anm::interrupt_tree(anm_id_furigana_1, 1);
        anm::interrupt_tree(anm_id_furigana_2, 1);
        anm::deleteVM(anm_id_speechBubble);
        anm_id_speechBubble = 0;
        break;
      case 7:
        for (int i = 0; i < 4; i++) {
          anm::interrupt_tree_run(anm_id_enemy_face[i], 3);
        }
        anm::interrupt_tree_run(anm_id_player_face, 2);
        anm::interrupt_tree(__anm_id_idfk, 2);
        active_side = 0;
        vm = anm::getVM(anm_id_text_line_1);
        if (vm) vm->__pos_2.y = 0.0;
        vm = anm::getVM(anm_id_text_line_2);
        if (vm) vm->__pos_2.y = 0.0;
        vm = anm::getVM(anm_id_furigana_1);
        if (vm) vm->__pos_2.y = 0.0;
        vm = anm::getVM(anm_id_furigana_2);
        if (vm) vm->__pos_2.y = 0.0;
        flags &= 0xfffffffd;
        next_text_line = 0;
        field_0x198 = 0;
        break;
      case 8:
        anm::interrupt_tree_run(anm_id_player_face, 3);
        arg0 = *reinterpret_cast<int32_t*>(current_instr->data);
        for (int i = 0; i < 4; i++) {
            int inte = i == arg0 ? 2 : 3;
            anm::interrupt_tree_run(anm_id_enemy_face[i], inte);
        }
        anm::interrupt_tree(__anm_id_idfk, 3);
        active_side = 1;
        vm = anm::getVM(anm_id_text_line_1);
        if (vm) vm->__pos_2.y = 0.0;
        vm = anm::getVM(anm_id_text_line_2);
        if (vm) vm->__pos_2.y = 0.0;
        vm = anm::getVM(anm_id_furigana_1);
        if (vm) vm->__pos_2.y = 0.0;
        vm = anm::getVM(anm_id_furigana_2);
        if (vm) vm->__pos_2.y = 0.0;
        flags &= 0xfffffffd;
        next_text_line = 0;
        field_0x198 = 0;
        break;
      case 9:
        anm::interrupt_tree_run(anm_id_player_face, 3);
        for (int i = 0; i < 4; i++) {
            anm::interrupt_tree_run(anm_id_enemy_face[i], 3);
        }
        anm::interrupt_tree(__anm_id_idfk, 3);
        active_side = 2;
        vm = anm::getVM(anm_id_text_line_1);
        if (vm) vm->__pos_2.y = 0.0;
        vm = anm::getVM(anm_id_text_line_2);
        if (vm) vm->__pos_2.y = 0.0;
        vm = anm::getVM(anm_id_furigana_1);
        if (vm) vm->__pos_2.y = 0.0;
        vm = anm::getVM(anm_id_furigana_2);
        if (vm) vm->__pos_2.y = 0.0;
        flags &= 0xfffffffd;
        next_text_line = 0;
        field_0x198 = 0;
        break;
      case 10:
        arg0 = *reinterpret_cast<int32_t*>(current_instr->data);
        flags ^= (arg0 ^ flags) & 1U;
        break;
      case 11:
        arg0 = *reinterpret_cast<int32_t*>(current_instr->data);
        if (pause_timer < 1) {
          pause_timer = arg0;
        }
        pause_timer--;
        if (!ns::keyboard::pressed(ns::Key::RETURN) && !ns::keyboard::pressed(ns::Key::W) && (0 < pause_timer)) {
          if (((flags & 0x41) != 0x41) /*||
             ((((INPUT_STRUCT.input & 0x200U) == 0 || ((uint)INPUT_STRUCT.field8_0x194[9] < 0x14))
              && (((INPUT_STRUCT.input & 1U) == 0 || ((uint)INPUT_STRUCT.field8_0x194[0] < 0x14)))))*/
             ) goto LAB_0042ee3b;
        } else {
          // SoundManager::play_sound_centered(0);
        }
        pause_timer = 0;
        next_text_line = 0;
        field_0x198 = 0;
        break;
      case 12:
        __dword_incremented_by_enemyAppear = 1;
        break;
      case 13:
        arg0 = *reinterpret_cast<int32_t*>(current_instr->data);
        anm::interrupt_tree_run(anm_id_player_face, arg0 + 0x11);
        break;
      case 14:
        arg0 = *reinterpret_cast<int32_t*>(current_instr->data);
        arg1 = *reinterpret_cast<int32_t*>(&current_instr->data[4]);
        anm::interrupt_tree_run(anm_id_enemy_face[arg1], arg0 + 0x11);
        break;
      case 15:
        vm = anm::getVM(anm_id_text_line_1);
        {auto s = std::string(reinterpret_cast<char*>(current_instr->data), current_instr->argsize);
        util_xor(reinterpret_cast<uint8_t*>(s.data()), s.size(), 0x77, 7, 16);
        s = s.data();
        anm::text::FUN_00475000(vm, text_colors[active_side], c_black,
                     ((uint)flags >> 1 & 1) + 4, 0, 0, sj2utf8(s));}
        anm::interrupt_tree(anm_id_text_line_1, 2);
        break;
      case 16:
        vm = anm::getVM(anm_id_text_line_2);
        {auto s = std::string(reinterpret_cast<char*>(current_instr->data), current_instr->argsize);
        util_xor(reinterpret_cast<uint8_t*>(s.data()), s.size(), 0x77, 7, 16);
        s = s.data();
        anm::text::FUN_00475000(vm, text_colors[active_side], c_black,
                     ((uint)flags >> 1 & 1) + 4, 0, 0, sj2utf8(s));}
        anm::interrupt_tree(anm_id_text_line_2, 2);
        break;
      case 17:
        if (next_text_line == 0) {
          if (field_0x198 == 0) {
            speechBubbleLength = 0.0;
            vm = anm::getVM(anm_id_text_line_1);
            anm::text::FUN_00475000(vm, text_colors[active_side], c_black,
                         (uint)flags >> 2 & 1, 0, 0, "  ");
            vm = anm::getVM(anm_id_text_line_2);
            anm::text::FUN_00475000(vm, text_colors[active_side], c_black,
                         (uint)flags >> 1 & 1, 0, 0, "  ");
            vm = anm::getVM(anm_id_furigana_1);
            anm::text::FUN_00475000(vm, text_colors[active_side], c_black,
                         (uint)flags >> 1 & 1, 0, 1, "  ");
            vm = anm::getVM(anm_id_furigana_2);
            anm::text::FUN_00475000(vm, text_colors[active_side], c_black,
                         (uint)flags >> 1 & 1, 0, 1, "  ");
            field_0x198 = 1;
            anm::interrupt_tree(anm_id_text_line_1, 3);
            anm::interrupt_tree(anm_id_text_line_2, 3);
            anm::interrupt_tree(anm_id_furigana_1, 3);
            anm::interrupt_tree(anm_id_furigana_2, 3);
          }
          auto s = std::string(reinterpret_cast<char*>(current_instr->data), current_instr->argsize);
          util_xor(reinterpret_cast<uint8_t*>(s.data()), s.size(), 0x77, 7, 16);
          s = s.data();
          if (s[0] == '|') {
            int iVar20 = std::atoi(&s[1]);
            s = std::strchr(&s[1], 0x2c);
            int iVar17 = std::atoi(&s[1]);
            s = &std::strchr(&s[1], 0x2c)[1];
            vm = anm::getVM(anm_id_furigana_1);
            if (vm) {
                vm->bitflags.f534_12 = true;
                vm->entity_pos = callout_pos;
                vm->interruptRun(2);
            }
            anm::text::FUN_00475000(vm, c_black, {0xa0, 0xa0, 0xa0, 0xff}, 2, iVar20, iVar17, sj2utf8(s));
            break;
          }
          int iVar20 = ((s.size() + 1) / 2 /*- 1*/) * 16 - 28;
          if (iVar20 < 0) iVar20 += 30;
          iVar20 *= 2;
          if (iVar20 > speechBubbleLength) {
            speechBubbleLength = iVar20;
          }
          OpenSpeechBubble((flags >> 2 & 0xf) * 3 + active_side,
                           callout_pos, speechBubbleLength);
          setBubbleLength(speechBubbleLength);
          vm = anm::getVM(anm_id_text_line_1);
          anm::text::FUN_00475000(vm, text_colors[active_side], c_black,
                       flags >> 1 & 1, 0, 0, sj2utf8(s));
          if (active_side == 0) {
            vm = anm::getVM(anm_id_text_line_1);
            if (vm) vm->entity_pos = callout_pos;
          } else if (active_side - 1U < 2) {
            vm = anm::getVM(anm_id_text_line_1);
            if (vm) vm->entity_pos = callout_pos;
            vm = anm::getVM(anm_id_furigana_1);
            if (vm) vm->entity_pos = callout_pos;
            vm = anm::getVM(anm_id_text_line_2);
            if (vm) vm->entity_pos = callout_pos;
            vm = anm::getVM(anm_id_furigana_2);
            if (vm) vm->entity_pos = callout_pos;
          }
          anm::interrupt_tree_run(anm_id_text_line_1, 2);
          next_text_line++;
        } else {
          auto s = std::string(reinterpret_cast<char*>(current_instr->data), current_instr->argsize);
          util_xor(reinterpret_cast<uint8_t*>(s.data()), s.size(), 0x77, 7, 16);
          s = s.data();
          if (s[0] == '|') {
            int iVar20 = std::atoi(&s[1]);
            s = std::strchr(&s[1], 0x2c);
            int iVar17 = std::atoi(&s[1]);
            s = &std::strchr(&s[1], 0x2c)[1];
            vm = anm::getVM(anm_id_furigana_2);
            if (vm) {
                vm->bitflags.f534_12 = true;
                vm->entity_pos = callout_pos;
                vm->interruptRec(2);
            }
            anm::text::FUN_00475000(vm, c_black, {0xa0, 0xa0, 0xa0, 0xff}, 2, iVar20, iVar17, sj2utf8(s));
            break;
          }
          int iVar20 = ((s.size() + 1) / 2 /*- 1*/) * 16 - 28;
          if (iVar20 < 0) iVar20 += 30;
          iVar20 *= 2;
          if (iVar20 > speechBubbleLength) {
            speechBubbleLength = iVar20;
          }
          OpenSpeechBubble((((uint)flags >> 2 & 0xf) + 8) * 3 + active_side,
                           callout_pos, speechBubbleLength);
          setBubbleLength(speechBubbleLength);
          vm = anm::getVM(anm_id_text_line_2);
          anm::text::FUN_00475000(vm, text_colors[active_side], c_black,
                       (flags >> 1) & 1, 0, 0, sj2utf8(s));
          if (active_side == 0) {
            vm = anm::getVM(anm_id_text_line_1);
            if (vm) vm->entity_pos = callout_pos;
          } else if (active_side - 1U < 2) {
            vm = anm::getVM(anm_id_text_line_1);
            if (vm) vm->entity_pos = callout_pos;
            vm = anm::getVM(anm_id_furigana_1);
            if (vm) vm->entity_pos = callout_pos;
            vm = anm::getVM(anm_id_text_line_2);
            if (vm) vm->entity_pos = callout_pos;
            vm = anm::getVM(anm_id_furigana_2);
            if (vm) vm->entity_pos = callout_pos;
          }
          anm::interrupt_tree_run(anm_id_text_line_2, 2);
          next_text_line = 0;
          field_0x198 = 0;
        }
        break;
      case 18:
        anm::deleteVM(anm_id_speechBubble);
        anm_id_speechBubble = 0;
        anm::interrupt_tree(anm_id_text_line_1, 3);
        anm::interrupt_tree(anm_id_text_line_2, 3);
        anm::interrupt_tree(anm_id_furigana_1, 3);
        anm::interrupt_tree(anm_id_furigana_2, 3);
        break;
      case 19:
        // iVar20 = CURRENT_STAGE_DATA->boss_bgm_id;
        // if ((SUPERVISOR.config._40_1_ & 0x10) != 0) {
        //   SoundManager::sub_4662e0(&SOUND_MANAGER,4,0,"dummy");
        // }
        // SoundManager::sub_4662e0(&SOUND_MANAGER,2,1,"dummy");
        // (&SCOREFILE_PTR->field_0x2d16e)[CURRENT_STAGE_DATA->boss_bgm_id] = 1;
        GUI_PTR->stage_logo_anmloaded->createEffect(2);
        GLOBALS.inner.field_0x14 = 0;
        break;
      case 20:
        arg0 = *reinterpret_cast<int32_t*>(current_instr->data);
        anm_id_intro = ENEMY_MANAGER_PTR->loadedAnms
            [CURRENT_STAGE_DATA["boss_data"][arg0]["msg_ename_anim_index"].asInt()]
            ->createEffect(CURRENT_STAGE_DATA["boss_data"][arg0]["msg_ename_anm_script"].asInt());
        vm = anm::getVM(GUI_PTR->boss_name_anmid);
        if (!vm) {
            int i = 0;
            if (GLOBALS.inner.CURRENT_CHAPTER < 41) {
                i = CURRENT_STAGE_DATA["boss_data"][1]["front_ename_anm_script"].asInt();
            } else {
                i = CURRENT_STAGE_DATA["boss_data"][0]["front_ename_anm_script"].asInt();
            }
            if ((-1 < i) && (-1 < i + 167)) {
                GUI_PTR->boss_name_anmid = GUI_PTR->front_anm->createEffect(i + GUI_ANMS["boss_name_first"].asInt());
            }
        }
        break;
      case 21:
        // GameThread::end_stage();
        // TODO: temp: put in function
        if ((GLOBALS.FLAGS & 0x30) != 0x20) {
          GUI_PTR->stage_clear_anmid = GUI_PTR->front_anm->createEffect(GUI_ANMS["stage_clear"].asInt());
          GUI_PTR->stage_clear_bonus = GLOBALS.inner.STAGE_NUM * 1000000;
          GLOBALS.inner.CURRENT_SCORE += GLOBALS.inner.STAGE_NUM * 1000000 / 10;
          if (999999999 < GLOBALS.inner.CURRENT_SCORE) {
            GLOBALS.inner.CURRENT_SCORE = 999999999;
          }
          GUI_PTR->timer_chapter_end = 0;
          GUI_PTR->some_flags_1a4 |= 0x100;
        }
        break;
      case 22:
        // if (GLOBALS.inner.STAGE_NUM != 6) {
        //   auVar27._0_4_ = 8.0;
        // }
        // if ((GAME_SPEED != 0) && (GAME_SPEED <= 1)) {
        //   auVar27._0_4_ = auVar27._0_4_ / GAME_SPEED;
        // }
        // SoundManager::sub_4662e0((zSoundManager *)&SOUND_MANAGER,5,(int)auVar27._0_4_,(char *)&INT_004a176c);
        break;
      case 23:
        anm::interrupt_tree(anm_id_player_face, 7);
        break;
      case 24:
        anm::interrupt_tree(anm_id_enemy_face[0], 7);
        anm::interrupt_tree(anm_id_enemy_face[1], 7);
        break;
      case 25:
        arg0 = *reinterpret_cast<int32_t*>(current_instr->data);
        vm = anm::getVM(anm_id_text_line_1);
        if (vm) vm->__pos_2.y = arg0;
        vm = anm::getVM(anm_id_text_line_2);
        if (vm) vm->__pos_2.y = arg0;
        vm = anm::getVM(anm_id_furigana_1);
        if (vm) vm->__pos_2.y = arg0;
        vm = anm::getVM(anm_id_furigana_2);
        if (vm) vm->__pos_2.y = arg0;
        break;
      case 26:
        flags |= 2;
        break;
      case 27:
        // arg0 = *reinterpret_cast<int32_t*>(current_instr->data);
        // if (GAME_SPEED != 0 && GAME_SPEED <= 1) arg0 /= GAME_SPEED;
        // SoundManager::sub_4662e0 ((zSoundManager *)&SOUND_MANAGER,5,arg0,(char *)&INT_004a176c);
        break;
      case 28:
        callout_pos.x = 2 * *reinterpret_cast<float*>(current_instr->data);
        callout_pos.y = 2 * *reinterpret_cast<float*>(&current_instr->data[4]);
        break;
      case 29:
        arg0 = *reinterpret_cast<int32_t*>(current_instr->data);
        flags ^= ((arg0 << 2) ^ flags) & 0x3cU;
        break;
      case 31:
        anm_id_enemy_face[1] = ENEMY_MANAGER_PTR->loadedAnms
            [CURRENT_STAGE_DATA["boss_data"][1]["msg_face_anim_index"].asInt()]
            ->createEffect(
                CURRENT_STAGE_DATA["boss_data"][1]["msg_face_anm_script"].asInt());
        field_0x1c0 = 0;
        break;
      case 32:
        anm::interrupt_tree(__anm_id_idfk, 3);
        active_side = *reinterpret_cast<int32_t*>(current_instr->data);
        vm = anm::getVM(anm_id_text_line_1);
        if (vm) vm->__pos_2.y = 0.0;
        vm = anm::getVM(anm_id_text_line_2);
        if (vm) vm->__pos_2.y = 0.0;
        vm = anm::getVM(anm_id_furigana_1);
        if (vm) vm->__pos_2.y = 0.0;
        vm = anm::getVM(anm_id_furigana_2);
        if (vm) vm->__pos_2.y = 0.0;
        flags &= 0xfffffffd;
        next_text_line = 0;
        field_0x198 = 0;
        break;
      case 33:
        arg0 = *reinterpret_cast<int32_t*>(current_instr->data);
        arg1 = *reinterpret_cast<int32_t*>(&current_instr->data[4]);
        if (arg0 == 0) {
          anm::interrupt_tree_run(anm_id_player_face, 3);
        } else {
          anm::interrupt_tree_run(anm_id_enemy_face[arg1], 3);
        }
        break;
      case 34:
        arg0 = *reinterpret_cast<int32_t*>(current_instr->data);
        arg1 = *reinterpret_cast<int32_t*>(&current_instr->data[4]);
        if (arg0 == 0) {
          anm::interrupt_tree_run(anm_id_player_face, 2);
        } else {
          anm::interrupt_tree_run(anm_id_enemy_face[arg1], 2);
        }
        break;
      case 35:
        GUI_PTR->anmid_0x110_msgRelated =
            GUI_PTR->front_anm->createEffect(0x3c);
        break;
      default:
        break;
      }
      current_instr = reinterpret_cast<MsgIns_t*>(&current_instr->data[current_instr->argsize]);
  }
  time_in_script++;
  LAB_0042ee3b:
  vm = anm::getVM(anm_id_speechBubble);
  if (!vm) return 0;
  vm = vm->search_children(speechBubbleType + GUI_ANMS["speech_bubble_child"].asInt(), 0);
  if (!vm) return 0;
  ns::vec3 p = vm->pos + vm->entity_pos + vm->__pos_2;
  vm->transform_coordinate(p);
  p.x *= (2.0 / anm::RESOLUTION_MULT);
  p.y *= (2.0 / anm::RESOLUTION_MULT);
  if (active_side == 1) {
    if (1.0 <= vm->scale.x) {
      p.x += 26.0;
    } else {
      p.x += ((vm->scale.x + 0.125) * 32.0 - 6.0);
    }
  } else if (active_side == 0) {
    p.x -= 36.0;
  } else if (active_side == 2) {
    p.x += ((vm->scale.x + 0.125) * 16.0 - 6.0);
  }
  vm = anm::getVM(anm_id_text_line_1);
  if (vm) vm->entity_pos = p;
  vm = anm::getVM(anm_id_furigana_1);
  if (vm) vm->entity_pos = p;
  vm = anm::getVM(anm_id_text_line_2);
  if (vm) vm->entity_pos = p;
  vm = anm::getVM(anm_id_furigana_2);
  if (vm) vm->entity_pos = p;
  return 0;
}
