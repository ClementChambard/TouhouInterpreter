#include "./PosVel.h"
#include <math/math.hpp>
#include <NSEngine.hpp>

#define GAME_SPEED ns::get_instance()->game_speed()

void PosVel::Update() {
    // pos.update();
    // speed.update();
    // angle.update();
    // radial_dist.update();
    // radial_speed.update();
}

void PosVel::setVel() {
    velocity.x = speed * ns::cos(angle);
    velocity.y = speed * ns::sin(angle);
}

void PosVel::setVel(float speed, float angle) {
    this->speed = speed;
    this->angle = angle;
    velocity.x = speed * ns::cos(angle);
    velocity.y = speed * ns::sin(angle);
}

void PosVel::setVel(float x, float y, float z) {
    velocity = {x, y, z};
    speed = ns::sqrt(x*x+y*y);
    angle = ns::atan(y/x);
}

ns::vec3 PosVel::getVel() {
    return {speed * ns::cos(angle), speed * ns::sin(angle), 0};
}

PosVel operator+(PosVel const& a, PosVel const& b) {
    PosVel res;
    res.pos = a.pos + b.pos;
    ns::vec3 vel = a.velocity + b.velocity;
    res.setVel(vel.x, vel.y, vel.z);
    return res;
}

void PosVel::update_velocities() {
    switch (flags & 0xf) {
        case 0:
            velocity = {math::lengthdir_vec(speed * GAME_SPEED, angle), 0.f};
            return;
        case 2:
        case 3:
            circle_radius += circle_radial_speed * GAME_SPEED;
            angle += speed * GAME_SPEED;
            math::angle_normalize(angle);
          return;
        case 4:
            some_angle2 += circle_radial_speed * GAME_SPEED;
            math::angle_normalize(some_angle2);
            velocity = {math::lengthdir_vec(speed * GAME_SPEED, some_angle1), 0.f};
    }
    return;
}

void PosVel::update_position() {
  ns::vec2 saved_pos;
  float Cos_a1;
  float Sin_a1;

  switch (flags & 0xf) {
  case 0:
    pos+= velocity;
    break;
  case 2:
    pos = velocity + ns::vec3(math::lengthdir_vec(circle_radius, angle), 0.f);
    break;
  case 3:
    saved_pos = math::lengthdir_vec(circle_radius, angle-some_angle1);
    Sin_a1 = ns::sin(some_angle1);
    Cos_a1 = ns::cos(some_angle1);
    pos.x = velocity.x + some_value * saved_pos.x * Cos_a1 -
                saved_pos.y * Sin_a1;
    pos.y = velocity.y + some_value * saved_pos.x * Sin_a1 +
                saved_pos.y * Cos_a1;
    pos.z = velocity.z + 0.0;
    break;
  case 4:
    saved_pos = pos;
    __another_pos += velocity;
    pos = __another_pos + ns::vec3(math::lengthdir_vec(ns::sin(some_angle2) *
                                circle_radius * GAME_SPEED, some_angle1 + ns::PI_1_2<f32>), 0.f);
    angle += math::point_direction(saved_pos.x, saved_pos.y, pos.x, pos.y);
    math::angle_normalize(angle);
  }
  // ?????????????
  // pos.x = trig_func_?_4049d0(pos.x * 100.f) / 100.f;
  // pos.y = trig_func_?_4049d0(pos.y * 100.f) / 100.f;
  return;
}
