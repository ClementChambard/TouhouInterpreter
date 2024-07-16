#ifndef LASERINFINITE_H_
#define LASERINFINITE_H_

#include "../Anm/AnmVM.h"
#include "./Laser.h"
#include "../Bullet/BulletHandler.h"

struct LaserInfiniteInner_t {
    ns::vec3 start_pos = {};
    ns::vec3 speed = {};
    float ang_aim = 0.f;
    float laser_inf_angular_velocity = 0.f;
    float laser_new_arg2 = 0.f;
    float laser_new_arg1 = 0.f;
    float laser_new_arg4 = 0.f;
    float spd_1 = 0.f;
    i32 laser_time_start = 0;
    i32 laser_trans_1 = 0;
    i32 laser_duration = 0;
    i32 laser_trans_2 = 0;
    i32 shot_sfx = 18;
    i32 shot_transform_sfx = -1;
    i32 laser_st_on_arg1 = 0;
    float distance = 0.f;
    i32 et_ex_index = 0;
    i32 type = 0;
    i32 color = 0;
    i32 flags = 0;
    EtEx_t ex[20];
};

class LaserInfinite : public Laser {
public:
    LaserInfinite();
    ~LaserInfinite() { }

    int initialize(void*) override;
    int on_tick() override;
    int on_draw() override;
    int cancel_as_bomb_rectangle(ns::vec3, ns::vec3,
                                 float, int, int) override;
    int cancel(int, int) override;

    LaserInfiniteInner_t& getInner() { return inner; }

private:
    LaserInfiniteInner_t inner {};
    i32 __field_94c = 0;
    anm::VM vm1 {}, vm2 {};
};

#endif // LASERINFINITE_H_
