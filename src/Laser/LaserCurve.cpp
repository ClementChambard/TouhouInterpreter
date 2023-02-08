#include "LaserCurve.h"
#include "../BulletManager.h"
#include "../Hardcoded.h"

LaserCurve::LaserCurve()
{
    vm1.sprite_id = -1;
    vm1.current_instr = -1;
    vm2.sprite_id = -1;
    vm2.current_instr = -1;
}

LaserCurve::~LaserCurve()
{
    LaserCurveTransform_t* node = transforms.next;
    while (node) {
        auto c = node->next;
        delete node;
        node = c;
    }
    if (nodes)
        delete[] nodes;
    if (vertices)
        delete[] vertices;
}

void LaserCurveTransform_t::posvel_from_prev(glm::vec3* out_pos, float* out_speed, float* out_angle, glm::vec3 const& pos_next, float speed_next, float angle_next, float /*time*/)
{
    if (move_type == 0) {
        *out_pos = pos_next - dir_vec * speed;
        *out_speed = speed;
        *out_angle = angle;
    }
    if (move_type == 1) {
        if (angle_accel < -990.0) {
            *out_pos = pos_next - dir_vec * (speed_next - accel);
            *out_speed = speed_next - accel;
            *out_angle = angle_next;
            return;
        }
        glm::vec3 v = { math::lengthdir_vec(-speed_next, angle_next) + math::lengthdir_vec(-accel, angle_accel), 0.f };
        *out_pos = pos_next + v;
        *out_speed = math::point_distance(0, 0, v.x, v.y);
        *out_angle = math::point_direction(0, 0, v.x, v.y);
    }
    if (move_type == 2) {
        float f = 1.f; // time - 0.f; // FUN_00497db0(time);
        *out_pos = pos_next - glm::vec3(math::lengthdir_vec(speed_next, angle_next), 0.f) * f;
        *out_speed = speed_next - accel;
        *out_angle = angle_next - angle_accel;
        // math::angle_normalize(*out_angle);
        //*out_pos -= glm::vec3(math::lengthdir_vec(*out_speed, *out_angle), 0.f) * (1.f - f);
    }
}

void LaserCurveTransform_t::posvel(glm::vec3* out_pos, float* out_speed, float* out_angle, float time)
{
    float time_in_trans = time - start_time;
    if (move_type == 0) {
        *out_pos = dir_vec * time_in_trans * speed + pos;
        *out_speed = speed;
        *out_angle = angle;
    }
    if (move_type == 1) {
        if (angle_accel < -990.0) {
            *out_pos = pos + dir_vec * speed * time_in_trans + 0.5f * accel * time_in_trans * time_in_trans;
            *out_speed = accel * time_in_trans + speed;
            *out_angle = angle;
            return;
        }
        glm::vec3 v = { math::lengthdir_vec(speed, angle) + math::lengthdir_vec(accel, angle_accel), 0.f };
        *out_pos = v * time_in_trans + pos; // this might be wrong : TODO: check
        *out_speed = math::point_distance(0, 0, v.x, v.y);
        *out_angle = math::point_direction(0, 0, v.x, v.y);
    }
    if (move_type == 2) {
        *out_angle = angle;
        *out_speed = speed;
        *out_pos = pos;
        for (int i = time_in_trans; i > 0; i--) {
            *out_angle += angle_accel;
            *out_speed += accel;
            // math::angle_normalize(*out_angle);
            *out_pos += glm::vec3(math::lengthdir_vec(*out_speed, *out_angle), 0.f);
        }
        //*out_pos += (time_in_trans /* - FUN_00497db0(time_in_trans) */) * glm::vec3(math::lengthdir_vec(*out_speed, *out_angle), 0.f);
    }
}

