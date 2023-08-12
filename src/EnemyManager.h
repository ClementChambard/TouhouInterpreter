#ifndef ENEMYMANAGER_H_
#define ENEMYMANAGER_H_

#include "./EclFileManager.h"
#include "./Enemy.h"
#include "./GlobalData.h"
#include "./UpdateFuncRegistry.h"
#include <Engine.hpp>
#include <string>

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
    static EnemyManager* GetInstance() {
        static EnemyManager* inst = new EnemyManager();
        ENEMY_MANAGER_PTR = inst;
        return inst;
    }
    static void Cleanup() { delete GetInstance(); }
    void Start(std::string const& eclFile, std::string const& sub = "main");
    void Update();
    Enemy* SpawnEnemy(std::string sub, float x, float y, int life, int score,
                      int item);
    int32_t closest_enemy_id(glm::vec2 pos);
    void EnmKillAll(const Enemy* caller = nullptr, bool byDialog = false);
    Enemy* EnmFind(int id) {
        // std::cout << "\n finding " << id << "...\n";
        for (auto n = active_enemy_list_head->next;
             n != active_enemy_list_tail; n = n->next)
            if (n->value && n->value->enemyId == id) {
                // std::cout << "found!\n";
                return n->value;
            }
        // std::cout << "not found!\n";
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
    int32_t enemyCount = 0;

// private:
    int on_tick();
    int on_draw();

    void on_draw_debug();

    UpdateFunc* f_on_tick = nullptr;
    UpdateFunc* f_on_draw = nullptr;
    int32_t ecl_int_vars[4] = {};
    float ecl_float_vars[8] = {};
    int32_t miss_count = 0;
    int32_t bomb_count = 0;
    int32_t can_still_capture_spell = 0;
    int32_t boss_ids[4] = {-1, -1, -1, -1};
    int32_t flags = 0;
    int32_t enemy_limit = 999999;
    int32_t next_enemy_id = 1;
    int32_t last_enemy_id = 0;
    int32_t time_in_stage = 0;
    int32_t field_0xac = 0;
    int32_t field_0xb0 = 0;
    // lots of unknown
    AnmFile* loadedAnms[6] = {};
    EclFileManager* fileManager = nullptr;
    EnemyList_t* active_enemy_list_head = nullptr;
    EnemyList_t* active_enemy_list_tail = nullptr;
    // undefinde4
    int32_t enemy_count_real = 0;
    // undefinde4
    friend class Enemy;
};

#endif // ENEMYMANAGER_H_
