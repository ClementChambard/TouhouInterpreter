#ifndef LASERLINE_H_
#define LASERLINE_H_

#include "Laser.h"
#include "../BulletEx.h"
#include <AnmOpener/AnmVM.h>

struct LaserLineInner_t {
    glm::vec3 start_pos;
    float ang_aim;
    float laser_new_arg_2;
    float laser_new_arg_1;
    float laser_new_arg_3;
    float laser_new_arg_4;
    float spd_1;
    int32_t bullet_type;
    int32_t bullet_color;
    float distance;
    //undefined 4
    int32_t flags;
    EtEx_t et_ex[20];
    int32_t shot_sfx;
    int32_t shot_transform_sfx;
};

class LaserLine : public Laser {
    public:
        LaserLine();
        ~LaserLine() override;

        void run_ex() override;
        int initialize(void* inner) override;
        int on_tick() override;
        int on_draw() override;

        int method_50_line();

        LaserLineInner_t& getInner() { return inner; }

    private:
        LaserLineInner_t inner;
        NSEngine::AnmVM vm1, vm2, vm3;
};

#endif // LASERLINE_H_
