#ifndef ENEMYMANAGER_H_
#define ENEMYMANAGER_H_

#include "./EclFileManager.h"
#include "./Enemy.h"
#include "../UpdateFuncRegistry.h"
#include "../Anm/AnmFile.h"

// TODO check macros
// XXX -600 XXX -200 XXX 200 XXX 600 XXX
#define Rank5(a, b, c, d, e) a
// XXX -200 XXX 200 XXX 
#define Rank3(a, b, c) a
#define Rank2(a, b) a
#define Diff(a, b, c, d) (GLOBALS.inner.DIFFICULTY == 0 ?\
    a : (GLOBALS.inner.DIFFICULTY == 1 ? b : \
    (GLOBALS.inner.DIFFICULTY == 2 ? c : d)))

class EnemyManager;
extern EnemyManager* ENEMY_MANAGER_PTR;
class EnemyManager {
public:
    EnemyManager();
    ~EnemyManager();
    EnemyManager(EnemyManager const&) = delete;
    EnemyManager& operator=(EnemyManager const&) = delete;

    void Start(cstr eclFile, cstr sub = "main");
    void Update();
    Enemy* SpawnEnemy(cstr sub, float x, float y, int life, int score,
                      int item);
    i32 closest_enemy_id(ns::vec2 pos);
    void EnmKillAll(const Enemy* caller = nullptr, bool byDialog = false);
    Enemy* EnmFind(int id) {
        for (auto n = active_enemy_list_head->next;
             n != active_enemy_list_tail; n = n->next)
            if (n->value && n->value->enemyId == id) {
                return n->value;
            }
        return nullptr;
    }
    int getBoss(int id) {
        for (int i = 0; i < 4; i++) {
            if (boss_ids[i] == id)
                return i;
        }
        return 3;
    }

    // XXX wrong
    int killableEnemyCount() { return enemyCount; }
    i32 enemyCount = 0;

// private:
    int on_tick();
    int on_draw();

    UpdateFunc* f_on_tick = nullptr;
    UpdateFunc* f_on_draw = nullptr;
    i32 ecl_int_vars[4] = {};
    float ecl_float_vars[8] = {};
    i32 miss_count = 0;
    i32 bomb_count = 0;
    i32 can_still_capture_spell = 0;
    i32 boss_ids[4] = {-1, -1, -1, -1};
    i32 flags = 0;
    i32 enemy_limit = 999999;
    i32 next_enemy_id = 1;
    i32 last_enemy_id = 0;
    ns::Timer_t time_in_stage = 0;
    i32 field_0xac = 0;
    i32 field_0xb0 = 0;
    // lots of unknown
    anm::File* loadedAnms[6] = {};
    EclFileManager* fileManager = nullptr;
    EnemyList_t* active_enemy_list_head = nullptr;
    EnemyList_t* active_enemy_list_tail = nullptr;
    // undefinde4
    i32 enemy_count_real = 0;
    // undefinde4
    friend class Enemy;
};

#endif // ENEMYMANAGER_H_
