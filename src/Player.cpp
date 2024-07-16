#include "./Player.h"
#include "./Spellcard.h"
#include "./Anm/AnmManager.h"
#include "./Gui.hpp"
#include "./Ecl/EnemyManager.h"
#include "./Hardcoded.h"
#include "./Input.h"
#include "./ItemManager.h"
#include "./Laser/LaserManager.h"
#include <functional>
#include <input.hpp>
#include <math/math.hpp>
#include <NSEngine.hpp>
#include <DrawFuncs.h>
#include "./GoastManager.h"
#include "AsciiPopupManager.hpp"
#include "Bomb.hpp"

Player* PLAYER_PTR = nullptr;

// at th17.exe:4463f0
Player::Player() {
    PLAYER_PTR = this;
    // load player anm file
    playerAnm = anm::loadFile(9,
                    PLAYERS[GLOBALS.inner.CHARACTER]["shottypes"]
                         [GLOBALS.inner.SHOTTYPE]["anm"].asCString());
    // if (!playerAnm) ; // log info

    // load sht file
    if (ALREADY_LOADED_SHTFILE) {
        sht_file = ALREADY_LOADED_SHTFILE;
        ALREADY_LOADED_SHTFILE = nullptr;
    } else {
        sht_file = open_sht(PLAYERS[GLOBALS.inner.CHARACTER]["shottypes"]
                            [GLOBALS.inner.SHOTTYPE]["sht"].asCString());
    }

    // update funcs
    on_game_tick = new UpdateFunc([this]() { return _on_tick(); });
    on_game_tick->flags &= 0xfffffffd;
    UPDATE_FUNC_REGISTRY.register_on_tick(on_game_tick, 23);
    on_draw = new UpdateFunc([this]() { return _on_draw(); });
    on_draw->flags &= 0xfffffffd;
    UPDATE_FUNC_REGISTRY.register_on_draw(on_draw, 29);

    // init vm
    playerAnm->copyFromLoaded(&vm, 0);
    vm.parent_vm = nullptr;
    vm.__root_vm__or_maybe_not = nullptr;
    vm.update();

    speed_focus = sht_file->header.move_f_str * 128;
    speed_focus_diag = sht_file->header.move_f_dia * 128;
    speed_unfocus = sht_file->header.move_nf_str * 128;
    speed_unfocus_diag = sht_file->header.move_nf_dia * 128;

    sht_file->header.SA_power_divisor = 100;
    GLOBALS.inner.MAXIMUM_POWER = sht_file->header.SA_power_divisor *
        sht_file->header.pwr_lvl_cnt;
    GLOBALS.inner.POWER_PER_LEVEL = sht_file->header.SA_power_divisor;

    sht_file->header.hitbox_u = PLAYERS[GLOBALS.inner.CHARACTER]
        ["hitbox"].asFloat();
    sht_file->header.itembox_u = PLAYERS[GLOBALS.inner.CHARACTER]
        ["itembox"].asFloat();
    sht_file->header.grazebox_u = PLAYERS[GLOBALS.inner.CHARACTER]
        ["grazebox"].asFloat();

    hurtbox_halfsize = { sht_file->header.hitbox_u * 0.5f, sht_file->
        header.hitbox_u * 0.5f, 5.f };
    item_attract_box_unfocused_halfsize = { sht_file->header.itembox_u * 0.5f,
        sht_file->header.itembox_u * 0.5f, 5.f };
    item_attract_box_focused_halfsize.x = item_attract_box_focused_halfsize.y =
        PLAYERS[GLOBALS.inner.CHARACTER]["attractbox"].asFloat() * 0.5;
    item_attract_box_focused_halfsize.z = 5.0;

    hurtbox.min_pos = inner.pos - hurtbox_halfsize;
    hurtbox.max_pos = inner.pos + hurtbox_halfsize;
    item_collect_box.min_pos = inner.pos - item_attract_box_unfocused_halfsize;
    item_collect_box.max_pos = inner.pos + item_attract_box_unfocused_halfsize;
    item_attract_box_focused.min_pos = inner.pos -
        item_attract_box_focused_halfsize;
    item_attract_box_focused.max_pos = inner.pos +
        item_attract_box_focused_halfsize;
    item_attract_box_unfocused.min_pos = inner.pos -
        item_attract_box_focused_halfsize;
    item_attract_box_unfocused.max_pos = inner.pos +
        item_attract_box_focused_halfsize;

    flags &= 0xfffffffb;

    for (int i = 0; i < 0x100; i++) {
        inner.bullets[i].id = i;
    }
}

void youmu_option_update_func(PlayerOption_t* opt) {
    opt->scaled_prefered_pos = PLAYER_PTR->prev_pos[opt->id * 8 + 7];
    if (!PLAYER_PTR->inner.focusing) {
        opt->scaled_prefered_pos_rel_to_player[0].x =
            opt->scaled_prefered_pos.x - PLAYER_PTR->inner.integer_pos.x;
        opt->scaled_prefered_pos_rel_to_player[0].y =
            opt->scaled_prefered_pos.y - PLAYER_PTR->inner.integer_pos.y;
    } else {
        opt->scaled_prefered_pos_rel_to_player[0].x = 0;
        opt->scaled_prefered_pos_rel_to_player[0].y = -0xc80;
        PLAYER_PTR->prev_pos[opt->id * 8 + 8].y =
            static_cast<int>(PLAYER_PTR->inner.integer_pos.y) + -0xc80;
        PLAYER_PTR->prev_pos[opt->id * 8 + 8].x =
            static_cast<int32_t>(PLAYER_PTR->inner.integer_pos.x);
        for (int i = 1; i <= 7; i++) {
            PLAYER_PTR->prev_pos[opt->id * 8 + i].x =
                (PLAYER_PTR->prev_pos[opt->id * 8 + 8].x -
                PLAYER_PTR->prev_pos[opt->id * 8].x) *
                0.125 * i + PLAYER_PTR->prev_pos[opt->id * 8].x;
            PLAYER_PTR->prev_pos[opt->id * 8 + i].y =
                (PLAYER_PTR->prev_pos[opt->id * 8 + 8].y -
                PLAYER_PTR->prev_pos[opt->id * 8].y) *
                0.125 * i + PLAYER_PTR->prev_pos[opt->id * 8].y;
        }
    }
    opt->scaled_prefered_pos.x = PLAYER_PTR->inner.integer_pos.x +
        opt->scaled_prefered_pos_rel_to_player[0].x;
    opt->scaled_prefered_pos.y = PLAYER_PTR->inner.integer_pos.y +
        opt->scaled_prefered_pos_rel_to_player[0].y;
    opt->focusing = PLAYER_PTR->inner.focusing;
}

