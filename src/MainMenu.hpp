#ifndef MAINMENU_HEADER_INCLUDED
#define MAINMENU_HEADER_INCLUDED

#include "Anm/AnmFile.h"
#include "MenuHelper.hpp"
#include "UpdateFuncRegistry.h"
struct MainMenu {

  MainMenu();
  ~MainMenu();

  int f_on_tick();
  int f_on_draw();

  UpdateFunc* on_tick = nullptr;
  UpdateFunc* on_draw = nullptr;
  anm::File* title_anm = nullptr;
  anm::File* title_v_anm = nullptr;
  i32 int_0x18_maybe_submenu = 0;
  i32 int_0x1c_old_0x18 = 0;
  i32 int_0x20_maybe_selection = 0;
  MenuHelper_t menu_helper_0x24 = {};
  MenuHelper_t menu_helper_0xfc = {};
  MenuHelper_t menu_helper_0x1d4 = {};
  ns::Timer_t timer_0x2ac = 0;
  anm::ID main_menu_options_anmid = 0;
  anm::ID settings_options_anmid = 0;
  anm::ID keys_options_anmid = 0;
  // 83 undefined4
  anm::ID main_bg_anmid = 0;
  // 3 undefined4
  anm::ID logo_anmid = 0;
  // 3 undefined4
  anm::ID top_playerselect_anmid = 0;
  anm::ID top_beastselect_anmid = 0;
  anm::ID top_spellcardselect_anmid = 0;
  anm::ID top_replay_anmid = 0;
  anm::ID top_playerdata_anmid = 0;
  anm::ID top_musicroom_anmid = 0;
  // undefined4
  anm::ID top_savereplay_anmid = 0;
  anm::ID top_stageselect_anmid = 0;
  anm::ID top_manual_anmid = 0;
  anm::ID top_playerdata_anmid2 = 0;
  anm::ID top_achievement_anmid = 0;
  // 10 undefined4
  anm::ID anmids_0x490[2] = {};
  // 12 undefined4
  anm::ID anmids_0x4c8[4] = {};
  // 14 undefined4


  int field5605_0x5df0 = 0;

void sub_456cd0();
void sub_458bf0();
void sub_457590();
void sub_4595b0();
void sub_459f20();
void sub_45c700();
void sub_45d2a0();

};

extern MainMenu* MAIN_MENU_PTR;

#endif // MAINMENU_HEADER_INCLUDED
