#ifndef ECLINSTRMACROS_H_
#define ECLINSTRMACROS_H_

#include "./shiftjis.h"
#include <Error.h>
#include <string>

#define _insNop                                                                \
  unsigned char *__arg = const_cast<unsigned char *>(instr->data);             \
  int __argmask = 1;                                                           \
  if ((instr->rank_mask & 63 & cont->difficultyMask)) {                        \
    switch (instr->id) {                                                       \
    case 0: {                                                                  \
      std::string __name = "nop";                                              \
      [[maybe_unused]] int __id = 0;

#define _ins(id, name)                                                         \
  break;                                                                       \
  }                                                                            \
  case id: {                                                                   \
    [[maybe_unused]] int __id = id;                                            \
    std::string __name = #name;

#define _insDefault                                                            \
  break;                                                                       \
  }                                                                            \
  default: {                                                                   \
  }                                                                            \
    }                                                                          \
    cont->stack.stackOffset -= stackToRemove;                                  \
    stackToRemove = 0;                                                         \
    }                                                                          \
    cont->currentLocation.offset += instr->size;

#define _popS(x)                                                               \
  int32_t x;                                                                   \
  eclStackPop(&cont->stack, x);

#define _popf(x)                                                               \
  float x;                                                                     \
  eclStackPop(&cont->stack, x);

#define _push(x) eclStackPush(&cont->stack, x);

#define _S(x)                                                                  \
  [[maybe_unused]] int32_t x = *reinterpret_cast<int32_t *>(__arg);            \
  [[maybe_unused]] int32_t __##x##_var = 0;                                    \
  bool __##x##_isvar = instr->param_mask & __argmask;                          \
  if (__##x##_isvar) {                                                         \
    __##x##_var = x;                                                           \
    x = checkVarI(x);                                                          \
  }                                                                            \
  __arg += 4;                                                                  \
  __argmask *= 2;

#define _f(x)                                                                  \
  [[maybe_unused]] float x = *reinterpret_cast<float *>(__arg);                \
  [[maybe_unused]] float __##x##_var = 0;                                      \
  bool __##x##_isvar = instr->param_mask & __argmask;                          \
  if (__##x##_isvar) {                                                         \
    __##x##_var = x;                                                           \
    x = checkVarF(x);                                                          \
  }                                                                            \
  __arg += 4;                                                                  \
  __argmask *= 2;

#define _rS(x)                                                                 \
  int32_t __##x##_var = *reinterpret_cast<int32_t *>(__arg);                   \
  [[maybe_unused]] int32_t &x = checkVarRI(__##x##_var);                       \
  __arg += 4;                                                                  \
  __argmask *= 2;

#define _rf(x)                                                                 \
  float __##x##_var = *reinterpret_cast<float *>(__arg);                       \
  [[maybe_unused]] float &x = checkVarRF(__##x##_var);                         \
  __arg += 4;                                                                  \
  __argmask *= 2;

#define _z(x)                                                                  \
  int32_t __##x##_size = *reinterpret_cast<int32_t *>(__arg);                  \
  __arg += 4;                                                                  \
  std::string x = "";                                                          \
  for (int i = 0; i < __##x##_size; i++)                                       \
    if (__arg[i] != 0)                                                         \
      x += __arg[i];                                                           \
  __arg += __##x##_size;                                                       \
  __argmask *= 2;

#define _zc(x)                                                                 \
  int32_t __##x##_size = *reinterpret_cast<int32_t *>(__arg);                  \
  __arg += 4;                                                                  \
  std::string x = "";                                                          \
  unsigned char *data = new unsigned char[__##x##_size];                       \
  memcpy(data, __arg, __##x##_size);                                           \
  util_xor(data, __##x##_size, 0x77, 7, 16);                                   \
  x = reinterpret_cast<char *>(data);                                          \
  delete[] data;                                                               \
  __arg += __##x##_size;                                                       \
  __argmask *= 2;

#define _args                                                                  \
  cont->stack.stackOffset -= stackToRemove;                                    \
  stackToRemove = 0;

#define I_(n) enemy.ecl_int_vars[n]
#define f_(n) enemy.ecl_float_vars[n]

#define _notImpl ns::warning("/!\\", __name, "is not implemented");

#define _alert ns::warning("/!\\ alert --->", __name, "has been used");

#define _warn(m) ns::warning("/!\\ warning at", __name, ":", m);

#define _ret return 0;

#define isBoss_                                                                \
  (EnemyManager::GetInstance()->boss_ids[0] > 0 &&                             \
   EnemyManager::GetInstance()->EnmFind(                                       \
       EnemyManager::GetInstance()->boss_ids[0]))

#define BOSS_                                                                  \
  EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->boss_ids[0])

#endif // ECLINSTRMACROS_H_
