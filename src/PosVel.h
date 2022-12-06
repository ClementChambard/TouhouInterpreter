#ifndef POSVEL_H_
#define POSVEL_H_

#include <glm/glm.hpp>
#include <Interpolator.h>

struct PosVel {
    void setVel(float speed, float angle);
    void setVel(float x, float y, float z);
    void setVel();
    glm::vec3 getVel();
    void Update();
    void update_velocities();
    void update_position();
    glm::vec3 pos = {0.f, 0.f, 0.f};
    glm::vec3 __another_pos = {0.f, 0.f, 0.f};
    float speed = 0.f;
    float angle = 0.f;
    float circle_radius = 0.f;
    float circle_radial_speed = 0.f;
    float some_angle1 = 0.f;
    float some_value = 0.f;
    float some_angle2 = 0.f;
    glm::vec3 velocity = {0.f, 0.f, 0.f};
    int32_t flags = 0;
};

PosVel operator+(PosVel const& a, PosVel const& b);

#endif // POSVEL_H_
