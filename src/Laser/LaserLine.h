#ifndef LASERLINE_H_
#define LASERLINE_H_

#include "Laser.h"
#include "../BulletEx.h"
#include "../AnmOpener/AnmVM.h"

struct LaserLineInner_t {
    glm::vec3 start_pos = {};
    float ang_aim = 0.f;
    float laser_new_arg_2 = 0.f;
    float laser_new_arg_1 = 0.f;
    float laser_new_arg_3 = 0.f;
    float laser_new_arg_4 = 0.f;
    float spd_1 = 0.f;
    int32_t bullet_type = 0;
    int32_t bullet_color = 0;
    float distance = 0.f;
    int32_t et_ex_index = 0;
    int32_t flags = 0;
    EtEx_t et_ex[20] = {};
    int32_t shot_sfx = 18;
    int32_t shot_transform_sfx = -1;
};

class LaserLine : public Laser {
    public:
        LaserLine();
        ~LaserLine() {}

        void run_ex() override;
        int initialize(void* inner) override;
        int on_tick() override;
        int on_draw() override;
        int cancel_as_bomb_rectangle(glm::vec3,glm::vec3,float,int,int) override;
        int cancel_as_bomb_circle(int,int,int,int) override;
        int cancel(int,int) override;
        int check_graze_or_kill(int) override { return 0; }
        int method_50_line();

        LaserLineInner_t& getInner() { return inner; }

    private:
        LaserLineInner_t inner;
        AnmVM vm1, vm2, vm3;
};

#endif // LASERLINE_H_
