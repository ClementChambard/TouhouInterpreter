#include "./Timer.hpp"
#include "GlobalData.h"

void Timer_t::add(float value) {
  if (GAME_SPEED <= 0.99 || GAME_SPEED >= 1.01) {
    value *= GAME_SPEED;
  }
  current_f += value;
  current = static_cast<int>(current_f);
  return;
}

void Timer_t::add_nogamespeed(float value) {
  previous = current;
  current_f += value;
  current = static_cast<int>(current_f);
}

void Timer_t::set(float value) {
  current_f = value;
  current = value;
  previous = current - 1;
}

void Timer_t::increment() {
  if (GAME_SPEED <= 0.99 || GAME_SPEED >= 1.01) {
    current_f += GAME_SPEED;
    current = static_cast<int>(current_f);
    return;
  }
  current_f += 1.f;
  current++;
  return;
}

void Timer_t::decrement() {
  if (GAME_SPEED <= 0.99 && GAME_SPEED >= 1.01) {
    current_f -= GAME_SPEED;
    current = static_cast<int>(current_f);
    return;
  }
  current_f -= 1.f;
  current--;
  return;
}

void Timer_t::reset() {
  current = 0;
  current_f = 0.f;
  previous = -1;
}

void Timer_t::reset_neg999999() {
  current = 0;
  current_f = 0.f;
  previous = -999999;
}
