#include "Player.h"
#include "Laser/LaserManager.h"
#include "BulletManager.h"
#include "Input.h"
#include "EnemyManager.h"
#include <math/Random.h>
#include "AnmOpener/AnmManager.h"
#include "Hardcoded.h"

Player* PLAYER_PTR = nullptr;

Player::Player()  // at th17.exe:4463f0
{
    PLAYER_PTR = this;
    // load player anm file
    AnmManager::LoadFile(9, PLAYERS[0]["shottypes"][0]["anm"].asString());
    //playerAnm = AnmManager::preload_anm(9,plAnmfilenames[GLOBALS.inner.CHARACTER]);
    //if (!playerAnm) ; // log info

    // load sht file
    if (ALREADY_LOADED_SHTFILE) {
        sht_file = ALREADY_LOADED_SHTFILE;
        ALREADY_LOADED_SHTFILE = nullptr;
    } else {
        sht_file = open_sht(PLAYERS[0]["shottypes"][0]["sht"].asString());//SHT_FILENAMES[GLOBALS.inner.GOAST + GLOBALS.inner.CHARACTER * 3]);
    }

    // update funcs
    on_game_tick = new UpdateFunc([this]() { return _on_tick(); });
    on_game_tick->flags &= 0xfffffffd;
    UPDATE_FUNC_REGISTRY->register_on_tick(on_game_tick, 23);
    on_draw = new UpdateFunc([this]() { return _on_draw(); });
    on_draw->flags &= 0xfffffffd;
    UPDATE_FUNC_REGISTRY->register_on_draw(on_draw, 29);

    // init vm
    //playerAnm->anm_init_copy_vm_from_loaded(&vm,0);
    vm(AnmManager::getLoaded(9)->getPreloaded(0));
    vm.parent = nullptr;
    // vm.__root_vm__or_maybe_not = nullptr;
    vm.update();

    //for (int i = 0; i < 4; i++) {
    //    *(int *)((int)__unknown_scaled_pos_array + 0x10+4*i + -0x5c) = (int)(*(float *)(sht_file + 0x10+4*i) * 128.0);
    //}

    sht_file->header.SA_power_divisor = 100;
    //GLOBALS.inner.MAXIMUM_POWER = sht_file->header.SA_power_divisor * sht_file->header.pwr_lvl_cnt;
    //GLOBALS.inner.POWER_PER_LEVEL = this->sht_file->header.SA_power_divisor;

    speed_focus = 200;
    speed_focus_diag = 200/1.42;
    speed_unfocus = 500;
    speed_unfocus_diag = 500/1.42;

    //sht_file->header.hitbox_u = *(undefined4 *)(&DAT_004a2118 + GLOBALS.inner.CHARACTER * 4);
    //sht_file->header.itembox_u = *(undefined4 *)(&DAT_004a2108 + GLOBALS.inner.CHARACTER * 4);
    //sht_file->header.grazebox_u = *(undefined4 *)(&DAT_004a20f8 + GLOBALS.inner.CHARACTER * 4);

    hurtbox_halfsize = {sht_file->header.hitbox_u * 0.5f, sht_file->header.hitbox_u * 0.5f, 5.f};
    item_attract_box_unfocused_halfsize = {sht_file->header.itembox_u * 0.5f, sht_file->header.itembox_u * 0.5f, 5.f};
    //item_attract_box_focused_halfsize.y = *(float *)(&DAT_004a20e8 + GLOBALS.inner.CHARACTER * 4) * 0.5;
    //item_attract_box_focused_halfsize.x = *(float *)(&DAT_004a20e8 + GLOBALS.inner.CHARACTER * 4) * 0.5;
    item_attract_box_focused_halfsize.z = 5.0;

    hurtbox.min_pos = inner.pos - hurtbox_halfsize;
    hurtbox.max_pos = inner.pos + hurtbox_halfsize;
    item_collect_box.min_pos = inner.pos - item_attract_box_unfocused_halfsize;
    item_collect_box.max_pos = inner.pos + item_attract_box_unfocused_halfsize;
    item_attract_box_focused.min_pos = inner.pos - item_attract_box_focused_halfsize;
    item_attract_box_focused.max_pos = inner.pos + item_attract_box_focused_halfsize;
    item_attract_box_unfocused.min_pos = inner.pos - item_attract_box_focused_halfsize;
    item_attract_box_unfocused.max_pos = inner.pos + item_attract_box_focused_halfsize;

    flags &= 0xfffffffb;

    for (int i = 0; i < 0x100; i++) {
        inner.bullets[i].id = i;
    }

}