int LaserCurve::initialize(void* arg)
{
    inner = *reinterpret_cast<LaserCurveInner_t*>(arg);

    bullet_type = inner.type;
    bullet_color = inner.color;
    kind = 2;
    __field_10__set_to_3_by_ex_delete = 2;

    vm1.destroy();
    if (bullet_type == 1) {
        // AnmLoaded::load_external_vm(LASER_MANAGER_PTR->bullet_anm, vm1, LASER_DATA["laser_curve"]["anm_type_0"].asInt());
        vm1(AnmManagerN::getLoaded(7)->getPreloaded(LASER_DATA["laser_curve"]["anm_type_0"].asInt()));
    } else {
        // AnmLoaded::load_external_vm(LASER_MANAGER_PTR->bullet_anm, vm1, bullet_type + LASER_DATA["laser_curve"]["anm_first"].asInt());
        vm1(AnmManagerN::getLoaded(7)->getPreloaded(bullet_type + LASER_DATA["laser_curve"]["anm_first"].asInt()));
        vm1.on_set_sprite = [](AnmVM* vm, int) { return ((Laser*)vm->getEntity())->bullet_color + LASER_DATA["laser_curve"]["sprite_first"].asInt(); };
        vm1.entity = this;
    }
    vm1.interrupt(2);
    vm1.update();
    vm1.bitflags.blendmode = 1;
    vm1.bitflags.rendermode = 1;
    vm1.bitflags.anchorY = 0b10;
    vm1.bitflags.anchorX = 0b00;
    vm1.bitflags.originMode = 1;

    // anm_init_copy_vm_from_loaded(LASER_MANAGER_PTR->bullet_anm, vm2, inner.color + 0x38);
    vm2(AnmManagerN::getLoaded(7)->getPreloaded(inner.color + LASER_DATA["spawn_anm_first"].asInt()));
    vm2.parent = nullptr;
    // vm2.__root_vm__or_maybe_not = nullptr;
    vm2.update();
    vm2.interrupt(2);
    vm2.update();
    vm2.bitflags.blendmode = 1;
    vm2.bitflags.rendermode = 1;
    vm2.bitflags.originMode = 1;
    vm2.layer = 14; // TODO : not needed after

    laser_offset = inner.start_pos;
    if (inner.distance != 0.0) {
        laser_offset += glm::vec3(math::lengthdir_vec(inner.distance, inner.ang_aim), 0.f);
        inner.distance = 0.0;
        inner.start_pos = laser_offset;
    }

    laser_st_width = inner.laser_new_arg4;
    angle = inner.ang_aim;
    speed = inner.spd_1;
    laser_speed = { math::lengthdir_vec(inner.spd_1, inner.ang_aim), 0.f };

    __timer_2c = 0;
    time_alive = inner.timer40_start;
    inv_timer = 30;

    nodes = new LaserCurveNode_t[inner.laser_time_start];
    for (int i = 0; i < inner.laser_time_start; i++) {
        nodes[i].pos = laser_offset;
        nodes[i].v_speed = { 0, 0, 0 };
        nodes[i].angle = inner.ang_aim;
        nodes[i].speed = inner.spd_1;
    }
    nodes[0].v_speed = laser_speed;

    vertices = new NSEngine::Vertex[inner.laser_time_start * 2];

    if (!inner.transforms) {
        transforms.next = nullptr;
        transforms.start_time = 0;
        transforms.end_time = 999999;
        transforms.move_type = 0;
        transforms.dir_vec = { cos(angle), sin(angle), 0.f };
        transforms.pos = laser_offset;
        transforms.angle = angle;
        transforms.speed = speed;

        math::angle_normalize(transforms.angle);
        et_ex_index = inner.ex_index;
    } else {
        transforms = *inner.transforms;
        for (LaserCurveTransform_t *n = inner.transforms->next, *n2 = &transforms; n != NULL; n = n->next, n2 = n2->next) {
            n2->next = new LaserCurveTransform_t();
            *n2->next = *n->next;
            n2->next->prev = n2;
        }
        inner.transforms = nullptr;
        et_ex_index = 99;
    }

    for (int i = 0; i < inner.laser_time_start; i++) {
        int node_time_alive = time_alive - i;
        for (LaserCurveTransform_t* trans = &transforms; trans; trans = trans->next) {
            if (node_time_alive >= trans->start_time && node_time_alive < trans->end_time) {
                if (i == 0) {
                    trans->posvel(&nodes[i].pos, &nodes[i].speed, &nodes[i].angle, time_alive - i);
                } else {
                    trans->posvel_from_prev(&nodes[i].pos, &nodes[i].speed, &nodes[i].angle, nodes[i - 1].pos, nodes[i - 1].speed, nodes[i - 1].angle, time_alive - i);
                }
                break;
            }
        }
    }

    // if (inner.shot_sfx >= 0) { SoundManager::play_sound_at_position(inner.shot_sfx); }

    return 0;
}

