#ifndef PLAYER_H_
#define PLAYER_H_

#include "UpdateFuncRegistry.h"
#include "AnmOpener/AnmVM.h"
#include "Interp.h"
#include "PosVel.h"
#include "ShtFile.h"

struct BoundingBox3_t {
    glm::vec3 min_pos{};
    glm::vec3 max_pos{};
};

struct PlayerOption_t {
    int32_t flags = 0;
    // undefined4[20]
    glm::vec<2, int> scaled_prefered_pos{};
    glm::vec<2, int> scaled_cur_pos{0, -0xc800};
    glm::vec<2, int> scaled_prefered_pos_rel_to_player[2]{};
    // undefined4[15]
    AnmID anmId1{};
    AnmID anmId2{};
    // undefined4[7]
    int32_t move_instantly = true; // boolean
    // undefined4
    void (*__some_code)(void) = nullptr; // function ptr
    // undefined4
    float shoot_angle_maybe = 0.f;
};

struct PlayerInner_t;
struct PlayerBullet_t {
    uint8_t flags = 0;
    // undefined1[3]
    int32_t id = 0;
    AnmID anmId{};
    int32_t __field_c = 0; //zTimer
    int32_t __field_20 = 0; //zTimer
    int32_t __field_34 = 0; //zTimer
    PosVel pos{};
    int32_t active = 0;
    // undefined4[4]
    int32_t damage = 0;
    glm::vec2 hitbox{};
    // undefined4
    int32_t shter = 0;
    uint32_t damageSourceId = 0;
    // undefined4[11]

    void update();
    void init(int shter, int tmer, PlayerInner_t& inner);
};

struct PlayerDamageSource_t {
    uint8_t flags;
    // undefined1[3]
    float field_0x4;
    float field_0x8;
    float field_0xc;
    float field_0x10;
    int32_t field_0x14;
    int32_t field_0x18;
    PosVel field_0x1c;
    int32_t field_0x60;
    int32_t field_0x64; //zTimer;
    int32_t field_0x78;
    int32_t field_0x7c;
    int32_t field_0x80;
    float field_0x84;
    int32_t field_0x88;
    int32_t field_0x8c;
    int32_t field_0x90;
    int32_t field_0x94;
    int32_t field_0x98;
};

struct PlayerInner_t {
    glm::vec3 pos{0.f, 400.f, 0.f};
    glm::vec<2, int32_t> integer_pos{0, 0xc800};
    int32_t time_in_state = 0; //zTimer
    int32_t time_in_stage = 0; //zTimer
    int32_t __time_in_stage__copy_3c = 0; //zTimer
    PlayerOption_t options[4]{};
    PlayerOption_t subseason_options[8]{};
    PlayerBullet_t bullets[256]{};
    int32_t last_created_damage_source_index = 0;
    PlayerDamageSource_t damage_sources[257]{};
    int32_t state = 0;
    AnmID focus_eff_anmId = 0;
    uint32_t __field_187a8__init_0 = 0;
    int32_t __timer_187ac = 0; //zTimer
    int32_t focusing = 0; // boolean
    int32_t shoot_key_short_timer = -1; //zTimer
    int32_t shoot_key_long_timer = -1; //zTimer
    int32_t __some_index = 0;
    // undefined4[30]
    int32_t iframes = 0; //zTimer
};

class Player {
    public:
        Player();
        ~Player();

    private:
        int _on_tick();
        int _on_draw();

        void move();
        void move_options(PlayerOption_t* options, uint cnt);
        void die();

        void check_shoot();
        void shoot(int short_tmr, int long_tmr);

        // undefined4
        UpdateFunc* on_game_tick = nullptr;
        UpdateFunc* on_draw = nullptr;
        // AnmLoaded* playerAnm
        AnmVM vm{};
        public:
        PlayerInner_t inner{};
        private:
        int32_t flags = 0;
        float speed_unfocus = 0.f;
        float speed_focus = 0.f;
        float speed_unfocus_diag = 0.f;
        float speed_focus_diag = 0.f;
        glm::vec3 last_movement{};
        glm::vec3 last_movement_no_zero{};
        glm::vec2 last_movement_cpy{};
        int32_t __some_other_ctr = 30;
        int32_t __some_counter = 0;
        float speed_multiplier = 1.f;
        glm::vec3 __could_be_subsun_pull_direction{};
        // undefined4
        glm::vec<2, int32_t> unknown_scaled_pos_array[33]{};
        BoundingBox3_t hurtbox{};
        glm::vec3 hurtbox_halfsize{};
        glm::vec3 item_attract_box_unfocused_halfsize{};
        glm::vec3 item_attract_box_focused_halfsize{};
        glm::vec3 death_pos{};
        glm::vec2 speed{};
        int32_t direction = 0;
        // undefined4
        ShtFile_t* sht_file = nullptr;
        // undefined4[2]
        Interp<float> player_scale_i{};
        float player_scale__requires_flag_0x10__from_ddc = 1.f;
        float damage_multiplier__used_by_winter_in_th16 = 1.f;
        // undefined4[9]
        BoundingBox3_t item_collect_box{};
        BoundingBox3_t item_attract_box_focused{};
        BoundingBox3_t item_attract_box_unfocused{};
        // undefined4[4]

        friend struct PlayerBullet_t;
};

extern Player* PLAYER_PTR;

#endif // PLAYER_H_