void FUN_00449630(PlayerInner_t *inner)
{
  int pow = 1;//GLOBALS.inner.CURRENT_POWER / GLOBALS.inner.POWER_PER_LEVEL;
  //inner[1].__time_in_stage__copy_3c.control = pow;
  if (true) {//GLOBALS.inner.CURRENT_POWER < GLOBALS.inner.MAXIMUM_POWER) {
      for (int i = 0; i < 4; i++) {
          AnmManager::interrupt_tree(inner->options[i].anmId2, 1);
          inner->options[i].anmId2 = 0;
      }
  } else {
    if (0 < pow) {
      for (int i = 0; i < pow; i++) {
        if (auto vm = AnmManager::getVM(inner->options[i].anmId2); vm && !vm->bitflags.f534_27_31) {
          vm->bitflags.activeFlags = 0b01;
          auto child = vm->childrens->next;
          while (child) {
              child->value->destroy();
              child = child->next;
          }
        }
        inner->options[i].anmId2 = AnmManager::SpawnVM(9, 10); //DAT_004a2090[GLOBALS.inner.CHARACTER * 4]);
        auto vm = AnmManager::getVM(inner->options[i].anmId2);
        //PLAYER_PTR->playerAnm->__field_134__some_kind_of_counter++;
        //auto vm = AnmManager::allocate_vm();
        //PLAYER_PTR->playerAnm->anm_init_copy_vm_from_loaded(vm, DAT_004a2090[GLOBALS.inner.CHARACTER * 4]);
        //vm->mode_of_create_child = 2;
        //inner->options[i].anmId2 = AnmManager::insert_in_world_list_front(&zStack_1c,vm);
        vm->bitflags.randomMode = 0b1;
        vm->entity_pos = {0,-32,0};
        vm->rotation.z = 0.f;
        vm->update();
      }
    }
  }
  if (inner->__some_index == pow) return;

  //fVar10 = 0.0;
  //local_30 = 0.0;
  //pzVar14 = inner->options;
  if (0 < pow) {
    //piVar18 = &inner->options[0].scaled_cur_pos.y;
    //local_2c = inner->options[0].scaled_prefered_pos_rel_to_player;
    do {
      //((zInt2 *)(piVar18 + -1))->x = (int32_t)(PLAYER_PTR->inner).integer_pos.x;
      //*piVar18 = (int32_t)inner->integer_pos.y;
      if (auto vm = AnmManager::getVM(0/*piVar18[0x14]*/); vm && !vm->bitflags.f534_27_31) {
          vm->bitflags.activeFlags = 0b01;
          auto child = vm->childrens->next;
          while (child) {
              child->value->destroy();
              child = child->next;
          }
      }
      //piVar18[0x14] = 0;
      //piVar18[0x1c] = (int32_t)local_30;
      //piVar18[1] = (int)(PLAYER_PTR->sht_file->option_pos[(int)(&FLOAT_004b673c)[(GLOBALS.inner.GOAST + GLOBALS.inner.CHARACTER * 3) * 8 + pow] + (int)local_30].x * 128.f);
      //piVar18[2] = (int)(PLAYER_PTR->sht_file->option_pos[(int)(&FLOAT_004b673c)[(GLOBALS.inner.GOAST + GLOBALS.inner.CHARACTER * 3) * 8 + pow] + (int)local_30].y * 128.f);
      //piVar18[3] = (int)(PLAYER_PTR->sht_file->option_pos[(int)(&FLOAT_004b673c)[(GLOBALS.inner.GOAST + GLOBALS.inner.CHARACTER * 3) * 8 + pow] + (int)local_30 + 10].x * 128.f);
      //piVar18[4] = (int)(PLAYER_PTR->sht_file->option_pos[(int)(&FLOAT_004b673c)[(GLOBALS.inner.GOAST + GLOBALS.inner.CHARACTER * 3) * 8 + pow] + (int)local_30 + 10].y * 128.f);
      //pzVar11 = local_2c + 1;
      //if (!inner->focusing) pzVar11 = local_2c;
      //local_2c[-2].y = (pzVar11->y + inner->integer_pos.y);
      //local_2c[-2].x = inner->integer_pos.x + pzVar11->x;
      //*piVar18 = (pzVar11->y + inner->integer_pos.y);
      //((zInt2 *)(piVar18 + -1))->x = inner->integer_pos.x + pzVar11->x;
      //piVar18[0x14] = AnmManager::SpawnVM(9, 10); //DAT_004a2090[GLOBALS.inner.CHARACTER * 4]);
      AnmVM* vm = nullptr;//AnmManager::getVM(piVar18[0x14]);
      //PLAYER_PTR->playerAnm->__field_134__some_kind_of_counter++;
      //auto vm = AnmManager::allocate_vm();
      //PLAYER_PTR->playerAnm->anm_init_copy_vm_from_loaded(vm, DAT_004a2090[GLOBALS.inner.CHARACTER * 4]);
      //vm->mode_of_create_child = 0;
      //AnmManager::insert_in_world_list_back(piVar18[0x14],vm);
      vm->bitflags.randomMode = 0b1;
      vm->bitflags.originMode = 0b01;
      vm->layer = 14;
      vm->entity_pos = {0,-32,0};
      vm->rotation.z = 0.f;
      vm->update();

      //pzVar14->flags = 2;

      //if (GLOBALS.inner.CHARACTER == 2) {
      //  piVar18[0x1f] = (int32_t)FUN_00449b10;
      //}
      //pzVar14++;
      //local_2c = (zInt2 *)((int)(local_2c + 0x1c) + 4);
      //piVar18 += 0x39;
      //local_30++;
    } while (false);//(int)fVar10 < GLOBALS.inner.CURRENT_POWER / GLOBALS.inner.POWER_PER_LEVEL);
    if (3 < 0)/*(uint)fVar10)*/ goto LAB_00449ad2;
  }

  //iVar16 = 4 - (int)fVar10;
  //do {
  //  pzVar14->flags = 0;
  //  AnmManager::interrupt_tree(pzVar14->anmId1,1);
  //  pzVar14 = pzVar14 + 1;
  //  iVar16 += -1;
  //} while (iVar16 != 0);

LAB_00449ad2:
  inner->__some_index = pow;
  inner->options[0].move_instantly = 1;
  inner->options[1].move_instantly = 1;
  inner->options[2].move_instantly = 1;
  inner->options[3].move_instantly = 1;
  return;
}

Player::~Player()
{
    if (on_game_tick)
        UPDATE_FUNC_REGISTRY->unregister(on_game_tick);
    if (on_draw)
        UPDATE_FUNC_REGISTRY->unregister(on_draw);
    PLAYER_PTR = nullptr;

    //if (!(GLOBALS.FLAGS & 1)) {
        //AnmManager::_unload_file(9);
        if (sht_file) {
            delete sht_file;
            sht_file = NULL;
        }
        ALREADY_LOADED_SHTFILE = NULL;
    //}
    //else {
    //    //AnmManager::remove_anm_of_file(playerAnm);
    //    ALREADY_LOADED_SHTFILE = sht_file;
    //}
}

