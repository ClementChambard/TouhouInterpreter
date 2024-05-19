#include "./shiftjis.h"
#include "./Spellcard.h"
#include "./AnmOpener/AnmManager.h"
#include "./AnmOpener/Text.hpp"
#include "./AsciiManager.hpp"
#include "./EnemyManager.h"
#include "./Gui.hpp"
#include "./Player.h"
#include "./Supervisor.h"
#include "./Bomb.hpp"
#include "./GlobalData.h"
#include <Error.h>

Spellcard *SPELLCARD_PTR = nullptr;

Spellcard *Spellcard::GetInstance() {
  static Spellcard *inst = new Spellcard();
  return inst;
}

Spellcard::Spellcard() {
  f_on_tick = new UpdateFunc([this]() { return this->on_tick(); });
  f_on_draw = new UpdateFunc([this]() { return this->on_draw(); });
  UPDATE_FUNC_REGISTRY->register_on_tick(f_on_tick, 32);
  UPDATE_FUNC_REGISTRY->register_on_draw(f_on_draw, 12);
  SPELLCARD_PTR = this;
}

Spellcard::~Spellcard() {
  UPDATE_FUNC_REGISTRY->unregister(f_on_tick);
  UPDATE_FUNC_REGISTRY->unregister(f_on_draw);
}

int Spellcard::on_tick() {
  if (!(this->flags & 1))
    return 1;

  field_0x8c++;

  if (__timer_20 >= 60 && !(flags & 0x200)) {
    // STAGE_PTR->flags &= 0xfffffffe;
  }

  if (__timer_20 >= 300 && !(flags & 8)) {
    bonus = ((bonus - (bonus_max * 2 / 3) / (duration - 300)) / 10) * 10;
  }

  __timer_20++;

  auto e = EnemyManager::GetInstance()->EnmFind(
      EnemyManager::GetInstance()->boss_ids[0]);
  if (e)
    boss0_pos = (e->getData()->final_pos.pos - boss0_pos) * 0.05f + boss0_pos;
  auto vm = anm::getVM(spell_circle_anmid);
  if (vm) {
    vm->entity_pos = boss0_pos;
  }
  if ((flags & 0x20) && BOMB_PTR->active != 1) {
    flags &= 0xffffffdf;
  }

  if (__timer_20 < 120) {
    return 1;
  }
  if (flags & 4) {
    if (flags & 0x100) {
      if (PLAYER_PTR->inner.pos.y < 320.0) {
        anm::interrupt_tree(ascii_anmid_10, 2);
        anm::interrupt_tree(text_anmid_14, 2);
        anm::interrupt_tree(ascii_anmid_18, 2);
        flags &= 0xfffffffb;
      }
    } else {
      if (PLAYER_PTR->inner.pos.y > 128.0) {
        anm::interrupt_tree(ascii_anmid_10, 2);
        anm::interrupt_tree(text_anmid_14, 2);
        anm::interrupt_tree(ascii_anmid_18, 2);
        flags &= 0xfffffffb;
      }
    }
  } else {
    if (flags & 0x100) {
      if (PLAYER_PTR->inner.pos.y > 352.0) {
        anm::interrupt_tree(ascii_anmid_10, 3);
        anm::interrupt_tree(text_anmid_14, 3);
        anm::interrupt_tree(ascii_anmid_18, 3);
        flags |= 4;
      }
    } else {
      if (PLAYER_PTR->inner.pos.y < 96.0) {
        anm::interrupt_tree(ascii_anmid_10, 3);
        anm::interrupt_tree(text_anmid_14, 3);
        anm::interrupt_tree(ascii_anmid_18, 3);
        flags |= 4;
      }
    }
  }
  return 1;
}

int Spellcard::on_draw() {
  if (!(flags & 1))
    return 1;
  auto anm__18 = anm::getVM(ascii_anmid_18);
  if (!anm__18) {
    ascii_anmid_18 = 0;
    return 1;
  }
  ASCII_MANAGER_PTR->font_id = 2;
  ASCII_MANAGER_PTR->group = 2;
  ASCII_MANAGER_PTR->color.a = anm__18->color_1.a;
  if (!(flags & 2)) {
    ASCII_MANAGER_PTR->create_string_f({282.0, 35.0, 0.0}, "$");
  } else {
    ASCII_MANAGER_PTR->create_string_f({266.0, 35.0, 0.0}, "%8d", bonus);
  }

  int times_captured = 0;
  //    *(int *)((int)SCOREFILE_PTR->shottype + this->field25_0x74 * 0x9c +
  //           (uint)((GLOBALS.FLAGS >> 4 & 3) == 2) * 4 + 0x958 +
  //           (GLOBALS.inner.SHOTTYPE + GLOBALS.inner.CHARACTER * 3) * 0x4820);
  int times_played = 1;
  //    *(int *)((int)SCOREFILE_PTR->shottype + this->field25_0x74 * 0x9c +
  //           (uint)((GLOBALS.FLAGS >> 4 & 3) == 2) * 4 + 0x960 +
  //           (GLOBALS.inner.SHOTTYPE + GLOBALS.inner.CHARACTER * 3) * 0x4820);

  if (times_captured > 99) {
    ASCII_MANAGER_PTR->create_string_f({360.0, 35.0, 0.0}, "MASTER");
  } else if (times_played < 100) {
    ASCII_MANAGER_PTR->create_string_f({360.0, 35.0, 0.0}, "%.2d/%.2d",
                                       times_captured, times_played);
  } else {
    ASCII_MANAGER_PTR->create_string_f({360.0, 35.0, 0.0}, "%.2d/99+",
                                       times_captured);
  }
  ASCII_MANAGER_PTR->font_id = 0;
  ASCII_MANAGER_PTR->group = 0;
  ASCII_MANAGER_PTR->color.a = 255;
  return 1;
}

