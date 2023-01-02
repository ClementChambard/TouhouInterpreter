#include "LaserInfinite.h"
#include "../GlobalData.h"
#include "LaserManager.h"

LaserInfinite::LaserInfinite()
{
    vm1.sprite_id = -1;
    vm1.current_instr = -1;
    vm2.sprite_id = -1;
    vm2.current_instr = -1;
}

int LaserInfinite::initialize(void* arg)
{
    inner = *reinterpret_cast<LaserInfiniteInner_t*>(arg);

    bullet_type = inner.type;
    bullet_color = inner.color;
    kind = 1;
    __field_10__set_to_3_by_ex_delete = 3;

    vm1.destroy();
    // AnmLoaded::load_external_vm(LASER_MANAGER_PTR->bullet_anm, vm1, BULLET_TYPE_TABLE[bullet_type]["script"].asInt());
    vm1(AnmManagerN::getLoaded(7)->getPreloaded(BULLET_TYPE_TABLE[bullet_type]["script"].asInt()));
    vm1.on_set_sprite = [](AnmVM* vm, int spr) {
        auto b = (Laser*)vm->getEntity();
        if (BULLET_TYPE_TABLE[b->bullet_type]["colors"][0]["main_sprite_id"].asInt() < 0)
            return spr;
        if (spr == 0)
            return BULLET_TYPE_TABLE[b->bullet_type]["colors"][b->bullet_color]["main_sprite_id"].asInt();
        if (spr == 1)
            return BULLET_TYPE_TABLE[b->bullet_type]["colors"][b->bullet_color]["spawn_sprite_id"].asInt();
        if (spr == 2)
            return BULLET_TYPE_TABLE[b->bullet_type]["colors"][b->bullet_color]["cancel_sprite_id"].asInt();
        return spr;
    };
    vm1.entity = this;
    vm1.interrupt(2);
    vm1.update();
    vm1.bitflags.originMode = 1;
    vm1.bitflags.rendermode = 1;
    vm1.bitflags.anchorY = 2;
    vm1.bitflags.blendmode = 1;
    vm1.layer = 14;

    // anm_init_copy_vm_from_loaded(LASER_MANAGER_PTR->bullet_anm, vm2, inner.__bmgr_004__color + LASER_DATA["spawn_anm_first"].asInt());
    vm2(AnmManagerN::getLoaded(7)->getPreloaded(inner.color + LASER_DATA["spawn_anm_first"].asInt()));
    vm2.parent = nullptr;
    // vm2.__root_vm__or_maybe_not = nullptr;
    vm2.update();
    vm2.interrupt(2);
    vm2.update();
    vm2.bitflags.originMode = 1;
    vm2.bitflags.rendermode = 1;
    vm2.bitflags.blendmode = 1;
    vm2.layer = 14;

    if (inner.shot_sfx >= 0) { /*SoundManager::play_sound_at_position(inner.shot_sfx);*/
    }

    laser_offset = inner.start_pos;

    if (inner.distance != 0.f)
        laser_offset += glm::vec3(math::lengthdir_vec(inner.distance, inner.ang_aim), 0.f);
    laser_inf_current_length = inner.laser_new_arg1;
    speed = inner.spd_1;
    angle = inner.ang_aim;
    et_ex_index = inner.et_ex_index;
    laser_st_width = 2.0;
    laser_id = inner.laser_st_on_arg1;
    __timer_2c = 0;

    return 0;
}