void FUN_00449630(PlayerInner_t* inner) {
    int pow = GLOBALS.inner.CURRENT_POWER / GLOBALS.inner.POWER_PER_LEVEL;
    // inner[1].__time_in_stage__copy_3c.control = pow;
    if (GLOBALS.inner.CURRENT_POWER < GLOBALS.inner.MAXIMUM_POWER) {
        for (int i = 0; i < 4; i++) {
            anm::interrupt_tree(inner->options[i].anmId2, 1);
            inner->options[i].anmId2 = 0;
        }
    } else {
        if (0 < pow) {
            for (int i = 0; i < pow; i++) {
                if (auto vm = anm::getVM(inner->options[i].anmId2);
                    vm && !vm->bitflags.f534_27_31) {
                    anm::deleteVM(vm);
                }
                inner->options[i].anmId2 = PLAYER_PTR->playerAnm->spawnVMExt(
                    PLAYERS[GLOBALS.inner.CHARACTER]["option_max"].asInt(),
                    0, {0, -32, 0}, 2);
            }
        }
    }
    if (inner->power == pow)
        return;

    if (0 < pow) {
        int i = 0;
        for (; i < pow; i++) {
            if (auto vm = anm::getVM(inner->options[i].anmId1);
                vm && !vm->bitflags.f534_27_31) {
                anm::deleteVM(inner->options[i].anmId1);
            }
            inner->options[i].id = i;
            inner->options[i].scaled_prefered_pos_rel_to_player[0].x =
                static_cast<int>(PLAYER_PTR->sht_file->option_pos[PLAYERS
                    [GLOBALS.inner.CHARACTER]["shottypes"]
                    [GLOBALS.inner.SHOTTYPE]["option_pos_lookup"][pow - 1]
                    .asInt() + i].x * 128.f);
            inner->options[i].scaled_prefered_pos_rel_to_player[0].y =
                static_cast<int>(PLAYER_PTR->sht_file->option_pos[PLAYERS
                    [GLOBALS.inner.CHARACTER]["shottypes"]
                    [GLOBALS.inner.SHOTTYPE]["option_pos_lookup"][pow - 1]
                    .asInt() + i].y * 128.f);
            inner->options[i].scaled_prefered_pos_rel_to_player[1].x =
                static_cast<int>(PLAYER_PTR->sht_file->option_pos[PLAYERS
                    [GLOBALS.inner.CHARACTER]["shottypes"]
                    [GLOBALS.inner.SHOTTYPE]["option_pos_lookup"]
                    [pow - 1 + HARDCODED_DATA["option_count"].asInt()]
                    .asInt() + i].x * 128.f);
            inner->options[i].scaled_prefered_pos_rel_to_player[1].y =
                static_cast<int>(PLAYER_PTR->sht_file->option_pos[PLAYERS
                    [GLOBALS.inner.CHARACTER]["shottypes"]
                    [GLOBALS.inner.SHOTTYPE]["option_pos_lookup"]
                    [pow - 1 + HARDCODED_DATA["option_count"].asInt()]
                    .asInt() + i].y * 128.f);
            inner->options[i].scaled_cur_pos.x = inner->integer_pos.x +
                inner->options[i].scaled_prefered_pos_rel_to_player
                [inner->focusing].x;
            inner->options[i].scaled_cur_pos.y = inner->integer_pos.y +
                inner->options[i].scaled_prefered_pos_rel_to_player
                [inner->focusing].y;
            inner->options[i].anmId1 = PLAYER_PTR->playerAnm->spawnVMExt(
                PLAYERS[GLOBALS.inner.CHARACTER]["option"].asInt(),
                0, {0, -32, 0}, 0, 14);
            inner->options[i].flags = 2;

            if (TOUHOU_VERSION == 17 && GLOBALS.inner.CHARACTER == 2) {
                inner->options[i].update_code = youmu_option_update_func;
            }
        }
        for (; i < 4; i++)
            anm::interrupt_tree(inner->options[i].anmId1, 1);
    } else {
        for (int i = 0; i < 4; i++)
            anm::interrupt_tree(inner->options[i].anmId1, 1);
    }

    inner->power = pow;
    inner->options[0].move_instantly = 1;
    inner->options[1].move_instantly = 1;
    inner->options[2].move_instantly = 1;
    inner->options[3].move_instantly = 1;
    return;
}

Player::~Player() {
    if (on_game_tick)
        UPDATE_FUNC_REGISTRY.unregister(on_game_tick);
    if (on_draw)
        UPDATE_FUNC_REGISTRY.unregister(on_draw);
    PLAYER_PTR = nullptr;

    if (!(GLOBALS.FLAGS & 1)) {
        // anm::_unload_file(9);
        if (sht_file) {
            delete sht_file;
            sht_file = NULL;
        }
        ALREADY_LOADED_SHTFILE = NULL;
    } else {
        //    //anm::remove_anm_of_file(playerAnm);
        ALREADY_LOADED_SHTFILE = sht_file;
    }
}

