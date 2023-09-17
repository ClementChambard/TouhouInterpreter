#include "./LaserLine.h"
#include "./LaserManager.h"
#include "../BulletManager.h"
#include "../ItemManager.h"
#include "../Player.h"
#include <math/Random.h>

LaserLine::LaserLine() {
    vm1.sprite_id = -1;
    vm1.instr_offset = -1;
    vm2.sprite_id = -1;
    vm2.instr_offset = -1;
    vm3.sprite_id = -1;
    vm3.instr_offset = -1;
}

int LaserLine::initialize(void* arg) {

    inner = *reinterpret_cast<LaserLineInner_t*>(arg);

    kind = 0;
    bullet_type = inner.bullet_type;
    bullet_color = inner.bullet_color;
    __field_10__set_to_3_by_ex_delete = 2;

    vm1.reset();
    vm1.index_of_sprite_mapping_func = LASER_ON_SPRITE_SET_FUNC;
    vm1.associated_game_entity = this;
    LASER_MANAGER_PTR->bullet_anm->load_external_vm(
        &vm1, BULLET_TYPE_TABLE[bullet_type]["script"].asInt());
    vm1.interrupt(2);
    vm1.update();
    vm1.bitflags.originMode = 1;
    vm1.bitflags.blendmode = 1;
    vm1.bitflags.rendermode = 1;
    vm1.bitflags.anchorY = 2;

    LASER_MANAGER_PTR->bullet_anm->copyFromLoaded(&vm2,
                inner.bullet_color + LASER_DATA["spawn_anm_first"].asInt());
    vm2.parent_vm = nullptr;
    vm2.__root_vm__or_maybe_not = nullptr;
    vm2.update();
    vm2.interrupt(2);
    vm2.update();
    vm2.bitflags.originMode = 1;
    vm2.bitflags.blendmode = 1;
    vm2.bitflags.rendermode = 1;

    if ((bullet_type < 18) || (bullet_type == 38)) {
        LASER_MANAGER_PTR->bullet_anm->copyFromLoaded(&vm3,
                                                      bullet_color + 0x5b);
        vm3.parent_vm = nullptr;
        vm3.__root_vm__or_maybe_not = nullptr;
        vm3.update();
        vm3.bitflags.blendmode = 1;
    } else {
        LASER_MANAGER_PTR->bullet_anm->copyFromLoaded(&vm3,
                                                      bullet_color + 0x53);
        vm3.parent_vm = nullptr;
        vm3.__root_vm__or_maybe_not = nullptr;
        vm3.update();
    }
    vm3.bitflags.originMode = 1;

    // TODO: no
    vm1.layer = 14;
    vm2.layer = 14;
    vm3.layer = 14;

    // if (-1 < inner.shot_sfx) SoundManager::play_sound_at_position(inner.shot_sfx);

    __timer_2c = 0;
    time_alive = 0;
    inv_timer = 30;
    ex_offscreen__timer = 3;

    laser_offset = inner.start_pos;
    if (inner.distance != 0.f) {
        laser_offset.x += inner.distance * cos(inner.ang_aim);
        laser_offset.y += inner.distance * sin(inner.ang_aim);
    }

    speed = inner.spd_1;
    angle = inner.ang_aim;
    laser_inf_current_length = inner.laser_new_arg_1;
    laser_st_width = inner.laser_new_arg_4;

    __field_7c__sometimes_0p01_or_0f = (inner.laser_new_arg_2 < inner.laser_new_arg_1) ? 0.01f : 0.f;

    laser_speed = { math::lengthdir_vec(speed, angle), 0.f };

    et_ex_index = inner.et_ex_index;

    return 0;
}