int LaserCurve::on_tick()
{
    // return 1;
    //  float* pfVar8;
    //  float* extraout_EDX;
    //  double uStack_18;
    //  float fStack_10;

    float GAME_SPEED = 1.0f;

    int et_ex_done = 0;
    do {
        run_ex();
        if (!flags)
            break;
        et_ex_done = 0;
        if (flags & 4) {
            if (ex_accel.timer < ex_accel.duration) {
                speed = ex_accel.acceleration * GAME_SPEED + speed;
                laser_speed += ex_accel.vec3_a14 * GAME_SPEED;
                angle = math::point_direction(0, 0, laser_speed.x, laser_speed.y);
                ex_accel.timer++;
            } else {
                et_ex_done++;
                flags &= 0xfffffffb;
            }
        }
        if (flags & 8) {
            if (ex_angleAccel.timer < ex_angleAccel.duration) {
                angle += ex_angleAccel.angular_velocity * GAME_SPEED;
                math::angle_normalize(angle);
                speed += ex_angleAccel.tangential_accel * GAME_SPEED;
                laser_speed = { math::lengthdir_vec(speed, angle), 0.f };
                ex_angleAccel.timer++;
            } else {
                et_ex_done++;
                flags &= 0xfffffff7;
            }
        }
        if (flags & 0x10) {
            if (ex_angle.aim_type == 0) {
                if (ex_angle.timer < ex_angle.duration)
                    laser_speed = { math::lengthdir_vec(speed - (ex_angle.timer * speed) / (float)ex_angle.duration, angle), 0.f };
                else {
                    // if (inner.shot_transform_sfx >= 0) SoundManager::play_sound_centered(inner.shot_transform_sfx);
                    ex_angle.turns_so_far++;
                    angle += ex_angle.angle;
                    speed = ex_angle.speed;
                    ex_angle.timer = 0;
                    laser_speed = { math::lengthdir_vec(speed, angle), 0.f };
                    if (ex_angle.turns_so_far >= ex_angle.max_turns) {
                        flags &= 0xffffffef;
                        et_ex_done++;
                    }
                }
                ex_angle.timer++;
            }
        }
        if (flags & 0x100) {
            ex_offscreen.timer--;
            if (ex_offscreen.timer < 1) {
                flags &= 0xfffffeff;
                et_ex_done++;
            }
            flags &= 0xfffffeff;
        }
        if (flags < 0) {
            if (ex_wait.timer <= 0) {
                et_ex_done++;
                flags &= 0x7fffffff;
            } else
                ex_wait.timer--;
        }
        if (ex_invuln__remaining_frames != 0) {
            ex_invuln__remaining_frames--;
        }
    } while (et_ex_done != 0);

    if (!(some_flags & 0x10)) {
        for (int i = 0; i < inner.laser_time_start; i++) {
            if (time_alive - i < 0) {
                nodes[i].pos = inner.start_pos;
                nodes[i].v_speed = { 0, 0, 0 };
                nodes[i].angle = inner.ang_aim;
                nodes[i].speed = inner.spd_1;
            } else {
                for (auto thi = &transforms; thi; thi = thi->next) {
                    if ((time_alive - i >= thi->start_time) && (time_alive - i < thi->end_time)) {
                        if (i == 0.0)
                            thi->posvel(&nodes[i].pos, &nodes[i].speed, &nodes[i].angle, time_alive - i);
                        else
                            thi->posvel_from_prev(&nodes[i].pos, &nodes[i].speed, &nodes[i].angle, nodes[i - 1].pos, nodes[i - 1].speed, nodes[i - 1].angle, time_alive - i);
                    }
                }
            }
        }
    }

    if (inv_timer > 0 || offscreen_timer > 0 || (flags & 0x100)) {
        inv_timer--;
        offscreen_timer--;
        check_graze_or_kill(0);
        vm1.update();
        vm2.update();
        time_alive++;
        return 0;
    }

    for (int i = 0; i < inner.laser_time_start; i++) {

        // uStack_18 = laser_offset + glm::vec3(math::lengthdir_vec(laser_inf_current_length, angle), 0.f);
        if ((((-192.0 < laser_st_width + nodes[i].pos.x) && (nodes[i].pos.x - laser_st_width < 192.0)) && (0.0 < laser_st_width + nodes[i].pos.y)) && (nodes[i].pos.y - laser_st_width < 448.0)) {
            check_graze_or_kill(0);
            vm1.update();
            vm2.update();
            time_alive++;
            return 0;
        }
    }

    return 1;
}

