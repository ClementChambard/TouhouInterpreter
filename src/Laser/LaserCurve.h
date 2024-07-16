#ifndef LASERCURVE_H_
#define LASERCURVE_H_

#include "Laser.h"
#include "../Anm/AnmVM.h"
#include "../Anm/RenderVertex.hpp"
#include "../Bullet/BulletHandler.h"

struct LaserCurveTransform_t {
    LaserCurveTransform_t* next = nullptr;
    LaserCurveTransform_t* prev = nullptr;
    i32 start_time = 0;
    i32 end_time = 0;
    i32 move_type = 0.f;
    ns::vec3 dir_vec = {};
    ns::vec3 pos = {};
    float angle = 0.f;
    float speed = 0.f;
    float accel = 0.f;
    float angle_accel = 0.f;

    void posvel(ns::vec3* pos, float* speed, float* angle,
                float node_time_alive);
    void posvel_from_prev(ns::vec3* pos, float* speed, float* angle,
                          ns::vec3 const& pos_prev, float speed_prev,
                          float angle_prev, float node_time_alive);
};

struct LaserCurveInner_t {
    ns::vec3 start_pos = {};
    float ang_aim = 0.f;
    float laser_new_arg4 = 0.f;
    float spd_1 = 0.f;
    i32 type = 0;
    i32 color = 0;
    i32 laser_time_start = 0;
    float distance = 0.f;
    i32 init_1 = 0;
    EtEx_t ex[20] = {};
    i32 shot_sfx = 0;
    i32 shot_transform_sfx = 0;
    i32 ex_index = 0;
    LaserCurveTransform_t* transforms = nullptr;
    int  timer40_start = 0;
};

struct LaserCurveNode_t {
    ns::vec3 pos = {};
    ns::vec3 v_speed = {};
    float angle = 0.f;
    float speed = 0.f;
};

class LaserCurve : public Laser {
public:
    LaserCurve();
    ~LaserCurve() override;

    void run_ex() override;
    int initialize(void*) override;
    int on_tick() override;
    int on_draw() override;

    int cancel(int param_2, int as_bomb) override;

    LaserCurveInner_t& getInner() { return inner; }

private:
    LaserCurveInner_t inner;
    anm::VM vm1;
    anm::VM vm2;
    LaserCurveNode_t* nodes = nullptr;
    anm::RenderVertex_t* vertices = nullptr;
    LaserCurveTransform_t transforms = {};
    int offscreen_timer = 0;
};

#endif // LASERCURVE_H_
