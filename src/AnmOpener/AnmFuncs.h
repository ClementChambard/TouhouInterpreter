#ifndef ANMFUNCS_H_
#define ANMFUNCS_H_

#include <cstdint>
#include "AnmVM.h"
#include <Timer.hpp>

struct EFFECT_1_buffer_t {
  AnmVM vms[5] = {};
  int32_t i1 = 0;
  int32_t i2 = 0;
};

struct EFFECT_2_buffer_t {
  AnmID ids[200] = {};
  glm::vec3 endpos[200] = {};
  glm::vec3 endbez[200] = {};
  int phase[200] = {};
  glm::vec3 midpoint = {};
  glm::vec3 startpoint = {};
  glm::vec3 endpoint = {};
  NSEngine::Timer_t timer = {};
};

extern int (*ANM_ON_SWITCH_FUNCS[5])(AnmVM*, int);
extern int32_t (*ANM_ON_SPRITE_SET_FUNCS[4])(AnmVM*, int32_t);
extern int (*ANM_ON_DRAW_FUNCS[8])(AnmVM*);
extern int (*ANM_ON_TICK_FUNCS[7])(AnmVM*);
extern int (*ANM_ON_DESTROY_FUNCS[5])(AnmVM*);
extern int (*ANM_ON_WAIT_FUNCS[1])(AnmVM*);
#endif // ANMFUNCS_H_