#include <NSEngine.h>
void _dzaww(AnmVM* vm, NSEngine::Vertex* vertices, int count)
{
    for (int i = 0; i < (count - 1) / 2; i++)
        NSEngine::engineData::layers[14].draw(vm->getSprite().texID, vertices[i * 2], vertices[i * 2 + 1], vertices[i * 2 + 3], vertices[i * 2 + 2], vm->bitflags.blendmode);
}

int LaserCurve::on_draw()
{
    for (int i = 0; i < inner.laser_time_start; i++) {
        float ang = nodes[i].angle + PI1_2;
        if (i > 0)
            ang += (nodes[i - 1].angle + PI1_2 - ang) * 0.5;
        math::angle_normalize(ang);
        glm::vec2 offset = math::lengthdir_vec(inner.laser_new_arg4 * 0.5, ang);
        vertices[2 * i].position.x = nodes[i].pos.x + offset.x;
        vertices[2 * i].position.y = -(nodes[i].pos.y + offset.y);
        vertices[2 * i].position.z = 0.0;
        vertices[2 * i].uv.x = (float)i / (float)(inner.laser_time_start - 1);
        vertices[2 * i].uv.y = vm1.getSprite().v1; // vm1.uv_quad_of_sprite[0].y;
        vertices[2 * i].color = { 255, 255, 255, 255 };
        // vertices[2*i].position.x += SURF_ORIGIN_ECL_X;
        // vertices[2*i].position.y += DAT_00524720;

        vertices[2 * i + 1].position.x = nodes[i].pos.x - offset.x;
        vertices[2 * i + 1].position.y = -(nodes[i].pos.y - offset.y);
        vertices[2 * i + 1].position.z = 0.0;
        vertices[2 * i + 1].color = { 255, 255, 255, 255 };
        vertices[2 * i + 1].uv.x = (float)i / (float)(inner.laser_time_start - 1);
        vertices[2 * i + 1].uv.y = vm1.getSprite().v2; // vm1.uv_quad_of_sprite[2].y;
        // vertices[2*i+1].position.x += SURF_ORIGIN_ECL_X;
        // vertices[2*i+1].position.y += DAT_00524720;
    }
    // AnmManager::draw_vm__mode_textureCircle(ANM_MANAGER_PTR, &vm1, vertices, inner.laser_time_start * 2);
    _dzaww(&vm1, vertices, inner.laser_time_start * 2);
    if (time_alive <= inner.laser_time_start) {
        vm2.pos = nodes[inner.laser_time_start - 1].pos;
        vm2.draw();
    }
    return 0;
}