int LaserLine::on_tick() {

    int finished_ex = 0;

    do {
        run_ex();
        if (!flags)
            break;
        finished_ex = 0;
        if (flags & 4) {
            if (ex_accel.timer >= ex_accel.duration) {
                finished_ex++;
                flags &= 0xfffffffb;
            } else {
                speed += ex_accel.acceleration * GAME_SPEED;
                laser_speed += ex_accel.vec3_a14 * GAME_SPEED;
                if (fabs(laser_speed.x) > 0.0001 || fabs(laser_speed.y) > 0.0001) {
                    speed = math::point_distance(0, 0, laser_speed.x, laser_speed.y);
                    angle = math::point_direction(0, 0, laser_speed.x, laser_speed.y);
                    math::angle_normalize(angle);
                }
                ex_accel.timer++;
            }
        }
        if (flags & 0x10) {
            if (ex_angle.aim_type == 0) {
                if (ex_angle.timer >= ex_angle.duration) {
                    laser_speed = { math::lengthdir_vec(speed - (ex_angle.timer * speed) / (float)ex_angle.duration, angle), 0.f };
                } else {
                    // if (-1 < inner.shot_transform_sfx) SoundManager::play_sound_centered(inner.shot_transform_sfx);
                    ex_angle.turns_so_far++;
                    angle += ex_angle.angle;
                    speed = ex_angle.speed;
                    ex_angle.timer = 0;
                    laser_speed = { math::lengthdir_vec(speed, angle), 0.f };
                    if (ex_angle.max_turns <= ex_angle.turns_so_far) {
                        flags &= 0xffffffef;
                        finished_ex++;
                    }
                }
                ex_angle.timer++;
            }
        }
        if (flags & 0x40)
            finished_ex += method_50_line();
        if (flags < 0) {
            if (ex_wait.timer > 0)
                ex_wait.timer--;
            else {
                finished_ex++;
                flags &= 0x7fffffff;
            }
        }
        if (ex_invuln__remaining_frames != 0)
            ex_invuln__remaining_frames--;
    } while (finished_ex != 0);

    if (inner.laser_new_arg_2 <= laser_inf_current_length) {
        __field_7c__sometimes_0p01_or_0f = speed * GAME_SPEED + __field_7c__sometimes_0p01_or_0f;
        laser_offset += laser_speed * GAME_SPEED;
        if ((0.0 < inner.laser_new_arg_3) && (inner.laser_new_arg_3 < __field_7c__sometimes_0p01_or_0f + laser_inf_current_length)) {
            laser_inf_current_length = inner.laser_new_arg_3 - __field_7c__sometimes_0p01_or_0f;
            inner.laser_new_arg_2 = laser_inf_current_length;
            if (laser_inf_current_length <= 0.0) {
                return 1;
            }
        }
    } else {
        laser_inf_current_length += speed * GAME_SPEED;
        if (inner.laser_new_arg_2 < laser_inf_current_length) {
            laser_inf_current_length = inner.laser_new_arg_2;
        }
    }

    if (inv_timer <= 0) {
        if (ex_offscreen__timer > 0)
            ex_offscreen__timer--;
        else {
            glm::vec2 b = math::lengthdir_vec(angle, laser_inf_current_length);
            if ((laser_offset.x + laser_st_width <= -192.0 || 192.0 <= laser_offset.x - laser_st_width || laser_offset.y + laser_st_width <= 0.0 || 448.0 <= laser_offset.y - laser_st_width) && (laser_offset.x + b.x + laser_st_width <= -192.0 || 192.0 <= laser_offset.x + b.x - laser_st_width || laser_offset.y + b.y + laser_st_width <= 0.0 || 448.0 <= laser_offset.y + b.y - laser_st_width)) {
                return 1;
            }
        }
    } else {
        inv_timer--;
        if (ex_offscreen__timer > 0)
            ex_offscreen__timer--;
    }

    check_graze_or_kill(0);

    auto spr = vm1.getSprite();
    vm1.bitflags.scaled = true;
    vm1.scale.x = laser_st_width / spr.w;
    vm1.scale.y = laser_inf_current_length / spr.h;
    vm1.update();
    if (__field_7c__sometimes_0p01_or_0f == 0.f)
        vm2.update();
    vm3.update();
    time_alive++;
    return 0;
}