int Player::_on_tick()
{
    INPUT_STRUCT.Update();
    // movement state
    switch (inner.state) {
        case 0: // Respawning
            inner.integer_pos.y = 61440 - (inner.time_in_state * 512) / 3;
            inner.pos.y = inner.integer_pos.y * 0.0078125;
            inner.options[0].move_instantly = true;
            inner.options[1].move_instantly = true;
            inner.options[2].move_instantly = true;
            inner.options[3].move_instantly = true;

            for (int i = 0; i < 33; i++)
                unknown_scaled_pos_array[i] = inner.integer_pos;

            if (inner.time_in_state < 30) {
                LASER_MANAGER_PTR->cancel_in_radius(death_pos,0,1,((float)inner.time_in_state * 512.0) / 30.0 + 64.0);
                LASER_MANAGER_PTR->cancel_in_radius(death_pos,0,0,((float)inner.time_in_state * 512.0) / 120.0 + 16.0);
            } else {
                if (false) {//if ((((uint8_t)SPELLCARD_PTR->flags & 1) != 0) && (SPELLCARD_PTR->field25_0x74 == 100)) {
                    //BulletManager::GetInstance()->cancel_radius(&inner.pos,0,128.0);
                }
                //BulletManager::GetInstance()->cancel_radius_as_bomb(inner.pos,0,640.0);
                LASER_MANAGER_PTR->cancel_in_radius(inner.pos,0,0,640.0);
            }

            if (inner.time_in_state >= 60) {
                inner.state = 1;
                inner.time_in_state = 0;
            }
            else break;
            [[fallthrough]];
        case 1: // Normal
            if (false) {// if (BOMB_PTR && (GLOBALS.inner.CURRENT_BOMBS > 0) && player_is_trying_to_bomb() && ((byte)INPUT_STRUCT.rising_edge & 2)) {
                //do_bomb();
            }
            move();
            break;
        case 4: // Deathbomb window
            if (inner.time_in_state > 7) {
                die();
            } else {
                //if ((GLOBALS.inner.HYPER_FLAGS & 2U) && !(GLOBALS.inner.HYPER_FLAGS & 4U)) {
                //    FUN_0040f6d0(GOAST_MANAGER_PTR,1);
                //    inner.time_in_state = 60;
                //}
                //if (BOMB_PTR && (GLOBALS.inner.CURRENT_BOMBS > 0) && player_is_trying_to_bomb() && ((byte)INPUT_STRUCT.rising_edge & 2)) {
                //    do_bomb();
                //    inner.time_in_state = 60;
                //}
                break;
            }
            [[fallthrough]];
        case 2: // Dying
            if (inner.time_in_state == 3) {
                // GLOBALS.inner.CURRENT_POWER -= GLOBALS.inner.POWER_PER_LEVEL / 2;
                // if (GLOBALS.inner.CURRENT_POWER < GLOBALS.inner.POWER_PER_LEVEL) {
                //     GLOBALS.inner.CURRENT_POWER = GLOBALS.inner.POWER_PER_LEVEL;
                // }
                for (int i = 0; i < 7; i++) {
                    //ItemManager::spawn_item
                            //(1,inner.pos,in_stack_ffffff98,(((i * 3.141593) / 28.0 + math::point_direction(0,0,inner.pos.x,inner.pos.y-224)) - 0.3926991,3.0,0,fVar17,-1);
                }

                //ItemManager::spawn_item
                            //(GLOBALS.inner.GOAST + 16,inner.pos + glm::vec3(-16,-96,0);,in_stack_ffffff98,-1.570796,2.2,0,fVar17,-1);
                //ItemManager::spawn_item
                            //(GLOBALS.inner.GOAST + 16,inner.pos + glm::vec3(+16,-96,0);,in_stack_ffffff98,-1.570796,2.2,0,fVar17,-1);

                //FUN_00449630(&inner);
            }
            if (inner.time_in_state > 30) {
                if (false) {//if (GLOBALS.inner.CURRENT_LIVES < 0 && inner.time_in_state == 31) {
                    //if (REPLAY_MANAGER_PTR->field3_0xc != 1) {
                    //    FUN_004447e0();
                    //}
                    inner.time_in_state++;
                }
                else {
                    //GAME_SPEED = 1.0;
                    inner.state = 0;
                    inner.iframes = 280;
                    inner.time_in_state = 0;
                    //FUN_00449c80(this,inner.pos, 30, 150, 32.f, 16.f);
                    //set_nb_bomb(GLOBALS.inner.field30_0x78);
                    death_pos = inner.pos;
                    inner.integer_pos.x = 0;
                    inner.integer_pos.y = 480 * 128.0;
                    inner.pos.x = 0;
                    inner.pos.y = 480 * 128.0 * 0.0078125;
                    inner.options[0].move_instantly = true;
                    inner.options[1].move_instantly = true;
                    inner.options[2].move_instantly = true;
                    inner.options[3].move_instantly = true;
                }
            }
            break;
        case 3: // IDK
            if (inner.time_in_state == 15) {
                LASER_MANAGER_PTR->cancel_all(1);
            }
    }

    // damage source
    for (int i = 0; i < 256; i++) {
        if (inner.damage_sources[i].flags & 1) {
            inner.damage_sources[i].field_0x1c.update_velocities();
            inner.damage_sources[i].field_0x1c.update_position();
            inner.damage_sources[i].field_0x4 += inner.damage_sources[i].field_0x8;
            inner.damage_sources[i].field_0xc += inner.damage_sources[i].field_0x10;
            math::angle_normalize(inner.damage_sources[i].field_0xc);
            inner.damage_sources[i].field_0x84 = 0;
            inner.damage_sources[i].field_0x64--;
            if (inner.damage_sources[i].field_0x64 <= 0) {
                inner.damage_sources[i].flags &= 0xfffffffe;
            }
        }
    }

    // speed boost anim ?
    if (inner.iframes < 1) {
        vm.bitflags.colmode = 0b00;
        if (!(flags & 0x20)) {
            if (speed_multiplier > 1.01) {
                if (abs(inner.time_in_state) % 8 < 4) {
                    vm.color2 = {255,255,0,255};
                    vm.bitflags.colmode = 0b01;
                }
                //playerAnm->create_4112b0(&got, {17, 19, 21}[GLOBALS.inner.CHARACTER],inner.pos,0,-1,0);
                //playerAnm->set_sprite(AnmManager::get_vm_with_id(got),vm.sprite_id);
            }
        }
        else {
            if (abs(inner.time_in_state) % 8 < 4) {
                vm.color2 = {255,0,0,255};
                vm.bitflags.colmode = 0b01;
            }
            //playerAnm->create_4112b0(&got,{17, 19, 21}[GLOBALS.inner.CHARACTER],inner.pos,0,-1,0);
            //playerAnm->set_sprite(AnmManager::get_vm_with_id(got),vm.sprite_id);
            //AnmManager::get_vm_with_id(got).color1 = {255,0,0,255};
        }
    }
    else inner.iframes--;

    speed_multiplier = 1.0;
    __could_be_subsun_pull_direction = {0,0,0};
    vm.update();

    // scale boxes
    if (!(flags & 0x10)) {
        vm.bitflags.scaled = 0b1;
        vm.scale = {1,1};
        hurtbox.min_pos = inner.pos - hurtbox_halfsize;
        hurtbox.max_pos = inner.pos + hurtbox_halfsize;
        item_collect_box.min_pos = inner.pos - item_attract_box_unfocused_halfsize * 0.5f;
        item_collect_box.max_pos = inner.pos + item_attract_box_unfocused_halfsize * 0.5f;
        item_attract_box_focused.min_pos = inner.pos - item_attract_box_focused_halfsize;
        item_attract_box_focused.max_pos = inner.pos + item_attract_box_focused_halfsize;
        item_attract_box_unfocused.min_pos = inner.pos - item_attract_box_unfocused_halfsize;
        item_attract_box_unfocused.max_pos = inner.pos + item_attract_box_unfocused_halfsize;
    } else {
        if (player_scale_i.end_time != 0) {
            player_scale__requires_flag_0x10__from_ddc = player_scale_i.step();
        }

        if ((player_scale_i.end_time == 0) || (inner.time_in_state % 3))
            vm.scale.x = vm.scale.y = player_scale__requires_flag_0x10__from_ddc;
        else vm.scale = {1.f, 1.f};
        vm.bitflags.scaled = 0b1;

        hurtbox.min_pos = inner.pos - hurtbox_halfsize * player_scale__requires_flag_0x10__from_ddc;
        hurtbox.max_pos = inner.pos + hurtbox_halfsize * player_scale__requires_flag_0x10__from_ddc;
        item_collect_box.min_pos = inner.pos - item_attract_box_unfocused_halfsize * 0.5f * player_scale__requires_flag_0x10__from_ddc;
        item_collect_box.max_pos = inner.pos + item_attract_box_unfocused_halfsize * 0.5f * player_scale__requires_flag_0x10__from_ddc;
        item_attract_box_focused.min_pos = inner.pos - item_attract_box_focused_halfsize * player_scale__requires_flag_0x10__from_ddc;
        item_attract_box_focused.max_pos = inner.pos + item_attract_box_focused_halfsize * player_scale__requires_flag_0x10__from_ddc;
        item_attract_box_unfocused.min_pos = inner.pos - item_attract_box_unfocused_halfsize * player_scale__requires_flag_0x10__from_ddc;
        item_attract_box_unfocused.max_pos = inner.pos + item_attract_box_unfocused_halfsize * player_scale__requires_flag_0x10__from_ddc;
    }

    inner.time_in_state++;
    inner.time_in_stage++;
    inner.__time_in_stage__copy_3c++;

    if (true) {//if (!GUI_PTR->msg && ENEMY_MANAGER_PTR && ENEMY_MANAGER_PTR->enemy_count_real && !(GAME_THREAD_PTR->flags & 0x4000U) && inner.time_in_stage >= 20 && (~(flags >> 2) & 1) != 0 && !(flags & 0x10U)) {
        check_shoot();
    } else {
        inner.shoot_key_short_timer = -1;
        inner.shoot_key_long_timer = -1;

        // looks like a timer reset
        // Maybe youmu related
        //field37_0x19040 = 0;
        //field38_0x19044 = 0;
        //if (!(field46_0x19090 & 1)) {
        //    field45_0x1908c = 0;
        //    field46_0x19090 |= 1;
        //}
        //field43_0x19084 = 0;
        //field44_0x19088 = 0;
        //field42_0x19080 = -1;

        //AnmManager::get_vm_with_id(field47_0x19094).delete();
        //field47_0x19094 = 0;

        //for (int i = 0; i < 12; i++) {
        //    if ((int)(&DAT_0052480c)[i] < 0) {
        //        (&DAT_0052480c)[i] = 31;
        //        (&DAT_0052483c)[i] = -1;
        //        break;
        //    }
        //    if ((&DAT_0052480c)[i] == 31) {
        //        (&DAT_0052483c)[i] = -1;
        //        break;
        //    }
        //}
        //for (int i = 0; i < 12; i++) {
        //    if ((int)(&DAT_0052480c)[i] < 0) {
        //        (&DAT_0052480c)[i] = 55;
        //        (&DAT_0052483c)[i] = -1;
        //        break;
        //    }
        //    if ((&DAT_0052480c)[i] == 55) {
        //        (&DAT_0052483c)[i] = -1;
        //        break;
        //    }
        //}
    }

    for (int i = 0; i < 256; i++) {
        if (inner.bullets[i].active) inner.bullets[i].update();
    }

    //if (auto vm = AnmManager::get_vm_with_id(field47_0x19094), vm)
    //    vm->entity_pos = inner.pos;
    //else field47_0x19094 = 0;

    flags &= 0xffffffbf;
    return 1;
}

