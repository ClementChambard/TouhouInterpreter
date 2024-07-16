#ifndef BULLETEX_H
#define BULLETEX_H

#include <math/types/types.hpp>
#include <Timer.hpp>

struct BulletExState_t {
    ns::Timer_t remainingTime = 0;
    i32 ex_id = -1;
    i32 stepAmount = 0;
    i32 stepType = 0;
    f32 startSpeed = 0;
    f32 startAngle = 0;
};

struct BulletExState_AngleAccel_t {
    ns::Timer_t timer = 0;
    f32 tangential_accel = 0.f;
    f32 angular_velocity = 0.f;
    i32 duration = 0;
};

struct BulletExState_Delay_t {
    ns::Timer_t timer = 0;
};

struct BulletExState_SaveAngle_t {
    f32 saved_speed = 0.f;
    f32 saved_angle = 0.f;
    ns::vec3 saved_pos = {};
};

struct BulletExState_Bounce_t {
    f32 bounce_speed = 0.f;
    f32 dim_x = 0.f;
    f32 dim_y = 0.f;
    i32 rebounds_so_far = 0;
    i32 max_rebounds = 0;
    i32 for_laser_line = 0;
    i32 wall_flags = 0;
};

struct BulletExState_Speedup_t {
    ns::Timer_t timer = 0;
    f32 unused_0 = 0.f;
};

struct BulletExState_Homing_t {
    ns::Timer_t timer = 0;
    f32 norm = 0.f;
    f32 angule = 0.f;
    f32 __m = 0.f;
    i32 duration = 0;
};

struct BulletExState_VelAdd_t {
    ns::Timer_t timer = 0;
    f32 magnitude = 0.f;
    f32 angle = 0.f;
    ns::vec3 vel = {};
    i32 duration = 0;
};

struct BulletExState_Wrap_t {
    ns::Timer_t __timer__used_by_lasers = 0;
    i32 wraps_count = 0;
    i32 num_wraps = 0;
    i32 wall_flags = 0;
};

struct BulletExState_Turn_t {
    ns::Timer_t timer = 0;
    f32 speed = 0.f;
    f32 angle = 0.f;
    i32 duration = 0;
    i32 max_turns = 0;
    i32 turns_so_far = 0;
    i32 aim_type = 0;
    i32 __d = 0;
};

struct BulletExState_Accel_t {
    ns::Timer_t timer = 0;
    f32 acceleration = 0.f;
    f32 angle = 0.f;
    ns::vec3 vec3_a14 = {};
    i32 duration = 0;
};

struct BulletExState_VelTime_t {
    ns::Timer_t timer = 0;
    f32 acceleration = 0.f;
    f32 field2_0x18 = 0.f;
    ns::vec3 acceleration_vector = {};
    i32 duration = 0;
};

struct BulletExState_Wait_t {
    ns::Timer_t timer = 0;
};

struct BulletExState_Move_t {
    ns::Timer_t timer = 0;
    f32 __field_bf8__had_4 = 0.f;
    ns::vec3 __rs__target_pos = {};
    i32 __a = 0;
    i32 __b__mode = 0;
};

struct BulletExState_Offscreen_t {
    ns::Timer_t timer = 0;
    i32 __b__unknown = 0;
};


#endif
