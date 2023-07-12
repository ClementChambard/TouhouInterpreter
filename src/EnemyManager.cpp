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

    SpawnEnemy(sub, 0, 0, 40, 1000, 0);
    // dospawn = false;
    ENEMY_MANAGER_PTR = this;
}

Enemy* EnemyManager::SpawnEnemy(std::string sub, float x, float y, int life,
                                int score, int item) {
    if (!dospawn)
        sub = "";
    if (enemyCount >= enemy_limit) {
        std::cout << "Can't spawn " << sub << ": Too much enemies.\n";
        return nullptr;
    }
    auto e = ListUtil::listInsertAfter(active_enemy_list_head,
                                       new Enemy())->value;
    e->Init(sub);
    e->enemy.abs_pos.pos = { x, y, 0 };
    e->enemy.scoreReward = score;
    e->enemy.life.max = e->enemy.life.current = life;
    e->enemy.drops.main_type = item;
    e->enemyId = next_enemy_id;
    e->enemy.own_chapter = GLOBALS.inner.CURRENT_CHAPTER;
    if (999 < life)
        e->enemy.flags |= 0x40000000;
    e->enemy.deathSnd = (e->enemyId & 1) + 3;
    e->update();
    if (e->enemy.deathScr == 0) {
        e->enemy.deathScr = 0x2c;
        std::cout << e->enemy.anm0anmID << "\n";
        if (e->enemy.anm0anmID == 2) {
            switch (e->enemy.anm0scr) {
            case 5:
            case 0x19:
            case 0x35:
            case 0x4f:
                e->enemy.deathScr = 0x28;
                break;
            case 10:
            case 0x38:
            case 0x53:
                e->enemy.deathScr = 0x30;
                break;
            case 0xf:
            case 0x5b:
                e->enemy.deathScr = 0x34;
                break;
            case 0x1e:
                e->enemy.deathScr = 0x3a;
                break;
            case 0x23:
                e->enemy.deathScr = 0x39;
                break;
            case 0x28:
                e->enemy.deathScr = 0x38;
            }
        }
        e->enemy.deathAnm = loadedAnms[1]->getSlot();
    }
    last_enemy_id = next_enemy_id;
    next_enemy_id++;
    enemyCount++;
    std::cout << "Spawning " << sub << " at (" << x << ',' << y
              << ") ID=" << e->enemyId << " -> total: " << enemyCount << "\n";
    return e;
}

EnemyManager::EnemyManager() {
    fileManager = EclFileManager::GetInstance();
    active_enemy_list_head = new EnemyList_t();
    active_enemy_list_tail = new EnemyList_t();
    active_enemy_list_head->next = active_enemy_list_tail;
    active_enemy_list_tail->previous = active_enemy_list_head;

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
        if (nn->value)
            delete nn->value;
        delete nn;
    }
    UPDATE_FUNC_REGISTRY->unregister(f_on_tick);
    UPDATE_FUNC_REGISTRY->unregister(f_on_draw);
}
int EnemyManager::closest_enemy_id(glm::vec2 pos) {
    int closest = 0;
    float dist = 999999999.f;
    for (EnemyList_t* node = active_enemy_list_head->next;
         node != active_enemy_list_tail; node = node->next) {
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
    // Update();
    // if ((((GAME_THREAD_PTR != NULL) &&
    //       (game_thread_flags = GAME_THREAD_PTR->flags,
    //        ((game_thread_flags >> 2 | game_thread_flags) & 1) == 0)) &&
    //      ((game_thread_flags & 0x400) == 0)) &&
    //     ((game_thread_flags & 2) == 0)) {
    field_0xac = 0;
    field_0xb0 = 0;
    auto current_node = active_enemy_list_head->next;
    while (current_node->value) {
        auto next_node = current_node->next;
        if (!(current_node->value->enemy.flags & 0x2000000) &&
            !current_node->value->update()) {
            current_node->value->enemy.flags &= 0xfffffffffffbffff;
            current_node = next_node;
        } else {
            if (current_node->previous)
                current_node->previous->next = next_node;
            if (next_node)
                next_node->previous = current_node->previous;
            delete current_node->value;
            delete current_node;
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

void EnemyManager::Update() {
    for (EnemyList_t* node = active_enemy_list_head->next;
         node != active_enemy_list_tail; node = node->next) {
        if (!node->value ||
            node->value->context.primaryContext.currentLocation.sub_id == -1) {
            node->previous->next = node->next;
            node->next->previous = node->previous;
            EnemyList_t* n = node;
            node = node->previous;
            if (node->value)
                delete n->value;
            delete n;
            enemyCount--;
            continue;
        }
        // node->value->Tick();
        node->value->enemy.flags &= ~0x40000;
        node->value->update();
    }
    if (Inputs::Keyboard().Pressed(NSK_k)) {
        for (EnemyList_t* node = active_enemy_list_head->next;
             node != active_enemy_list_tail; node = node->next) {
            if (node->value)
                node->value->getData()->life.current = 0;
        }
    }
    time_in_stage++;
    if (boss_ids[0] > 0) {
        for (EnemyList_t* node = active_enemy_list_head->next;
             node != active_enemy_list_tail; node = node->next)
            if (node->value && node->value->enemyId == boss_ids[0])
                return;
        boss_ids[0] = -1;
    }
}

void EnemyManager::EnmKillAll(const Enemy* caller, bool byDialog) {
    for (auto n = active_enemy_list_head->next;
         n != active_enemy_list_tail; n = n->next) {
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
    for (auto n = active_enemy_list_head->next;
         n != active_enemy_list_tail; n = n->next) {
        auto e = n->value;
        if (e)
            e->DebugDraw();
    }
}
