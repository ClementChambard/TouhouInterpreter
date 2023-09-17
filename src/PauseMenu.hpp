#ifndef PAUSEMENU_INCLUDED_H
#define PAUSEMENU_INCLUDED_H

#include "AnmOpener/AnmFile.h"
#include "AnmOpener/AnmVM.h"
#include "./MenuHelper.hpp"
#include <Timer.hpp>
#include "./UpdateFuncRegistry.h"

class PauseMenu {
public:
  PauseMenu();
  ~PauseMenu();

  void open_submenu1_finish_replay();
  void open_submenu2();
  void open_submenu3();

  int32_t flags_0 = 0;
  UpdateFunc* on_tick = nullptr;
  UpdateFunc* on_draw = nullptr;
  NSEngine::Timer_t timer4_0xc = {};
  NSEngine::Timer_t timer5_0x20 = {};
  MenuHelper_t menu_helper = {};
  MenuHelper_t keyboard_menu_helper = {};
  AnmID menu_anmid = 0;
  AnmID pause_blur_anmid = 0;
  int32_t field_1ec = 0;
  int32_t field_1f0_old_1ec = 0;
  int32_t submenu = 0;
  int32_t keyboard_entry_size = 0;
  int32_t field_1fc = 0;
  int32_t field_200 = 0;
  int32_t prev_submenu = 0;
  // saved dat...
  // lot undefined
  char keyboard_entry[8] = {};
  // undefined4
  // saved game speed
  // lots undefined
  int32_t flags_3f0 = 0;
  AnmFile* front_anm = nullptr;

private:
  int f_on_tick();
  int f_on_draw();
  void draw_keyboard(glm::vec3 const& entry_pos);
  void draw_save_replay();
  void draw_score_rankings();
  void draw_thing_w_keyboard();
  void init_pause_blur_effect();
  void update_submenus();

  void go_to_submenu(int i) { submenu = i; timer4_0xc = 0; }

};

extern PauseMenu* PAUSE_MENU_PTR;

#endif // !PAUSEMENU_INCLUDED_H