int LaserLine::on_draw() {
    float vmAngle = angle + 1.570796;
    math::angle_normalize(vmAngle);
    vm1.pos = laser_offset;
    vm1.bitflags.rotated = true;
    vm1.rotation.z = vmAngle;
    // vm1.draw();
    AnmManager::drawVM(&vm1);
    vm3.pos = laser_offset + glm::vec3(
        math::lengthdir_vec(laser_inf_current_length, angle), 0.f);
    // vm3.draw();
    AnmManager::drawVM(&vm3);
    if (__field_7c__sometimes_0p01_or_0f == 0.0) {
        vm2.pos = laser_offset;
        // vm2.draw();
        AnmManager::drawVM(&vm2);
    }
    return 0;
}

void LaserLine::run_ex() {
    while (et_ex_index < 17 && inner.et_ex[et_ex_index].type &&
        !(inner.et_ex[et_ex_index].slot == 0 && flags != 0)) {
        if (inner.et_ex[et_ex_index].type == 1) {
            flags |= 1;
            ex_speedup.timer = 0;
            ex_speedup.unused_0 = 0.0;
        }

        // NO 2

        if (inner.et_ex[et_ex_index].type == 4) {
            flags |= 4;
            ex_accel.acceleration = inner.et_ex[et_ex_index].r;
            ex_accel.angle = inner.et_ex[et_ex_index].s;
            if (inner.et_ex[et_ex_index].s <= -990.0)
                ex_accel.angle = angle;
            if (990.0 <= inner.et_ex[et_ex_index].s)
                ex_accel.angle = math::point_direction(laser_offset.x,
                                                       laser_offset.y,
                                                       PLAYER_PTR->inner.pos.x,
                                                       PLAYER_PTR->inner.pos.y);
            ex_accel.timer = 0;
            ex_accel.duration = inner.et_ex[et_ex_index].a;
            ex_accel.vec3_a14 = { math::lengthdir_vec(
                ex_accel.acceleration, ex_accel.angle), 0.f };
            if (et_ex_index != 0 && inner.shot_transform_sfx >= 0) {
                // SoundManager::play_sound_centered(inner.shot_transform_sfx);
            }
        }

        if (inner.et_ex[et_ex_index].type == 8) {
            flags |= 8;
            ex_angleAccel.tangential_accel = inner.et_ex[et_ex_index].r;
            ex_angleAccel.angular_velocity = inner.et_ex[et_ex_index].s;
            ex_angleAccel.timer = 0;
            ex_angleAccel.duration = inner.et_ex[et_ex_index].a;
            if (et_ex_index != 0 && inner.shot_transform_sfx >= 0) {
                // SoundManager::play_sound_centered(inner.shot_transform_sfx);
            }
        }

        if (inner.et_ex[et_ex_index].type == 0x10) {
            flags |= 0x10;
            ex_angle.angle = inner.et_ex[et_ex_index].r;
            ex_angle.speed = inner.et_ex[et_ex_index].s;
            if (ex_angle.speed <= -999.0)
                ex_angle.speed = speed;
            ex_angle.timer = 0;
            ex_angle.turns_so_far = 0;
            ex_angle.duration = inner.et_ex[et_ex_index].a;
            ex_angle.max_turns = inner.et_ex[et_ex_index].b;
            ex_angle.aim_type = inner.et_ex[et_ex_index].c;
        }

        // NO 20

        if (inner.et_ex[et_ex_index].type == 0x40) {
            if (inner.et_ex[et_ex_index].a > 0) {
                flags |= 0x40;
                ex_bounce.bounce_speed = inner.et_ex[et_ex_index].r;
                if (ex_bounce.bounce_speed < 0.0)
                    ex_bounce.bounce_speed = speed;
                inner.et_ex[et_ex_index].a--;
                ex_bounce.rebounds_so_far = 0;
                ex_bounce.max_rebounds = inner.et_ex[et_ex_index].a;
                ex_bounce.for_laser_line = inner.et_ex[et_ex_index].b;
            }
        }

        if (inner.et_ex[et_ex_index].type == 0x80) {
            ex_invuln__remaining_frames = inner.et_ex[et_ex_index].a;
        }

        if (inner.et_ex[et_ex_index].type == 0x100) {
            ex_offscreen__timer = inner.et_ex[et_ex_index].a;
        }

        if (inner.et_ex[et_ex_index].type == 0x200) {
            LASER_MANAGER_PTR->bullet_anm->copyFromLoaded(&vm1,
                BULLET_TYPE_TABLE[inner.et_ex[et_ex_index].a]["script"].asInt()
                + inner.et_ex[et_ex_index].b);
            vm1.parent_vm = nullptr;
            vm1.__root_vm__or_maybe_not = nullptr;
            vm1.update();
        }

        if (inner.et_ex[et_ex_index].type == 0x400) {
            __field_10__set_to_3_by_ex_delete = 3;
        }

        if (inner.et_ex[et_ex_index].type == 0x800) {
            // SoundManager::play_sound_at_position(inner.et_ex[et_ex_index].a);
        }

        if (inner.et_ex[et_ex_index].type == 0x1000) {
            flags |= 0x1000;
            ex_wrap.__timer__used_by_lasers = inner.et_ex[et_ex_index].a;
        }

        if (inner.et_ex[et_ex_index].type == 0x2000) {
            EnemyBulletShooter_t bs;
            bs.__shot_transform_sfx = -1;
            bs.__vec3_8 = { glm::vec2(laser_offset) + math::lengthdir_vec(
                laser_inf_current_length, angle), 0.f };
            bs.aim_type = inner.et_ex[et_ex_index].a;
            bs.__start_transform = inner.et_ex[et_ex_index].b;
            bs.cnt_count = inner.et_ex[et_ex_index].c;
            bs.cnt_layers = inner.et_ex[et_ex_index].d;
            bs.ang_aim = inner.et_ex[et_ex_index].r;
            bs.ang_bullet_dist = inner.et_ex[et_ex_index].s;
            bs.spd1 = inner.et_ex[et_ex_index].m;
            bs.spd2 = inner.et_ex[et_ex_index].n;
            if (bs.ang_aim <= -999990.0)
                bs.ang_aim = angle;
            if (999990.0 <= bs.ang_aim)
                bs.ang_aim = math::point_direction(laser_offset.x,
                                                   laser_offset.y,
                                                   PLAYER_PTR->inner.pos.x,
                                                   PLAYER_PTR->inner.pos.y);
            if (bs.spd1 <= -999990.0)
                bs.spd1 = speed;
            et_ex_index++;
            bs.type = inner.et_ex[et_ex_index].a;
            bs.__color = inner.et_ex[et_ex_index].b;
            bs.sfx_flag = 0;
            for (int i = 0; i < 20; i++)
                bs.ex[i] = inner.et_ex[i];
            BulletManager::GetInstance()->Shoot(&bs);
            if (inner.et_ex[et_ex_index].c != 0)
                cancel(0, 0);
        }

        if (inner.et_ex[et_ex_index].type == 0x8000) {
            laser_id = inner.et_ex[et_ex_index].a;
        }

        if (inner.et_ex[et_ex_index].type == 0x10000) {
            et_ex_index = inner.et_ex[et_ex_index].a;
            continue;
        }

        if (inner.et_ex[et_ex_index].type == 0x100000) {
            if (inner.et_ex[et_ex_index].a == 0)
                vm1.bitflags.blendmode = 0;
            else
                vm1.bitflags.blendmode = 1;
            et_ex_index++;
        }

        if (inner.et_ex[et_ex_index].type == (int32_t)0x80000000) {
            flags |= 0x80000000;
            ex_wait.timer = inner.et_ex[et_ex_index].a;
        }

        et_ex_index++;
    }
}

