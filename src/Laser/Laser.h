#ifndef LASER_H_
#define LASER_H_

#include <math/types/types.hpp>
#include <Timer.hpp>
#include "../Bullet/BulletEx.h"

class Laser {
public:
    Laser() {}
    virtual ~Laser();

    virtual void method_0(int, int) {}
    virtual void run_ex() {}
    virtual int initialize(void*) { return 0; }
    virtual int on_tick() { return 0; }
    virtual int on_draw() { return 0; }
    virtual int cancel_as_bomb_rectangle(ns::vec3, ns::vec3,
                                         float, int, int) { return 0; }
    virtual int cancel_as_bomb_circle(ns::vec3 const&, float, int, int) { return 0; }
    virtual int cancel(int, int) { return 0; }
    virtual int method_30(int, int) { return 0; }
    virtual int check_graze_or_kill(int) { return 0; }

    void set_offset(float x, float y, float z) {
        laser_offset = ns::vec3{x, y, z};
    }
    void set_speed(float x, float y, float z) {
        laser_speed = ns::vec3{x, y, z};
    }
    void set_speed(float s) { speed = s; }
    void set_width(float w) { laser_st_width = w; }
    void set_angle(float a) { angle = a; }
    void set_id(int i) { laser_id = i; }

    Laser* next = nullptr;
    Laser* prev = nullptr;
    u32 some_flags = 0;
    i32 __field_10__set_to_3_by_ex_delete = 0;
    i32 kind = 0;
    ns::Timer_t __timer_18 = 0;
    ns::Timer_t __timer_2c = 0;
    ns::Timer_t time_alive = 0;
    ns::vec3 laser_offset = {};
    ns::vec3 laser_speed = {};
    float angle = 0.f;
    float laser_inf_current_length = 0.f;
    float laser_st_width = 0.f;
    float speed = 0.f;
    float __field_7c__sometimes_0p01_or_0f = 0.f;
    i32 laser_id = 0;
    BulletExState_Speedup_t ex_speedup = {};
    BulletExState_Accel_t ex_accel = {};
    BulletExState_AngleAccel_t ex_angleAccel = {};
    BulletExState_Turn_t ex_angle = {};
    BulletExState_Bounce_t ex_bounce = {};
    BulletExState_Wait_t ex_wait = {};
    BulletExState_Wrap_t ex_wrap = {};
    BulletExState_Offscreen_t ex_offscreen = {};
    // BulletExState_UNASSIGNED x11
    i32 et_ex_index = 0;
    i32 flags = 0;
    // undefined4
    ns::Timer_t inv_timer = 0;
    ns::Timer_t ex_offscreen__timer = 0;
    i32 ex_invuln__remaining_frames = 0;
    i32 bullet_type = 0;
    i32 bullet_color = 0;

    friend class LaserManager;
    friend class LaserLine;
    friend class LaserBeam;
    friend class LaserInfinite;
    friend class LaserCurve;
    friend Laser* find_laser_by_id(int id);
    friend int allocate_new_laser(i32 type, void* init_arg);
};

#endif // LASER_H_