void LaserCurve::run_ex()
{
    while (et_ex_index < 18 && inner.ex[et_ex_index].type && (inner.ex[et_ex_index].slot || flags == 0) && !(inner.ex[et_ex_index].type & flags)) {

        if (inner.ex[et_ex_index].type == 0x400) {
            __field_10__set_to_3_by_ex_delete = 3;
        }

        if (inner.ex[et_ex_index].type < 0x41) {
            switch (inner.ex[et_ex_index].type) {
            case 1:
                flags |= 1;
                ex_speedup.timer = 0.0;
                break;
            default:
                break;
            case 4: {
                auto last_transform = &transforms;
                for (auto t = last_transform->next; t; t = t->next)
                    last_transform = t;
                last_transform->next = new LaserCurveTransform_t();
                last_transform->end_time = inner.ex[et_ex_index].b;
                last_transform->next->start_time = inner.ex[et_ex_index].b;
                last_transform->next->next = nullptr;
                last_transform->next->prev = last_transform;
                last_transform->next->move_type = 1;
                last_transform->posvel(&last_transform->next->pos, &last_transform->next->speed, &last_transform->next->angle, last_transform->end_time);
                last_transform->next->dir_vec = { cos(last_transform->next->angle), sin(last_transform->next->angle), 0.f };
                last_transform->next->accel = inner.ex[et_ex_index].r;
                last_transform->next->angle_accel = inner.ex[et_ex_index].s;
                if (inner.ex[et_ex_index].a < 0)
                    last_transform->next->end_time = 999999;
                else {
                    last_transform->next->end_time = inner.ex[et_ex_index].a + inner.ex[et_ex_index].b;
                    last_transform = last_transform->next;
                    last_transform->next = new LaserCurveTransform_t();
                    last_transform->end_time = inner.ex[et_ex_index].a + inner.ex[et_ex_index].b;
                    last_transform->next->start_time = inner.ex[et_ex_index].a + inner.ex[et_ex_index].b;
                    last_transform->next->next = nullptr;
                    last_transform->next->prev = last_transform;
                    last_transform->next->move_type = 0;
                    last_transform->posvel(&last_transform->next->pos, &last_transform->next->speed, &last_transform->next->angle, last_transform->end_time);
                    last_transform->next->dir_vec = { cos(last_transform->next->angle), sin(last_transform->next->angle), 0.f };
                    last_transform->next->end_time = 0x497423f0;
                }
                break;
            }
            case 8: {
                auto last_transform = &transforms;
                for (auto t = last_transform->next; t; t = t->next)
                    last_transform = t;
                last_transform->next = new LaserCurveTransform_t();
                last_transform->end_time = inner.ex[et_ex_index].b;
                last_transform->next->start_time = inner.ex[et_ex_index].b;
                last_transform->next->next = nullptr;
                last_transform->next->prev = last_transform;
                last_transform->next->move_type = 2;
                last_transform->posvel(&last_transform->next->pos, &last_transform->next->speed, &last_transform->next->angle, last_transform->end_time);
                last_transform->next->dir_vec = { cos(last_transform->next->angle), sin(last_transform->next->angle), 0.f };
                last_transform->next->accel = inner.ex[et_ex_index].r;
                last_transform->next->angle_accel = inner.ex[et_ex_index].s;
                if (inner.ex[et_ex_index].a < 0)
                    last_transform->next->end_time = 999999;
                else {
                    last_transform->next->end_time = inner.ex[et_ex_index].a + inner.ex[et_ex_index].b;
                    last_transform = last_transform->next;
                    last_transform->next = new LaserCurveTransform_t();
                    last_transform->end_time = inner.ex[et_ex_index].a + inner.ex[et_ex_index].b;
                    last_transform->next->start_time = inner.ex[et_ex_index].a + inner.ex[et_ex_index].b;
                    last_transform->next->next = nullptr;
                    last_transform->next->prev = last_transform;
                    last_transform->next->move_type = 0;
                    last_transform->posvel(&last_transform->next->pos, &last_transform->next->speed, &last_transform->next->angle, last_transform->end_time);
                    last_transform->next->dir_vec = { cos(last_transform->next->angle), sin(last_transform->next->angle), 0.f };
                    last_transform->next->end_time = 0x497423f0;
                }
                break;
            }
            case 0x10:
                flags |= 0x10;
                ex_angle.angle = inner.ex[et_ex_index].r;
                ex_angle.speed = inner.ex[et_ex_index].s;
                if (inner.ex[et_ex_index].s <= -999.0)
                    ex_angle.speed = speed;
                ex_angle.timer = 0;
                ex_angle.duration = inner.ex[et_ex_index].a;
                ex_angle.max_turns = inner.ex[et_ex_index].b;
                ex_angle.turns_so_far = 0;
                ex_angle.aim_type = inner.ex[et_ex_index].c;
                break;
            case 0x40:
                if (0 < inner.ex[et_ex_index].a) {
                    flags |= 0x40;
                    ex_bounce.bounce_speed = inner.ex[et_ex_index].r;
                    if (inner.ex[et_ex_index].r < 0.0)
                        ex_bounce.bounce_speed = speed;
                    inner.ex[et_ex_index].a--;
                    ex_bounce.max_rebounds = inner.ex[et_ex_index].a;
                    ex_bounce.rebounds_so_far = 0;
                    ex_bounce.for_laser_line = inner.ex[et_ex_index].b;
                }
            }
        }

        if (inner.ex[et_ex_index].type == 0x80) {
            ex_invuln__remaining_frames = inner.ex[et_ex_index].a;
        }

        if (inner.ex[et_ex_index].type == 0x100) {
            flags |= 0x100;
            offscreen_timer = inner.ex[et_ex_index].a;
            ex_offscreen.timer = inner.ex[et_ex_index].a;
            ex_offscreen.__b__unknown = inner.ex[et_ex_index].b;
        }

        if (inner.ex[et_ex_index].type == 0x200) {
            vm1(AnmManagerN::getLoaded(7)->getPreloaded(BULLET_TYPE_TABLE[inner.ex[et_ex_index].a]["script"].asInt() + inner.ex[et_ex_index].b));
            vm1.parent = nullptr;
            // vm1.__root_vm__or_maybe_not = nullptr;
            vm1.update();
        }

        if (inner.ex[et_ex_index].type == 0x100000) {
            if (inner.ex[et_ex_index].a == 0)
                vm1.bitflags.blendmode = 0;
            else
                vm1.bitflags.blendmode = 1;
        }

        if (inner.ex[et_ex_index].type == 0x10000000) {
            some_flags = (some_flags ^ (inner.ex[et_ex_index].a << 4 ^ some_flags)) & 0x10;
        }

        if (inner.ex[et_ex_index].type == (int32_t)0x80000000) {
            if (inner.ex[et_ex_index].a > 0) {
                flags |= 0x80000000;
                ex_wait.timer = inner.ex[et_ex_index].a;
            }
        }

        if (inner.ex[et_ex_index].type == 0x10000) {
            et_ex_index = inner.ex[et_ex_index].a;
            continue;
        }

        if (inner.ex[et_ex_index].type == 0x800) { /*SoundManager::play_sound_at_position(inner.ex[et_ex_index].a); */
            et_ex_index++;
        } else if (inner.ex[et_ex_index].type == 0x1000) {
            flags |= 0x1000;
            ex_wrap.__timer__used_by_lasers = inner.ex[et_ex_index].a;
        }

        if (inner.ex[et_ex_index].type == 0x2000) {
            EnemyBulletShooter_t bs = {};
            bs.__shot_transform_sfx = -1;
            bs.__vec3_8 = laser_offset + glm::vec3(math::lengthdir_vec(laser_inf_current_length, angle), 0.f);
            bs.aim_type = inner.ex[et_ex_index].a;
            bs.__start_transform = inner.ex[et_ex_index].b;
            bs.cnt_count = inner.ex[et_ex_index].c;
            bs.cnt_layers = inner.ex[et_ex_index].d;
            bs.ang_aim = inner.ex[et_ex_index].r;
            bs.ang_bullet_dist = inner.ex[et_ex_index].s;
            bs.spd1 = inner.ex[et_ex_index].m;
            bs.spd2 = inner.ex[et_ex_index].n;
            if (-999990.0 >= bs.ang_aim)
                bs.ang_aim = angle;
            if (999990.0 <= bs.ang_aim)
                bs.ang_aim = math::point_direction(laser_offset.x, laser_offset.y, Globals::get()->playerX, Globals::get()->playerY);
            if (bs.spd1 <= -999990.0)
                bs.spd1 = speed;
            et_ex_index++;
            bs.type = inner.ex[et_ex_index].a;
            bs.__color = inner.ex[et_ex_index].b;
            for (int i = 0; i < 20; i++)
                bs.ex[i] = inner.ex[i];
            BulletManager::GetInstance()->Shoot(&bs);
            if (inner.ex[et_ex_index].c != 0) {
                cancel(0, 0);
            }
        }

        if (inner.ex[et_ex_index].type == 0x8000) {
            laser_id = inner.ex[et_ex_index].a;
        }

        et_ex_index++;
    }
    return;
}

int LaserCurve::cancel(int , int as_bomb)
{
    if (!as_bomb || ex_invuln__remaining_frames == 0) {
        for (int i = 0; i < inner.laser_time_start; i++) {
            if (i % 3 == 0) {
                //BULLET_MANAGER_PTR->bullet_anm->__field_134__some_kind_of_counter++;
                //vm = AnmManager::allocate_vm();
                //anm_init_copy_vm_from_loaded(BULLET_MANAGER_PTR->bullet_anm,vm,inner.color * 2 + 0xd1);
                AnmVM* vm = AnmManagerN::getVM(AnmManagerN::SpawnVM(7, inner.color * 2 + 0xd1));
                vm->bitflags.randomMode = true;
                vm->entity_pos = nodes[i].pos;
                vm->rotation.z = 0.0;
                vm->update();
                //(vm->prefix).mode_of_create_child = 0;
                //AnmManager::insert_in_world_list_back(&local_14, vm);
            }
        }
        __field_10__set_to_3_by_ex_delete = 1;
    }
    return 0;
}
