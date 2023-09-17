#ifndef ANMFUNCS_H_
#define ANMFUNCS_H_

#include "./AnmVM.h"
#include <Timer.hpp>
#include <cstdint>
#include <functional>
#include <vector>

typedef std::function<int32_t(AnmVM*)> pAnmFunc_t;
typedef std::function<int32_t(AnmVM*, int32_t)> pAnmFuncI_t;

struct AnmFuncs {
  static pAnmFuncI_t on_switch(size_t i);
  static pAnmFuncI_t on_sprite_set(size_t i);
  static pAnmFunc_t on_draw(size_t i);
  static pAnmFunc_t on_tick(size_t i);
  static pAnmFunc_t on_destroy(size_t i);
  static pAnmFunc_t on_wait(size_t i);
  static pAnmFunc_t on_create(size_t i);

  static void set_on_switch(size_t i, pAnmFuncI_t f);
  static void set_on_sprite_set(size_t i, pAnmFuncI_t f);
  static void set_on_draw(size_t i, pAnmFunc_t f);
  static void set_on_tick(size_t i, pAnmFunc_t f);
  static void set_on_destroy(size_t i, pAnmFunc_t f);
  static void set_on_wait(size_t i, pAnmFunc_t f);
  static void set_on_create(size_t i, pAnmFunc_t f);

private:
  static std::vector<pAnmFuncI_t> m_on_switch;
  static std::vector<pAnmFuncI_t> m_on_sprite_set;
  static std::vector<pAnmFunc_t> m_on_draw;
  static std::vector<pAnmFunc_t> m_on_tick;
  static std::vector<pAnmFunc_t> m_on_destroy;
  static std::vector<pAnmFunc_t> m_on_wait;
  static std::vector<pAnmFunc_t> m_on_create;
};

#endif // ANMFUNCS_H_