void Player::check_shoot()
{
    if (inner.state != 1) {
        //field29_0x19040 = 0;
        //*(undefined *)&field30_0x19044 = 0;
        //if (field35_0x19084 == 0) {
        //    return;
        //}
        //FUN_004655d0(0x1e);
        //FUN_004655d0(0x37);
        //AnmVm::delete_by_id(field39_0x19094);
        //field39_0x19094 = 0;
        //if (!(field38_0x19090 & 1)) {
        //    field37_0x1908c = 0;
        //    field38_0x19090 |= 1;
        //}
        //field35_0x19084 = 0;
        //field36_0x19088 = 0;
        //field34_0x19080 = 0xffffffff;
        return;
    }

    //if (GLOBALS.inner.CHARACTER == 2) {
    //    uVar3 = (uint)GLOBALS.inner.HYPER_FLAGS >> 1 & 1;
    //    if ((uVar3 == 0) || (GLOBALS.inner.field44_0xe0 != 3)) {
    //    if ((uVar3 == 0) || (GLOBALS.inner.field44_0xe0 != 1)) {
    //        if ((this->inner).focusing == 0) goto LAB_0044ab18;
    //        if ((((byte)INPUT_STRUCT.input & 1) != 0) && (-1 < (int)this->field35_0x19084)) {
    //        if (this->field35_0x19084 == 0) {
    //            SoundManager::play_sound_at_position(0x1e);
    //            if (GLOBALS.inner.GOAST == 0) {
    //            iVar6 = 0x14;
    //            }
    //            else {
    //            iVar6 = 0x13;
    //            }
    //            pzVar4 = AnmLoaded::create_4112b0
    //                            (PLAYER_PTR->playerAnm,&local_14,iVar6,&(this->inner).pos,0,-1,
    //                                (zAnmId)0x0);
    //            this->field39_0x19094 = pzVar4->value;
    //        }
    //        Timer::increment((zTimer *)&this->field34_0x19080);
    //        FUN_004655a0(0x1e);
    //        FUN_004655a0(0x37);
    //        if (this->field35_0x19084 != 0x3c) {
    //            return 0;
    //        }
    //        FUN_004655d0(0x1e);
    //        SoundManager::play_sound_at_position(0x37);
    //        AnmManager::interrupt_tree(this->field39_0x19094,2);
    //        return 0;
    //        }
    //        if (0 < (int)this->field35_0x19084) {
    //        FUN_004655d0(0x1e);
    //        FUN_004655d0(0x37);
    //        AnmVm::delete_by_id((zAnmId)this->field39_0x19094);
    //        pzVar5 = (zTimer *)&this->field34_0x19080;
    //        this->field39_0x19094 = 0;
    //        if (0x3b < (int)this->field35_0x19084) goto LAB_0044a95f;
    //        goto LAB_0044ab06;
    //        }
    //        if (-1 < (int)this->field35_0x19084) {
    //        return 0;
    //        }
    //    }
    //    else if (-1 < (int)this->field35_0x19084) {
    //        if (((byte)INPUT_STRUCT.input & 1) == 0) {
    //        return 0;
    //        }
    //        AnmVm::delete_by_id((zAnmId)this->field39_0x19094);
    //        this->field39_0x19094 = 0;
    //    LAB_0044a95f:
    //        Timer::set_value((zTimer *)&this->field34_0x19080,-0x28);
    //        fVar1 = (this->inner).pos.z;
    //        *(undefined8 *)&this->field40_0x19098 = *(undefined8 *)&(this->inner).pos;
    //        this->field42_0x190a0 = fVar1;
    //        return 0;
    //    }
    //    local_18 = (this->inner).pos.z;
    //    local_20 = *(undefined8 *)&(this->inner).pos;
    //    FUN_0044a0f0(this,(float *)&this->field40_0x19098);
    //    FUN_0044a710(p1,this->field35_0x19084 + 0x28,0);
    //    FUN_0044a0f0(this,(float *)&local_20);
    //    Timer::increment((zTimer *)&this->field34_0x19080);
    //    pzVar5 = extraout_ECX;
    //    if ((int)this->field35_0x19084 < 0) {
    //        return 0;
    //    }
    //    LAB_0044ab06:
    //    Timer::set_value(pzVar5,0);
    //    return 0;
    //    }
    //    LAB_0044ab18:
    //    if ((int)this->field35_0x19084 < 1) {
    //    if ((int)this->field35_0x19084 < 0) {
    //        local_18 = (this->inner).pos.z;
    //        local_20 = *(undefined8 *)&(this->inner).pos;
    //        iVar2 = (this->inner).focusing;
    //        (this->inner).focusing = 1;
    //        FUN_0044a0f0(this,(float *)&this->field40_0x19098);
    //        FUN_0044a710(p2,this->field35_0x19084 + 0x28,0);
    //        FUN_0044a0f0(this,(float *)&local_20);
    //        (this->inner).focusing = iVar2;
    //        Timer::increment((zTimer *)&this->field34_0x19080);
    //    }
    //    }
    //    else {
    //    FUN_004655d0(0x1e);
    //    FUN_004655d0(0x37);
    //    AnmVm::delete_by_id((zAnmId)this->field39_0x19094);
    //    this->field39_0x19094 = 0;
    //    if ((int)this->field35_0x19084 < 0x3c) {
    //        Timer::set_value((zTimer *)&this->field34_0x19080,0);
    //    }
    //    else {
    //        Timer::set_value((zTimer *)&this->field34_0x19080,-0x28);
    //        fVar1 = (this->inner).pos.z;
    //        *(undefined8 *)&this->field40_0x19098 = *(undefined8 *)&(this->inner).pos;
    //        this->field42_0x190a0 = fVar1;
    //    }
    //    }
    //}

    if (inner.shoot_key_short_timer < 0) {
        if (!(INPUT_STRUCT.input & 1)) goto LAB_0044ac8d;
        inner.shoot_key_short_timer = 0;
    }
    shoot(inner.shoot_key_short_timer,inner.shoot_key_long_timer);
    if (inner.shoot_key_short_timer < 14) {
        inner.shoot_key_short_timer++;
    }
    else if (!(INPUT_STRUCT.input & 1)) {
        inner.shoot_key_short_timer = -1;
    }
    else {
        inner.shoot_key_short_timer -= 14;
    }
    LAB_0044ac8d:
    if (inner.shoot_key_long_timer < 0) {
        return;
    }
    if (0x76 < inner.shoot_key_long_timer) {
        if (INPUT_STRUCT.input & 1) {
            inner.shoot_key_long_timer -= 0x77;
            return;
        }
        inner.shoot_key_long_timer = -1;
        return;
    }
    inner.shoot_key_long_timer++;
}