int Player::_on_tick() {
    INPUT_STRUCT.Update();
    if (ns::keyboard::pressed(ns::Key::P))
        try_kill();

    // movement state
    switch (inner.state) {
    case 0:  // Respawning
        inner.integer_pos.y = 61440 - (inner.time_in_state.current_f * 512) / 3;
        inner.pos.y = inner.integer_pos.y * 0.0078125;
        inner.options[0].move_instantly = true;
        inner.options[1].move_instantly = true;
        inner.options[2].move_instantly = true;
        inner.options[3].move_instantly = true;

        for (int i = 0; i < 33; i++)
            prev_pos[i] = inner.integer_pos;

        if (inner.time_in_state < 30) {
            LASER_MANAGER_PTR->cancel_in_radius(death_pos, 0, 1, (inner.time_in_state.current_f * 512.0) / 30.0 + 64.0);
            LASER_MANAGER_PTR->cancel_in_radius(death_pos, 0, 0, (inner.time_in_state.current_f * 512.0) / 120.0 + 16.0);
        } else {
            // if ((SPELLCARD_PTR->flags & 1) && (SPELLCARD_PTR->field_0x74 == 100)) {
            //     BulletManager::GetInstance()->cancel_radius(&inner.pos,0,128.0);
            // }
            // BulletManager::GetInstance()->cancel_radius_as_bomb(inner.pos,0,640.0);
            LASER_MANAGER_PTR->cancel_in_radius(inner.pos, 0, 0, 640.0);
        }

        if (inner.time_in_state >= 60) {
            inner.state = 1;
            inner.time_in_state = 0;
        } else {
            break;
        }
        [[fallthrough]];
    case 1: // Normal
        if (BOMB_PTR && (GLOBALS.inner.CURRENT_BOMBS > 0) && player_is_trying_to_bomb() && ns::keyboard::pressed(ns::Key::X)) {
            Bomb::start();
        }
        move();
        break;
    case 4: // Deathbomb window
        if (inner.time_in_state > 7) {
            die();
        } else {
            if ((GLOBALS.inner.HYPER_FLAGS & 2U) &&
                !(GLOBALS.inner.HYPER_FLAGS & 4U)) {
                if (GOAST_MANAGER_PTR)
                    GOAST_MANAGER_PTR->hyper_die(true);
                inner.time_in_state = 60;
            }
            if (BOMB_PTR && (GLOBALS.inner.CURRENT_BOMBS > 0) && player_is_trying_to_bomb() && ns::keyboard::pressed(ns::Key::X)) {
                Bomb::start();
                inner.time_in_state = 60;
            }
            break;
        }
        [[fallthrough]];
    case 2: // Dying
        if (inner.time_in_state.had_value(3)) {
            GLOBALS.inner.CURRENT_POWER -= GLOBALS.inner.POWER_PER_LEVEL / 2;
            if (GLOBALS.inner.CURRENT_POWER < GLOBALS.inner.POWER_PER_LEVEL)
                GLOBALS.inner.CURRENT_POWER = GLOBALS.inner.POWER_PER_LEVEL;

            for (int i = 0; i < 7; i++) {
                ITEM_MANAGER_PTR->spawn_item(1, inner.pos, ((i * 3.141593) / 28.0 + math::point_direction(inner.pos.x, inner.pos.y, 0, -224)) - 0.3926991, 3.0, 0, -1);
            }

            ITEM_MANAGER_PTR->spawn_item(GLOBALS.inner.SHOTTYPE + 16, inner.pos + ns::vec3(-16, -96, 0), -1.570796, 2.2, 0, -1);
            ITEM_MANAGER_PTR->spawn_item(GLOBALS.inner.SHOTTYPE + 16, inner.pos + ns::vec3(+16, -96, 0), -1.570796, 2.2, 0, -1);

            FUN_00449630(&inner);
        }
        if (inner.time_in_state > 30) {
            if (GLOBALS.inner.CURRENT_LIVES < 0 && inner.time_in_state.had_value(31)) {
                // if (REPLAY_MANAGER_PTR->field3_0xc != 1) FUN_004447e0();
                inner.time_in_state++;
            } else {
                // GAME_SPEED = 1.0;
                inner.state = 0;
                inner.iframes = 280;
                inner.time_in_state = 0;
                // FUN_00449c80(this,inner.pos, 30, 150, 32.f, 16.f);
                // set_nb_bomb(GLOBALS.inner.field30_0x78);
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
        if (inner.time_in_state.had_value(15)) {
            LASER_MANAGER_PTR->cancel_all(1);
        }
    }

    // update damage sources
    for (int i = 0; i < 256; i++) {
        if (inner.damage_sources[i].flags & 1) {
            inner.damage_sources[i].pos.update_velocities();
            inner.damage_sources[i].pos.update_position();
            inner.damage_sources[i].field_0x4 += inner.damage_sources[i].field_0x8;
            inner.damage_sources[i].angle += inner.damage_sources[i].angle_increase;
            inner.damage_sources[i].field_0x84 = 0;
            inner.damage_sources[i].time_to_live--;
            if (inner.damage_sources[i].time_to_live <= 0)
                inner.damage_sources[i].flags &= 0xfffffffe;

            math::angle_normalize(inner.damage_sources[i].angle);
        }
    }

    // speed boost anim ?
    static const int SOME_SCRIPT_ID[] = {17, 19, 21, 17, 17};
    if (inner.iframes <= 0) {
        vm.bitflags.colmode = 0b00;
        if (!(flags & 0x20)) {
            if (speed_multiplier > 1.01) {
                if (inner.time_in_state.had_true([](i32 i){ return abs(i) % 8 < 4; })) {
                    vm.color_2 = { 255, 255, 0, 255 };
                    vm.bitflags.colmode = 0b01;
                }
                anm::VM* vm;
                playerAnm->createEffectPos(SOME_SCRIPT_ID[GLOBALS.inner.CHARACTER], 0, inner.pos, -1, &vm);
                playerAnm->setSprite(vm, this->vm.sprite_id);
            }
        } else {
            if (inner.time_in_state.had_true([](i32 i){ return abs(i) % 8 < 4; })) {
                vm.color_2 = { 255, 0, 0, 255 };
                vm.bitflags.colmode = 0b01;
            }
            anm::VM* vm;
            playerAnm->createEffectPos(SOME_SCRIPT_ID[GLOBALS.inner.CHARACTER], 0, inner.pos, -1, &vm);
            playerAnm->setSprite(vm, this->vm.sprite_id);
            vm->color_1 = {255, 0, 0, 255};
        }
    } else {
        inner.iframes--;
    }

    // reset some values
    speed_multiplier = 1.0;
    __could_be_subsun_pull_direction = { 0, 0, 0 };

    // update vm
    vm.update();

    // scale player
    if (!(flags & 0x10)) {
        vm.bitflags.scaled = 0b1;
        vm.scale = { 1, 1 };
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

        if ((player_scale_i.end_time == 0) || inner.time_in_state.was_modulo(3))
            vm.scale.x = vm.scale.y = player_scale__requires_flag_0x10__from_ddc;
        else
            vm.scale = { 1.f, 1.f };
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

    if (!GUI_PTR->msg &&
        ENEMY_MANAGER_PTR && ENEMY_MANAGER_PTR->enemy_count_real &&
        /*!(GAME_THREAD_PTR->flags & 0x4000U) && */
        inner.time_in_stage >= 20 && (~(flags >> 2) & 1) != 0 && !(flags & 0x10U)) {
        check_shoot();
    } else {
        inner.shoot_key_short_timer = -1;
        inner.shoot_key_long_timer = -1;

        // Maybe youmu related
        field_0x19080 = 0;
        field_0x19040 = 0;
        field_0x19044 = 0;

        anm::deleteVM(extra_anm_id.val);
        extra_anm_id = 0;

        // for (int i = 0; i < 12; i++) {
        //     if ((int)(&DAT_0052480c)[i] < 0) {
        //         (&DAT_0052480c)[i] = 31;
        //         (&DAT_0052483c)[i] = -1;
        //         break;
        //     }
        //     if ((&DAT_0052480c)[i] == 31) {
        //         (&DAT_0052483c)[i] = -1;
        //         break;
        //     }
        // }
        // for (int i = 0; i < 12; i++) {
        //     if ((int)(&DAT_0052480c)[i] < 0) {
        //         (&DAT_0052480c)[i] = 55;
        //         (&DAT_0052483c)[i] = -1;
        //         break;
        //     }
        //     if ((&DAT_0052480c)[i] == 55) {
        //         (&DAT_0052483c)[i] = -1;
        //         break;
        //     }
        // }
    }

    for (int i = 0; i < 256; i++) {
        if (inner.bullets[i].active)
            inner.bullets[i].update();
    }

    // youmu charge anim ?
    if (auto vm_ = anm::getVM(extra_anm_id); vm_)
        vm_->entity_pos = inner.pos;
    else
        extra_anm_id = 0;

    flags &= 0xffffffbf;
    return 1;
}

int create_damage_source(ns::vec3 const& pos, float angle, int timeToLive, int damage, float w, float h)
{
    math::angle_normalize(angle);

    // find damage source index
    int id = PLAYER_PTR->inner.last_created_damage_source_index;
    for (int i = 0; i < 257; i++) {
        id = (id + 1) % 256;
        if (!(PLAYER_PTR->inner.damage_sources[id].flags & 1))
            break;
        if (i == 256) {
            PLAYER_PTR->inner.last_created_damage_source_index = id;
            return id + 1;
        }
    }

    PLAYER_PTR->inner.damage_sources[id].flags = (PLAYER_PTR->inner.damage_sources[id].flags & 0xfffffff9) | 1;
    PLAYER_PTR->inner.damage_sources[id].pos = {};
    PLAYER_PTR->inner.damage_sources[id].pos.pos = pos;
    PLAYER_PTR->inner.damage_sources[id].hitbox = { w, h };
    PLAYER_PTR->inner.damage_sources[id].angle = angle;
    PLAYER_PTR->inner.damage_sources[id].angle_increase = 0;
    PLAYER_PTR->inner.damage_sources[id].time_to_live = timeToLive;
    PLAYER_PTR->inner.damage_sources[id].damage = damage;
    PLAYER_PTR->inner.damage_sources[id].field_0x78 = 0;
    PLAYER_PTR->inner.damage_sources[id].field_0x7c = 9999999;
    PLAYER_PTR->inner.damage_sources[id].field_0x80 = 1;
    PLAYER_PTR->inner.damage_sources[id].ds_on_hit = 0;
    PLAYER_PTR->inner.damage_sources[id].field_0x84 = 0;
    PLAYER_PTR->inner.damage_sources[id].field_0x88 = 0;
    PLAYER_PTR->inner.damage_sources[id].field_0x8c = 0;
    PLAYER_PTR->inner.last_created_damage_source_index = id;
    return id + 1;
}

int create_damage_source_3(ns::vec3 const& pos, int timeToLive, int damage, float field_4, float field_8)
{
    // find damage source index
    int id = PLAYER_PTR->inner.last_created_damage_source_index;
    for (int i = 0; i < 257; i++) {
        id = (id + 1) % 256;
        if (!(PLAYER_PTR->inner.damage_sources[id].flags & 1))
            break;
        if (i == 256) {
            PLAYER_PTR->inner.last_created_damage_source_index = id;
            return id + 1;
        }
    }

    PLAYER_PTR->inner.damage_sources[id].flags = (PLAYER_PTR->inner.damage_sources[id].flags & 0xfffffffb) | 3;
    PLAYER_PTR->inner.damage_sources[id].pos = {};
    PLAYER_PTR->inner.damage_sources[id].pos.pos = pos;
    PLAYER_PTR->inner.damage_sources[id].field_0x4 = field_4;
    PLAYER_PTR->inner.damage_sources[id].field_0x8 = field_8;
    PLAYER_PTR->inner.damage_sources[id].time_to_live = timeToLive;
    PLAYER_PTR->inner.damage_sources[id].damage = damage;
    PLAYER_PTR->inner.damage_sources[id].field_0x78 = 0;
    PLAYER_PTR->inner.damage_sources[id].field_0x7c = 9999999;
    PLAYER_PTR->inner.damage_sources[id].field_0x80 = 1;
    PLAYER_PTR->inner.damage_sources[id].ds_on_hit = 0;
    PLAYER_PTR->inner.damage_sources[id].field_0x84 = 0;
    PLAYER_PTR->inner.damage_sources[id].field_0x88 = 0;
    PLAYER_PTR->inner.damage_sources[id].field_0x8c = 0;
    PLAYER_PTR->inner.last_created_damage_source_index = id;
    return id + 1;
}

void Player::set_pos(ns::vec3 const& p)
{
    inner.integer_pos = { (i32)(p.x * 128), (i32)(p.y * 128) };
    inner.pos = ns::vec3(inner.integer_pos.x, inner.integer_pos.y, 0) / 128.f;
    inner.options[0].move_instantly = true;
    inner.options[1].move_instantly = true;
    inner.options[2].move_instantly = true;
    inner.options[3].move_instantly = true;
}

void Player::check_shoot()
{
    // for youmu ?
    if (inner.state != 1) {
        field_0x19040 = 0;
        field_0x19044 = 0;

        if (field_0x19080.had_value(0)) {
            return;
        }

        // FUN_004655d0(0x1e);
        // FUN_004655d0(0x37);

        anm::deleteVM(extra_anm_id.val);
        extra_anm_id = 0;

        field_0x19080 = 0;
        return;
    }

    // youmu shoot code
    if (GLOBALS.inner.CHARACTER == 2 && TOUHOU_VERSION == 17) {
        if (!((GLOBALS.inner.HYPER_FLAGS >> 1) & 1) ||
             (GLOBALS.inner.HYPER_TYPE != 3)) {
            if (!((GLOBALS.inner.HYPER_FLAGS >> 1) & 1) ||
                (GLOBALS.inner.HYPER_TYPE != 1)) {
                if (!inner.focusing)
                    goto LAB_0044ab18;
                if ((INPUT_STRUCT.input & 1) && field_0x19080 >= 0) {
                    if (field_0x19080.had_value(0)) {
                        //                    SoundManager::play_sound_at_position(31);
                        if (GLOBALS.inner.SHOTTYPE == 0)
                            extra_anm_id = PLAYER_PTR->playerAnm->createEffectPos(20, 0, inner.pos);
                        else
                            extra_anm_id = PLAYER_PTR->playerAnm->createEffectPos(19, 0, inner.pos);
                    }
                    field_0x19080++;
                    //                FUN_004655a0(31, inner.pos.x);
                    //                FUN_004655a0(55, inner.pos.x);
                    if (field_0x19080.had_value(60)) {
                        //                    FUN_004655d0(31);
                        //                    SoundManager::play_sound_at_position(55);
                        anm::interrupt_tree(extra_anm_id, 2);
                    }
                    return;
                }
                if (field_0x19080 > 0) {
                    //                FUN_004655d0(31);
                    //                FUN_004655d0(55);
                    anm::deleteVM(extra_anm_id.val);
                    extra_anm_id = 0;
                    if (field_0x19080 >= 60) {
                        field_0x19080 = -40;
                        field_0x19098 = inner.pos;
                    } else
                        field_0x19080 = 0;
                    return;
                }
                if (field_0x19080 >= 0)
                    return;
            } else if (field_0x19080 >= 0) {
                if (INPUT_STRUCT.input & 1) {
                    anm::deleteVM(extra_anm_id.val);
                    extra_anm_id = 0;
                    field_0x19080 = -40;
                    field_0x19098 = inner.pos;
                }
                return;
            }
            ns::vec3 old_pos = inner.pos;
            set_pos(field_0x19098);
            shoot(field_0x19080.current + 40, 0);
            set_pos(old_pos);
            field_0x19080++;
            if (field_0x19080 < 0)
                return;
            field_0x19080 = 0;
            return;
        }
    LAB_0044ab18:
        if (field_0x19080 < 1) {
            if (field_0x19080 < 0) {
                bool temp = inner.focusing;
                inner.focusing = 1;
                ns::vec3 old_pos = inner.pos;
                set_pos(field_0x19098);
                shoot(field_0x19080.current + 40, 0);
                set_pos(old_pos);
                inner.focusing = temp;
                field_0x19080++;
            }
        } else {
            //        FUN_004655d0(31);
            //        FUN_004655d0(55);
            anm::deleteVM(extra_anm_id.val);
            extra_anm_id = 0;
            if (field_0x19080 < 60)
                field_0x19080 = 0;
            else {
                field_0x19080 = -40;
                field_0x19098 = inner.pos;
            }
        }
    }

    if (inner.shoot_key_short_timer < 0) {
        if (!(INPUT_STRUCT.input & 1)) {
            if (inner.shoot_key_long_timer > 0x76) {
                // if (INPUT_STRUCT.input & 1) inner.shoot_key_long_timer -= 0x77;
                /*else*/ inner.shoot_key_long_timer = -1;
            } else if (inner.shoot_key_long_timer >= 0)
                inner.shoot_key_long_timer++;
            return;
        }
        inner.shoot_key_short_timer = 0;
    }

    if (inner.shoot_key_short_timer.ticked())
        shoot(inner.shoot_key_short_timer.current, inner.shoot_key_long_timer.current);

    if (inner.shoot_key_short_timer < 14) {
        inner.shoot_key_short_timer++;
    } else if (INPUT_STRUCT.input & 1) {
        inner.shoot_key_short_timer.add_nogamespeed(-14);
    } else {
        inner.shoot_key_short_timer = -1;
    }

    if (inner.shoot_key_long_timer > 0x76) {
        if (INPUT_STRUCT.input & 1)
            inner.shoot_key_long_timer.add_nogamespeed(-0x77);
        else
            inner.shoot_key_long_timer = -1;
    } else if (inner.shoot_key_long_timer >= 0)
        inner.shoot_key_long_timer++;
}

void Player::shoot(int short_tmr, int long_tmr) {
    // get shooterset
    size_t shooter_set = GLOBALS.inner.CURRENT_POWER /
                         GLOBALS.inner.POWER_PER_LEVEL;
    if (inner.focusing)
        shooter_set += sht_file->header.pwr_lvl_cnt + 1;

    // special shooterset for hyper
    if ((((GLOBALS.inner.HYPER_FLAGS >> 1 & 1) == 0) ||
        (GLOBALS.inner.HYPER_TYPE != 1)) ||
        ((GLOBALS.inner.HYPER_FLAGS & 4U) != 0)) {
        if ((((GLOBALS.inner.HYPER_FLAGS >> 1 & 1) != 0) &&
            (GLOBALS.inner.HYPER_TYPE == 3)) &&
            ((GLOBALS.inner.HYPER_FLAGS & 4U) == 0)) {
            shooter_set = GLOBALS.inner.CURRENT_POWER /
                          GLOBALS.inner.POWER_PER_LEVEL + 15;
        }
    } else {
        shooter_set = GLOBALS.inner.CURRENT_POWER /
                      GLOBALS.inner.POWER_PER_LEVEL + 10;
    }

    if (shooter_set >= sht_file->shooters.size())
        return;
    auto shooters = sht_file->shooters[shooter_set];

    // shoot all shooters in set
    for (size_t i = 0; i < shooters.size(); i++) {
        // check time
        if ((shooters[i].long_fire_rate == 0) ?
            (short_tmr % shooters[i].fire_rate) == shooters[i].start_delay :
            (long_tmr % shooters[i].long_fire_rate) ==
              shooters[i].long_start_delay) {
            // idk
            if (shooters[i].__unknown_21__2_is_unique_bullet == 2)
                if (inner.unique_bullets[(shooters[i].option < 0x10) ?
                    shooters[i].option & 0xf : shooters[i].option >> 4])
                    continue;

            // spawn using first free bullet
            for (int j = 0; j < 0x100; j++) {
                if (inner.bullets[j].active == 0) {
                    inner.bullets[j].init((shooter_set << 8 | i),
                                           short_tmr, inner);
                    break;
                }
            }
        }
    }
}

void PlayerBullet_t::init(int shter, int tmer [[maybe_unused]], PlayerInner_t& inner) {
    // get shooter that shooted this bullet
    auto shooter = PLAYER_PTR->sht_file->shooters[shter >> 8][shter & 0xff];
    this->shter = shter;

    // some init
    active = 1;
    flags |= 1;
    __field_c = 0;
    damage = shooter.damage;
    hitbox.x = shooter.hitbox.x;
    hitbox.y = shooter.hitbox.y;
    pos = {};
    pos.speed = shooter.speed;
    pos.angle = shooter.angle;

    // set position
    if ((shooter.option & 0xf) == 0)
        pos.pos = inner.pos;
    else
        pos.pos = { inner.options[shooter.option - 1].scaled_cur_pos.x * 0.0078125f, inner.options[shooter.option - 1].scaled_cur_pos.y * 0.0078125f, 0.f };

    // marisa lasers
    if (shooter.__unknown_21__2_is_unique_bullet == 2)
        inner.unique_bullets[(shooter.option < 0x10) ? shooter.option & 0xf : shooter.option >> 4] = GLOBALS.inner.CURRENT_POWER / GLOBALS.inner.POWER_PER_LEVEL;

    // special angle
    if (pos.angle < 1000.0) {
        if (pos.angle < 995.0)
            math::angle_normalize(pos.angle);
        else if ((shooter.option & 0xf) == 0)
            math::angle_normalize(pos.angle);
        else {
            pos.angle = inner.options[((int)(char)shooter.option & 0xfU) - 1].shoot_angle_maybe;
            math::angle_normalize(pos.angle);
        }
    } else {
        if ((shooter.option & 0xf) != 0) {
            pos.angle = (Random::Floatm11() * 3.141593) / 12.0 + inner.options[((int)(char)shooter.option & 0xfU) - 1].shoot_angle_maybe;
            math::angle_normalize(pos.angle);
            pos.speed = 2 * Random::Floatm11() + shooter.speed;
        } else {
            math::angle_normalize(pos.angle);
        }
    }

    // go back one frame
    pos.update_velocities();
    pos.pos.x += shooter.offset.x - pos.velocity.x;
    pos.pos.y += shooter.offset.y - pos.velocity.y;

    // spawn bullet vm
    anmId = PLAYER_PTR->playerAnm->spawnVMExt(shooter.anm + 5, 0, pos.pos, 0);
    auto vm = anm::getVM(anmId);
    if (vm->bitflags.autoRotate) {
        vm->bitflags.rotated = 0b1;
        vm->rotation.z = shooter.angle;
    }

    // create damageSource
    damageSourceId = create_damage_source(pos.pos, pos.angle, 9999999, damage, hitbox.x, hitbox.y);
    inner.damage_sources[damageSourceId - 1].ds_on_hit = 1;
    inner.damage_sources[damageSourceId - 1].bullet_id = id;
    inner.damage_sources[damageSourceId - 1].field_0x7c = 10000000;

    // call special init func
    if (shooter.on_init && shooter.on_init(this)) {
        active = 0;
        anm::deleteVM(anmId);
        inner.damage_sources[damageSourceId - 1].flags &= 0xfffffffe;
    }

    // play spawn sound (why sfx_hit though)
    // if (shooter.sfx_hit >= 0) SoundManager::play_sound_at_position(shooter.sfx_hit);
}

void PlayerBullet_t::update()
{
    // get shooter that shooted the bullet
    auto shooter = PLAYER_PTR->sht_file->shooters[shter >> 8][shter & 0xff];

    // special update func
    if (shooter.on_tick && shooter.on_tick(this))
        return;

    // update pos and velocity
    pos.update_velocities();
    pos.update_position();

    // update vm
    auto vm = anm::getVM(anmId);
    if (!vm) {
        active = 0;
        anmId = 0;
        if (damageSourceId)
            PLAYER_PTR->inner.damage_sources[damageSourceId - 1].flags &= 0xfffffffe;
        return;
    }
    vm->entity_pos = pos.pos;
    if (vm->bitflags.autoRotate) {
        vm->bitflags.rotated = 0b1;
        vm->rotation.z = pos.angle;
    }

    // check offscreen
    if (shooter.__unknown_21__2_is_unique_bullet != 2) {
        ns::vec2 corners[4];
        vm->getSpriteCorners2D(corners);
        if (0xe < __field_c) {
            float DAT_00524720 = 0, DAT_0052471c = -192.f;
            if ((DAT_0052471c > corners[0].x || corners[0].x > DAT_0052471c + 384.0 || DAT_00524720 > corners[0].y || corners[0].y > DAT_00524720 + 448.0) && (DAT_0052471c > corners[1].x || corners[1].x > DAT_0052471c + 384.0 || DAT_00524720 > corners[1].y || corners[1].y > DAT_00524720 + 448.0) && (DAT_0052471c > corners[2].x || corners[2].x > DAT_0052471c + 384.0 || DAT_00524720 > corners[2].y || corners[2].y > DAT_00524720 + 448.0) && (DAT_0052471c > corners[3].x || corners[3].x > DAT_0052471c + 384.0 || DAT_00524720 > corners[3].y || corners[3].y > DAT_00524720 + 448.0)) {
                anm::deleteVM(anmId.val);
                anmId = 0;
                active = 0;
                if (damageSourceId)
                    PLAYER_PTR->inner.damage_sources[damageSourceId - 1].flags &= 0xfffffffe;
                return;
            }
        }
    }

    // update my damage source
    if (damageSourceId && (flags & 1)) {
        PLAYER_PTR->inner.damage_sources[damageSourceId - 1].pos.pos = pos.pos;
        PLAYER_PTR->inner.damage_sources[damageSourceId - 1].angle = pos.angle;
        PLAYER_PTR->inner.damage_sources[damageSourceId - 1].hitbox = hitbox;
        PLAYER_PTR->inner.damage_sources[damageSourceId - 1].damage = damage;
    }

    // time alive ?
    __field_c++;
    return;
}

const int PlayerMovement__dirTable[] = {
    0, 0,
    0, -1,
    0, 1,
    -1, 0,
    1, 0,
    -1, -1,
    1, -1,
    -1, 1,
    1, 1
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
                    anm::interrupt_tree(o.anmId1, 1);
                    anm::interrupt_tree(o.anmId2, 1);
                    continue;
                }
            } else {
                o.scaled_prefered_pos.x = o.scaled_prefered_pos_rel_to_player[inner.focusing].x + inner.integer_pos.x;
                o.scaled_prefered_pos.y = o.scaled_prefered_pos_rel_to_player[inner.focusing].y + inner.integer_pos.y;
                if (o.update_code)
                    o.update_code(&o);
            }

            if (o.move_instantly) {
                o.scaled_cur_pos = o.scaled_prefered_pos;
                o.move_instantly = false;
            } else {
                if (__some_other_ctr >= 30) {
                    if ((((o.scaled_prefered_pos.x - o.scaled_cur_pos.x) * __some_other_ctr) / 100 == 0) && (((o.scaled_prefered_pos.y - o.scaled_cur_pos.y) * __some_other_ctr) / 100 == 0))
                        o.scaled_cur_pos = o.scaled_prefered_pos;
                    else {
                        o.scaled_cur_pos.x += ((o.scaled_prefered_pos.x - o.scaled_cur_pos.x) * __some_other_ctr) / 100;
                        o.scaled_cur_pos.y += ((o.scaled_prefered_pos.y - o.scaled_cur_pos.y) * __some_other_ctr) / 100;
                    }
                }
            }

            if (auto vm_ = anm::getVM(o.anmId1.val); vm_)
                vm_->entity_pos = ns::vec3(o.scaled_cur_pos.x, o.scaled_cur_pos.y, 0.f) / 128.f;
            if (auto vm_ = anm::getVM(o.anmId2.val); vm_)
                vm_->entity_pos = ns::vec3(o.scaled_cur_pos.x, o.scaled_cur_pos.y, 0.f) / 128.f;
        }
    }
}

