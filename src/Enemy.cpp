#include "./AnmOpener/AnmManager.h"
#include "./Supervisor.h"
#include "./Enemy.h"
#include "./EclContext.h"
#include "./EclFileManager.h"
#include "./EnemyManager.h"
#include "./GlobalData.h"
#include "./ItemManager.h"
#include "./Player.h"
#include "./Spellcard.h"
#include "./Gui.hpp"
#include "Bomb.hpp"
#include <DrawFuncs.h>
#include <math/Random.h>
#include <NSEngine.h>

Enemy::Enemy() {
    fileManager = EclFileManager::GetInstance();
    enemy.enemy = this;
    enemy.node.value = this;
    if (GLOBALS.inner.STAGE_NUM == 7)
        GLOBALS.inner.DIFFICULTY = 5;
    context.primaryContext.difficultyMask = 1 << GLOBALS.inner.DIFFICULTY;
    for (int i = 0; i < 16; i++)
        enemy.anmIds[i] = 0;
    ENEMY_MANAGER_PTR->enemy_count_real++;
}

Enemy::~Enemy() {
    if (enemy.flags & 0x800000) {
      ENEMY_MANAGER_PTR->boss_ids[enemy.ownBossId] = 0;
    }
    for (int i = 0; i < 16; i++)
        AnmManager::deleteVM(enemy.anmIds[i].val);
    /* Clear async list */
    clear_async_contexts();
    ENEMY_MANAGER_PTR->enemy_count_real--;
    if (enemy.fog.fog_ptr) delete enemy.fog.fog_ptr;
}

// TODO
Enemy::Enemy(char const* sub) {
  // EclVm::init_prolly(&this->ecl);
  // EclVm::reset_ecl_prolly(&this->ecl);
  enemy.enemy = this;
  enemy.node.value = this;
  enemyId = ENEMY_MANAGER_PTR->next_enemy_id;
  ENEMY_MANAGER_PTR->last_enemy_id = ENEMY_MANAGER_PTR->next_enemy_id;
  ENEMY_MANAGER_PTR->next_enemy_id++;
  if (ENEMY_MANAGER_PTR->next_enemy_id == 0) {
    ENEMY_MANAGER_PTR->next_enemy_id++;
  }
  fileManager = ENEMY_MANAGER_PTR->fileManager;
  // this->set_sub(sub);
  Init(sub);
  context.currentContext->currentLocation.offset = 0;
  context.currentContext->time = 0.0;
}

void Enemy::Init(std::string const& sub) {
    eclContextInit(&context.primaryContext, fileManager->getSubId(sub));
    context.currentContext = &context.primaryContext;
}

void EnemyDrop_t::eject_all_drops(glm::vec3 const& pos) {
    if (main_type != 0)
        ITEM_MANAGER_PTR->spawn_item(main_type, pos, -1.570796, 2.2, 0, -1);
    float a = Random::Floatm11() * 3.141593;
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < extra_counts[i]; j++) {
            ITEM_MANAGER_PTR->spawn_item(i + 1,
                (Random::Float01() * 0.5f + 0.5f) * glm::vec3 {
                    (cos(a) * area.x), sin(a) * area.y, 0
                } + pos, -1.570796, 2.2, 0, -1);
            a += 1.570796 + Random::Floatm11() * 3.141593 * 0.25;
            math::angle_normalize(a);
        }
        extra_counts[i] = 0;
    }
    main_type = 0;
    return;
}

void Enemy::DebugDraw() {
    NSEngine::draw_set_layer(NSEngine::engineData::debugLayer);
    NSEngine::draw_rectangle_rotated_color(enemy.final_pos.pos.x,
        -enemy.final_pos.pos.y, enemy.hurtbox_size.x, enemy.hurtbox_size.y,
        -enemy.rotation, { 255, 0, 255, 90 }, { 255, 0, 255, 90 },
        { 255, 0, 255, 90 }, { 255, 0, 255, 90 });

    float wt = 64;
    float ht = 4;
    float y = -16;
    NSEngine::draw_rectangle_rotated_color(enemy.final_pos.pos.x,
        -enemy.final_pos.pos.y + y, wt, ht, 0, { 0, 0, 0, 255 },
        { 0, 0, 0, 255 }, { 0, 0, 0, 255 }, { 0, 0, 0, 255 });
    float w = (enemy.life.current / static_cast<float>(enemy.life.max)) * wt;
    float x = -(wt - w) / 2;
    NSEngine::draw_rectangle_rotated_color(enemy.final_pos.pos.x + x,
        -enemy.final_pos.pos.y + y, w, ht, 0, { 255, 0, 0, 255 },
        { 255, 0, 0, 255 }, { 255, 0, 0, 255 }, { 255, 0, 0, 255 });
    // if (0 && enemy.moveLimitSize.x > 0)
    //{
    // NSEngine::draw_set_color({255,255,0,255});
    // NSEngine::draw_rectangle(enemy.moveLimitPos.x-enemy.moveLimitSize.x/2,
    // -enemy.moveLimitPos.y+enemy.moveLimitSize.y/2,
    // enemy.moveLimitPos.x+enemy.moveLimitSize.x/2,
    // -enemy.moveLimitPos.y-enemy.moveLimitSize.y/2);
    //}
}

