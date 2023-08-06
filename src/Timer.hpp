#ifndef TIMER_INCLUDED_H
#define TIMER_INCLUDED_H

#include <cstdint>

struct Timer_t {
  Timer_t() {}
  Timer_t(float c) { set(c); }
  Timer_t(int32_t c) { set(c); }

  void add(float value);
  void add_nogamespeed(float value);
  void set(float value);
  void increment();
  void decrement();
  void reset();
  void reset_neg999999();

  Timer_t &operator=(int32_t c) {
    set(c);
    return *this;
  }

  Timer_t &operator++() {
    increment();
    return *this;
  }
  Timer_t operator++(int) {
    auto o = *this;
    increment();
    return o;
  }
  Timer_t &operator--() {
    decrement();
    return *this;
  }
  Timer_t operator--(int) {
    auto o = *this;
    decrement();
    return o;
  }

  int32_t previous = -1;
  int32_t current = 0;
  float current_f = 0.f;
  // float* __game_speed__disused = nullptr;
  // int32_t control = 0;
};

inline bool operator==(Timer_t const &t1, Timer_t const &t2) {
  return t1.current == t2.current;
}
inline bool operator==(Timer_t const &t1, int32_t t2) {
  return t1.current == t2;
}
inline bool operator==(int32_t t1, Timer_t const &t2) { return t2 == t1; }
inline bool operator!=(Timer_t const &t1, Timer_t const &t2) {
  return t1.current != t2.current;
}
inline bool operator!=(Timer_t const &t1, int32_t t2) {
  return t1.current != t2;
}
inline bool operator!=(int32_t t1, Timer_t const &t2) { return t2 != t1; }
inline bool operator<=(Timer_t const &t1, Timer_t const &t2) {
  return t1.current <= t2.current;
}
inline bool operator<=(Timer_t const &t1, int32_t t2) {
  return t1.current <= t2;
}
inline bool operator<=(int32_t t1, Timer_t const &t2) {
  return t1 <= t2.current;
}
inline bool operator>=(Timer_t const &t1, Timer_t const &t2) {
  return t1.current >= t2.current;
}
inline bool operator>=(Timer_t const &t1, int32_t t2) {
  return t1.current >= t2;
}
inline bool operator>=(int32_t t1, Timer_t const &t2) {
  return t1 >= t2.current;
}
inline bool operator<(Timer_t const &t1, Timer_t const &t2) {
  return t1.current < t2.current;
}
inline bool operator<(Timer_t const &t1, int32_t t2) { return t1.current < t2; }
inline bool operator<(int32_t t1, Timer_t const &t2) { return t1 < t2.current; }
inline bool operator>(Timer_t const &t1, Timer_t const &t2) {
  return t1.current > t2.current;
}
inline bool operator>(Timer_t const &t1, int32_t t2) { return t1.current > t2; }
inline bool operator>(int32_t t1, Timer_t const &t2) { return t1 > t2.current; }
inline bool operator!(Timer_t const &t) { return !t.current; }

#endif // !TIMER_INCLUDED_H
