#ifndef _BULLETHANDLER_H_INCLUDED
#define _BULLETHANDLER_H_INCLUDED

#include <glm/glm.hpp>

struct EtEx_t {
    float r = 0.f;
    float s = 0.f;
    float m = 0.f;
    float n = 0.f;
    int32_t a = 0;
    int32_t b = 0;
    int32_t c = 0;
    int32_t d = 0;
    int32_t type = 0;
    int32_t slot = 0;
    const char* string = 0;
    EtEx_t& operator=(EtEx_t& other);
};

struct EnemyBulletShooter_t {
    int32_t type = 0;
    uint32_t __color = 0;
    glm::vec3 __vec3_8 = {};
    float ang_aim = 0.f;
    float ang_bullet_dist = 0.f;
    float spd1 = 2.f;
    float spd2 = 0.f;
    float distance = 0.f;
    EtEx_t ex[20] = {};
    float __laser_new_arg_4 = 0.f;
    float __field_344 = 0.f;
    float __field_348 = 0.f;
    float __field_34c = 0.f;
    int32_t __laser_timing_start = 0;
    int32_t __laser_timing_trans1 = 0;
    int32_t __laser_timing_duration = 0;
    int32_t __laser_timing_trans2 = 0;
    int32_t __laser_timing_end = 0;
    int16_t cnt_count = 1;
    int16_t cnt_layers = 1;
    int32_t aim_type = 0;
    uint32_t sfx_flag = 0x23;
    uint32_t shot_sfx = 0x15;
    uint32_t __shot_transform_sfx = 0x26;
    uint32_t __start_transform = 0;
    uint32_t __field_37c = 0;
};

void BM_reset(EnemyBulletShooter_t* bm);

#endif
