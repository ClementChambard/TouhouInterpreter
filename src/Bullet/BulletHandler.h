#ifndef _BULLETHANDLER_H_INCLUDED
#define _BULLETHANDLER_H_INCLUDED

#include <math/types/types.hpp>

struct EtEx_t {
    f32 r = 0.f;
    f32 s = 0.f;
    f32 m = 0.f;
    f32 n = 0.f;
    i32 a = 0;
    i32 b = 0;
    i32 c = 0;
    i32 d = 0;
    i32 type = 0;
    i32 slot = 0;
    const char* string = nullptr;
    EtEx_t& operator=(EtEx_t const& other);
};

struct EnemyBulletShooter_t {
    i32 type = 0;
    u32 __color = 0;
    ns::vec3 __vec3_8{};
    f32 ang_aim = 0.f;
    f32 ang_bullet_dist = 0.f;
    f32 spd1 = 2.f;
    f32 spd2 = 0.f;
    f32 distance = 0.f;
    EtEx_t ex[20] = {};
    f32 __laser_new_arg_4 = 0.f;
    f32 __field_344 = 0.f;
    f32 __field_348 = 0.f;
    f32 __field_34c = 0.f;
    i32 __laser_timing_start = 0;
    i32 __laser_timing_trans1 = 0;
    i32 __laser_timing_duration = 0;
    i32 __laser_timing_trans2 = 0;
    i32 __laser_timing_end = 0;
    i16 cnt_count = 1;
    i16 cnt_layers = 1;
    i32 aim_type = 0;
    u32 sfx_flag = 0x23;
    u32 shot_sfx = 0x15;
    u32 __shot_transform_sfx = 0x26;
    u32 __start_transform = 0;
    u32 __field_37c = 0;
};

void BM_reset(EnemyBulletShooter_t* bm);

#endif
