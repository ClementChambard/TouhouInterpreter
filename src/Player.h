#ifndef PLAYER_H_
#define PLAYER_H_

#include "AnmOpener/AnmVM.h"
#include "Interp.h"
#include "PosVel.h"
#include "ShtFile.h"
#include "UpdateFuncRegistry.h"

struct BoundingBox3_t {
    glm::vec3 min_pos {};
    glm::vec3 max_pos {};
};

struct PlayerOption_t {
    int32_t flags = 0;
    // undefined4[20]
    glm::vec<2, int> scaled_prefered_pos {};
    glm::vec<2, int> scaled_cur_pos { 0, -0xc800 };
    glm::vec<2, int> scaled_prefered_pos_rel_to_player[2] {};
    // undefined4[15]
    AnmID anmId1 {};
    AnmID anmId2 {};
    // undefined4[5]
    int32_t focusing = false; // boolean
    int32_t id = 0;
    int32_t move_instantly = true; // boolean
    // undefined4
    void (*update_code)(PlayerOption_t*) = nullptr; // function ptr
    // undefined4
    float shoot_angle_maybe = 0.f;
};

struct PlayerInner_t;
struct PlayerBullet_t {
    uint8_t flags = 0;
    // undefined1[3]
    int32_t id = 0;
    AnmID anmId {};
    int32_t __field_c = 0; // zTimer
    int32_t __field_20 = 0; // zTimer
    int32_t __field_34 = 0; // zTimer
    PosVel pos {};
    int32_t active = 0;
    int32_t targeted_entity = 0;
    int32_t hitting = 0;
    int32_t blink = 0;
    int32_t damage = 0;
    glm::vec2 hitbox {};
    int32_t field_0xa8 = 0;
    int32_t shter = 0;
    uint32_t damageSourceId = 0;
    // 0xb4
    // undefined4[11]

    void update();
    void init(int shter, int tmer, PlayerInner_t& inner);
};

struct PlayerDamageSource_t {
    uint8_t flags;
    // undefined1[3]
    float field_0x4;
    float field_0x8;
    float angle;
    float angle_increase;
    glm::vec2 hitbox;
    PosVel pos;
    int32_t time_to_live; // zTimer;
    int32_t damage;
    int32_t field_0x78;
    int32_t field_0x7c;
    int32_t field_0x80;
    float field_0x84;
    int32_t field_0x88;
    int32_t field_0x8c;
    int32_t bullet_id;
    int32_t field_0x94;
    int32_t ds_on_hit;
};

struct PlayerInner_t {
    glm::vec3 pos { 0.f, 400.f, 0.f };
    glm::vec<2, int32_t> integer_pos { 0, 0xc800 };
    int32_t time_in_state = 0; // zTimer
    int32_t time_in_stage = 0; // zTimer
    int32_t __time_in_stage__copy_3c = 0; // zTimer
    PlayerOption_t options[4] {};
    PlayerOption_t subseason_options[8] {};
    PlayerBullet_t bullets[256] {};
    int32_t last_created_damage_source_index = 0;
    PlayerDamageSource_t damage_sources[257] {};
    int32_t state = 0;
    AnmID focus_eff_anmId = 0;
    uint32_t maybe_th15_graze_snowflake = 0;
    int32_t __timer_187ac = 0; // zTimer
    int32_t focusing = 0; // boolean
    int32_t shoot_key_short_timer = -1; // zTimer
    int32_t shoot_key_long_timer = -1; // zTimer
    int32_t power = 0;
    int32_t unique_bullets[16] = {};
    // undefined4[14]
    int32_t iframes = 0; // zTimer
};

class Player {
public:
    Player();
    ~Player();
    Player(Player const&) = delete;
    Player& operator=(Player const&) = delete;

    void set_pos(glm::vec3 const& p);

private:
    int _on_tick();
    int _on_draw();

    void move();
    void move_options(PlayerOption_t* options, uint cnt);
    void die();
    void try_kill();

    void check_shoot();
    void shoot(int short_tmr, int long_tmr);

    // undefined4
    UpdateFunc* on_game_tick = nullptr;
    UpdateFunc* on_draw = nullptr;
    // AnmLoaded* playerAnm
public:
    AnmVM vm {};
    PlayerInner_t inner {};
    int32_t flags = 0;
    float speed_unfocus = 0.f;
    float speed_focus = 0.f;
    float speed_unfocus_diag = 0.f;
    float speed_focus_diag = 0.f;
    glm::vec3 last_movement {};
    glm::vec3 last_movement_no_zero {};
    glm::vec2 last_movement_cpy {};
    int32_t __some_other_ctr = 30;
    int32_t __some_counter = 0;
    float speed_multiplier = 1.f;
    glm::vec3 __could_be_subsun_pull_direction {};
    // undefined4
    glm::vec<2, int32_t> prev_pos[33] {};
    BoundingBox3_t hurtbox {};
    glm::vec3 hurtbox_halfsize {};
    glm::vec3 item_attract_box_unfocused_halfsize {};
    glm::vec3 item_attract_box_focused_halfsize {};
    glm::vec3 death_pos {};
    glm::vec2 speed {};
    int32_t direction = 0;
    // undefined4
    ShtFile_t* sht_file = nullptr;
    int32_t field_0x19040 = 0;
    int32_t field_0x19044 = 0;
    Interp<float> player_scale_i {};
    float player_scale__requires_flag_0x10__from_ddc = 1.f;
    float damage_multiplier__used_by_winter_in_th16 = 1.f;
    int32_t field_0x19080 = 0; // zTimer
    AnmID extra_anm_id {};
    glm::vec3 field_0x19098 {};
    BoundingBox3_t item_collect_box {};
    BoundingBox3_t item_attract_box_focused {};
    BoundingBox3_t item_attract_box_unfocused {};
    int32_t field_0x190ec = 0;
    int32_t field_0x190f0 = 0;
    int32_t field_0x190f4 = 0;
    int32_t field_0x190f8 = 0;

    friend struct PlayerBullet_t;
};

extern Player* PLAYER_PTR;

extern int create_damage_source(glm::vec3 const&, float, int, int, float, float);

extern int applyDamage(glm::vec3 const& enemyPos, glm::vec2* enemy_hb, float hb_size, uint* param_4, glm::vec3& dmg_pos, int param_6, int enemy_id, float ang);

#endif // PLAYER_H_
