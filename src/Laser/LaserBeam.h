#ifndef LASERBEAM_H_
#define LASERBEAM_H_

#include "../AnmOpener/AnmVM.h"
#include "./Laser.h"

struct LaserBeamInner_t {
    glm::vec3 startPos {};
    float __bmgr_340__laser_new_arg_4 = 0.f;
    float __bmgr_010__laser_new_arg_3 = 0.f;
    // undefined4
    float __bmgr_014__ang_aim = 0.f;
    // undefined4 x3
    int32_t __laser_beam_on_arg_1 = 0;
    int32_t __bmgr_004_color = 0;
    float __bmgr_024__distance = 0.f;
    int32_t __bmgr_350__laser_timing_trans1 = 0;

    // a lot of undefined stuff
};

class LaserBeam : public Laser {
public:
    LaserBeam();
    ~LaserBeam() { }

    void method_8(int) { }
    int on_tick() override { return 1; }

    LaserBeamInner_t& getInner() { return inner; }

private:
    LaserBeamInner_t inner {};
    AnmVM vm;
    // undefined4 x 0x400
};

#endif // LASERBEAM_H_
