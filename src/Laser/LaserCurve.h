#ifndef LASERCURVE_H_
#define LASERCURVE_H_

#include "Laser.h"
#include "../AnmOpener/AnmVM.h"
#include <vertex.h>

struct LaserCurveTransform_t {
    LaserCurveTransform_t* next = nullptr;
    LaserCurveTransform_t* prev = nullptr;
    int32_t start_time = 0;
    int32_t end_time = 0;
    int32_t move_type = 0.f;
    glm::vec3 dir_vec = {};
    glm::vec3 pos = {};
    float angle = 0.f;
    float speed = 0.f;
    float accel = 0.f;
    float angle_accel = 0.f;

    void posvel(glm::vec3* pos, float* speed, float* angle, float node_time_alive);
    void posvel_from_prev(glm::vec3* pos, float* speed, float* angle, glm::vec3 const& pos_prev, float speed_prev, float angle_prev, float node_time_alive);
};

struct LaserCurveInner_t {
    glm::vec3 start_pos = {};
    float ang_aim = 0.f;
    float laser_new_arg4 = 0.f;
    float spd_1 = 0.f;
    int32_t type = 0;
    int32_t color = 0;
    int32_t laser_time_start = 0;
    float distance = 0.f;
    int32_t init_1 = 0;
    EtEx_t ex[20] = {};
    int32_t shot_sfx = 0;
    int32_t shot_transform_sfx = 0;
    int32_t ex_index = 0;
    LaserCurveTransform_t* transforms = nullptr;
    int  timer40_start = 0;
};

struct LaserCurveNode_t {
    glm::vec3 pos = {};
    glm::vec3 v_speed = {};
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
        AnmVM vm1;
        AnmVM vm2;
        LaserCurveNode_t* nodes = nullptr;
        NSEngine::Vertex* vertices = nullptr;
        LaserCurveTransform_t transforms = {};
        int offscreen_timer = 0;
};

#endif // LASERCURVE_H_