void Spellcard::Stop() {
  if (!(flags & 1)) {
    return;
  }
  // STAGE_PTR->flags |= 1;
  anm::interrupt_tree(ascii_anmid_10, 1);
  anm::interrupt_tree(text_anmid_14, 1);
  anm::interrupt_tree(ascii_anmid_18, 1);
  flags &= 0xfffffffe;
  anm::deleteVM(spell_bg_anm_id);
  spell_bg_anm_id = 0;
  flags &= 0xffffffdf;
  GUI_PTR->vm_timer_digit_hi->interrupt(3);
  GUI_PTR->vm_timer_digit_lo->interrupt(3);
  anm::deleteVM(spell_circle_anmid);
  spell_circle_anmid = 0;
  if (!(flags & 2)) {
    GUI_PTR->midScreenInfo(0, 1);
    if (flags & 0x80) {
      // SoundManager::play_sound_centered(0x45);
    }
    return;
  }
  GLOBALS.inner.CURRENT_SCORE += bonus / 10;
  GLOBALS.inner.CURRENT_SCORE = fmin(GLOBALS.inner.CURRENT_SCORE, 999999999);
  GUI_PTR->midScreenInfo(bonus, 0);
  // if (REPLAY_MANAGER_PTR->field3_0xc != 1) {
  //   piVar1 =
  //       &SCOREFILE_PTR->shottype[GLOBALS.inner.SHOTTYPE + 3 * GLOBALS.inner]
  //            .field_598_0x958 +
  //       (uint)(((uint)GLOBALS.FLAGS >> 4 & 3) == 2) +
  //       SPELLCARD_PTR->spell_id * 0x27;
  //   if (*piVar1 < 99999) {
  //     (*piVar1)++;
  //   }

  //   if (*(int *)(&SCOREFILE_PTR->field_0x29278 +
  //                (SPELLCARD_PTR->spell_id * 0x9c +
  //                 (uint)(((uint)GLOBALS.FLAGS >> 4 & 3) == 2) * 4 + 0x80) +
  //                -0x80) < 99999) {
  //     *(int *)(&SCOREFILE_PTR->field_0x29278 +
  //              (SPELLCARD_PTR->spell_id * 0x9c +
  //               (uint)(((uint)GLOBALS.FLAGS >> 4 & 3) == 2) * 4 + 0x80) +
  //              -0x80) =
  //         *(int *)(&SCOREFILE_PTR->field_0x29278 +
  //                  (SPELLCARD_PTR->spell_id * 0x9c +
  //                   (uint)(((uint)GLOBALS.FLAGS >> 4 & 3) == 2) * 4 + 0x80) +
  //                  -0x80) +
  //         1;
  //   }
  //   iVar3 = 0;
  //   piVar1 = (int *)&SCOREFILE_PTR->field_0x2927c;
  //   iVar2 = 0x65;
  //   do {
  //     iVar5 = iVar2;
  //     if ((piVar1[-1] != 0) || (*piVar1 != 0)) {
  //       iVar3 += 1;
  //     }
  //     piVar1 = piVar1 + 0x27;
  //     iVar2 = iVar5 + -1;
  //   } while (iVar2 != 0);
  //   if (100 < iVar3) {
  //     TrophyAnimation::operator_new(iVar5 + 0x1f);
  //   }
  // }
  // SoundManager::play_sound_centered(0x2e);
  if (flags & 0x80) {
    // SoundManager::play_sound_centered(0x45);
  }
  return;
}