void Player::move()
{
    // get direction
    if ((INPUT_STRUCT.input & 0b01010000) == 0b01010000)
        direction = 5;
    else if ((INPUT_STRUCT.input & 0b01100000) == 0b01100000)
        direction = 7;
    else if ((INPUT_STRUCT.input & 0b10010000) == 0b10010000)
        direction = 6;
    else if ((INPUT_STRUCT.input & 0b10100000) == 0b10100000)
        direction = 8;
    else if ((INPUT_STRUCT.input & 0b00010000) == 0b00010000)
        direction = 1;
    else if ((INPUT_STRUCT.input & 0b00100000) == 0b00100000)
        direction = 2;
    else if ((INPUT_STRUCT.input & 0b01000000) == 0b01000000)
        direction = 3;
    else if ((INPUT_STRUCT.input & 0b10000000) == 0b10000000)
        direction = 4;
    else
        direction = 0;

    // check if player is focusing
    if (!ENEMY_MANAGER_PTR || !ENEMY_MANAGER_PTR->killableEnemyCount() || inner.time_in_stage < 4) {
        __some_other_ctr = 31;
        inner.focusing = 0;
    } else {
        inner.focusing = (INPUT_STRUCT.input & 0b00001000) != 0;
    }

    // get speed
    float spd;
    if (!inner.focusing) {
        if (anm::getVM(inner.focus_eff_anmId))
            anm::interrupt_tree(inner.focus_eff_anmId, 1);
        inner.focus_eff_anmId = 0;
        spd = (direction > 4) ? speed_unfocus_diag : speed_unfocus;
    } else {
        if (inner.focus_eff_anmId == (unsigned)0)
            inner.focus_eff_anmId =
                anm::getLoaded(8)->createEffect(26, 14);
        auto vm_ = anm::getVM(inner.focus_eff_anmId);
        if (!vm_)
            inner.focus_eff_anmId = 0;
        else {
            if (!(flags & 0x10))
                vm_->scale_2 = { 1, 1 };
            else
                vm_->scale_2 = (2 * player_scale__requires_flag_0x10__from_ddc - 1.f) * ns::vec2(1, 1);
            vm_->bitflags.scaled = 0b1;
        }
        spd = (direction > 4) ? speed_focus_diag : speed_focus;
    }

    // calculate directionnal speed
    float xspd = (PlayerMovement__dirTable[direction * 2] * spd + 128.f * __could_be_subsun_pull_direction.x) * speed_multiplier;
    float yspd = (PlayerMovement__dirTable[direction * 2 + 1] * spd + 128.f * __could_be_subsun_pull_direction.y) * speed_multiplier;

    // update player vm script
    bool fst_update = false;
    if (xspd < 0 && speed.x > -1) {
        playerAnm->copyFromLoaded(&vm, 1);
        fst_update = true;
    } else if (xspd == 0 && speed.x > 0) {
        playerAnm->copyFromLoaded(&vm, 4);
        fst_update = true;
    } else if (xspd > 0 && speed.x < 1) {
        playerAnm->copyFromLoaded(&vm, 3);
        fst_update = true;
    } else if (xspd == 0 && speed.x < 0) {
        playerAnm->copyFromLoaded(&vm, 2);
        fst_update = true;
    }
    if (fst_update) {
        vm.parent_vm = nullptr;
        vm.__root_vm__or_maybe_not = nullptr;
        vm.update();
    }

    // move according to speed
    speed.x = xspd;
    speed.y = yspd;
    last_movement = { speed * ns::get_instance()->game_speed(), 0 };
    last_movement_cpy = last_movement;
    if (direction != 0)
        last_movement_no_zero = last_movement;
    inner.integer_pos.x += last_movement.x;
    inner.integer_pos.y += last_movement.y;

    // screen boundaries
    if (inner.integer_pos.x < -0x5c00)
        inner.integer_pos.x = -0x5c00;
    else if (inner.integer_pos.x > 0x5c00)
        inner.integer_pos.x = 0x5c00;
    if (inner.integer_pos.y < 0x1000)
        inner.integer_pos.y = 0x1000;
    else if (inner.integer_pos.y > 0xd800)
        inner.integer_pos.y = 0xd800;

    // update real pos
    inner.pos = ns::vec3(inner.integer_pos.x, inner.integer_pos.y, 0) / 128.f;

    // set focus vm pos if exists
    if (auto vm_ = anm::getVM(inner.focus_eff_anmId); vm_)
        vm_->entity_pos = inner.pos;

    // previous pos used for trail options (TH17 youmu, may not work with TH10 marisa A)
    if (yspd != 0 || xspd != 0) {
        int i = 32;
        for (; i > inner.power * 8; i--)
            prev_pos[i] = prev_pos[inner.power * 8];

        for (; i > 0; i--)
            prev_pos[i] = prev_pos[i - 1];
    }
    prev_pos[0] = inner.integer_pos;

    // IDK
    if (flags & 2)
        __some_counter++;
    move_options(inner.options, 4);
    move_options(inner.subseason_options, 8);
    if (__some_counter >= 30)
        inner.power = 0;

    // th15 graze snowflake probably
    if (inner.__timer_187ac > 0) {
        auto some_vm = anm::getVM(inner.maybe_th15_graze_snowflake);
        if (!some_vm) {
            inner.maybe_th15_graze_snowflake =
                anm::getLoaded(8)->createEffectPos(27, 0, inner.pos, 14);
        } else {
            some_vm->entity_pos = inner.pos;
        }

        inner.__timer_187ac--;
        if (inner.__timer_187ac <= 0) {
            anm::interrupt_tree(inner.maybe_th15_graze_snowflake, 1);
            inner.__timer_187ac = 0;
        }
    }
}

