#ifndef BULLET_H_
#define BULLET_H_

#include "AnmOpener/AnmManager.h"
#include "BulletHandler.h"
#include <Interpolator.h>
#include "BulletEx.h"
#include "Interp.h"
#include "Hardcoded.h"

class Bullet;
struct BulletList_t {
    Bullet* value = nullptr;
    BulletList_t* next = nullptr;
    BulletList_t* previous = nullptr;
};


class Bullet {
    public:
        Bullet();
        ~Bullet();
        void Update();
        void Draw();
        void Reset();

    private:

        int on_tick();
        int run_et_ex();

        void _delete();
        int cancel(bool item);

        BulletList_t freelist_node = {};
        BulletList_t tick_list_node = {};
        int32_t flags = 0;
        int32_t ex_invuln__remaining_frames = 0;
        AnmVM vm = {};
        int32_t anm_extra_id = 0;
        glm::vec3 pos = {};
        glm::vec3 velocity = {};
        float speed = 0.f;
        float angle = 0.f;
        float hitbox_diameter = 0.f;
        float __hitbox_diameter_copy = 0.f;
        int32_t index_in_bullet_array;
        int32_t __ex_react_a = 0;
        //undefined4 __unknown;
        //undefined4 __unknown;
        int32_t __field_664_had_5 = 5;
        int32_t cancel_sprite_id = 0;
        int32_t ex_index = 0;
        int32_t __ex_goto_b_loop_count = 0;
        int32_t active_ex_flags = 0;
        int32_t __field_678_had_35 = 35;
        Bullet* next_in_layer = nullptr;
        //undefined4 __unknown;
        int32_t transform_sound = 0;
        int32_t layer = 0;
        EtEx_t et_ex[20] = {};
        BulletExState_Speedup_t ex_speedup;
        BulletExState_Accel_t ex_accel;
        BulletExState_AngleAccel_t ex_angleaccel;
        BulletExState_Turn_t ex_angle;
        BulletExState_Bounce_t ex_bounce;
        BulletExState_Wait_t ex_wait;
        BulletExState_Wrap_t ex_wrap;
        BulletExState_Homing_t ex_homing;
        BulletExState_Move_t ex_move;
        BulletExState_VelAdd_t ex_veladd;
        BulletExState_VelTime_t ex_veltime;
        BulletExState_Offscreen_t ex_offscreen;
        BulletExState_SaveAngle_t ex_save;
        BulletExState_Delay_t ex_delay;
        //undefined4 __unknown;
        //Interpolator for position
        Interp<glm::vec3> pos_i = {};
        Interp<float> scale_i = {};
        float scale = 1.f;
        int32_t __timer_e24 = 0;
        int32_t __timer_e38 = 0;
        //undefined4 __unknown;
        uint16_t state = 0;
        //undefined2 __unknown;
        int32_t __timer_e54 = 0;
        int32_t __timer_e68 = 0;
        Json::Value sprite_data;
        int16_t type = 0;
        int16_t color = 0;
        //undefined4 __unknown;

        //Added
        int32_t ex_offscreen__remaining_frames = 0;
        int32_t ex_slot = 0;

        friend class BulletManager;
};

#endif // BULLET_H_