void Player::shoot(int short_tmr, int long_tmr)
{
    int shooter_set = 0;//(GLOBALS.inner.CURRENT_POWER / GLOBALS.inner.POWER_PER_LEVEL);
    if (inner.focusing) shooter_set += sht_file->header.pwr_lvl_cnt + 1;

    //if ((((GLOBALS.inner.HYPER_FLAGS >> 1 & 1) == 0) || (GLOBALS.inner.field44_0xe0 != 1)) || ((GLOBALS.inner.HYPER_FLAGS & 4U) != 0)) {
    //    if ((((GLOBALS.inner.HYPER_FLAGS >> 1 & 1) != 0) && (GLOBALS.inner.field44_0xe0 == 3)) && ((GLOBALS.inner.HYPER_FLAGS & 4U) == 0)) {
    //        shooter_set = (GLOBALS.inner.CURRENT_POWER / GLOBALS.inner.POWER_PER_LEVEL) + 15;
    //    }
    //}
    //else {
    //    shooter_set = (GLOBALS.inner.CURRENT_POWER / GLOBALS.inner.POWER_PER_LEVEL) + 10;
    //}

    auto shooters = sht_file->shooters[shooter_set];

    for (size_t i = 0; i < shooters.size(); i++) {
        if ((shooters[i].long_fire_rate == 0) ? (short_tmr % shooters[i].fire_rate) == shooters[i].start_delay : (long_tmr % shooters[i].long_fire_rate) == shooters[i].long_start_delay) {
            if (shooters[i].__unknown_21 == 2) {
                //if ((inner.field18_0x187f0[(shooters[i].option < 0x10) ? shooters[i].option & 0xf : shooters[i].option >> 4] != 0) continue;
            }
            for (int j = 0; j < 0x100; j++)
            {
                if (inner.bullets[j].active == 0) {
                    inner.bullets[j].init((shooter_set << 8 | i),short_tmr,inner);
                    break;
                }
            }
        }
    }
}