void Player::try_kill()
{

    if ((flags & 8) == 0) { } // SoundManager::play_sound_centered(2);

    anm::getLoaded(8)->createEffectPos(0x1d, 0, inner.pos);

    if (SPELLCARD_PTR->flags & 1) {
        if (SPELLCARD_PTR->__timer_20 < 0x3c) {
            if (BOMB_PTR->active) SPELLCARD_PTR->flags |= 0x20;
        } else {
            SPELLCARD_PTR->bonus = 0;
            SPELLCARD_PTR->flags &= 0xffffffdd;
        }
    }

    inner.time_in_state = 0;
    inner.state = 4;
    inner.iframes = 6;
    playerAnm->copyFromLoaded(&vm, 0);
    vm.parent_vm = nullptr;
    vm.__root_vm__or_maybe_not = nullptr;
    vm.update();
}

void Player::die() {
    // add it to effectmanager
    anm::getLoaded(8)->createEffectPos(0x1c, 0, inner.pos);

    // update life & bombs
    GLOBALS.inner.CURRENT_LIVES--;
    if (GLOBALS.inner.bombpiece_related < 0) {
        GLOBALS.inner.CURRENT_BOMBS = 0;
    } else {
        GLOBALS.inner.CURRENT_BOMBS = GLOBALS.inner.bombpiece_related;
        if (8 < GLOBALS.inner.bombpiece_related) {
            GLOBALS.inner.CURRENT_BOMBS = 8;
        }
    }
    if (GLOBALS.inner.CURRENT_LIVES >= 0) {
        GUI_PTR->set_life_meter(GLOBALS.inner.CURRENT_LIVES,
                                GLOBALS.inner.CURRENT_LIFE_PIECES);
    }
    GUI_PTR->set_bomb_meter(GLOBALS.inner.CURRENT_BOMBS,
                            GLOBALS.inner.CURRENT_BOMB_PIECES);

    // update inner
    inner.state = 2;
    inner.time_in_state = 0;
    inner.iframes = 180;
    inner.power = 0;

    playerAnm->copyFromLoaded(&vm, 0);
    vm.parent_vm = nullptr;
    vm.__root_vm__or_maybe_not = nullptr;
    vm.update();

    // update options vms
    for (int i = 0; i < 4; i++) {
        anm::interrupt_tree(inner.options[i].anmId1, 1);
        anm::interrupt_tree(inner.options[i].anmId2, 1);
    }

    // update spell capture
    if ((SPELLCARD_PTR->flags & 1) != 0) {
        if (SPELLCARD_PTR->__timer_20 < 0x3c) {
            if (BOMB_PTR->active == 1)
                SPELLCARD_PTR->flags |= 0x20;
        } else {
            SPELLCARD_PTR->bonus = 0;
            SPELLCARD_PTR->flags &= 0xffffffdd;
        }
    }

    // update globals
    ENEMY_MANAGER_PTR->miss_count++;
    ENEMY_MANAGER_PTR->can_still_capture_spell = 0;
    if ((GLOBALS.inner.HYPER_FLAGS & 2) &&
        !(GLOBALS.inner.HYPER_FLAGS & 4)) GLOBALS.inner.HYPER_TIME = 1;
    if (GLOBALS.inner.MISS_COUNT_GLOBAL < 999999)
        GLOBALS.inner.MISS_COUNT_GLOBAL++;
}

