#include "Enemy.h"
#include "EnemyManager.h"
#include "EclContext.h"
#include "EclFileManager.h"
#include "GlobalData.h"

#include <AnmOpener/AnmManagerN.h>


Enemy::Enemy()
{
    fileManager = EclFileManager::GetInstance();
    enemy.enemy = this;
    context.primaryContext.difficultyMask = 1 << Globals::get()->difficulty;
    for (int i = 0; i < 16; i++) enemy.anmIds[i] = 0;
}

Enemy::~Enemy()
{
    /* Clear async list */
    while(asyncListHead)
    {
        auto n = asyncListHead;
        asyncListHead = asyncListHead->next;
        if (n->entry) delete n->entry;
        delete n;
    }
    for (int i = 0; i < 16; i++) NSEngine::AnmManagerN::deleteVM(enemy.anmIds[i].val);
}

void Enemy::Init(std::string sub)
{
    eclContextInit(&context.primaryContext, fileManager->getSubId(sub));
}

#include <DrawFuncs.h>
#include <NSEngine.h>

void Enemy::DebugDraw()
{
    if (0 && enemy.moveLimitSize.x > 0)
    {
        NSEngine::draw_set_layer(4);
        NSEngine::draw_set_color({255,255,0,255});
        NSEngine::draw_rectangle(enemy.moveLimitPos.x-enemy.moveLimitSize.x/2,
                                 -enemy.moveLimitPos.y+enemy.moveLimitSize.y/2,
                                 enemy.moveLimitPos.x+enemy.moveLimitSize.x/2,
                                 -enemy.moveLimitPos.y-enemy.moveLimitSize.y/2);
    }
}


void EnemyData::calc_final_pos()
{
  final_pos.velocity = rel_pos.pos + abs_pos.pos - final_pos.pos;
  final_pos.update_position();
  // move limits
  if ((flags & 0x20000U) != 0) {
    if (final_pos.pos.x < moveLimitPos.x - moveLimitSize.x * 0.5) {
      final_pos.pos.x = moveLimitPos.x - moveLimitSize.x * 0.5;
    }
    if (final_pos.pos.x > moveLimitPos.x + moveLimitSize.x * 0.5) {
      final_pos.pos.x = moveLimitPos.x + moveLimitSize.x * 0.5;
    }
    if (final_pos.pos.y < moveLimitPos.y - moveLimitSize.y / 2.f) {
      final_pos.pos.y = moveLimitPos.y - moveLimitSize.y / 2.f;
    }
    if (final_pos.pos.y > moveLimitPos.y + moveLimitSize.y / 2.f) {
      final_pos.pos.y = moveLimitPos.y + moveLimitSize.y / 2.f;
    }
    abs_pos.pos = final_pos.pos - rel_pos.pos;
  }

}

int EnemyData::step_interpolators()
{
  prev_final_pos = final_pos;
  if (abs_angle_i.end_time != 0 && (abs_pos.flags & 0xf) != 2 && (abs_pos.flags & 0xf) != 3) {
    abs_pos.angle = abs_angle_i.step();
    math::angle_normalize(abs_pos.angle);
  }
  if (abs_speed_i.end_time != 0) {
    abs_pos.speed = abs_speed_i.step();
  }
  if (rel_angle_i.end_time != 0 && (rel_pos.flags & 0xf) != 2 && (rel_pos.flags & 0xf) != 3) {
    rel_pos.angle = rel_angle_i.step();
    math::angle_normalize(rel_pos.angle);
  }
  if (rel_speed_i.end_time != 0) {
    rel_pos.speed = rel_speed_i.step();
  }
  if (abs_radial_dist_i.end_time != 0) {
    glm::vec2 out = abs_radial_dist_i.step();
    abs_pos.circle_radius = out.x;
    abs_pos.circle_radial_speed = out.y;
  }
  if (rel_radial_dist_i.end_time != 0) {
    glm::vec2 out = rel_radial_dist_i.step();
    rel_pos.circle_radius = out.x;
    rel_pos.circle_radial_speed = out.y;
  }
  if (abs_pos_i.end_time == 0) {
    abs_pos.update_velocities();
  }
  else {
    glm::vec3 out = abs_pos_i.step();
    abs_pos.velocity = out - abs_pos.pos;
  }
  if (rel_pos_i.end_time == 0) {
    rel_pos.update_velocities();
  }
  else {
    glm::vec3 out = rel_pos_i.step();
    rel_pos.velocity = out - rel_pos.pos;
  }
  abs_pos.update_position();
  if ((flags & 0x4000000U) != 0) {
    //rel_pos.pos += SUPERVISOR.cameras[0].__vec3_104;
  }
  rel_pos.update_position();

  calc_final_pos();
  // update anm main
  NSEngine::AnmVM* vm0 = nullptr;
  if ((flags & 0x100000U) != 0) {
    int dir = final_pos.velocity.x > 0.03;
    if (final_pos.velocity.x < -0.03) dir = -1;
    if (anmMainSubscr != dir) {
      int scr = 0;
      if (anmMainSubscr == -1) {
        scr = 3 - (dir != 0);
      }
      else if (anmMainSubscr == 0) {
        scr = (dir != -1) + 1;
      }
      else {
        scr = 0;
        if ((anmMainSubscr == 1) && (scr = anmMainSubscr, dir == 0)) {
          scr = 4;
        }
      }

      vm0 = NSEngine::AnmManagerN::getVM(anmIds[0].val);
      if (vm0 == nullptr) {
        anmIds[0].val = 0;
      }
      glm::vec3 pos = {0, 0, 0};
      if (vm0) {
        pos = vm0->pos;
        NSEngine::AnmManagerN::deleteVM(anmIds[0].val);
        anmIds[0].val = 0;
      }
      vm0 = NSEngine::AnmManagerN::getVM(NSEngine::AnmManagerN::SpawnVM(anm0anmID, anm0scr + scr));
      vm0->bitflags.randomMode = 1;
      if (anmLayers > -8) {
        vm0->layer = anmLayers + 7;
        if (anmLayers < 17) vm0->bitflags.originMode = 0b01;
      }
      vm0->entity_pos = pos;
      vm0->rotation.current.z = vm0->rotation.goal.z = 0.0;
      vm0->update();
      //vm0->mode_of_create_child = 8;
      anmIds[0].val = vm0->id.val;
      anmMainSubscr = dir;
    }
  }

  // update sprite size
  vm0 = NSEngine::AnmManagerN::getVM(anmIds[0].val);
  if (vm0) {
    auto s = vm0->getSprite();
    finalSpriteSize.x = fabs(s.w * vm0->scale.current.x);
    finalSpriteSize.y = fabs(s.h * vm0->scale.current.y);
  } else anmIds[0].val = 0;

  //check offscreen
  if ((final_pos.pos.x + finalSpriteSize.x * 0.5 < -192.0) || (final_pos.pos.x - finalSpriteSize.x * 0.5 > 192.0)) {
    return (flags & 0x10004U) == 0x10000 ? -1 : 0;
  }
  else {
    if ((final_pos.pos.y + finalSpriteSize.y * 0.5 >= 0.0) && (final_pos.pos.y - finalSpriteSize.y * 0.5 <= 448.0)) {
      flags |= 0x10000;
      return 0;
    }
    if ((flags & 0x10000U) == 0) {
      return 0;
    }
    return (flags & 8U) == 0 ? -1 : 0;
  }

}