void PlayerBullet_t::init(int shter, int tmer [[maybe_unused]], PlayerInner_t& inner)
{
    auto shooter = PLAYER_PTR->sht_file->shooters[shter >> 8][shter &0xff];
    active = 1;
    this->shter = shter;
    __field_c = 0;

    damage = shooter.damage;
    hitbox.x = shooter.hitbox.x;
    hitbox.y = shooter.hitbox.y;
    pos={};
    if ((shooter.option & 0xf) == 0) pos.pos = inner.pos;
    else pos.pos = {inner.options[((int)(char)shooter.option & 0xfu) * 0xe4].scaled_cur_pos.x * 0.0078125f,inner.options[((int)(char)shooter.option & 0xfu) * 0xe4].scaled_cur_pos.y * 0.0078125f,0.f};

    if (shooter.__unknown_21 == 2) {
        //inner.field18_0x187f0[(shooter.option < 0x10) ? shooter.option & 0xf : shooter.option >> 4] = GLOBALS.inner.CURRENT_POWER / GLOBALS.inner.POWER_PER_LEVEL;
    }

    pos.speed = shooter.speed;

    pos.angle = shooter.angle;
    if (pos.angle < 1000.0) {
        if (pos.angle < 995.0) math::angle_normalize(pos.angle);
        else if ((shooter.option & 0xf) == 0) math::angle_normalize(pos.angle);
        else {
            pos.angle = inner.options[((int)(char)shooter.option & 0xfU) - 1].shoot_angle_maybe;
            math::angle_normalize(pos.angle);
        }
    } else {
        if ((shooter.option & 0xf) != 0) {
            pos.angle = (Random::Floatm11() * 3.141593) / 12.0 + inner.options[((int)(char)shooter.option & 0xfU) - 1].shoot_angle_maybe;
            math::angle_normalize(pos.angle);
            (this->pos).speed = 2*Random::Floatm11() + shooter.speed;
        } else {
            math::angle_normalize(pos.angle);
        }
    }

    pos.update_velocities();
    pos.pos.x += shooter.offset.x - pos.velocity.x;
    pos.pos.y += shooter.offset.y - pos.velocity.y;

    //PLAYER_PTR->playerAnm->__field_134__some_kind_of_counter++;
    //pzVar5 = AnmManager::allocate_vm();
    //PLAYER_PTR->playerAnm->anm_init_copy_vm_from_loaded(pzVar5,&shooter.anm + 5);
    //pzVar5->bitflags.randomMode = 0b1;
    //pzVar5->entity_pos = pos.pos;
    //pzVar5->update();
    //pzVar5->mode_of_create_child = 0;
    //AnmManager::insert_in_world_list_back(local_30,pzVar5);
    //anmId = pzVar5 ? local_30->value : 0;
    //if (pzVar5->bitflags.autoRotate) {
    //    pzVar5->bitflags.rotated = 0b1;
    //    pzVar5->rotation.z = shooter.angle;
    //}

    // create damageSource
    //damageSourceId = FUN_00449de0((undefined8 *)&this->pos,(this->pos).angle,9999999,damage);
    inner.damage_sources[damageSourceId].field_0x98 = 1;
    inner.damage_sources[damageSourceId].field_0x90 = id;
    inner.damage_sources[damageSourceId].field_0x7c = 10000000;

    flags |= 1;

    if (shooter.on_init && shooter.on_init()) {
        active = 0;
        //AnmVm::delete_by_id(anmId);
        inner.damage_sources[damageSourceId].flags &= 0xfffffffe;
    }

    //if (shooter.sfx_hit >= 0) SoundManager::play_sound_at_position(shooter.sfx_hit);
}

void PlayerBullet_t::update()
{
    auto shooter = PLAYER_PTR->sht_file->shooters[shter >> 8][shter &0xff];
    if (shooter.on_tick && shooter.on_tick()) return;

    pos.update_velocities();
    pos.update_position();

    auto vm = (AnmVM*)nullptr; //AnmManager::get_vm_with_id(anmId);
    if (!vm) {
        active = 0;
        anmId = 0;
        if (damageSourceId) PLAYER_PTR->inner.damage_sources[damageSourceId].flags &= 0xfffffffe;
        return;
    }

    //if (*(char *)(iVar5 + 0x21) != 2) {
    //    AnmVm::write_sprite_corners_2d(vm,&local_44);
    //    if (0xe < (__field_c).current) {
    //    fVar10 = (float)DAT_0052471c;
    //    if ((fVar10 < local_44.x) && (local_44.x < fVar10 + 384.0)) {
    //        if (((float)DAT_00524720 < local_44.y) && (local_44.y < (float)DAT_00524720 + 448.0))
    //        goto LAB_0044af7d;
    //    }
    //    if ((fVar10 < local_38) && (local_38 < fVar10 + 384.0)) {
    //        if (((float)DAT_00524720 < local_34) && (local_34 < (float)DAT_00524720 + 448.0))
    //        goto LAB_0044af7d;
    //    }
    //    if ((fVar10 < local_2c) && (local_2c < fVar10 + 384.0)) {
    //        if (((float)DAT_00524720 < local_28) && (local_28 < (float)DAT_00524720 + 448.0))
    //        goto LAB_0044af7d;
    //    }
    //    if ((fVar10 < local_20) && (local_20 < fVar10 + 384.0)) {
    //        if (((float)DAT_00524720 < local_1c) && (local_1c < (float)DAT_00524720 + 448.0))
    //        goto LAB_0044af7d;
    //    }
    //    AnmVm::delete_by_id((zAnmId)field2_0x8);
    //    field2_0x8 = 0;
    //    field7_0x8c = 0;
    //    if (damageSourceId != 0) {
    //        puVar7 = (uint *)FUN_0040de80(damageSourceId);
    //        *puVar7 = *puVar7 & 0xfffffffe;
    //    }
    //    return;
    //    }
    //}
//LAB_0044af7d:

    if (damageSourceId && (flags & 1)) {
        PLAYER_PTR->inner.damage_sources[damageSourceId - 1].field_0x1c.pos = pos.pos;
        PLAYER_PTR->inner.damage_sources[damageSourceId - 1].field_0x8 = pos.angle;
        //*(undefined4 *)(&PLAYER_PTR->inner.bullets[0xff] + damageSourceId * 0x9c + 0x5c) = hitbox.x;
        //*(undefined4 *)(&PLAYER_PTR->inner.bullets[0xff] + damageSourceId * 0x9c + 0x60) = hitbox.y;
        //(PLAYER_PTR->inner).damage_sources[damageSourceId - 1].field10_0x64.control = field11_0x9c;
    }

    vm->entity_pos = pos.pos;
    if (vm->bitflags.autoRotate) {
        vm->bitflags.rotated = 0b1;
        vm->rotation.z = pos.angle;
    }

    __field_c++;
    return;
}

