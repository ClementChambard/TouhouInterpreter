#ifndef ECLVM_H_
#define ECLVM_H_
#include "EclFileManager.h"
#include "EclContext.h"
#include "BulletHandler.h"
#include "PosVel.h"
#include "Interp.h"

#include <AnmOpener/AnmVM.h>
#include <iostream>

class Enemy;
struct EnemyList_t {
    Enemy* value = nullptr;
    EnemyList_t* next = nullptr;
    EnemyList_t* previous = nullptr;
};

struct EnemyDrop_t {
    int32_t main_type = 0;
    int32_t extra_counts[17];
    glm::vec2 area = {};
};

struct EnemyDropSeason_t {
    int32_t bonusTimer = 0;
    int32_t max_time = 0;
    int32_t min_count = 0;
    int32_t dmg_for_season_drop = 999999;
    int32_t dmb_acounted_for_season_drop = 0;
};

struct EnemyFog_t {
    // fog ptr
    float fog_radius = 0.f;
    float fog__a = 0.f;
    int32_t fog_color = 0;
    float fog_angle__a = 0.f;
    float fog_angle__b = 0.f;
};

struct EnemyInterrupt_t {
    int32_t life = -1;
    int32_t time = -1;
    std::string subNext = "";
    std::string subTimeout = "";
};

struct EnemyLife_t {
    int32_t current = 0;
    int32_t max = 0;
    int32_t curAtk = 0;
    int32_t curx7 = 0;
    int32_t nextAtk = 0;
    int32_t totalDmgIncIgn = 0;
    int32_t isSpell;
};

struct EnemyData {
    PosVel prev_final_pos = {};
    PosVel final_pos = {};
    PosVel abs_pos = {};
    PosVel rel_pos = {};
    float rotation = 0.f;
    glm::vec2 hurtbox_size = {};
    glm::vec2 hitbox_size = {};
    NSEngine::AnmID anmIds[16] = {};
    glm::vec3 anmPos[16] = {};
    int32_t anmRelated[16] = {};
    int32_t selectedAnmID = -1;
    int32_t anm0anmID = -1;
    int32_t anm0scr = -1;
    int32_t anmSetMain = -1;
    int32_t anmMainSubscr = -1;
    int32_t ecl550 = 0;
    int32_t anmLayers = 0;
    glm::vec3 lastDmgPos = {};
    int32_t ecl_int_vars[4] = {};
    float ecl_float_vars[8] = {};
    int32_t timeInEcl = 0;
    int32_t timeAlive = 0;
    float slowdown = 0;
    EnemyList_t* node = nullptr;
    InterpStrange abs_pos_i = {};
    InterpStrange rel_pos_i = {};
    Interp<float> abs_angle_i = {};
    Interp<float> abs_speed_i = {};
    Interp<float> rel_angle_i = {};
    Interp<float> rel_speed_i = {};
    Interp<glm::vec2> abs_radial_dist_i = {};
    Interp<glm::vec2> rel_radial_dist_i = {};
    Interp<glm::vec2> abs_ellipse_i = {};
    Interp<glm::vec2> rel_ellipse_i = {};
    EnemyBulletShooter_t BSs[16] = {};
    int32_t et_ex_id[16] = {};
    glm::vec3 bulletOffsets[16] = {};
    glm::vec3 bulletOrigins[16] = {};
    glm::vec2 finalSpriteSize = {};
    glm::vec2 moveLimitPos = {};
    glm::vec2 moveLimitSize = {};
    int32_t scoreReward = 0;
    EnemyLife_t life = {};
    EnemyDrop_t drops = {};
    int32_t extra_dmg = 0;
    uint32_t deathSnd = -1;
    int32_t deathAnm = -1;
    int32_t deathScr = -1;
    int32_t frBefNxtHurtFx = 0;
    int32_t hitSnd = -1;
    int32_t invFrame = 0;
    int32_t noHbFrame = 0;
    int32_t timerCountDown = 0;
    float bombDmgMul = 1;
    EnemyDropSeason_t dropsSeason = {};
    uint64_t flags = 0;
    int32_t bombShieldOnAnmMain = -1;
    int32_t bombShieldOffAnmMain = -1;
    int32_t ownBossId = -1;
    float etProtRange = 0;
    EnemyInterrupt_t interrupts[8] = {};
    Enemy* enemy = nullptr;
    EnemyFog_t fog;
    std::string setDeath = "";
    //some void*

    void calc_final_pos();
    int step_interpolators();
};

class Enemy {

    public:

        Enemy();
        ~Enemy();

        void Init(std::string sub);
        void Tick();
        void Die();

        void DebugDraw();
        EnemyData* getData() { return &enemy; }

    private:

        void updateContext(EclRunContext_t* cont);
        void execInstr(EclRunContext_t* cont, EclRawInstr_t* instr);
        int32_t checkVarI(int32_t id);
        float checkVarF(float id);
        int32_t& checkVarRI(int32_t id);
        float& checkVarRF(float id);

        EclRunContextHolder_t context;
        EclFileManager* fileManager;
        EclRunContextList_t* asyncListHead = nullptr;
        EnemyData enemy;
        void* onDeath = nullptr;
        int32_t enemyId;
        int32_t parentEnemyId;
        int32_t stackToRemove = 0;
        // uint32_t unknown
        friend class EnemyManager;
};


#endif // ECLVM_H_
