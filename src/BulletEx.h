#ifndef BULLETEX_H
#define BULLETEX_H

#include <glm/glm.hpp>
#include <Timer.hpp>

struct BulletExState_t {
    ns::Timer_t remainingTime = 0;
    int ex_id = -1;
    int stepAmount = 0;
    int stepType = 0;
    float startSpeed = 0;
    float startAngle = 0;
};

struct BulletExState_AngleAccel_t {
    ns::Timer_t timer = 0;
    float tangential_accel = 0.f;
    float angular_velocity = 0.f;
    int32_t duration = 0;
};

struct BulletExState_Delay_t {
    ns::Timer_t timer = 0;
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
    ns::Timer_t timer = 0;
    float unused_0 = 0.f;
};

struct BulletExState_Homing_t {
    ns::Timer_t timer = 0;
    float norm = 0.f;
    float angule = 0.f;
    float __m = 0.f;
    int32_t duration = 0;
};

struct BulletExState_VelAdd_t {
    ns::Timer_t timer = 0;
    float magnitude = 0.f;
    float angle = 0.f;
    glm::vec3 vel = {};
    int32_t duration = 0;
};

struct BulletExState_Wrap_t {
    ns::Timer_t __timer__used_by_lasers = 0;
    int32_t wraps_count = 0;
    int32_t num_wraps = 0;
    int32_t wall_flags = 0;
};

struct BulletExState_Turn_t {
    ns::Timer_t timer = 0;
    float speed = 0.f;
    float angle = 0.f;
    int32_t duration = 0;
    int32_t max_turns = 0;
    int32_t turns_so_far = 0;
    int32_t aim_type = 0;
    int32_t __d = 0;
};

struct BulletExState_Accel_t {
    ns::Timer_t timer = 0;
    float acceleration = 0.f;
    float angle = 0.f;
    glm::vec3 vec3_a14 = {};
    int32_t duration = 0;
};

struct BulletExState_VelTime_t {
    ns::Timer_t timer = 0;
    float acceleration = 0.f;
    float field2_0x18 = 0.f;
    glm::vec3 acceleration_vector = {};
    int32_t duration = 0;
};

struct BulletExState_Wait_t {
    ns::Timer_t timer = 0;
};

struct BulletExState_Move_t {
    ns::Timer_t timer = 0;
    float __field_bf8__had_4 = 0.f;
    glm::vec3 __rs__target_pos = {};
    int32_t __a = 0;
    int32_t __b__mode = 0;
};

struct BulletExState_Offscreen_t {
    ns::Timer_t timer = 0;
    int32_t __b__unknown = 0;
};


#endif