const int PlayerMovement__dirTable[] = {
     0,  0,
     0, -1,
     0,  1,
    -1,  0,
     1,  0,
    -1, -1,
     1, -1,
    -1,  1,
     1,  1
};

void Player::move_options(PlayerOption_t* options, uint cnt)
{
    for (uint i = 0; i < cnt; i++) {
        auto& o = options[i];
        if (o.flags) {
            if (flags & 2) {
                o.scaled_prefered_pos = inner.integer_pos;
                if (0x1d < __some_counter) {
                    o.flags = 0;
                    AnmManager::interrupt_tree(o.anmId1, 1);
                    AnmManager::interrupt_tree(o.anmId2, 1);
                    continue;
                }
            } else {
                o.scaled_prefered_pos = o.scaled_prefered_pos_rel_to_player[inner.focusing] + inner.integer_pos;
                if (o.__some_code) o.__some_code();
            }

            if (o.move_instantly) {
                o.scaled_cur_pos = o.scaled_prefered_pos;
                o.move_instantly = false;
            } else {
                if (0x1d < __some_other_ctr) {
                    if ((((o.scaled_prefered_pos.x - o.scaled_cur_pos.x) * __some_other_ctr) / 100 == 0) && (((o.scaled_prefered_pos.y - o.scaled_cur_pos.y) * __some_other_ctr) / 100 == 0))
                        o.scaled_cur_pos = o.scaled_prefered_pos;
                    else {
                        o.scaled_cur_pos.x += ((o.scaled_prefered_pos.x - o.scaled_cur_pos.x) * __some_other_ctr) / 100;
                        o.scaled_cur_pos.y += ((o.scaled_prefered_pos.y - o.scaled_cur_pos.y) * __some_other_ctr) / 100;
                    }
                }
            }

            if (auto vm = AnmManager::getVM(o.anmId1.val); vm) vm->entity_pos = glm::vec3(o.scaled_cur_pos.x*0.0078125f, o.scaled_cur_pos.y*0.0078125f, 0.f);
            if (auto vm = AnmManager::getVM(o.anmId2.val); vm) vm->entity_pos = glm::vec3(o.scaled_cur_pos.x*0.0078125f, o.scaled_cur_pos.y*0.0078125f, 0.f);
        }
    }
}

void Player::move()
{
    // get direction
    if      ((INPUT_STRUCT.input & 0b01010000) == 0b01010000) direction = 5;
    else if ((INPUT_STRUCT.input & 0b01100000) == 0b01100000) direction = 7;
    else if ((INPUT_STRUCT.input & 0b10010000) == 0b10010000) direction = 6;
    else if ((INPUT_STRUCT.input & 0b10100000) == 0b10100000) direction = 8;
    else if ((INPUT_STRUCT.input & 0b00010000) == 0b00010000) direction = 1; // up
    else if ((INPUT_STRUCT.input & 0b00100000) == 0b00100000) direction = 2; // down
    else if ((INPUT_STRUCT.input & 0b01000000) == 0b01000000) direction = 3; // left
    else if ((INPUT_STRUCT.input & 0b10000000) == 0b10000000) direction = 4; // right
    else                                                      direction = 0;

    if (!EnemyManager::GetInstance() || !EnemyManager::GetInstance()->killableEnemyCount() || inner.time_in_stage < 4) {
        __some_other_ctr = 31;
        inner.focusing = 0;
    } else {
        inner.focusing = (INPUT_STRUCT.input & 0b00001000) != 0;
    }


    float spd;
    if (!inner.focusing) {
        if (AnmManager::getVM(inner.focus_eff_anmId)) AnmManager::interrupt_tree(inner.focus_eff_anmId, 1);
        inner.focus_eff_anmId = 0;
        spd = (direction > 4) ? speed_unfocus_diag : speed_unfocus;
    } else {
        if (inner.focus_eff_anmId == 0) inner.focus_eff_anmId = AnmManager::SpawnVM(1, 26); //EFFECT_MANAGER_PTR->effect_anm->create_effect(nullptr,26,14,nullptr);
        auto vm = AnmManager::getVM(inner.focus_eff_anmId);
        if (!vm) inner.focus_eff_anmId = 0;
        else {
            if (!(flags & 0x10)) vm->scale_2 = {1,1};
            else vm->scale_2 = (2 * player_scale__requires_flag_0x10__from_ddc - 1.f) * glm::vec2(1,1);
            vm->bitflags.scaled = 0b1;
        }
        spd = (direction > 4) ? speed_focus_diag : speed_focus;
    }

    float xspd = (PlayerMovement__dirTable[direction*2] * spd + 128.f * __could_be_subsun_pull_direction.x) * speed_multiplier;
    float yspd = (PlayerMovement__dirTable[direction*2+1] * spd + 128.f * __could_be_subsun_pull_direction.y) * speed_multiplier;

    bool fst_update = false;
    if      (xspd <  0 && speed.x > -1) { /*playerAnm->anm_init_copy_vm_from_loaded(&vm, 1);*/vm(AnmManager::getLoaded(9)->getPreloaded(1)); fst_update = true; }
    else if (xspd == 0 && speed.x >  0) { /*playerAnm->anm_init_copy_vm_from_loaded(&vm, 4);*/vm(AnmManager::getLoaded(9)->getPreloaded(4)); fst_update = true; }
    else if (xspd >  0 && speed.x <  1) { /*playerAnm->anm_init_copy_vm_from_loaded(&vm, 3);*/vm(AnmManager::getLoaded(9)->getPreloaded(3)); fst_update = true; }
    else if (xspd == 0 && speed.x <  0) { /*playerAnm->anm_init_copy_vm_from_loaded(&vm, 2);*/vm(AnmManager::getLoaded(9)->getPreloaded(2)); fst_update = true; }
    if (fst_update) {
        vm.parent = nullptr;
        //vm.__root_vm__or_maybe_not = nullptr;
        vm.update();
    }

    static constexpr float GAME_SPEED = 1.f;

    speed.x = xspd;
    speed.y = yspd;
    last_movement = {speed * GAME_SPEED, 0};
    last_movement_cpy = last_movement;
    if (direction != 0) last_movement_no_zero = last_movement;
    inner.integer_pos += glm::vec<2, int32_t>{last_movement.x, last_movement.y};
    if (inner.integer_pos.x < -0x5c00) inner.integer_pos.x = -0x5c00;
    else if (inner.integer_pos.x > 0x5c00) inner.integer_pos.x = 0x5c00;
    if (inner.integer_pos.y < 0x1000) inner.integer_pos.y = 0x1000;
    else if (inner.integer_pos.y > 0xd800) inner.integer_pos.y = 0xd800;
    inner.pos.x = (float)inner.integer_pos.x * 0.0078125;
    inner.pos.y = (float)inner.integer_pos.y * 0.0078125;

    if (auto vm = AnmManager::getVM(inner.focus_eff_anmId); vm) vm->entity_pos = inner.pos;

    if (yspd != 0 || xspd != 0) {
        int i = 32;
        for (; i > inner.__some_index * 8; i--)
            unknown_scaled_pos_array[i] = unknown_scaled_pos_array[inner.__some_index * 8];

        auto piVar9 = &unknown_scaled_pos_array[i].y;
        for (; i > 0; i--) {
            ((glm::vec2*)(piVar9 + -1))->x = piVar9[-3];
            i--;
            *piVar9 = piVar9[-2];
            piVar9 = piVar9 + -2;
        }
    }
    unknown_scaled_pos_array[0] = inner.integer_pos;
    if (flags & 2) __some_counter++;
    move_options(inner.options, 4);
    move_options(inner.subseason_options, 8);
    if (__some_counter >= 30) inner.__some_index = 0;

    if (inner.__timer_187ac > 0) {
        auto some_vm = AnmManager::getVM(inner.__field_187a8__init_0);
        if (!some_vm) {
            //EFFECT_MANAGER_PTR->effect_anm->__field_134__some_kind_of_counter++;
            //some_vm = AnmManager::allocate_vm();
            //EFFECT_MANAGER_PTR->effect_anm->anm_init_copy_vm_from_loaded(some_vm,27);
            //some_vm->mode_of_create_child = 0;
            //AnmManager::insert_in_world_list_back(&inner.__field_187a8__init_0,some_vm);
            inner.__field_187a8__init_0 = AnmManager::SpawnVM(1, 27);
            some_vm = AnmManager::getVM(inner.__field_187a8__init_0);
            some_vm->bitflags.randomMode = 0b1;
            some_vm->bitflags.originMode = 0b01;
            some_vm->layer = 14;
            some_vm->entity_pos = inner.pos;
            some_vm->rotation.z = 0.0;
            some_vm->update();
        }
        else some_vm->entity_pos = inner.pos;

        inner.__timer_187ac--;
        if (inner.__timer_187ac <= 0) {
            AnmManager::interrupt_tree(inner.__field_187a8__init_0, 1);
            inner.__timer_187ac = 0;
        }
    }
}

