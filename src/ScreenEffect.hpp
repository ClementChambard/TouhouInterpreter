#ifndef SCREENEFFECT_INCLUDED_H
#define SCREENEFFECT_INCLUDED_H

#include <Timer.hpp>
#include "./UpdateFuncRegistry.h"

class ScreenEffect {
public:
  ScreenEffect(int, int, int, int, int, int);
  ~ScreenEffect();

  // undefined4 flags
  UpdateFunc* on_tick = nullptr;
  UpdateFunc* on_draw = nullptr;
  int type = 0;
  int field_0x10 = 0;
  int alpha = 0;
  int field_0x18 = 0; // duration
  int field_0x1c = 0; // col_val
  int field_0x20 = 0; // color
  int field_0x24 = 0;
  int field_0x28 = 0;
  ns::Timer_t time = {};

private:
  int f_on_tick_1();
  int f_on_tick_2();
  int f_on_tick_3();
  int f_on_tick_4();
  int f_on_tick_5();
  int f_on_tick_6();
  int f_on_tick_7();
  int f_on_draw_1();
  int f_on_draw_2();
  int f_on_draw_3();
  int f_on_draw_4();
  int f_on_draw_5();
};

#endif // !SCREENEFFECT_INCLUDED_H
