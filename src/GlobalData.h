#ifndef GLOBALDATA_H_
#define GLOBALDATA_H_

#include <cstdint>

struct GlobalsInner_t {
    int32_t STAGE_NUM = 0;
    // undefined4
    int32_t CURRENT_CHAPTER = 0;
    int32_t TIME_IN_STAGE = 0;
    // undefined4[2]
    int32_t CHARACTER = 0;
    int32_t SHOTTYPE = 0;
    int32_t CURRENT_SCORE = 0;
    int32_t DIFFICULTY = 0;
    int32_t CONTINUES_USED = 0;
    int32_t RANK = 0;
    int32_t GRAZE = 0;
    int32_t GRAZE_IN_CHAPTER__POSSIBLY_BROKEN = 0;
    int32_t SPELL_ID = 0;
    int32_t MISS_COUNT_GLOBAL = 0;
    int32_t CONTINUES_USED2 = 0;
    int32_t NUM_POINT_ITEMS_COLLECTED = 0;
    int32_t CURRENT_PIV = 1000000;
    int32_t MINIMUM_PIV = 1000000;
    int32_t MAXIMUM_PIV = 99999999;
    int32_t CURRENT_POWER = 0;
    int32_t MAXIMUM_POWER = 0;
    int32_t POWER_PER_LEVEL = 0;
    // undefined4
    int32_t CURRENT_LIVES = 0;
    int32_t CURRENT_LIFE_PIECES = 0;
    int32_t lifepiece_related = 0;
    int32_t CURRENT_BOMBS = 0;
    int32_t CURRENT_BOMB_PIECES = 0;
    int32_t bombpiece_related = 0;
    // undefined4
    // undefined4
    int32_t HYPER_FILL = 0;
    int32_t TOKENS[5] = {};
    // undefined4
    // undefined4
    // undefined4
    // undefined4
    // undefined4
    // undefined4
    int32_t __unknown_timer = 0;
    int32_t HYPER_TIME = 0;
    // undefined4
    // undefined4
    // undefined4
    int32_t HYPER_FLAGS = 0;
    // undefined4
    // undefined4
    // undefined4
    // undefined4
    // undefined4
};

class Globals {
public:
    static Globals* get() { static Globals* inst = new Globals(); return inst; }
        int timeout = 0;
        int DS1 = 0;
        int DS2 = 0;
        int DS3 = 0;
        int DS4 = 0;
        int photoCount = 0;
        int hyper = 0;
        int graze_recent = 0;
        int hyperdying = 0;
        int achievement_mode = -1;

        int32_t HIGH_SCORE = 0;
        int32_t __field_4 = 0;
        int32_t FLAGS = 0;
        int32_t __field_c = 0;
        int32_t __field_10 = 0;
        int32_t DEMOPLAY_TIMER = 0;
        int32_t __field_18 = 0;
        GlobalsInner_t inner{};
};

extern Globals GLOBALS;
extern float GAME_SPEED;

#endif // GLOBALDATA_H_