int FUN_004038d0(float* xout, float* yout, float lx1, float ly1, float lx2,
    float ly2, float x1, float y1, float x2, float y2) {
    float fVar6;
    float fVar7;

    fVar6 = (ly2 - y2) * (x2 - x1) + (x2 - lx2) * (y2 - y1);
    fVar7 = (x2 - lx1) * (y2 - y1) + (ly1 - y2) * (x2 - x1);
    if (fVar7 * fVar6 <= 0.0) {
        float fVar3;
        float fVar4;
        float fVar5;
        if ((fVar6 == 0.0) || (fVar7 != 0.0)) {
            if (0.0 < ((y2 - ly2) * (lx2 - lx1) + (lx2 - x2) * (ly2 - ly1))
                * ((y1 - ly2) * (lx2 - lx1) + (lx2 - x1) * (ly2 - ly1))) {
                return 0;
            }
        } else if (fmax(lx1, lx2) < fmin(x1, x2) || fmax(ly1, ly2) <
                fmin(y1, y2) || fmax(x1, x2) < fmin(lx1, lx2) ||
                fmax(y1, y2) < fmin(ly1, ly2)) {
            return 0;
        }
        if (0.01 <= fabs(x1 - x2)) {
            fVar3 = (y1 - y2) / (x1 - x2);
            fVar7 = y2 - ((y1 - y2) * x2) / (x1 - x2);
        } else {
            fVar3 = 0.0;
            fVar7 = x2;
        }
        fVar4 = 0.0;
        fVar5 = lx2;
        if (0.01 <= fabs(lx1 - lx2)) {
            fVar4 = (ly1 - ly2) / (lx1 - lx2);
            fVar5 = ly2 - ((ly1 - ly2) * lx2) / (lx1 - lx2);
        }
        if (0.01 <= fabs(x1 - x2)) {
            if (0.01 <= fabs(lx1 - lx2)) {
                *xout = (fVar5 - fVar7) / (fVar3 - fVar4);
                *yout = ((fVar5 - fVar7) * fVar3) / (fVar3 - fVar4) + fVar7;
                return 1;
            }
            *xout = lx2;
            *yout = fVar3 * lx2 + fVar7;
            return 1;
        }
        if (0.01 <= fabs(lx1 - lx2)) {
            *xout = x2;
            *yout = fVar4 * x2 + fVar5;
            return 1;
        }
        if (fabs(x2 - lx2) < 0.001) {
            *xout = x2;
            *yout = y2;
            return 1;
        }
    }
    return 0;
}

