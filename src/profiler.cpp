#include "./profiler.h"

const unsigned int __nTimes = 30;
float __times[__nTimes];
float __next_times[__nTimes];
unsigned int __nToDr = 0;
float __ignored_tim = 0;
unsigned int __ti = 0;
std::chrono::time_point<std::chrono::high_resolution_clock> __begin_time;

#include <cmath>
NSEngine::Color hsv(float H, float S, float V) {
    float C = V * S;

    float X = C * (1 - fabs((std::fmod((H / 60.f), 2) - 1)));

    float m = V - C;

    float R{}, G{}, B{};

    if (  0 <= H && H <  60) R = C, G = X, B = 0;
    if ( 60 <= H && H < 120) R = X, G = C, B = 0;
    if (120 <= H && H < 180) R = 0, G = C, B = X;
    if (180 <= H && H < 240) R = 0, G = X, B = C;
    if (240 <= H && H < 300) R = X, G = 0, B = C;
    if (300 <= H && H < 360) R = C, G = 0, B = X;

    return {(uint8_t)((R+m)*255), (uint8_t)((G+m)*255),
            (uint8_t)((B+m)*255), 255};
}