#include "Hardcoded.h"
void Spellcard::Init(int id, int time, int mode, std::string name) {
  __timer_20 = 0;
  spell_id = id;

  // strcpy name
  // pcVar10 = param_3;
  // do {
  //   cVar3 = *pcVar10;
  //   pcVar10 = pcVar10 + 1;
  //   pcVar10[(int)this + (0x33 - (int)param_3)] = cVar3;
  // } while (cVar3 != '\0');

  flags = (flags & 0xffffff67) | 3;

  // if (REPLAY_MANAGER_PTR->field3_0xc != 1) {
  //   iVar11 = param_2 * 156;
  //   pcVar8 = (char *)((int)SCOREFILE_PTR->shottype +
  //                   (GLOBALS.inner.SHOTTYPE + GLOBALS.inner.CHARACTER * 3) *
  //                         0x4820 +
  //                     iVar11 + 0x8d8);
  //   pcVar10 = param_3;
  //   do {
  //     cVar3 = *pcVar10;
  //     pcVar10 = pcVar10 + 1;
  //     *pcVar8 = cVar3;
  //     paVar6 = SCOREFILE_PTR;
  //     pcVar8 = pcVar8 + 1;
  //   } while (cVar3 != '\0');
  //   spell_circle_id = (uint)GLOBALS.FLAGS >> 4 & 3;
  //   iVar2 = (uint)(spell_circle_id == 2) * 4;
  //   iVar9 = (uint)(spell_circle_id == 2) * 4 + 0x960 +
  //           (GLOBALS.inner.SHOTTYPE + GLOBALS.inner.CHARACTER * 3) * 0x4820 +
  //           iVar11;
  //   if (*(int *)((int)SCOREFILE_PTR->shottype + iVar9) < 99999) {
  //     piVar1 = (int *)((int)SCOREFILE_PTR->shottype + iVar9);
  //     *piVar1 = *piVar1 + 1;
  //   }
  //   pcVar10 = param_3;
  //   do {
  //     cVar3 = *pcVar10;
  //     pcVar10 = pcVar10 + 1;
  //     pcVar10[(int)paVar6 + (iVar11 - (int)param_3) + 0x291f7] = cVar3;
  //   } while (cVar3 != '\0');
  //   if (*(int *)(&SCOREFILE_PTR->field_0x29280 + iVar2 + iVar11) < 99999) {
  //     *(int *)(&SCOREFILE_PTR->field_0x29280 + iVar2 + iVar11) =
  //         *(int *)(&SCOREFILE_PTR->field_0x29280 + iVar2 + iVar11) + 1;
  //   }
  // }

  GUI_PTR->vm_timer_digit_hi->interrupt(2);
  GUI_PTR->vm_timer_digit_lo->interrupt(2);
  field_0x8c = 1;
  flags &= 0xffffff9f;
  ascii_anmid_10 = ASCII_MANAGER_PTR->ascii_anm->createEffect(0);
  text_anmid_14 = SUPERVISOR.text_anm->createEffect(2);
  ascii_anmid_18 = ASCII_MANAGER_PTR->ascii_anm->createEffect(1);

  auto vm = anm::getVM(text_anmid_14);
  anm::text::FUN_00475120(vm, c_white, c_black, 0, 0, sj2utf8(name), name.size());

  // SoundManager::play_sound_centered(0x21);

  spell_circle_anmid = anm::getLoaded(8)->createEffect(13);
  auto boss_0 = ENEMY_MANAGER_PTR->EnmFind(ENEMY_MANAGER_PTR->boss_ids[0]);
  boss0_pos = boss_0->getData()->final_pos.pos;
  auto spell_circle = anm::getVM(spell_circle_anmid);
  if (!spell_circle) {
    spell_circle_anmid = 0;
  } else {
    spell_circle->entity_pos = boss0_pos;
    spell_circle->search_children(11, 0)->int_script_vars[2] = time;
    spell_circle->search_children(12, 0)->int_script_vars[2] = time;
  }
  duration = time;
  int bonuses[] = {500000, 1000000, 1500000, 2000000, 1000000};
  bonus = bonuses[GLOBALS.inner.DIFFICULTY] * GLOBALS.inner.STAGE_NUM;
  bonus_max = fmin(this->bonus, 999999999);
  anm::getLoaded(8)->createEffect(20);
  if (mode < 0 || mode > 3)
    return;
  auto bossdata = STAGE_DATA_TABLE[GLOBALS.inner.STAGE_NUM]["boss_data"][mode];
  if (bossdata["spell_bg_anim_index"] >= 0 &&
      bossdata["spell_bg_anm_script"] >= 0)
    spell_bg_anm_id =
        ENEMY_MANAGER_PTR->loadedAnms[bossdata["spell_bg_anim_index"].asInt()]
            ->createEffect(bossdata["spell_bg_anm_script"].asInt());
  if (bossdata["spell_portrait_anim_index"] >= 0 &&
      bossdata["spell_portrait_anm_script"] >= 0)
    ENEMY_MANAGER_PTR->loadedAnms[bossdata["spell_portrait_anim_index"].asInt()]
        ->createEffect(bossdata["spell_portrait_anm_script"].asInt());
  flags = (flags ^
           ((bossdata["stage_bg_visible_during_spell"].asInt() << 9) ^ flags)) &
          0x200;
  GLOBALS.inner.HYPER_FLAGS &= 0xfffff7ff;
  ns::info("Spell card", sj2utf8(name), " ->", time);
  flags |= 3;
}