int Player::_on_draw() {
    if (inner.state != 2) {
        vm.entity_pos = inner.pos;
        vm.bitflags.originMode = 0b01;
        anm::drawVM(&vm);
    }
    if (!ns::get_instance()->flags().debugInfo)
        return 1;
    ns::draw_set_layer(ns::DEBUG_LAYER_ID);
    static ns::Color c = { 255, 0, 0, 128 };
    static ns::Color c2 = { 255, 255, 0, 128 };
    for (int i = 0; i < 256; i++) {
        auto& d = inner.damage_sources[i];
        if (!(d.flags & 1))
            continue;
        if (d.field_0x84)
            ns::draw_rectangle_rotated_color(d.pos.pos.x, -d.pos.pos.y, d.hitbox.x, d.hitbox.y, -d.angle, c2, c2, c2, c2);
        else
            ns::draw_rectangle_rotated_color(d.pos.pos.x, -d.pos.pos.y, d.hitbox.x, d.hitbox.y, -d.angle, c, c, c, c);
    }
    return 1;
}

void FUN_0044d8a0(ns::vec3 const& param_1)
{

    if (GLOBALS.inner.HYPER_FLAGS & 2) {
        while (99 < PLAYER_PTR->field_0x190ec) {
            ITEM_MANAGER_PTR->total_items_created = ITEM_MANAGER_PTR->total_items_created + 1;
            if (ITEM_MANAGER_PTR->cancel_items_freelist_head.next != NULL) {
                ITEM_MANAGER_PTR->cancel_items_freelist_head.next->field_0xc6c = ITEM_MANAGER_PTR->field_0xe4b984;
                int uVar5 = ITEM_MANAGER_PTR->total_items_created;

                if (ITEM_MANAGER_PTR->field_0xe4b980 < 0x400) {
                    if (ITEM_MANAGER_PTR->field_0xe4b980 < 0x200) {
                        if (ITEM_MANAGER_PTR->field_0xe4b980 < 0x100) {
                            uVar5 &= 0x80000003;
                            if ((int)uVar5 < 0) {
                                uVar5 = ((uVar5 - 1) | 0xfffffffc) + 1;
                            }
                        } else {
                            uVar5 &= 0x80000007;
                            if ((int)uVar5 < 0) {
                                uVar5 = ((uVar5 - 1) | 0xfffffff8) + 1;
                            }
                            uVar5 += 4;
                        }
                    } else {
                        uVar5 &= 0x8000000f;
                        if ((int)uVar5 < 0) {
                            uVar5 = ((uVar5 - 1) | 0xfffffff0) + 1;
                        }
                        uVar5 += 8;
                    }
                } else {
                    uVar5 &= 0x8000001f;
                    if ((int)uVar5 < 0) {
                        uVar5 = ((uVar5 - 1) | 0xffffffe0) + 1;
                    }
                    uVar5 += 0x10;
                }

                auto i = ITEM_MANAGER_PTR->cancel_items_freelist_head.next;
                i->intangibility_frames = uVar5;
                i->state = 5;
                i->item_type = 9;
                i->__field_c60__init_to_item_type_but_only_for_piv_items = 9;
                i->position = param_1;
                i->velocity_angle = Random::Floatm11() * 0.5235988 - 1.570796;
                i->velocity_magnitude = Random::Float01() * 1.2 + 1.0;
                i->velocity = { math::lengthdir_vec(i->velocity_magnitude, i->velocity_angle), 0 };
                ITEM_MANAGER_PTR->cancel_items_freelist_head.next->time = 0;
                ITEM_MANAGER_PTR->cancel_items_freelist_head.next->__might_be_unused__force_autocollect__from_th16 = 0;
                if (i->node_in_free_list.next)
                    i->node_in_free_list.next->node_in_free_list.prev = i->node_in_free_list.prev;
                if (i->node_in_free_list.prev)
                    i->node_in_free_list.prev->node_in_free_list.next = i->node_in_free_list.next;
                i->node_in_free_list.next = nullptr;
                i->node_in_free_list.prev = nullptr;
                i->appear_sound = -1;
            }
            PLAYER_PTR->field_0x190ec -= 100;
        }
    }
    return;
}