int LaserInfinite::on_tick()
{
    while (et_ex_index < 18 && inner.ex[et_ex_index].type && !(inner.ex[et_ex_index].slot == 0 && flags != 0)) {
        if (inner.ex[et_ex_index].type == 0x80) {
            ex_invuln__remaining_frames = inner.ex[et_ex_index].a;
        } else if (inner.ex[et_ex_index].type == 0x400) {
            __field_10__set_to_3_by_ex_delete = 3;
        } else if (inner.ex[et_ex_index].type == 0x100000) {
            if (inner.ex[et_ex_index].a == 0) vm1.bitflags.blendmode = 0;
            else vm1.bitflags.blendmode = 1;
            et_ex_index++;
            continue;
        }
        et_ex_index++;
    }
    if (flags != 0) {
        if (flags < 0) {
            if (ex_wait.timer <= 0) flags &= 0x7fffffff;
            else ex_wait.timer--;
        }
        if (ex_invuln__remaining_frames != 0) ex_invuln__remaining_frames--;
    }
    float GAME_SPEED = 1.0f;
    if ((laser_inf_current_length < inner.laser_new_arg2) && (laser_inf_current_length = speed * GAME_SPEED + laser_inf_current_length, inner.laser_new_arg2 < laser_inf_current_length)) {
        laser_inf_current_length = inner.laser_new_arg2;
    }

    angle = inner.laser_inf_angular_velocity * GAME_SPEED + angle;
    math::angle_normalize(angle);

    //if (inner.flags & 1) {
        //pzVar10 = NULL;
        //if (ENEMY_MANAGER_PTR->boss_ids[0] != 0) {
            //pzVar4 = ENEMY_MANAGER_PTR->active_enemy_list_head;
            //while ((pzVar4 != NULL && (pzVar10 = pzVar4->entry, pzVar10->enemy_id != ENEMY_MANAGER_PTR->boss_ids[0]))) {
                //pzVar4 = pzVar4->next;
            //}
            //if (pzVar10 != NULL) {
                //pzVar10 = EnemyManager::get_boss_enemy_full(0);
                //*(undefined8*)&laser_offset = *(undefined8*)&(pzVar10->data).final_pos.pos;
                //laser_offset.z = (pzVar10->data).final_pos.pos.z;
            //}
        //}
    //}

    laser_offset += inner.speed * GAME_SPEED;
    switch (__field_10__set_to_3_by_ex_delete) {
    case 3:
        if (__timer_18 >= inner.laser_time_start) {
            __timer_18 = 0;
            __field_10__set_to_3_by_ex_delete = 4;
        }
        break;
    case 4:
        if (__timer_18 < inner.laser_trans_1) {
            laser_st_width = (__timer_18 * inner.laser_new_arg4) / (float)inner.laser_trans_1;
            break;
        }
        __timer_18 = 0;
        __field_10__set_to_3_by_ex_delete = 2;
        laser_st_width = inner.laser_new_arg4;
    case 2:
        if (__timer_18 >= inner.laser_duration) {
            __timer_18 = 0;
            __field_10__set_to_3_by_ex_delete = 5;
            if (__timer_18 >= inner.laser_trans_2) {
                return 1;
            }
            laser_st_width = inner.laser_new_arg4 - (__timer_18 * inner.laser_new_arg4) / (float)inner.laser_trans_2;
        }
        break;
    case 5:
        if (__timer_18 >= inner.laser_trans_2) {
            return 1;
        }
        laser_st_width = inner.laser_new_arg4 - (__timer_18 * inner.laser_new_arg4) / (float)inner.laser_trans_2;
    }

    if (((__field_10__set_to_3_by_ex_delete == 4) || (__field_10__set_to_3_by_ex_delete == 2)) && (16.0 < laser_inf_current_length)) {
        float col_w = laser_st_width;
        if (32.0 <= laser_st_width) {
            col_w -= (laser_st_width + 16.0) / 3.0;
        } else {
            col_w = laser_st_width * 0.5;
        }
        int collide = 0; // = enemy_collide_player_rectangle(puVar11, laser_offset, angle, 0, col_w, laser_inf_current_length * 0.9);
        if (collide == 1) {
            cancel_as_bomb_rectangle({Globals::get()->playerX, Globals::get()->playerY, 0}, {32, 32, 0}, 0, 0, 1);
        } else if (collide == 2) {
            // TODO graze
            //if (__timer_2c % 3 == 0) {
                //fVar15 = math_wrap_angle_402920(angle + 1.570796);
                //FUN_00403bf0(&local_20, &local_1c, angle, (PLAYER_PTR->inner).pos.x, (PLAYER_PTR->inner).pos.y, fVar15);
                //local_18._0_8_ = CONCAT44(local_1c, local_20);
                ////Player::do_graze(&local_18);
            //}
            __timer_2c++;
        }
    }

    auto spr = vm1.getSprite();
    vm1.bitflags.scaled = true;
    //vm1.scale.current.x = vm1.scale.goal.x = laser_st_width / spr.w;
    vm1.scale.x = laser_st_width / spr.w;
    //vm1.scale.current.y = vm1.scale.goal.y = laser_inf_current_length / spr.h;
    vm1.scale.y = laser_inf_current_length / spr.h;
    vm1.update();
    if (__field_7c__sometimes_0p01_or_0f == 0.0) vm2.update();
    return 0;
}

