#ifndef POSVEL_H_
#define POSVEL_H_

#include <math/types/types.hpp>

struct PosVel {
    void setVel(float speed, float angle);
    void setVel(float x, float y, float z);
    void setVel();
    ns::vec3 getVel();
    void Update();
    void update_velocities();
    void update_position();
    ns::vec3 pos = {0.f, 0.f, 0.f};
    ns::vec3 __another_pos = {0.f, 0.f, 0.f};
    f32 speed = 0.f;
    f32 angle = 0.f;
    f32 circle_radius = 0.f;
    f32 circle_radial_speed = 0.f;
    f32 some_angle1 = 0.f;
    f32 some_value = 0.f;
    f32 some_angle2 = 0.f;
    ns::vec3 velocity = {0.f, 0.f, 0.f};
    i32 flags = 0;
};

PosVel operator+(PosVel const& a, PosVel const& b);

#endif // POSVEL_H_
