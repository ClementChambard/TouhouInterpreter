#ifndef ANMFUNCS_H_
#define ANMFUNCS_H_

#include <cstdint>
#include "AnmVM.h"

extern int (*ANM_ON_SWITCH_FUNCS[5])(AnmVM*, int);
extern int32_t (*ANM_ON_SPRITE_SET_FUNCS[6])(AnmVM*, int32_t);
extern void (*ANM_ON_DRAW_FUNCS[8])(AnmVM*);
extern int (*ANM_ON_DESTROY_FUNCS[5])(AnmVM*);

#endif // ANMFUNCS_H_
