#ifndef ANMFUNCS_H_
#define ANMFUNCS_H_

#include "./AnmVM.h"
#include <cstdint>
#include <functional>
#include <vector>

namespace anm {

typedef std::function<int32_t(VM*)> pVMFunc_t;
typedef std::function<int32_t(VM*, int32_t)> pVMFuncI_t;

struct Funcs {
  static pVMFuncI_t on_switch(size_t i);
  static pVMFuncI_t on_sprite_set(size_t i);
  static pVMFunc_t on_draw(size_t i);
  static pVMFunc_t on_tick(size_t i);
  static pVMFunc_t on_destroy(size_t i);
  static pVMFunc_t on_wait(size_t i);
  static pVMFunc_t on_create(size_t i);

  static void set_on_switch(size_t i, pVMFuncI_t f);
  static void set_on_sprite_set(size_t i, pVMFuncI_t f);
  static void set_on_draw(size_t i, pVMFunc_t f);
  static void set_on_tick(size_t i, pVMFunc_t f);
  static void set_on_destroy(size_t i, pVMFunc_t f);
  static void set_on_wait(size_t i, pVMFunc_t f);
  static void set_on_create(size_t i, pVMFunc_t f);

private:
  static std::vector<pVMFuncI_t> m_on_switch;
  static std::vector<pVMFuncI_t> m_on_sprite_set;
  static std::vector<pVMFunc_t> m_on_draw;
  static std::vector<pVMFunc_t> m_on_tick;
  static std::vector<pVMFunc_t> m_on_destroy;
  static std::vector<pVMFunc_t> m_on_wait;
  static std::vector<pVMFunc_t> m_on_create;
};

} // namespace anm

#endif // ANMFUNCS_H_