void EnemyData::calc_final_pos() {
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

int EnemyData::step_interpolators() {
    prev_final_pos = final_pos;
    if (abs_angle_i.end_time != 0 &&
        (abs_pos.flags & 0xf) != 2 && (abs_pos.flags & 0xf) != 3) {
        abs_pos.angle = abs_angle_i.step();
        math::angle_normalize(abs_pos.angle);
    }
    if (abs_speed_i.end_time != 0) {
        abs_pos.speed = abs_speed_i.step();
    }
    if (rel_angle_i.end_time != 0 &&
        (rel_pos.flags & 0xf) != 2 && (rel_pos.flags & 0xf) != 3) {
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
    } else {
        glm::vec3 out = abs_pos_i.step();
        abs_pos.velocity = out - abs_pos.pos;
    }
    if (rel_pos_i.end_time == 0) {
        rel_pos.update_velocities();
    } else {
        glm::vec3 out = rel_pos_i.step();
        rel_pos.velocity = out - rel_pos.pos;
    }
    abs_pos.update_position();
    if (flags & 0x4000000) {
        rel_pos.pos += SUPERVISOR.cameras[0].__vec3_104;
    }
    rel_pos.update_position();

    calc_final_pos();
    // update anm main
    AnmVM* vm0 = nullptr;
    if ((flags & 0x100000U) != 0) {
        int dir = final_pos.velocity.x > 0.03;
        if (final_pos.velocity.x < -0.03)
            dir = -1;
        if (anmMainSubscr != dir) {
            int scr = 0;
            if (anmMainSubscr == -1) {
                scr = 3 - (dir != 0);
            } else if (anmMainSubscr == 0) {
                scr = (dir != -1) + 1;
            } else {
                scr = 0;
                if ((anmMainSubscr == 1) && (scr = anmMainSubscr, dir == 0)) {
                    scr = 4;
                }
            }

            vm0 = AnmManager::getVM(anmIds[0].val);
            if (vm0 == nullptr) {
                anmIds[0].val = 0;
            }
            glm::vec3 pos = { 0, 0, 0 };
            if (vm0) {
                pos = vm0->pos;
                AnmManager::deleteVM(anmIds[0].val);
                anmIds[0].val = 0;
            }
            anmIds[0] = ENEMY_MANAGER_PTR->loadedAnms[anm0anmID]->spawnVMExt(anm0scr + scr, 0, pos, 8, anmLayers + 7);
            anmMainSubscr = dir;
        }
    }

    // update sprite size
    vm0 = AnmManager::getVM(anmIds[0].val);
    if (vm0) {
        auto s = vm0->getSprite();
        finalSpriteSize.x = fabs(s.w * vm0->scale.x);
        finalSpriteSize.y = fabs(s.h * vm0->scale.y);
    } else {
        anmIds[0].val = 0;
    }

    // check offscreen
    if ((final_pos.pos.x + finalSpriteSize.x * 0.5 < -192.0) ||
        (final_pos.pos.x - finalSpriteSize.x * 0.5 > 192.0)) {
        return (flags & 0x10004) == 0x10000 ? -1 : 0;
    } else {
        if ((final_pos.pos.y + finalSpriteSize.y * 0.5 >= 0.0) &&
            (final_pos.pos.y - finalSpriteSize.y * 0.5 <= 448.0)) {
            flags |= 0x10000;
            return 0;
        }
        if ((flags & 0x10000) == 0) {
            return 0;
        }
        return (flags & 8) == 0 ? -1 : 0;
    }
}

int Enemy::die() {
    // if (enemy.deathSnd >= 0)
    //     SoundManager::play_sound_at_position(enemy.deathSnd);

    float rot_z = -math::point_direction(enemy.final_pos.pos.x,
                                        enemy.final_pos.pos.y,
                                        enemy.lastDmgPos.x,
                                        enemy.lastDmgPos.y);
    // ENEMY_MANAGER_PTR->field_0x190 = rot_z;
    if (enemy.deathScr >= 0) {
        ENEMY_MANAGER_PTR->loadedAnms[enemy.deathAnm]
            ->createEffectPos(enemy.deathScr, rot_z, enemy.final_pos.pos, 3);
    }

    enemy.drops.eject_all_drops(enemy.final_pos.pos);
    if (enemy.setDeath != "") {
        clear_async_contexts();
        reset_ecl();
        set_sub(enemy.setDeath);
        ecl_run(0.0);
        enemy.setDeath = "";
    }

    if (onDeath)
        onDeath();

    return 1;
}

void Enemy::clear_async_contexts() {
    while (asyncListHead) {
        delete asyncListHead->entry;
        auto c = asyncListHead->next;
        delete asyncListHead;
        asyncListHead = c;
    }
}

void Enemy::reset_ecl() {
    context.primaryContext.set_by_ins_18_19 &= 0xfffffffe;
    context.primaryContext.time = 0.0;
    context.primaryContext.currentLocation.offset = -1;
    context.primaryContext.currentLocation.sub_id = -1;
    context.primaryContext.asyncId = -1;
    context.primaryContext.enemy = this;
    context.primaryContext.set_by_ins_20 = 0;
    context.primaryContext.float_i[0].end_time = 0;
    context.primaryContext.float_i[1].end_time = 0;
    context.primaryContext.float_i[2].end_time = 0;
    context.primaryContext.float_i[3].end_time = 0;
    context.primaryContext.float_i[4].end_time = 0;
    context.primaryContext.float_i[5].end_time = 0;
    context.primaryContext.float_i[6].end_time = 0;
    context.primaryContext.float_i[7].end_time = 0;
    context.currentContext = &context.primaryContext;
    context.primaryContext.stack.stackOffset = 0;
    context.primaryContext.stack.baseOffset = 0;
    asyncListHead = nullptr;
    // __unused__prev_ptr_of_list_head = NULL;
    // __unused__field_c_ptr_of_list_head = NULL;
    return;
}

void Enemy::set_sub(std::string const& sub) {
    context.currentContext->currentLocation.sub_id = fileManager->getSubId(sub);
    context.currentContext->currentLocation.offset = 0;
    context.currentContext->time = 0.0;
}

int EnemyData::step_game_logic() {
    // bomb shield logic
    if (!(flags & 0x10000000)) {
        if ((BOMB_PTR->active != 1) && (flags & 0x20000000)) {
            anmSetMain = bombShieldOffAnmMain;
            AnmManager::recreate_vm(anmIds[0], bombShieldOffAnmMain);
            flags &= 0xdffffffe;
        }
    } else {
        if (BOMB_PTR->active == 1) {
            if (!((flags >> 0x1d) & 1)) {
                anmSetMain = bombShieldOnAnmMain;
                AnmManager::recreate_vm(anmIds[0], bombShieldOnAnmMain);
                flags |= 0x20000001;
            }
        } else if (((flags >> 0x1d) & 1)) {
            anmSetMain = bombShieldOffAnmMain;
            AnmManager::recreate_vm(anmIds[0], bombShieldOffAnmMain);
            flags &= 0xdffffffe;
        }
    }

    if (flags & 0x800) {
        uint ret;
        int dam = applyDamage(final_pos.pos, (flags & 0x1000) ?
            &hurtbox_size : nullptr, (flags & 0x1000) ? 0.f :
            hurtbox_size.x * 0.5f, &ret, lastDmgPos, 1,
            enemy->enemyId, (flags & 0x1000) ? rotation : 0);
        if (dam && ret && enemy->die())
            return 1;
    }

    for (int i = 0; i < 8; i++) {
        if (interrupts[i].life >= 0 && interrupts[i].time > 0) {
            if (flags & 0x800000) {
                GUI_PTR->remaining_spell_time_seconds =
                  (interrupts[i].time - timeInEcl) / 0x3c;
                GUI_PTR->remaining_spell_time_centiseconds =
                  (((interrupts[i].time - timeInEcl) % 0x3c) * 100) / 0x3c;
                if (GUI_PTR->remaining_spell_time_seconds > 99) {
                  GUI_PTR->remaining_spell_time_seconds = 99;
                  GUI_PTR->remaining_spell_time_centiseconds = 99;
                }
            }
            if (interrupts[i].time <= timeInEcl) {
                life.current = interrupts[i].life;
                interrupts[i].life = -1;
                timeInEcl = 0;
                flags |= 0x1000000;
                if (!(SPELLCARD_PTR->flags & 8)) {
                    SPELLCARD_PTR->flags |= 0x80;
                    if (SPELLCARD_PTR->flags & 1) {
                        if (SPELLCARD_PTR->__timer_20 >= 60) {
                            SPELLCARD_PTR->bonus = 0;
                            SPELLCARD_PTR->flags &= 0xffffffdd;
                        } else if (BOMB_PTR->active == 1) {
                            SPELLCARD_PTR->flags |= 0x20;
                        }
                    }
                    ENEMY_MANAGER_PTR->can_still_capture_spell = 0;
                } else if ((SPELLCARD_PTR->flags & 9) == 9) {
                    flags &= 0xfeffffff;
                    // GLOBALS.inner.field32_0x80 += __bool_cleared_by_ecl_570;
                }
                __bool_cleared_by_ecl_570 = 0;

                if (interrupts[i].subTimeout != "") {
                    timeInEcl = 0;
                    enemy->clear_async_contexts();
                    enemy->reset_ecl();
                    enemy->set_sub(interrupts[i].subTimeout);
                    // param: (float *)(&PTR_GAME_SPEED)
                    // [(int)(enemy->time_in_ecl).__game_speed__disused]
                    if (enemy->ecl_run())
                        return -1;
                }
            }
            break;
        }
    }

    flags &= 0xffdfffff;
    if (!(flags & 0x21)) {
        uint ret;
        int totalDamage = 0;
        if (0.0 < hurtbox_size.x) {
            totalDamage = applyDamage(final_pos.pos, (flags & 0x1000) ?
                &hurtbox_size : nullptr, (flags & 0x1000) ? 0.f :
                hurtbox_size.x * 0.5f, &ret, lastDmgPos, 0, enemy->enemyId,
                (flags & 0x1000) ? rotation : 0);
            totalDamage *= PLAYER_PTR
                ->damage_multiplier__used_by_winter_in_th16;
        }
        if (func_from_ecl_flag_ext_dmg)
            totalDamage += func_from_ecl_flag_ext_dmg(this, totalDamage);
        if (extra_dmg > 0)
            totalDamage += extra_dmg, extra_dmg = 0;

        if ((PLAYER_PTR->inner.state == 2) || (PLAYER_PTR->inner.state == 0))
            totalDamage /= 5;

        if (GUI_PTR->msg) totalDamage = 0;

        if (totalDamage > 0) {
            if (!ret) ENEMY_MANAGER_PTR->field_0xb0 += totalDamage;
            else {
                if (totalDamage < life.current)
                    ENEMY_MANAGER_PTR->field_0xac += totalDamage;
                else
                    ENEMY_MANAGER_PTR->field_0xac += ((int)(((totalDamage -
                    life.current) >> 0x1f & 3U) + (totalDamage - life.current
                    )) >> 2) + totalDamage;
            }
        }

        if ((BOMB_PTR->active == 1) && bombDmgMul < 1.0) {
            if ((totalDamage != 0) && (bombDmgMul <= 0.0))
                // SoundManager::play_sound_at_position(0x24);
            totalDamage *= bombDmgMul;
        }

        if (totalDamage != 0) {
            if ((SPELLCARD_PTR->flags & 0x21) == 0x21) {
                totalDamage /= 0x1e;
            }

            if (!(flags & 0x10) && invFrame <= 0) {
                life.totalDmgIncIgn += totalDamage;
                if (!(life.isSpell & 1)) {
                    life.current -= totalDamage;
                    life.curx7 = life.current * 7;
                } else {
                    life.curx7 -= totalDamage;
                    life.current = (life.curx7 - life.nextAtk * 7) / 7
                        + life.nextAtk;
                }
            } else {
                life.totalDmgIncIgn += totalDamage;
            }

            timerCountDown = 30;
            life.curAtk = life.current;
            life.nextAtk = 0;
            for (int i = 0; i < 8; i++) {
                if (-1 < interrupts[i].life) {
                    life.curAtk = life.current - interrupts[i].life;
                    life.nextAtk = interrupts[i].life;
                    if (life.current <= interrupts[i].life) {
                        if (__bool_cleared_by_ecl_570 &&
                            own_chapter == GLOBALS.inner.CURRENT_CHAPTER) {
                            // GLOBALS.inner.field32_0x80 +=
                            // __bool_cleared_by_ecl_570;
                            __bool_cleared_by_ecl_570 = 0;
                        }
                        life.current = interrupts[i].life;
                        interrupts[i].life = -1;
                        timeInEcl = 0;
                        flags &= 0xfeffffff;
                        if (interrupts[i].subNext != "") {
                            enemy->clear_async_contexts();
                            enemy->reset_ecl();
                            enemy->set_sub(interrupts[i].subNext);
                            // param: (float *)(&PTR_GAME_SPEED)
                            // [(int)(enemy->time_in_ecl).__game_speed__disused]
                            if (enemy->ecl_run())
                                return -1;
                        }
                    }
                    break;
                }
            }
            if ((((life.current < 1) & ~(flags >> 7)) != 0) && enemy->die())
                return 1;
            flags |= 0x200000;
        }
    }

    if ((life.isSpell & 2) && enemy->die())
        return 1;

    if (!(flags & 0x22) && noHbFrame <= 0 && !(flags & 0x4000000)) {
        if (!hitbox_func) {
            int a = 0;
            if (!(flags & 0x1000)) {
                // a = Bullet::try_kill_player(final_pos.pos,
                //      0, hitbox_size.x * 0.5);
            } else {
                // if (auto vm =
                //  anm_find_existing_or_clear_id(&enemy->anmIds[0]); vm) {
                // zStack_14.y = (hitbox_size.y * 0.5) *
                //  cos(vm->rotation.z + PI1_2);
                // zStack_14.x = -(hitbox_size.y * 0.5) *
                //  sin(vm->rotation.z + PI1_2);
                //}
                // pvVar10 = extraout_ECX;
                // zStack_14.x += final_pos.pos.x;
                // zStack_14.y += final_pos.pos.y;
                // zStack_14.z  = final_pos.pos.z;
                // a = enemy_collide_player_rectangle(pvVar10, &zStack_14,
                //  rotation, 0, hitbox_size.x, hitbox_size.y);
            }
            if (((flags & 0x200) && a == 2) &&
                timeInEcl % 6 == 0)
                PLAYER_PTR->do_graze(PLAYER_PTR->inner.pos);
        } else {
            hitbox_func(this);
        }
    }

    if (timerCountDown > 0)
        timerCountDown--;

    auto vm0 = AnmManager::getVM(anmIds[0]);
    if (!vm0) {
        anmIds[0] = 0;
        return 0;
    }

    if (frBefNxtHurtFx != 0) {
        vm0->bitflags.colmode = 0b00;
        frBefNxtHurtFx--;
        return 0;
    }

    if (flags & 0x80000000) {
        if (abs(timeInEcl) % 4 == 0) {
            vm0->color_2 = { 255, 0, 255, 255 };
            vm0->bitflags.colmode = 0b01;
        } else {
            vm0->bitflags.colmode = 0b00;
        }
    }

    if (!(flags & 0x200000) || (flags & 0x2000)) {
        if (abs(timeInEcl) % 4 == 0) {
            if ((flags & 0x40800000) && (SPELLCARD_PTR->flags & 9) != 9) {
                if (!(SPELLCARD_PTR->flags & 1)) {
                    if (life.curAtk >= 500)
                        return 0;
                } else if (life.curAtk >= 100) {
                    return 0;
                }
                vm0->color_2 = { 0, 0, 255, 255 };
                vm0->bitflags.colmode = 0b01;
            }
        } else {
            vm0->bitflags.colmode = 0b00;
        }
        return 0;
    }

    vm0->color_2 = { 0, 0, 255, 255 };
    vm0->bitflags.colmode = 0b01;
    frBefNxtHurtFx = 3; // why not 4

    // hit sound
    if (hitSnd >= 0) {
        // SoundManager::play_sound_at_position(hitSnd);
    } else if ((flags & 0x408000000) && (SPELLCARD_PTR->flags & 9) != 9) {
        if (!(SPELLCARD_PTR->flags & 1)) {
            if (life.curAtk < 900) {
                // SoundManager::play_sound_at_position(0x23);
            }
        } else if (life.curAtk < 200) {
        //     SoundManager::play_sound_at_position(0x23);
        }
    } else {
        // SoundManager::play_sound_at_position(0x22);
    }

    return 0;
}

int Enemy::ecl_run(float speed) {
    // IDK
    // int run_ret = 1;
    // next_node = (zLinkedList<zEclRunContext> *)&(enemy->ecl).enemy;
    // do {
    //  while(true) {
    //    auto current_node = next_node;
    //    if (!current_node) {
    //      context.currentContext = &context.primaryContext;
    //      return 0;
    //    }
    //    next_node = current_node->next;
    //    auto cur_context = current_node->entry;
    //    context.currentContext = cur_context;
    //    if (run_ret != 0) break;
    //    int context_ended = ecl_run(cur_context, speed);
    //    if (context_ended != 0) {
    //      delete context.currentContext;
    //      if (current_node->next)
    //          current_node->next->prev = current_node->prev;
    //      if (current_node->prev)
    //          current_node->prev->next = current_node->next;
    //      current_node->next = NULL;
    //      current_node->prev = NULL;
    //      delete current_node;
    //    }
    //  }
    //  run_ret = ecl_run(cur_context, speed);
    //} while (run_ret == 0);
    // return -1;
    context.currentContext = &context.primaryContext;
    if (updateContext(&context.primaryContext, speed) != 0)
        return -1;
    auto node = asyncListHead;
    while (node) {
        auto next = node->next;
        if (!node->entry) {
            // if (node->next) node->next->prev = node->prev;
            // if (node->prev) node->prev->next = node->next;
            // delete node;
            node = next;
            continue;
        }
        context.currentContext = node->entry;
        updateContext(node->entry, speed);
        if (node->entry->currentLocation.sub_id < 0) {
            // if (node->next) node->next->prev = node->prev;
            // if (node->prev) node->prev->next = node->next;
            // delete node->entry;
            // delete node;
            // node = nullptr;
        }
        node = next;
    }
    // context.primaryContext.currentLocation.sub_id < 0;
    return 0;
}

int EnemyData::update() {
    if (!(flags & 0x40000)) {
        flags |= 0x40000;

        // param: (float *)(&PTR_GAME_SPEED)
        // [(int)(enemy->time_in_ecl).__game_speed__disused]
        if (step_interpolators() || enemy->ecl_run() ||
            (func_from_ecl_func_set && func_from_ecl_func_set(this)) ||
            step_game_logic()) {
            enemy->Die();
            return -1;
        }

        update_distorsion();

        if (flags & 0x4000000) {
            for (size_t i = 0; i < 14; i++) {
                if (auto vm = AnmManager::getVM(anmIds[i]); vm)
                    vm->entity_pos = final_pos.pos;
            }
        } else {
            for (size_t i = 0; i < 14; i++) {
                if (auto vm = AnmManager::getVM(anmIds[i]); vm) {
                    glm::vec3 p = anmPos[i] + final_pos.pos;
                    if (anmRelated[i] >= 0) {
                        if (auto vm2 = AnmManager::getVM(
                            anmIds[anmRelated[i]]); vm2)
                            p += vm2->pos;
                        else
                            anmIds[anmRelated[i]] = 0;
                    }
                    vm->entity_pos = p;
                    if (vm->bitflags.autoRotate) {
                        vm->rotation.z = rotation = math::point_direction(
                            0, 0, final_pos.velocity.x, final_pos.velocity.y);
                        vm->bitflags.rotated = true;
                    }
                } else {
                    anmIds[i] = 0;
                }
            }
        }

        if (invFrame > 0)
            invFrame--;
        if (noHbFrame > 0)
            noHbFrame--;
        timeAlive++;
        timeInEcl++;
    }
    return 0;
}

int Enemy::update() {
    if (enemy.slowdown > 0) {
        float saved_game_speed = GAME_SPEED;

        if (GAME_SPEED - enemy.slowdown * GAME_SPEED >= 0) {
            if (GAME_SPEED - enemy.slowdown * GAME_SPEED <= 1.0)
                GAME_SPEED = GAME_SPEED - enemy.slowdown * GAME_SPEED;
            else
                GAME_SPEED = 1.f;
        } else {
            GAME_SPEED = 0.f;
        }

        for (size_t i = 0; i < 16; i++) {
            if (auto vm = AnmManager::getVM(enemy.anmIds[i]); vm)
                vm->slowdown = enemy.slowdown;
            else
                enemy.anmIds[i] = 0;
        }

        int ret = enemy.update();
        enemy.flags |= 0x100000000;
        GAME_SPEED = saved_game_speed;
        return ret;
    }
    if (enemy.flags & 0x100000000) {
        for (size_t i = 0; i < 16; i++) {
            if (auto vm = AnmManager::getVM(enemy.anmIds[i]); vm)
                vm->slowdown = 0;
            else
                enemy.anmIds[i] = 0;
        }
    }
    return enemy.update();
}

void Enemy::Tick() {
    if (context.primaryContext.currentLocation.sub_id == -1)
        return;

    /* Update all contexts */
    context.currentContext = &context.primaryContext;
    updateContext(&context.primaryContext);
    for (EclRunContextList_t* node = asyncListHead;
         node != nullptr; node = node->next) {
        context.currentContext = node->entry;
        updateContext(node->entry);
    }

    /* Cleanup unused contexts from the list */
    for (EclRunContextList_t* node = asyncListHead; node != nullptr;) {
        if (node->entry->currentLocation.sub_id == -1) {
            EclRunContextList_t* nodeS = node;
            if (node == asyncListHead)
                asyncListHead = node->next;
            if (node->prev)
                node->prev->next = node->next;
            if (node->next)
                node->next->prev = node->prev;
            node = node->next;
            delete nodeS->entry;
            delete nodeS;
        } else {
            node = node->next;
        }
    }

    int r = enemy.step_interpolators();
    if (r != 0)
        Die();

    /* Update position of attached anims  */
    for (int i = 0; i < 16; i++) {
        auto anm = AnmManager::getVM(enemy.anmIds[i].val);
        if (!anm) {
            enemy.anmIds[i].val = 0;
            continue;
        }
        glm::vec3 pos = enemy.final_pos.pos + enemy.anmPos[i];
        anm->setEntityPos(pos.x, pos.y, enemy.final_pos.pos.z);
    }

    /* Update time variables */
    enemy.timeAlive++;
    enemy.timeInEcl = context.primaryContext.time;
    if (enemy.invFrame > 0)
        enemy.invFrame--;
    if (enemy.noHbFrame > 0)
        enemy.noHbFrame--;

    /* Update interrupts */
    for (int i = 0; i < 8; i++) {
        auto& I = enemy.interrupts[i];
        if (I.time > 0)
            I.time--;
        std::string interrupt = "";
        if (I.life >= 0 && I.life >= enemy.life.current) {
            interrupt = I.subNext;
            enemy.life.current = I.life;
            I.life = -1;
        } else if (I.time == 0) {
            interrupt = I.subTimeout;
            I.time = -1;
        }
        if (interrupt != "") {
            // delete all async contexts
            while (asyncListHead) {
                auto n = asyncListHead;
                asyncListHead = asyncListHead->next;
                if (n->entry)
                    delete n->entry;
                delete n;
            }
            // reset context
            eclContextInit(&context.primaryContext,
                           fileManager->getSubId(interrupt));
        }
    }

    /* Check if enemy can be safely removed */
    //    float x = enemy.final_pos.pos.x;
    //    float y = enemy.final_pos.pos.y;
    //    if (enemy.timeAlive > 120 && (abs(x) > 234 || y < -64 || y > 486))
    //    {
    //        for (int i = 0; i < 4; i++) if (EnemyManager::GetInstance()->
    //          data.boss_ids[i] == enemyId) return;
    //        Die();
    //    }
    if (enemy.life.current <= 0 && (enemy.flags & 0b1011000) == 0) {
        auto f = enemy.flags;
        if (f & 256)
            Die();
        else if (f & 32 || f & 128)
            return;
        if (f & 1024 && !false)
            return;
        Die();
    }
}

int Enemy::updateContext(EclRunContext_t* cont, float speed) {
    if (cont->currentLocation.sub_id < 0)
        return -1;

    const char* off = fileManager->getSubStartPtr(cont->currentLocation.sub_id)
        + cont->currentLocation.offset;
    const EclRawInstr_t* instr = reinterpret_cast<const EclRawInstr_t*>(off);
    while (cont->currentLocation.sub_id != -1 && instr->time <= cont->time) {
        if (execInstr(cont, instr) == -1)
            return -1;
        off = fileManager->getSubStartPtr(cont->currentLocation.sub_id) +
            cont->currentLocation.offset;
        instr = reinterpret_cast<const EclRawInstr_t*>(off);
    }

    cont->time += speed;

    for (int i = 0; i < 8; i++) {
        if (cont->float_i[i].end_time != 0) {
            checkVarRF(cont->float_interps_locs[i]) = cont->float_i[i].step();
        }
    }
    return 0;
}

// TODO(ClementChambard): this is not the correct way to kill an enemy
void Enemy::Die() {
    context.primaryContext.currentLocation.sub_id = -1;
    context.primaryContext.currentLocation.offset = -1;
    for (int i = 0; i < 16; i++) {
        AnmManager::deleteVM(enemy.anmIds[i].val);
        enemy.anmIds[i].val = 0;
    }
}

void EnemyData::enm_create_from_ecl() {
    if (ENEMY_MANAGER_PTR->enemy_count_real >=
        ENEMY_MANAGER_PTR->enemy_limit) return;
    auto context = enemy->context.currentContext;
    if (context->currentLocation.offset == -1 ||
        context->currentLocation.sub_id == -1) return;
    EnemySpawnData_t enemy_spawn_data;

    const EclRawInstr_t* ins = reinterpret_cast<const EclRawInstr_t*>(
        enemy->fileManager->getSubStartPtr(context->currentLocation.sub_id)
        + context->currentLocation.offset);

    uint8_t* __arg = const_cast<uint8_t*>(ins->data);
    int __argmask = 1;
    int32_t __sub_name_size = *reinterpret_cast<int32_t*>(__arg);
    __arg += 4;
    std::string sub_name = "";
    for (int i = 0; i < __sub_name_size; i++)
        if (__arg[i] != 0)
            sub_name += __arg[i];
    __arg += __sub_name_size;
    __argmask *= 2;

    float x = *reinterpret_cast<float*>(__arg);
    if (ins->param_mask & __argmask) {
        x = enemy->checkVarF(x);
    }
    __arg += 4;
    __argmask *= 2;

    float y = *reinterpret_cast<float*>(__arg);
    if (ins->param_mask & __argmask) {
        y = enemy->checkVarF(y);
    }
    __arg += 4;
    __argmask *= 2;

    int32_t life = *reinterpret_cast<int32_t*>(__arg);
    if (ins->param_mask & __argmask) {
        life = enemy->checkVarI(life);
    }
    __arg += 4;
    __argmask *= 2;

    int32_t score_reward = *reinterpret_cast<int32_t*>(__arg);
    if (ins->param_mask & __argmask) {
        score_reward = enemy->checkVarI(score_reward);
    }
    __arg += 4;
    __argmask *= 2;

    int32_t main_drop = *reinterpret_cast<int32_t*>(__arg);
    if (ins->param_mask & __argmask) {
        main_drop = enemy->checkVarI(main_drop);
    }
    context->stack.stackOffset -= enemy->stackToRemove;
    enemy->stackToRemove = 0;

    enemy_spawn_data.pos.y = 0.0;
    enemy_spawn_data.pos.z = 0.0;
    enemy_spawn_data.mirrored = 0;
    enemy_spawn_data.flag_4000000 = 0;
    enemy_spawn_data.pos.x = x;
    enemy_spawn_data.pos.y = y;
    int oc = ins->id;
    if (oc == 300 || oc == 309 || oc == 321 || oc == 311 || oc == 304) {
        enemy_spawn_data.pos.x += final_pos.pos.x;
        enemy_spawn_data.pos.y += final_pos.pos.y;
    }
    if (oc == 311 || oc == 304 || oc == 312 || oc == 305) {
        enemy_spawn_data.mirrored = 1;
    }
    if (flags & 0x80000) {
        enemy_spawn_data.pos.x = enemy_spawn_data.pos.x * -1.0;
        enemy_spawn_data.mirrored ^= 1;
    }
    enemy_spawn_data.life = life;
    enemy_spawn_data.score_reward = score_reward;
    enemy_spawn_data.main_drop = main_drop;
    memcpy(enemy_spawn_data.ecl_int_vars, ecl_int_vars,
           4 * sizeof(int32_t) + 8 * sizeof(float));
    enemy_spawn_data.parent_enemy_id = enemy->enemyId;
    ENEMY_MANAGER_PTR->allocate_new_enemy(sub_name.c_str(), enemy_spawn_data);
}