int LaserLine::method_50_line() {
    glm::vec3 laser_pos2 = { glm::vec2(laser_offset) +
        math::lengthdir_vec(laser_inf_current_length, angle), 0.f };
    if (-192.0 < laser_pos2.x + 0.0 && laser_pos2.x - 0.0 < 192.0 &&
        0.0 < laser_pos2.y + 0.0 && laser_pos2.y - 0.0 < 448.0)
        return 0;
    bool has_bounced = false;
    if (((ex_bounce.for_laser_line & 1) != 0) && (laser_pos2.y < 0.0)) {
        if ((ex_bounce.for_laser_line & 0x10) == 0) {
            FUN_004038d0(&inner.start_pos.x, &inner.start_pos.y, laser_offset.x,
                laser_offset.y, laser_pos2.x, laser_pos2.y, 256.0, 0.0,
                         -256.0, 0.0);
            inner.start_pos.z = 0.0;
            inner.ang_aim = -angle;
            inner.spd_1 = ex_bounce.bounce_speed;
            inner.distance = 0.0;
            allocate_new_laser(0, &inner);
        }
        has_bounced = true;
    }
    if (((ex_bounce.for_laser_line & 2) != 0) && (448.0 < laser_pos2.y)) {
        if ((ex_bounce.for_laser_line & 0x10) == 0) {
            FUN_004038d0(&inner.start_pos.x, &inner.start_pos.y, laser_offset.x,
                laser_offset.y, laser_pos2.x, laser_pos2.y, 256.0, 448.0,
                         -256.0, 448.0);
            inner.start_pos.z = 0.0;
            inner.ang_aim = -angle;
            inner.spd_1 = ex_bounce.bounce_speed;
            inner.distance = 0.0;
            allocate_new_laser(0, &inner);
        }
        has_bounced = true;
    }
    if (((ex_bounce.for_laser_line & 4) != 0) && (laser_pos2.x < -192.0)) {
        if ((ex_bounce.for_laser_line & 0x10) == 0) {
            FUN_004038d0(&inner.start_pos.x, &inner.start_pos.y, laser_offset.x,
                laser_offset.y, laser_pos2.x, laser_pos2.y, -192.0, 640.0,
                         -192.0, -192.0);
            inner.start_pos.z = 0.0;
            inner.ang_aim = PI - angle;
            inner.spd_1 = ex_bounce.bounce_speed;
            inner.distance = 0.0;
            math::angle_normalize(inner.ang_aim);
            allocate_new_laser(0, &inner);
        }
        has_bounced = true;
    }
    if (((ex_bounce.for_laser_line & 8) == 0) || (laser_pos2.x <= 192.0)) {
        if (!has_bounced) {
            return 0;
        }
    } else if ((ex_bounce.for_laser_line & 0x10) == 0) {
        FUN_004038d0(&inner.start_pos.x, &inner.start_pos.y, laser_offset.x,
            laser_offset.y, laser_pos2.x, laser_pos2.y, 192.0, 640.0,
                     192.0, -192.0);
        inner.start_pos.z = 0.0;
        inner.ang_aim = PI - angle;
        inner.spd_1 = ex_bounce.bounce_speed;
        inner.distance = 0.0;
        allocate_new_laser(0, &inner);
    }
    flags &= 0xffffffbf;
    if (-1 < inner.shot_transform_sfx) {
        /*SoundManager::play_sound_centered(inner.shot_transform_sfx);*/
    }
    return 1;
}

