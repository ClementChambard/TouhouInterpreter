#ifndef LASERCURVE_H_
#define LASERCURVE_H_

#include "Laser.h"
#include "../AnmOpener/AnmVM.h"
#include <vertex.h>

struct LaserCurveListThing_t {
    LaserCurveListThing_t* next = nullptr;
    float b = 0.f;
    float c = 0.f;
    float d = 0.f;
    int32_t e = 0.f;
    glm::vec3 fgh = {};
    glm::vec3 ijk = {};
    float l = 0.f;
    float m = 0.f;
    float n = 0.f;
    float o = 0.f;
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
    LaserCurveListThing_t* listThing = nullptr;
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

        LaserCurveInner_t& getInner() { return inner; }

    private:
        LaserCurveInner_t inner;
        AnmVM vm1;
        AnmVM vm2;
        LaserCurveNode_t* nodes = nullptr;
        NSEngine::Vertex* vertices = nullptr;
        LaserCurveListThing_t listThing = {};
};

#endif // LASERCURVE_H_
