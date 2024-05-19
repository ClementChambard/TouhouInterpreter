#ifndef GUI_INCLUDED_H
#define GUI_INCLUDED_H

#include "AnmOpener/AnmFile.h"
#include "AnmOpener/AnmVM.h"
#include "GuiMsgVm.hpp"
#include "UpdateFuncRegistry.h"

struct BossBar {
  float visual_bar_pc = 0.0;
  float actual_bar_pc = 0.0;
  float current_life = 0.0;
  // undefined4
  float markers[8] = {};
  anm::ID anm_ids[7] = {};
  int vms_created = false;
  int vms_hidden_by_player = false;
};

class Gui {
public:
  Gui();
  ~Gui();
  void set_bomb_meter(int bombs, int bomb_pieces);
  void set_life_meter(int lives, int life_pieces);
  void midScreenInfo(int bonus, int type);
  void FUN_0042a400();
  void create_boss_name();

  // undefined4
  UpdateFunc *on_tick = nullptr;
  UpdateFunc *on_draw_52 = nullptr;
  anm::ID life_meter_anmids[8] = {};
  anm::ID bomb_meter_anmids[8] = {};
  anm::ID timer_digit_hi_anmid = 0;
  anm::ID timer_digit_lo_anmid = 0;
  anm::VM *life_meter_vms[8] = {};
  anm::VM *bomb_meter_vms[8] = {};
  anm::VM *vm_timer_digit_hi = nullptr;
  anm::VM *vm_timer_digit_lo = nullptr;
  anm::ID boss_marker_anmid = 0;
  anm::ID spellcard_score_anmids[10] = {};
  anm::ID some_anmid_0xc8 = 0;
  anm::ID some_anmid_0xcc = 0;
  // undefined4
  // undefined4
  anm::ID boss_name_anmid = 0;
  anm::ID stars_anmids[10] = {};
  anm::ID difficuty_screen_anmid = 0;
  anm::ID difficuty_side_anmid = 0;
  anm::ID player_shottype_anmid = 0;
  anm::ID anmid_0x110_msgRelated = 0;
  // undefined4
  anm::ID stage_clear_anmid = 0;
  anm::ID some_anmid_0x11c = 0;
  anm::ID anmid_0x120_spellTimeShowEnd = 0;
  anm::ID th15_chapter_end_anmid = 0;
  int32_t th15_chapter_graze = 0;
  float th15_chapter_kill_pc = 0;
  float field_0x130 = 0;
  int32_t field_0x134 = 0;
  int32_t th15_chapter_total_bonus = 0;
  int32_t field_0x13c = 0;
  int32_t th15_chapter_retry_count = 0;
  int32_t field_0x144_spellcard_related = 0;
  anm::ID frame_anmid = 0;
  int32_t __timer_14c = 0;
  UpdateFunc *on_draw_49 = nullptr;
  // undefined4
  int32_t current_score = 0;
  // undefined4
  anm::File *stage_logo_anmloaded = nullptr;
  // undefined4
  // undefined4
  // undefined4
  int32_t stars_nb = 0;
  // undefined4
  // undefined4
  // undefined4
  // undefined4
  // undefined4
  // undefined4
  // undefined4
  // undefined4
  int32_t some_flags_1a4 = 0;
  int32_t timer_chapter_end = 0;
  int32_t th15_some_time_value_1bc = 0;
  GuiMsgVm_t* msg = nullptr;
  uint8_t* msg_file_data = nullptr;
  int32_t remaining_spell_time_seconds = 0;
  int32_t remaining_spell_time_centiseconds = 0;
  int32_t old_remaining_spell_time_seconds = 0;
  BossBar boss_bars[3] = {};
  anm::File *front_anm = nullptr;
  int32_t stage_clear_bonus = 0;
  // undefined4

private:
  int f_on_tick();
  int f_on_draw();
};

extern Gui *GUI_PTR;

#endif // !GUI_INCLUDED_H
