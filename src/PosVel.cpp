#include "PosVel.h"

void PosVel::Update()
{
    //pos.update();
    //speed.update();
    //angle.update();
    //radial_dist.update();
    //radial_speed.update();
}

void PosVel::setVel()
{
    velocity.x = speed * cos(angle);
    velocity.y = speed * sin(angle);
}

void PosVel::setVel(float speed, float angle)
{
    this->speed = speed;
    this->angle = angle;
    velocity.x = speed * cos(angle);
    velocity.y = speed * sin(angle);
}

void PosVel::setVel(float x, float y, float z)
{
    velocity = {x, y, z};
    speed = sqrt(x*x+y*y);
    angle = atan(y/x);
}

glm::vec3 PosVel::getVel()
{
    return {speed * cos(angle), speed * sin(angle), 0};
}

PosVel operator+(PosVel const& a, PosVel const& b)
{
    PosVel res;
    res.pos = a.pos + b.pos;
    glm::vec3 vel = a.velocity + b.velocity;
    res.setVel(vel.x, vel.y, vel.z);
    return res;
}

void PosVel::update_velocities()
{
    switch(flags & 0xf) {
        case 0:
            velocity = {math::lengthdir_vec(speed, angle), 0.f};
            return;
        case 2:
        case 3:
            circle_radius += circle_radial_speed;
            angle += speed;
            math::angle_normalize(angle);
          return;
        case 4:
            some_angle2 += circle_radial_speed;
            math::angle_normalize(some_angle2);
            velocity = {math::lengthdir_vec(speed, some_angle1), 0.f};
    }
    return;
}

void PosVel::update_position()
{
  glm::vec2 saved_pos;
  float Cos_a1;
  float Sin_a1;

  switch(flags & 0xf) {
  case 0:
    pos+= velocity;
    break;
  case 2:
    pos = velocity + glm::vec3(math::lengthdir_vec(circle_radius, angle), 0.f);
    break;
  case 3:
    saved_pos = math::lengthdir_vec(circle_radius, angle-some_angle1);
    Sin_a1 = sinf(some_angle1);
    Cos_a1 = cosf(some_angle1);
    pos.x = velocity.x + some_value * saved_pos.x * Cos_a1 - saved_pos.y * Sin_a1;
    pos.y = velocity.y + some_value * saved_pos.x * Sin_a1 + saved_pos.y * Cos_a1;
    pos.z = velocity.z + 0.0;
    break;
  case 4:
    saved_pos = pos;
    __another_pos += velocity;
    pos = __another_pos + glm::vec3(math::lengthdir_vec(sinf(some_angle2) * circle_radius, some_angle1 + PI1_2), 0.f);
    angle += math::point_direction(saved_pos.x, saved_pos.y, pos.x, pos.y);
    math::angle_normalize(angle);
  }
  // ?????????????
  //pos.x = trig_func_?_4049d0(pos.x * 100.f) / 100.f;
  //pos.y = trig_func_?_4049d0(pos.y * 100.f) / 100.f;
  return;
}
