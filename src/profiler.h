#ifndef PROFILER
#define PROFILER

#include <vertex.h>
#include <chrono>

extern const unsigned int __nTimes;
extern float __times[];
extern float __next_times[];
extern unsigned int __nToDr;
extern float __ignored_tim;
extern unsigned int __ti;
extern std::chrono::time_point<std::chrono::high_resolution_clock> __begin_time;

#define __BG {0, 0, 0, 255}
#define __X0 -180.f
#define __Y0 -20.f
#define __SIZY 7.f
#define __SIZX 230.f
#define __NOW std::chrono::high_resolution_clock::now()

#define __DURNS(a, b) std::chrono::duration_cast<std::chrono::nanoseconds>\
                      (b-a).count()

#define __START __ti = 0; \
                for (uint i = 0; i < __nTimes; i++) {\
                    __times[i] = __next_times[i]; \
                    __next_times[i] = 0; \
                } \
                __begin_time = __NOW;

#define __STOP __nToDr = __ti;

#define __SPLIT __next_times[__ti++] += __DURNS(__begin_time, __NOW) + \
                    __ignored_tim; \
                __begin_time = __NOW; \
                __ignored_tim = 0;

#define __IGNORE __ignored_tim += __DURNS(__begin_time, __NOW); \
                 __begin_time = __NOW;

#define __RESUME __begin_time = __NOW;

#define __RESTART __ti = 0;
#define __DRAW float __x0 = __X0; \
               float __hue = 0.f; \
               float __hueInc = 360.f / static_cast<float>(__nToDr); \
               float __y0 = __Y0 - __SIZY; \
               ns::draw_rectangle_color(__X0, __Y0, __X0 + __SIZX, \
                                    __Y0 + __SIZY, __BG, __BG, __BG, __BG); \
               ns::draw_rectangle_color(__X0 + __SIZX + __SIZY * 0.5 -1, \
                                              __Y0 -1, __X0 + __SIZX + __SIZY \
                                              * 1.5 + __SIZY * (__nToDr - 1) * \
                                              1.5 + 1, __Y0 + __SIZY +1, __BG, \
                                              __BG, __BG, __BG); \
               for (int i = 0; i < __nToDr; i++) { \
                   float __end = __x0 + __SIZX / 16666666.66 * __times[i]; \
                   auto __col = hsv(__hue, 1, 1); \
                   ns::draw_rectangle_color(fmin(__x0, __X0+__SIZX), \
                        __Y0, fmin(__end, __X0+__SIZX), __Y0+__SIZY, __col, \
                            __col, __col, __col); \
                   __x0 = __end; \
                   __hue += __hueInc; \
                   ns::draw_rectangle_color(__X0+__SIZX + __SIZY * 0.5 + \
                       __SIZY * i * 1.5, __Y0, __X0 + __SIZX + __SIZY * 1.5 + \
                        __SIZY*i*1.5, __Y0+__SIZY, __col, __col, __col, __col);\
               }

#define __MONITOR(v, c) \
             ns::draw_rectangle_color(__X0, __y0, __X0+__SIZX, __y0+ \
                                            __SIZY, __BG, __BG, __BG, __BG); \
             ns::draw_rectangle_color(__X0, __y0, __X0+__SIZX*fmin(v, 1)\
                                            , __y0+__SIZY, c, c, c, c); \
             __y0 -= __SIZY;

ns::Color hsv(float H, float S, float V);

#endif
