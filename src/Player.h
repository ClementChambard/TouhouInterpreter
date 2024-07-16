#ifndef PLAYER_H_
#define PLAYER_H_

#include "./Anm/AnmFile.h"
#include <Interp.hpp>
#include "./PosVel.h"
#include "./ShtFile.h"
#include "./UpdateFuncRegistry.h"

struct BoundingBox3_t {
    ns::vec3 min_pos {};
    ns::vec3 max_pos {};
};

struct PlayerOption_t {
    i32 flags = 0;
    // undefined4[20]
    ns::ivec2 scaled_prefered_pos {};
    ns::ivec2 scaled_cur_pos { 0, -0xc800 };
    ns::ivec2 scaled_prefered_pos_rel_to_player[2] {};
    // undefined4[15]
    anm::ID anmId1 {};
    anm::ID anmId2 {};
    // undefined4[5]
    i32 focusing = false;
    i32 id = 0;
    i32 move_instantly = true;
    // undefined4
    void (*update_code)(PlayerOption_t*) = nullptr;
    // undefined4
    f32 shoot_angle_maybe = 0.f;
};

struct PlayerInner_t;
struct PlayerBullet_t {
    u8 flags = 0;
    // undefined1[3]
    i32 id = 0;
    anm::ID anmId {};
    ns::Timer_t __field_c = 0;
    ns::Timer_t __field_20 = 0;
    ns::Timer_t __field_34 = 0;
    PosVel pos {};
    i32 active = 0;
    i32 targeted_entity = 0;
    i32 hitting = 0;
    i32 blink = 0;
    i32 damage = 0;
    ns::vec2 hitbox {};
    i32 field_0xa8 = 0;
    i32 shter = 0;
    u32 damageSourceId = 0;
    // 0xb4
    // undefined4[11]

    void update();
    void init(i32 shter, i32 tmer, PlayerInner_t& inner);
};

struct PlayerDamageSource_t {
    u8 flags;
    // undefined1[3]
    f32 field_0x4;
    f32 field_0x8;
    f32 angle;
    f32 angle_increase;
    ns::vec2 hitbox;
    PosVel pos;
    i32 time_to_live;
    i32 damage;
    i32 field_0x78;
    i32 field_0x7c;
    i32 field_0x80;
    f32 field_0x84;
    i32 field_0x88;
    i32 field_0x8c;
    i32 bullet_id;
    i32 field_0x94;
    i32 ds_on_hit;
};

struct PlayerInner_t {
    ns::vec3 pos { 0.f, 400.f, 0.f };
    ns::ivec2 integer_pos { 0, 0xc800 };
    ns::Timer_t time_in_state = 0;
    ns::Timer_t time_in_stage = 0;
    ns::Timer_t __time_in_stage__copy_3c = 0;
    PlayerOption_t options[4] {};
    PlayerOption_t subseason_options[8] {};
    PlayerBullet_t bullets[256] {};
    i32 last_created_damage_source_index = 0;
    PlayerDamageSource_t damage_sources[257] {};
    i32 state = 0;
    anm::ID focus_eff_anmId = 0;
    u32 maybe_th15_graze_snowflake = 0;
    ns::Timer_t __timer_187ac = 0;
    i32 focusing = false;
    ns::Timer_t shoot_key_short_timer = -1;
    ns::Timer_t shoot_key_long_timer = -1;
    i32 power = 0;
    i32 unique_bullets[16] = {};
    // undefined4[14]
    ns::Timer_t iframes = 0;
};

class Player {
public:
    Player();
    ~Player();
    Player(Player const&) = delete;
    Player& operator=(Player const&) = delete;

    void set_pos(ns::vec3 const& p);

    void do_graze(ns::vec3 const& pos);

private:
    i32 _on_tick();
    i32 _on_draw();

    void move();
    void move_options(PlayerOption_t* options, u32 cnt);
    void die();
    void try_kill();

    void check_shoot();
    void shoot(i32 short_tmr, i32 long_tmr);

    // undefined4
    UpdateFunc* on_game_tick = nullptr;
    UpdateFunc* on_draw = nullptr;
public:
    anm::File* playerAnm =  nullptr;
    anm::VM vm {};
    PlayerInner_t inner {};
    i32 flags = 0;
    f32 speed_unfocus = 0.f;
    f32 speed_focus = 0.f;
    f32 speed_unfocus_diag = 0.f;
    f32 speed_focus_diag = 0.f;
    ns::vec3 last_movement {};
    ns::vec3 last_movement_no_zero {};
    ns::vec2 last_movement_cpy {};
    i32 __some_other_ctr = 30;
    i32 __some_counter = 0;
    f32 speed_multiplier = 1.f;
    ns::vec3 __could_be_subsun_pull_direction {};
    // undefined4
    ns::ivec2 prev_pos[33] {};
    BoundingBox3_t hurtbox {};
    ns::vec3 hurtbox_halfsize {};
    ns::vec3 item_attract_box_unfocused_halfsize {};
    ns::vec3 item_attract_box_focused_halfsize {};
    ns::vec3 death_pos {};
    ns::vec2 speed {};
    i32 direction = 0;
    // undefined4
    ShtFile_t* sht_file = nullptr;
    i32 field_0x19040 = 0;
    i32 field_0x19044 = 0;
    ns::Interp<f32> player_scale_i {};
    f32 player_scale__requires_flag_0x10__from_ddc = 1.f;
    f32 damage_multiplier__used_by_winter_in_th16 = 1.f;
    ns::Timer_t field_0x19080 = 0;
    anm::ID extra_anm_id {};
    ns::vec3 field_0x19098 {};
    BoundingBox3_t item_collect_box {};
    BoundingBox3_t item_attract_box_focused {};
    BoundingBox3_t item_attract_box_unfocused {};
    i32 field_0x190ec = 0;
    i32 field_0x190f0 = 0;
    i32 field_0x190f4 = 0;
    i32 field_0x190f8 = 0;

    friend struct PlayerBullet_t;
};

extern Player* PLAYER_PTR;

extern i32 create_damage_source(ns::vec3 const&, f32,
                                i32, i32, f32, f32);
extern i32 create_damage_source_3(ns::vec3 const&, i32,
                                  i32, f32, f32);

extern i32 applyDamage(ns::vec3 const& enemyPos, ns::vec2* enemy_hb,
                       f32 hb_size, u32* param_4, ns::vec3& dmg_pos,
                       i32 param_6, i32 enemy_id, f32 ang);

#endif // PLAYER_H_