void Enemy::Tick()
{
    if (context.primaryContext.currentLocation.sub_id == -1) return;

    /* Update all contexts */
    updateContext(&context.primaryContext);
    for (EclRunContextList_t* node = asyncListHead; node != nullptr; node = node->next)
    {
        updateContext(node->entry);
    }

    /* Cleanup unused contexts from the list */
    for (EclRunContextList_t* node = asyncListHead; node != nullptr;)
    {
        if (node->entry->currentLocation.sub_id == -1)
        {
            EclRunContextList_t* nodeS = node;
            if (node == asyncListHead) asyncListHead = node->next;
            if (node->prev) node->prev->next = node->next;
            if (node->next) node->next->prev = node->prev;
            node = node->next;
            delete nodeS->entry;
            delete nodeS;
        }
        else node = node->next;
    }

    int r = enemy.step_interpolators();
    if (r!=0) Die();

    /* Update position of attached anims  */
    for (int i = 0; i < 16; i++)
    {
        auto anm = NSEngine::AnmManagerN::getVM(enemy.anmIds[i].val);
        if (!anm) { enemy.anmIds[i].val = 0; continue; }
        glm::vec3 pos = enemy.final_pos.pos + enemy.anmPos[i];
        anm->setEntityPos(pos.x, pos.y, enemy.final_pos.pos.z);
    }

    /* Update time variables */
    enemy.timeAlive++;
    enemy.timeInEcl = context.primaryContext.time;
    if (enemy.invFrame > 0) enemy.invFrame--;
    if (enemy.noHbFrame > 0) enemy.noHbFrame--;

    /* Update interrupts */
    for (int i = 0; i < 8; i++)
    {
        auto& I = enemy.interrupts[i];
        if (I.time > 0) I.time--;
        std::string interrupt = "";
        if (I.life >= 0 && I.life >= enemy.life.current) { interrupt = I.subNext; enemy.life.current = I.life; I.life = -1; }
        else if (I.time == 0) { interrupt = I.subTimeout; I.time = -1; }
        if (interrupt != "")
        {
            // delete all async contexts
            while(asyncListHead)
            {
                auto n = asyncListHead;
                asyncListHead = asyncListHead->next;
                if (n->entry) delete n->entry;
                delete n;
            }
            // reset context
            eclContextInit(&context.primaryContext, fileManager->getSubId(interrupt));
        }
    }

    /* Check if enemy can be safely removed */
//    float x = enemy.final_pos.pos.x;
//    float y = enemy.final_pos.pos.y;
//    if (enemy.timeAlive > 120 && (abs(x) > 234 || y < -64 || y > 486))
//    {
//        for (int i = 0; i < 4; i++) if (EnemyManager::GetInstance()->data.boss_ids[i] == enemyId) return;
//        Die();
//    }
    if (enemy.life.current <= 0 && (enemy.flags & 0b1011000) == 0)
    {
            auto f = enemy.flags;
            if (f & 256) Die();
            else if (f & 32 || f & 128) return;
            if (f & 1024 && !false) return;
            Die();
    }
}

void Enemy::updateContext(EclRunContext_t* cont)
{
    context.currentContext = cont;
    if (cont->currentLocation.sub_id < 0) return;

    char* off = fileManager->getSubStartPtr(cont->currentLocation.sub_id) + cont->currentLocation.offset;
    EclRawInstr_t* instr = (EclRawInstr_t*) off;
    while ( cont->currentLocation.sub_id != -1 && instr->time <= cont->time )
    {
        execInstr(cont, instr);
        off = fileManager->getSubStartPtr(cont->currentLocation.sub_id) + cont->currentLocation.offset;
        instr = (EclRawInstr_t*) off;
    }

    cont->time++;
}

// TODO: this is not the correct way to kill an enemy
void Enemy::Die()
{
    context.primaryContext.currentLocation.sub_id = -1;
    for (int i = 0; i < 16; i++) { NSEngine::AnmManagerN::deleteVM(enemy.anmIds[i].val); enemy.anmIds[i].val = -1; }
}
