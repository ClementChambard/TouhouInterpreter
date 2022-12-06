#ifndef BULLETEX_H
#define BULLETEX_H

#include <glm/glm.hpp>

struct BulletExState_t {
    int remainingTime = 0;
    int ex_id = -1;
    int stepAmount = 0;
    int stepType = 0;
    float startSpeed = 0;
    float startAngle = 0;
};

struct BulletExState_AngleAccel_t {
    int32_t timer = 0;
    float tangential_accel = 0.f;
    float angular_velocity = 0.f;
    int32_t duration = 0;
};
struct BulletExState_Delay_t {
    int32_t timer = 0;
};
struct BulletExState_SaveAngle_t {
    float saved_speed = 0.f;
    float saved_angle = 0.f;
    glm::vec3 saved_pos = {};
};
struct BulletExState_Bounce_t {
    float bounce_speed = 0.f;
    float dim_x = 0.f;
    float dim_y = 0.f;
    int32_t rebounds_so_far = 0;
    int32_t max_rebounds = 0;
    int32_t for_laser_line = 0;
    int32_t wall_flags = 0;
};
struct BulletExState_Speedup_t {
    int32_t timer = 0;
    float unused_0 = 0.f;
};
struct BulletExState_Homing_t {
    int32_t timer = 0;
    float norm = 0.f;
    float angule = 0.f;
    float __m = 0.f;
    int32_t duration = 0;
};
struct BulletExState_VelAdd_t {
    int32_t timer = 0;
    float magnitude = 0.f;
    float angle = 0.f;
    glm::vec3 vel = {};
    int32_t duration = 0;
};
struct BulletExState_Wrap_t {
    int32_t __timer__used_by_lasers;
    int32_t wraps_count;
    int32_t num_wraps;
    int32_t wall_flags;
};
struct BulletExState_Turn_t {
    int32_t timer;
    float speed;
    float angle;
    int32_t duration;
    int32_t max_turns;
    int32_t turns_so_far;
    int32_t aim_type;
    int32_t __d;
};
struct BulletExState_Accel_t {
    int32_t timer;
    float acceleration;
    float angle;
    glm::vec3 vec3_a14;
    int32_t duration;
};
struct BulletExState_VelTime_t {
    int32_t timer;
    float acceleration;
    float field2_0x18;
    glm::vec3 acceleration_vector;
    int32_t duration;
};
struct BulletExState_Wait_t {
    int32_t timer;
};
struct BulletExState_Move_t {
    int32_t timer;
    float __field_bf8__had_4;
    glm::vec3 __rs__target_pos;
    int32_t __a;
    int32_t __b__mode;
};
struct BulletExState_Offscreen_t {
    int32_t timer;
    int32_t __b__unknown;
};


#endif
