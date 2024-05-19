#ifndef GLOBALDATA_H_
#define GLOBALDATA_H_

#include <glm/glm.hpp>
#include <Timer.hpp>

struct GlobalsInner_t {
    i32 STAGE_NUM = 0;
    // undefined4
    i32 CURRENT_CHAPTER = 0;
    i32 TIME_IN_STAGE = 0;
    // undefined4
    i32 field_0x14 = 0;
    i32 CHARACTER = 0;
    i32 SHOTTYPE = 0;
    i32 CURRENT_SCORE = 0;
    i32 DIFFICULTY = 0;
    i32 CONTINUES_USED = 0;
    i32 RANK = 0;
    i32 GRAZE = 0;
    i32 GRAZE_IN_CHAPTER__POSSIBLY_BROKEN = 0;
    i32 SPELL_ID = 0;
    i32 MISS_COUNT_GLOBAL = 0;
    i32 CONTINUES_USED2 = 0;
    i32 NUM_POINT_ITEMS_COLLECTED = 0;
    i32 CURRENT_PIV = 1000000;
    i32 MINIMUM_PIV = 1000000;
    i32 MAXIMUM_PIV = 99999999;
    i32 CURRENT_POWER = 0;
    i32 MAXIMUM_POWER = 0;
    i32 POWER_PER_LEVEL = 0;
    // undefined4
    i32 CURRENT_LIVES = 2;
    i32 CURRENT_LIFE_PIECES = 0;
    i32 lifepiece_related = 0;
    i32 CURRENT_BOMBS = 2;
    i32 CURRENT_BOMB_PIECES = 0;
    i32 bombpiece_related = 2;
    i32 cnt_7c = 0;
    i32 cnt_80 = 0;
    i32 HYPER_FILL = 0;
    i32 TOKENS[5] = {};
    i32 HYPER_FILL2 = 0;
    i32 TOKENS2[5] = {};
    ns::Timer_t __unknown_timer = 0;
    ns::Timer_t HYPER_TIME = 0;
    i32 MAX_HYPER_TIME = 0;
    i32 HYPER_TYPE = 0;
    i32 HYPER_REFILL = 0;
    i32 HYPER_FLAGS = 0;
    // undefined4
    // undefined4
    // undefined4
    // undefined4
    // undefined4
};

class Globals {
public:
    static Globals* get() { static Globals* inst = new Globals(); return inst; }
        i32 timeout = 0;
        i32 DS1 = 0;
        i32 DS2 = 0;
        i32 DS3 = 0;
        i32 DS4 = 0;
        i32 photoCount = 0;
        i32 hyper = 0;
        i32 graze_recent = 0;
        i32 hyperdying = 0;
        i32 achievement_mode = -1;

        i32 HIGH_SCORE = 0;
        i32 __field_4 = 0;
        i32 FLAGS = 0;
        i32 __field_c = 0;
        i32 CREDIT = 0;
        i32 DEMOPLAY_TIMER = 0;
        i32 __field_18 = 0;
        GlobalsInner_t inner{};
};

// TEMPORARY
extern bool GAME_PAUSED;

extern Globals GLOBALS;

#endif // GLOBALDATA_H_
