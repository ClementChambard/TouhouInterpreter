#include "EnemyManager.h"
#include <NSlist.h>

#include "GlobalData.h"

#include <sstream>

bool dospawn = true;

void EnemyManager::Start(std::string eclFile, std::string sub)
{
    if (eclFile.size() > 3 && eclFile[3] >= '0' && eclFile[3] < '8') Globals::get()->stage_id = eclFile[3] - '0';

    fileManager->LoadEcl(eclFile);

    SpawnEnemy(sub, 0, 0, 40, 1000, 0);
    //dospawn = false;
}

Enemy* EnemyManager::SpawnEnemy(std::string sub, float x, float y, int life, int score, int item)
{
    if (!dospawn) sub = "";
    if (enemyCount >= data.enemy_limit)
    {
        std::cout << "Can't spawn " << sub << ": Too much enemies.\n";
        return nullptr;
    }
    auto e = ListUtil::listInsertAfter(active_enemy_list_head, new Enemy())->value;
    e->Init(sub);
    e->enemy.abs_pos.pos = {x, y, 0};
    e->enemy.scoreReward = score;
    e->enemy.life.max = e->enemy.life.current = life;
    e->enemy.drops.main_type = item;
    e->enemyId = data.next_enemy_id;
    data.last_enemy_id = data.next_enemy_id;
    data.next_enemy_id++;
    enemyCount++;
    std::cout << "Spawning " << sub << " at (" << x << ',' << y << ") ID=" << e->enemyId << " -> total: " << enemyCount << "\n";
    return e;
}

EnemyManager::EnemyManager()
{
    fileManager = EclFileManager::GetInstance();
    active_enemy_list_head = new EnemyList_t();
    active_enemy_list_tail = new EnemyList_t();
    active_enemy_list_head->next = active_enemy_list_tail;
    active_enemy_list_tail->previous = active_enemy_list_head;

    f_on_tick = new UpdateFunc([this](){ return this->on_tick(); });
    f_on_draw = new UpdateFunc([this](){ return this->on_draw(); });
    UPDATE_FUNC_REGISTRY->register_on_tick(f_on_tick, 27);
    UPDATE_FUNC_REGISTRY->register_on_draw(f_on_draw, 23);
}

EnemyManager::~EnemyManager()
{
    fileManager->CloseEcl();
    delete fileManager;
    auto n = active_enemy_list_head;
    while (n)
    {
        auto nn = n;
        n = n->next;
        if (nn->value) delete nn->value;
        delete nn;
    }
    UPDATE_FUNC_REGISTRY->unregister(f_on_tick);
    UPDATE_FUNC_REGISTRY->unregister(f_on_draw);
}
#include <InputManager.h>
void EnemyManager::Update()
{
    for (EnemyList_t* node = active_enemy_list_head->next; node != active_enemy_list_tail; node = node->next)
    {
        if (!node->value || node->value->context.primaryContext.currentLocation.sub_id == -1)
        {
            node->previous->next = node->next;
            node->next->previous = node->previous;
            EnemyList_t* n = node;
            node = node->previous;
            if (node->value) delete n->value;
            delete n;
            enemyCount--;
            continue;
        }
        node->value->Tick();
    }
    if (Inputs::Keyboard().Pressed(NSK_k))
        for (EnemyList_t* node = active_enemy_list_head->next; node != active_enemy_list_tail; node = node->next)
        {
            if (node->value) node->value->getData()->life.current = 0;
        }
    data.time_in_stage++;
    if (data.boss_ids[0] > 0) {
        for (EnemyList_t* node = active_enemy_list_head->next; node != active_enemy_list_tail; node = node->next)
            if (node->value && node->value->enemyId == data.boss_ids[0])
                return;
        data.boss_ids[0] = -1;
    }
}

void EnemyManager::EnmKillAll(Enemy* caller, bool byDialog)
{
    for (auto n = active_enemy_list_head->next; n != active_enemy_list_tail; n=n->next)
    {
        auto e = n->value;
        if (e && e != caller)
        {
            auto f = e->enemy.flags;
            if (f & 256) e->Die();
            else if (f & 32 || f & 128) continue;
            if (f & 1024 && !byDialog) continue;
            e->Die();
        }
    }
}
