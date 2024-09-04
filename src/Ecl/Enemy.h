#ifndef ECLVM_H_
#define ECLVM_H_
#include "../Bullet/BulletHandler.h"
#include "./EclContext.h"
#include "./EclFileManager.h"
#include "../PosVel.h"
#include "../Fog.hpp"

#include <Interp.hpp>

class Enemy;
struct EnemyList_t {
    Enemy* value = nullptr;
    EnemyList_t* next = nullptr;
    EnemyList_t* previous = nullptr;
};

struct EnemyDrop_t {
    i32 main_type = 0;
    i32 extra_counts[32];
    ns::vec2 area = { 32, 32 };

    void eject_all_drops(ns::vec3 const& pos);
};

struct EnemyDropSeason_t {
    i32 bonusTimer = 0;
    i32 max_time = 0;
    i32 min_count = 0;
    i32 dmg_for_season_drop = 999999;
    i32 dmb_acounted_for_season_drop = 0;
};

struct EnemyFog_t {
    Fog_t* fog_ptr = nullptr;
    float fog_radius = 0.f;
    float r = 0.0;
    ns::Color fog_color = {0, 0, 0, 0};
    float oscillate_x = 0.f;
    float oscillate_y = 0.f;
};

struct EnemyInterrupt_t {
    i32 life = -1;
    i32 time = -1;
    char subNext[0x40] = "";
    char subTimeout[0x40] = "";
};

struct EnemyLife_t {
    i32 current = 0;
    i32 max = 0;
    i32 curAtk = 0;
    i32 curx7 = 0;
    i32 nextAtk = 0;
    i32 totalDmgIncIgn = 0;
    i32 isSpell;
};

using ns::Interp;
using ns::InterpStrange;
struct EnemyData {
    PosVel prev_final_pos = {};
    PosVel final_pos = {};
    PosVel abs_pos = {};
    PosVel rel_pos = {};
    ns::vec2 hurtbox_size = {};
    ns::vec2 hitbox_size = {};
    float rotation = 0.f;
    anm::ID anmIds[16] = {};
    ns::vec3 anmPos[16] = {};
    i32 anmRelated[16] = { -1, -1, -1, -1, -1, -1, -1, -1,
                               -1, -1, -1, -1, -1, -1, -1, -1 };
    i32 selectedAnmID = -1;
    i32 anm0anmID = -1;
    i32 anm0scr = -1;
    i32 anmSetMain = -1;
    i32 anmMainSubscr = -1;
    i32 ecl550 = 0;
    i32 anmLayers = 0;
    ns::vec3 lastDmgPos = {};
    i32 ecl_int_vars[4] = {};
    float ecl_float_vars[8] = {};
    ns::Timer_t timeInEcl = 0;
    ns::Timer_t timeAlive = 0;
    float slowdown = 0;
    EnemyList_t node = {};
    InterpStrange abs_pos_i = {};
    InterpStrange rel_pos_i = {};
    Interp<float> abs_angle_i = {};
    Interp<float> abs_speed_i = {};
    Interp<float> rel_angle_i = {};
    Interp<float> rel_speed_i = {};
    Interp<ns::vec2> abs_radial_dist_i = {};
    Interp<ns::vec2> rel_radial_dist_i = {};
    Interp<ns::vec2> abs_ellipse_i = {};
    Interp<ns::vec2> rel_ellipse_i = {};
    EnemyBulletShooter_t BSs[16] = {};
    i32 et_ex_id[16] = {};
    ns::vec3 bulletOffsets[16] = {};
    ns::vec3 bulletOrigins[16] = {};
    ns::vec2 finalSpriteSize = {};
    ns::vec2 moveLimitPos = {};
    ns::vec2 moveLimitSize = {};
    i32 scoreReward = 0;
    EnemyLife_t life = {};
    EnemyDrop_t drops = {};
    i32 extra_dmg = 0;
    u32 deathSnd = -1;
    i32 deathAnm = -1;
    i32 deathScr = 0;
    i32 frBefNxtHurtFx = 0;
    i32 hitSnd = -1;
    ns::Timer_t invFrame = 0;
    ns::Timer_t noHbFrame = 0;
    ns::Timer_t timerCountDown = 0;
    float bombDmgMul = 1;
    EnemyDropSeason_t dropsSeason = {};
    u64 flags = 0;
    i32 bombShieldOnAnmMain = -1;
    i32 bombShieldOffAnmMain = -1;
    i32 ownBossId = -1;
    float etProtRange = 0;
    EnemyInterrupt_t interrupts[8] = {};
    Enemy* enemy = nullptr;
    EnemyFog_t fog;
    char setDeath[0x40] = "";
    std::function<int(EnemyData*)> func_from_ecl_func_set = nullptr;
    u32 is_func_set_2 = 0;
    std::function<int(EnemyData*, int)> func_from_ecl_flag_ext_dmg = nullptr;
    std::function<int(EnemyData*)> hitbox_func = nullptr;
    i32 own_chapter = 0;
    i32 __bool_cleared_by_ecl_570 = 0;

    int update();
    int step_interpolators();
    int step_game_logic();
    void update_distorsion();
    void calc_final_pos();
};

class Enemy {
public:
    Enemy();
    ~Enemy();

    void Init(cstr sub);
    void Tick();
    void Die();
    int update();

    EnemyData* getData() { return &enemy; }

// private:
    int die();
    int ecl_run(float speed = 1.f);
    void clear_async_contexts();
    void reset_ecl();
    void set_sub(cstr sub);

    int updateContext(EclRunContext_t* cont, float speed = 1.f);
    int execInstr(EclRunContext_t* cont, const EclRawInstr_t* instr);
    i32 checkVarI(i32 id);
    float checkVarF(float id);
    i32& checkVarRI(i32 id);
    float& checkVarRF(float id);

    EclRunContextHolder_t context;
    EclFileManager* fileManager;
    EclRunContextList_t* asyncListHead = nullptr;
    EnemyData enemy;
    std::function<void(void)> onDeath = nullptr;
    i32 enemyId;
    i32 parentEnemyId;
    i32 stackToRemove = 0;
    // ui32 unknown
    friend class EnemyManager;
    friend struct EnemyData;
};

#endif // ECLVM_H_
