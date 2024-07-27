#include "./EnemyManager.h"
#include "./GlobalData.h"
#include "./Player.h"
#include <NSlist.h>
#include <InputManager.h>
#include <sstream>

EnemyManager* ENEMY_MANAGER_PTR = nullptr;

bool dospawn = true;

void EnemyManager::Start(std::string const& eclFile, std::string const& sub) {
    fileManager->LoadEcl(eclFile);
    allocate_new_enemy(sub.c_str(), {
        .score_reward = 1000,
        .life = 40,
    });
    ENEMY_MANAGER_PTR = this;
}

EnemyManager::EnemyManager() {
    fileManager = EclFileManager::GetInstance();
    f_on_tick = new UpdateFunc([this]() { return this->on_tick(); });
    f_on_draw = new UpdateFunc([this]() { return this->on_draw(); });
    UPDATE_FUNC_REGISTRY->register_on_tick(f_on_tick, 27);
    UPDATE_FUNC_REGISTRY->register_on_draw(f_on_draw, 23);
}

EnemyManager::~EnemyManager() {
    fileManager->CloseEcl();
    delete fileManager;
    auto n = active_enemy_list_head;
    while (n) {
        auto nn = n;
        n = n->next;
        delete nn->value;
    }
    UPDATE_FUNC_REGISTRY->unregister(f_on_tick);
    UPDATE_FUNC_REGISTRY->unregister(f_on_draw);
}
int EnemyManager::closest_enemy_id(glm::vec2 pos) {
    int closest = 0;
    float dist = 999999999.f;
    for (EnemyList_t* node = active_enemy_list_head; node; node = node->next) {
        float d = math::point_distance_sq(node->value->enemy.final_pos.pos.x,
            node->value->enemy.final_pos.pos.y, pos.x, pos.y);
        if (d < dist) {
            dist = d;
            closest = node->value->enemyId;
        }
    }
    return closest;
}

int EnemyManager::on_tick() {
    // if ((((GAME_THREAD_PTR != NULL) &&
    //       (game_thread_flags = GAME_THREAD_PTR->flags,
    //        ((game_thread_flags >> 2 | game_thread_flags) & 1) == 0)) &&
    //      ((game_thread_flags & 0x400) == 0)) &&
    //     ((game_thread_flags & 2) == 0)) {
    field_0xac = 0;
    field_0xb0 = 0;
    auto current_node = active_enemy_list_head;
    while (current_node) {
        auto next_node = current_node->next;
        if (!(current_node->value->enemy.flags & 0x2000000) &&
            !current_node->value->update()) {
            current_node->value->enemy.flags &= 0xfffffffffffbffff;
            current_node = next_node;
        } else {
            if (current_node == active_enemy_list_head)
                active_enemy_list_head = current_node->next;
            if (current_node == active_enemy_list_tail)
                active_enemy_list_tail = current_node->previous;
            if (current_node->previous)
                current_node->previous->next = next_node;
            if (next_node)
                next_node->previous = current_node->previous;
            delete current_node->value;
            current_node = next_node;
            enemyCount--;
        }
    }
    if (PLAYER_PTR->damage_multiplier__used_by_winter_in_th16 <= 1.01) {
        PLAYER_PTR->flags &= 0xffffffdf;
    } else {
        PLAYER_PTR->flags |= 0x20;
    }
    PLAYER_PTR->damage_multiplier__used_by_winter_in_th16 = 1.0;
    time_in_stage++;
    //}
    return 1;
}

int EnemyManager::on_draw() {
    if (NSEngine::getInstance()->flags().flags.debugInfo)
        on_draw_debug();
    return 1;
}

void EnemyManager::EnmKillAll(const Enemy* caller, bool byDialog) {
    for (auto n = active_enemy_list_head; n; n = n->next) {
        auto e = n->value;
        if (e && e != caller) {
            auto f = e->enemy.flags;
            if (f & 256) {
                e->die();
                e->Die();
            } else if (f & 32 || f & 128) {
                continue;
            }
            if (f & 1024 && !byDialog)
                continue;
            e->die();
            e->Die();
        }
    }
}

void EnemyManager::on_draw_debug() {
    for (auto n = active_enemy_list_head; n; n = n->next)
        n->value->DebugDraw();
}

Enemy* EnemyManager::allocate_new_enemy(char const* sub,
                                EnemySpawnData_t const& spawn) {
    Enemy* enm = new Enemy(sub);
    enm->enemy.abs_pos.pos.x = spawn.pos.x;
    enm->enemy.abs_pos.pos.y = spawn.pos.y;
    enm->enemy.abs_pos.pos.z = spawn.pos.z;
    enm->enemy.scoreReward = spawn.score_reward;
    enm->enemy.life.current = spawn.life;
    enm->enemy.life.max = spawn.life;
    enm->enemy.life.curAtk = spawn.life;
    enm->enemy.drops.main_type = spawn.main_drop;
    enm->enemy.flags ^=
        ((spawn.mirrored << 0x13) ^ enm->enemy.flags) & 0x80000;
    enm->context.currentContext->difficultyMask =
        1 << GLOBALS.inner.DIFFICULTY;
    memcpy(&enm->enemy.ecl_int_vars,
           &spawn.ecl_int_vars,
           4*sizeof(int32_t) + 8*sizeof(float));
    enm->enemy.invFrame = 2;
    enm->enemy.ecl550 = 0x0;
    enm->enemy.flags ^=
        ((spawn.flag_4000000 << 0x1a) ^ enm->enemy.flags) & 0x4000000;
    enm->parentEnemyId = spawn.parent_enemy_id;
    if (spawn.life >= 1000) {
        enm->enemy.flags |= 0x40000000;
    }
    enm->enemy.own_chapter = GLOBALS.inner.CURRENT_CHAPTER;
    enm->update();
    enm->enemy.deathSnd = (enm->enemyId & 0x1) + 3;
    if (enm->enemy.deathScr != 0) goto after_switch;
    enm->enemy.deathScr = 0x2c;
    enm->enemy.deathAnm = 0x1;
    if (enm->enemy.anm0anmID != 0x2) goto after_switch;
    switch (enm->enemy.anm0scr) {
    case 0x19:
    case 0x35:
    case 0x4f:
    case 0x5:
        enm->enemy.deathScr = 0x28;
        break;
    case 0x5b:
    case 0xf:
        enm->enemy.deathScr = 0x34;
        break;
    case 0x38:
    case 0x53:
    case 0xa:
        enm->enemy.deathScr = 0x30;
        break;
    case 0x1e:
        enm->enemy.deathScr = 0x3a;
        break;
    case 0x23:
        enm->enemy.deathScr = 0x39;
        break;
    case 0x28:
        enm->enemy.deathScr = 0x38;
    }
after_switch:
    if (!active_enemy_list_head) {
        active_enemy_list_head = &enm->enemy.node;
        active_enemy_list_tail = &enm->enemy.node;
        enemy_count_real++;
        return enm;
    }
    if (active_enemy_list_tail->next) {
      enm->enemy.node.next = active_enemy_list_tail->next;
      active_enemy_list_tail->next->previous = &enm->enemy.node;
    }
    enm->enemy.node.next = active_enemy_list_tail->next;
    active_enemy_list_tail->next = &enm->enemy.node;
    enm->enemy.node.previous = active_enemy_list_tail;
    active_enemy_list_tail = &enm->enemy.node;
    enemy_count_real++;
    return enm;
}