void Player::die()
{
    // death effect
    //EffectManager::addEffect(0x1c);
    // <==>
    //if (int effid = EffectManager::get_next_index(); effid != -1) EFFECT_MANAGER_PTR->anm_ids[effid] = EFFECT_MANAGER_PTR->effect_anm->create_4112b0(nullptr,0x1c,inner.pos,0,-1,nullptr);

    // update life & bombs
    //GLOBALS.inner.CURRENT_LIVES += -1;
    //if ((int)GLOBALS.inner.field30_0x78 < 0) {
    //    GLOBALS.inner.CURRENT_BOMBS = 0;
    //}
    //else {
    //    GLOBALS.inner.CURRENT_BOMBS = GLOBALS.inner.field30_0x78;
    //    if (8 < (int)GLOBALS.inner.field30_0x78) {
    //    GLOBALS.inner.CURRENT_BOMBS = 8;
    //    }
    //}
    //if (GUI_PTR != NULL) {
    //    FUN_0042fd50(GUI_PTR,GLOBALS.inner.CURRENT_BOMBS,GLOBALS.inner.field29_0x74);
    //}
    //if (-1 < GLOBALS.inner.CURRENT_LIVES) {
    //    FUN_0042fc60(GUI_PTR,GLOBALS.inner.CURRENT_LIVES,GLOBALS.inner.field26_0x68);
    //}
    //FUN_0042fd50(GUI_PTR,GLOBALS.inner.CURRENT_BOMBS,GLOBALS.inner.field29_0x74);

    // update inner
    inner.state = 2;
    inner.time_in_state = 0;
    inner.iframes = 180;
    inner.__some_index = 0;
    //playerAnm->anm_init_copy_vm_from_loaded(&vm,0);
    vm(AnmManager::getLoaded(9)->getPreloaded(0));
    vm.parent = nullptr;
    //vm.__root_vm__or_maybe_not = nullptr;
    vm.update();

    // update options vms
    for (int i = 0; i < 4; i++) {
        AnmManager::interrupt_tree(inner.options[i].anmId1,1);
        AnmManager::interrupt_tree(inner.options[i].anmId2,1);
    }

    // update spell capture
    //if ((SPELLCARD_PTR->flags & 1) != 0) {
    //    if ((SPELLCARD_PTR->_timer_20).current < 0x3c) {
    //        if (BOMB_PTR->active == 1)
    //            SPELLCARD_PTR->flags |= 0x20;
    //    }
    //    else {
    //        SPELLCARD_PTR->bonus = 0;
    //        SPELLCARD_PTR->flags &= 0xffffffdd;
    //    }
    //}

    // update globals
    //ENEMY_MANAGER_PTR->miss_count++;
    //ENEMY_MANAGER_PTR->can_still_capture_spell = 0;
    //if ((GLOBALS.inner.HYPER_FLAGS & 2) && !(GLOBALS.inner.HYPER_FLAGS & 4)) GLOBALS.inner.HYPER_TIME = 1;
    //if (GLOBALS.inner.MISS_COUNT_GLOBAL < 999999) GLOBALS.inner.MISS_COUNT_GLOBAL++;
}

int Player::_on_draw()
{
    if (inner.state != 2)
    {
        vm.entity_pos = inner.pos;
        vm.bitflags.originMode = 0b01;
        vm.setLayer(15);
        vm.draw();
    }
    return 1;
}