int LaserInfinite::on_draw()
{
    float zrot = angle + 1.570796;
    math::angle_normalize(zrot);
    //vm1.rotation.current.z = vm1.rotation.goal.z = zrot;
    vm1.rotation.z = zrot;
    vm1.bitflags.rotated = true;
    vm1.pos = laser_offset;
    vm1.draw();
    if (__field_7c__sometimes_0p01_or_0f == 0.0) {
        vm2.pos = laser_offset;
        vm2.draw();
    }
    return 0;
}

int LaserInfinite::cancel(int param_2, int as_bomb)
{
  if (as_bomb && ex_invuln__remaining_frames != 0) return 0;

    glm::vec3 inc = {math::lengthdir_vec(8.f, angle), 0.f};
    glm::vec3 p = laser_offset + inc;
    inc *= 2;
    int i = 0;
    for (; (i+1)*16.f < laser_inf_current_length; i++) {
        if (-192.0 < p.x + 16.0 && p.x - 16.0 < 192.0 && 0.0 < p.y + 16.0 && p.y - 16.0 < 448.0) {
            if (bullet_type < 18 || bullet_type == 34 || bullet_type == 38) {
                //BULLET_MANAGER_PTR->bullet_anm->__field_134__some_kind_of_counter++;
                //vm = AnmManager::allocate_vm();
                //anm_init_copy_vm_from_loaded(BULLET_MANAGER_PTR->bullet_anm,vm,inner.color * 2 + 0xd1);
                auto vm = AnmManagerN::getVM(AnmManagerN::SpawnVM(7, inner.color * 2 + 0xd1));
                vm->bitflags.randomMode=1;
                vm->entity_pos = p;
                vm->update();
                //vm->mode_of_create_child = 0;
            }
            else if (bullet_type < 32) {
                //BULLET_MANAGER_PTR->bullet_anm->__field_134__some_kind_of_counter++;
                //vm = AnmManager::allocate_vm();
                //anm_init_copy_vm_from_loaded(BULLET_MANAGER_PTR->bullet_anm,vm,inner.color * 2 + 0x101);
                auto vm = AnmManagerN::getVM(AnmManagerN::SpawnVM(7, inner.color * 2 + 0x101));
                vm->bitflags.randomMode=1;
                vm->entity_pos = p;
                vm->update();
                //vm->mode_of_create_child = 0;
            }
            else if (bullet_type < 34) {
                //BULLET_MANAGER_PTR->bullet_anm->__field_134__some_kind_of_counter++;
                //vm = AnmManager::allocate_vm();
                //anm_init_copy_vm_from_loaded(BULLET_MANAGER_PTR->bullet_anm,vm,inner.color * 2 + 0x119);
                auto vm = AnmManagerN::getVM(AnmManagerN::SpawnVM(7, inner.color * 2 + 0x119));
                vm->bitflags.randomMode=1;
                vm->entity_pos = p;
                vm->update();
                //vm->mode_of_create_child = 0;
            }
            //if (param_2 != 0 && -192.0 < p.x + 32.0 && p.x - 32.0 < 192.0 && 0.0 < p.y + 32.0 && p.y - 32.0 < 448.0) {
                //BULLET_MANAGER_PTR->__related_to_cancels++
                //ItemManager::spawn_item
                          //(ITEM_MANAGER_PTR,9,&p,in_stack_ffffffa0,Rng::randf_neg_1_to_1(&REPLAY_SAFE_RNG) * 0.1745329 - 1.570796,2.2,0,extraout_ECX,0xffffffff);
            //}
        }
        p += inc;
    }
    __field_10__set_to_3_by_ex_delete = 1;
    return i;
}