int LaserLine::cancel_as_bomb_rectangle(glm::vec3, glm::vec3, float,
                                        int, int) { return 0; }
int LaserLine::cancel_as_bomb_circle(glm::vec3 const&, float, int, int) { return 0; }
int LaserLine::cancel(int, int as_bomb)
{
    // undefined4 extraout_ECX;
    // undefined4 in_stack_ffffffa0;

    if (as_bomb && (ex_invuln__remaining_frames != 0))
        return 0;
    int n = 0;
    glm::vec3 inc = { math::lengthdir_vec(8.f, angle), 0.f };
    glm::vec3 p = laser_offset + inc;
    inc *= 2.f;
    if (laser_inf_current_length > 16.f) {
        float len = 8.0;
        do {
            if (bullet_type < 18 || bullet_type == 34 || bullet_type == 38) {
                BULLET_MANAGER_PTR->bullet_anm->createEffectPos(inner.bullet_color * 2 + 0xd1, 0, p);
            } else {
                if (bullet_type < 32) {
                    BULLET_MANAGER_PTR->bullet_anm->createEffectPos(inner.bullet_color * 2 + 0x101, 0, p);
                }
                if (bullet_type < 0x22) {
                    BULLET_MANAGER_PTR->bullet_anm->createEffectPos(inner.bullet_color * 2 + 0x119, 0, p);
                }
            }
            if (((as_bomb && (-192.0 < p.x + 32.0)) && ((p.x - 32.0 < 192.0 &&
                ((0.0 < p.y + 32.0 && (p.y - 32.0 < 448.0))))))) {
                BULLET_MANAGER_PTR->__unknown_cancel_counter++;
                ITEM_MANAGER_PTR->spawn_item(9, p, Random::Floatm11() *
                                             0.1745329 - 1.570796, 2.2, 0, -1);
            }
            n++;
            p += inc;
            len += 16.0;
        } while (len + 8.0 < laser_inf_current_length);
    }
    __field_10__set_to_3_by_ex_delete = 1;
    return n;
}
