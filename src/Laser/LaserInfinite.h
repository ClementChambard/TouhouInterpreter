#ifndef LASERINFINITE_H_
#define LASERINFINITE_H_

#include "../AnmOpener/AnmVM.h"
#include "Laser.h"

struct LaserInfiniteInner_t {
    glm::vec3 start_pos = {};
    glm::vec3 speed = {};
    float ang_aim = 0.f;
    float laser_inf_angular_velocity = 0.f;
    float laser_new_arg2 = 0.f;
    float laser_new_arg1 = 0.f;
    float laser_new_arg4 = 0.f;
    float spd_1 = 0.f;
    int32_t laser_time_start = 0;
    int32_t laser_trans_1 = 0;
    int32_t laser_duration = 0;
    int32_t laser_trans_2 = 0;
    int32_t shot_sfx = 18;
    int32_t shot_transform_sfx = -1;
    int32_t laser_st_on_arg1 = 0;
    float distance = 0.f;
    int32_t et_ex_index = 0;
    int32_t type = 0;
    int32_t color = 0;
    int32_t flags = 0;
    EtEx_t ex[20];
};

class LaserInfinite : public Laser {

public:
    LaserInfinite();
    ~LaserInfinite() { }

    int initialize(void*) override;
    int on_tick() override;
    int on_draw() override;
    int cancel_as_bomb_rectangle(glm::vec3, glm::vec3, float, int, int) override;
    int cancel(int, int) override;

    LaserInfiniteInner_t& getInner() { return inner; }

private:
    LaserInfiniteInner_t inner {};
    int32_t __field_94c = 0;
    AnmVM vm1 {}, vm2 {};
};

#endif // LASERINFINITE_H_