int LaserInfinite::cancel_as_bomb_rectangle(glm::vec3 p1, glm::vec3 p2, float rot, int item, int as_bomb)
{
    char canceled_node[260] = {};

    if (as_bomb && ex_invuln__remaining_frames > 0) return 0;

    float aa = angle - rot;
    math::angle_normalize(aa);
    glm::vec3 rect_inc = {math::lengthdir_vec(8.0, aa), 0.f};
    glm::vec3 rect_pos = {
        cos(-rot) * (laser_offset.x - p1.x) - sin(-rot) * (laser_offset.y - p1.y) + rect_inc.x,
        sin(-rot) * (laser_offset.x - p1.x) + cos(-rot) * (laser_offset.y - p1.y) + rect_inc.y,
        0.f
    };
    glm::vec3 inc = {math::lengthdir_vec(8.0, angle), 0.f};
    glm::vec3 p = laser_offset + inc;
    rect_inc *= 2;
    inc*=2;
    int i = 0;
    for (; (i+1)*16.f < laser_inf_current_length; i++) {
        if (-p2.x * 0.5 <= rect_pos.x && rect_pos.x <= p2.x * 0.5 && -p2.y * 0.5 <= rect_pos.y && rect_pos.y <= p2.y * 0.5) {
            canceled_node[i] = 1;
            //gen_items_from_et_cancel(p, item);
            if (bullet_type < 0x12 || bullet_type == 0x22 || bullet_type == 0x26) {
                //BULLET_MANAGER_PTR->bullet_anm->__field_134__some_kind_of_counter++;
                //vm = AnmManager::allocate_vm();
                //anm_init_copy_vm_from_loaded(BULLET_MANAGER_PTR->bullet_anm, vm, inner.color * 2 + 0xd1);
                auto vm = AnmManagerN::getVM(AnmManagerN::SpawnVM(7, inner.color * 2 + 0xd1));
                vm->bitflags.randomMode = 1;
                vm->entity_pos = p;
                vm->update();
                //vm->mode_of_create_child = 0;
                //if (EffectManager::get_next_index() != -1) EFFECT_MANAGER_PTR->anm_ids[EffectManager::get_next_index()].value = idofanm;
            } else if (bullet_type < 0x20) {
                //BULLET_MANAGER_PTR->bullet_anm->__field_134__some_kind_of_counter++;
                //vm = AnmManager::allocate_vm();
                //anm_init_copy_vm_from_loaded(BULLET_MANAGER_PTR->bullet_anm, vm, inner.color * 2 + 0x101);
                auto vm = AnmManagerN::getVM(AnmManagerN::SpawnVM(7, inner.color * 2 + 0x101));
                vm->bitflags.randomMode = 1;
                vm->entity_pos = p;
                vm->update();
                //vm->mode_of_create_child = 0;
                //if (EffectManager::get_next_index() != -1) EFFECT_MANAGER_PTR->anm_ids[EffectManager::get_next_index()].value = idofanm;
            } else if (bullet_type < 0x22) {
                //BULLET_MANAGER_PTR->bullet_anm->__field_134__some_kind_of_counter++;
                //vm = AnmManager::allocate_vm();
                //anm_init_copy_vm_from_loaded(BULLET_MANAGER_PTR->bullet_anm, vm, inner.color * 2 + 0x119);
                auto vm = AnmManagerN::getVM(AnmManagerN::SpawnVM(7, inner.color * 2 + 0x119));
                vm->bitflags.randomMode = 1;
                vm->entity_pos = p;
                vm->update();
                //vm->mode_of_create_child = 0;
                //if (EffectManager::get_next_index() != -1) EFFECT_MANAGER_PTR->anm_ids[EffectManager::get_next_index()].value = idofanm;
            }
        }
        p += inc;
        rect_pos += rect_inc;
    }
    if (i != 0) {
        int j = 0;
        if (0 < i) {
            do {
                if (canceled_node[j] == 0)
                    break;
                j++;
            } while (j < i);
            if (j != 0) {
                laser_inf_current_length = 0.f;
                while (j < i) {
                    if (canceled_node[j] == 0) {
                        float len = 0.0;
                        do {
                            if (canceled_node[j])
                                break;
                            j++;
                            len++;
                        } while (j < i);
                        LaserLineInner_t inner_;
                        inner_.start_pos = laser_offset + inc * (float)j;
                        inner_.ang_aim = angle;
                        inner_.laser_new_arg_2 = len * 16.f;
                        inner_.laser_new_arg_1 = len * 16.f;
                        inner_.laser_new_arg_3 = inner.laser_new_arg2 - j * 16.f;
                        inner_.laser_new_arg_4 = laser_st_width;
                        inner_.spd_1 = 8.f;
                        inner_.bullet_type = inner.type;
                        inner_.bullet_color = inner.color;
                        inner_.flags = inner_.flags ^ ((uint)inner.flags >> 1 ^ inner_.flags) & 1;
                        allocate_new_laser(0, &inner_);
                    } else {
                        j++;
                    }
                }
                return 1;
            }
        }
    }
    return 1;
}