std::function<int(PlayerDamageSource_t*, ns::vec3 const&, float, float, float)> hitFuncs[] = {
    nullptr,
    [](PlayerDamageSource_t* param_1, ns::vec3 const& param_2, float param_3, float param_4, float param_5) -> int {
        FUN_0044d8a0(param_2);
        auto& shooter = PLAYER_PTR->sht_file->shooters[PLAYER_PTR->inner.bullets[param_1->bullet_id].shter >> 8][PLAYER_PTR->inner.bullets[param_1->bullet_id].shter & 0xff];
        if (shooter.on_hit)
            return shooter.on_hit(&PLAYER_PTR->inner.bullets[param_1->bullet_id], param_2, param_3, param_4, param_5);
        if (GLOBALS.inner.HYPER_FLAGS & 2)
            PLAYER_PTR->field_0x190ec += 0x32;
        return pl_b_default_on_hit(PLAYER_PTR->inner.bullets[param_1->bullet_id]);
    },
    [](PlayerDamageSource_t* param_1, ns::vec3 const& param_2, float, float, float) -> int {
        FUN_0044d8a0((param_2 + param_1->pos.pos) / 2.f);
        int damage = PLAYER_PTR->inner.bullets[param_1->bullet_id].damage;
        (PLAYER_PTR->inner).bullets[param_1->bullet_id].damage = 1;
        return damage;
    },
    [](PlayerDamageSource_t*, ns::vec3 const& param_2, float, float, float) -> int {
        anm::VM* vm;
        PLAYER_PTR->playerAnm->createEffectPos(20, 0, param_2, -1, &vm);
        vm->__script_vars_33_34_35.x = ns::cos(BOMB_PTR->field_0x2c) * 6.5;
        vm->__script_vars_33_34_35.y = ns::sin(BOMB_PTR->field_0x2c) * 6.5;
        return -1;
    },
    [](PlayerDamageSource_t*, ns::vec3 const&, float, float, float) { if (GOAST_MANAGER_PTR) GOAST_MANAGER_PTR->field_0x50 = 20; return -1; }
};

int applyDamage(ns::vec3 const& enemyPos, ns::vec2* enemy_hb, float hb_size, uint* param_4, ns::vec3& dmg_pos, int param_6, int enemy_id, float ang)
{

    float local_4c = BOMB_PTR->active ? BOMB_PTR->method_c(/*enemyPos,enemy_hb*/) : 0;

    if (param_4)
        *param_4 = local_4c > 0;

    for (int i = 0; i < 256; i++) {
        auto& ds = PLAYER_PTR->inner.damage_sources[i];
        if ((PLAYER_PTR->inner.damage_sources[i].flags & 1) && (PLAYER_PTR->inner.damage_sources[i].time_to_live % PLAYER_PTR->inner.damage_sources[i].field_0x80 == 0) && PLAYER_PTR->inner.damage_sources[i].field_0x88 < 1) {

            if ((PLAYER_PTR->inner.damage_sources[i].flags & 2) == 0) {
                if (enemy_hb == NULL) {
                    float yy = (enemyPos.y - ds.pos.pos.y) * ns::cos(-ds.angle) + (enemyPos.x - ds.pos.pos.x) * ns::sin(-ds.angle);
                    float xx = (enemyPos.x - ds.pos.pos.x) * ns::cos(-ds.angle) - (enemyPos.y - ds.pos.pos.y) * ns::sin(-ds.angle);
                    if (((ds.hitbox.x / 2 + hb_size < ns::abs(xx)) || (ds.hitbox.y / 2 < ns::abs(yy))) && ((ds.hitbox.x / 2 < ns::abs(xx) || (ds.hitbox.y / 2 + hb_size < ns::abs(yy))))) {
                        if ((hb_size * hb_size <= (xx - ds.hitbox.x / 2) * (xx - ds.hitbox.x / 2) + (yy - ds.hitbox.y / 2) * (yy - ds.hitbox.y / 2)) && (hb_size * hb_size <= (xx + ds.hitbox.x / 2) * (xx + ds.hitbox.x / 2) + (yy - ds.hitbox.y / 2) * (yy - ds.hitbox.y / 2)) && (hb_size * hb_size <= (xx - ds.hitbox.x / 2) * (xx - ds.hitbox.x / 2) + (yy + ds.hitbox.y / 2) * (yy + ds.hitbox.y / 2)) && (hb_size * hb_size <= (xx + ds.hitbox.x / 2) * (xx + ds.hitbox.x / 2) + (yy + ds.hitbox.y / 2) * (yy + ds.hitbox.y / 2)))
                            continue;
                    }
                } else
                    continue; // if (!FUN_00404320(&ds,ds.angle,enemyPos.x,enemyPos.y,enemy_hb->x,enemy_hb->y,ang)) continue;
            } else if (!enemy_hb) {
                if ((hb_size + ds.field_0x4) * (hb_size + ds.field_0x4) < (ds.pos.pos.y - enemyPos.y) * (ds.pos.pos.y - enemyPos.y) + (ds.pos.pos.x - enemyPos.x) * (ds.pos.pos.x - enemyPos.x))
                    continue;
            } else {
                float xx = (ds.pos.pos.x - enemyPos.x) * ns::cos(-ang) - (ds.pos.pos.y - enemyPos.y) * ns::sin(-ang);
                float yy = (ds.pos.pos.y - enemyPos.y) * ns::cos(-ang) + (ds.pos.pos.x - enemyPos.x) * ns::sin(-ang);
                if (((ds.field_0x4 + enemy_hb->x / 2 < ns::abs(xx)) || (enemy_hb->y / 2 < ns::abs(yy))) && ((enemy_hb->x / 2 < ns::abs(xx) || (enemy_hb->y / 2 + ds.field_0x4 < ns::abs(yy))))) {
                    if ((ds.field_0x4 * ds.field_0x4 <= (yy - enemy_hb->y / 2) * (yy - enemy_hb->y / 2) + (xx - enemy_hb->x / 2) * (xx - enemy_hb->x / 2)) && (ds.field_0x4 * ds.field_0x4 <= (yy - enemy_hb->y / 2) * (yy - enemy_hb->y / 2) + enemy_hb->x / 2) && (ds.field_0x4 * ds.field_0x4 <= (xx - enemy_hb->x / 2) * (xx - enemy_hb->x / 2) + enemy_hb->y / 2) && (ds.field_0x4 * ds.field_0x4 <= (yy + enemy_hb->y / 2) * (yy + enemy_hb->y / 2) + (xx + enemy_hb->x / 2) * (xx + enemy_hb->x / 2)))
                        continue;
                }
            }

            if (ds.field_0x8c - 1U < 4) {
                // if ((*(int *)(local_34 + ds.field_0x8c * 4 + 0xc) != 0) && ((int)PLAYER_PTR->inner.damage_sources[i].damage < *(int *)(local_44 + ds.field_0x8c * 4 + 0xc))) continue;
                // local_4c -= *(int *)(local_44 + ds.field_0x8c * 4 + -4);
                //*(undefined4 *)(local_34 + ds.field_0x8c * 4 + 0xc) = 1;
                //*(uint32_t *)(local_44 + ds.field_0x8c * 4 + 0xc) = PLAYER_PTR->inner.damage_sources[i].damage;
            }

            if (enemy_id != 0) {
                if (ds.field_0x84 == enemy_id)
                    continue;
                ds.field_0x84 = enemy_id;
            }

            if (param_4 && (ds.flags & 4))
                *param_4 = 1;

            int uVar3 = ds.damage;
            if (param_6 == 0) {
                if (ds.ds_on_hit != 0) {
                    ds.field_0x94 = enemy_id;
                    int r = hitFuncs[ds.ds_on_hit](&ds, enemyPos, enemy_hb ? enemy_hb->x : hb_size, ang, hb_size);
                    if (r >= 0)
                        ds.field_0x78 += r;
                    else
                        ds.field_0x78 += ds.damage;
                } else
                    ds.field_0x78 += ds.damage;
            }

            if (PLAYER_PTR->inner.damage_sources[i].field_0x8c - 1 < 4) {
                //*(uint32_t *)(local_44 + PLAYER_PTR->inner.damage_sources[i].field14_0x8c * 4 + -4) = uVar3;
            }

            local_4c += uVar3;
            dmg_pos = ds.pos.pos;

            if ((ds.field_0x7c < 9999999) && (ds.field_0x7c <= ds.field_0x78)) {
                ds.flags &= 0xfffffffe;
                ds.damage = 0;
            }
        }
    }

    local_4c = math::min(local_4c, (float)PLAYER_PTR->sht_file->header.max_dmg);
    if (((param_6 == 0) && local_4c)) {
        GLOBALS.inner.CURRENT_SCORE += (local_4c / 10 + 10) / 10;
        if (999999999 < GLOBALS.inner.CURRENT_SCORE)
            GLOBALS.inner.CURRENT_SCORE = 999999999;
    }
    return local_4c;
}


void Player::do_graze(ns::vec3 const& pos) {
    GLOBALS.inner.GRAZE += 1;
    GLOBALS.inner.GRAZE_IN_CHAPTER__POSSIBLY_BROKEN += 1;
    if (99999999 < GLOBALS.inner.GRAZE) {
        GLOBALS.inner.GRAZE = 99999999;
    }
    if (99999999 < GLOBALS.inner.GRAZE_IN_CHAPTER__POSSIBLY_BROKEN) {
        GLOBALS.inner.GRAZE_IN_CHAPTER__POSSIBLY_BROKEN = 99999999;
    }
    ns::vec3 graze_pos;
    graze_pos.x = (pos.x + PLAYER_PTR->inner.pos.x) / 2.f;
    graze_pos.y = (pos.y + PLAYER_PTR->inner.pos.y) / 2.f;
    graze_pos.z = 0.f;
    anm::getLoaded(8)->createEffectPos(24, 0, graze_pos);
    POPUP_MANAGER_PTR->generate_small_score_popup(graze_pos,
        GLOBALS.inner.GRAZE_IN_CHAPTER__POSSIBLY_BROKEN,
        ns::Color{0xc0,0xc0,0xff,0xff});
    // SoundManager::play_sound_at_position(0x2a);
    return;
}
