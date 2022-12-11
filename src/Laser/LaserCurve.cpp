#include "LaserCurve.h"

LaserCurve::LaserCurve()
{
    vm1.sprite_id = -1;
    vm1.current_instr = -1;
    vm2.sprite_id = -1;
    vm2.current_instr = -1;
}

LaserCurve::~LaserCurve()
{
    LaserCurveListThing_t* node = listThing.next;
    while (node) {
        auto c = node->next;
        delete node;
        node = c;
    }
    if (nodes) delete[] nodes;
    if (listNodes) delete[] listNodes;
}

void FUN_0043d830(LaserCurveListThing_t* node, glm::vec3* pos1, float* s1, float* a1, glm::vec3* pos2,
    float s2, float a2, float param_8)
{
    if (node->e == 0) {
        *pos1 = *pos2 - node->fgh * node->m;
        *s1 = node->m;
        *a1 = node->l;
    }
    if (node->e == 1) {
        if (node->o <= -990.0 && node->o != -990.0) {
            *pos1 = *pos2 - node->fgh * (s2 - node->n);
            *s1 = node->m - node->n;
            *a1 = a2;
            return;
        }
        glm::vec3 v = { math::lengthdir_vec(-s2, a2) + math::lengthdir_vec(-node->n, node->o), 0.f };
        *pos1 = *pos2 + v;
        *s1 = math::point_distance(0, 0, v.x, v.y);
        *a1 = math::point_direction(0, 0, v.x, v.y);
    }
    if (node->e == 2) {
        float f = 0.f; // FUN_00497db0(param_8);
        *pos1 = *pos2 - glm::vec3(math::lengthdir_vec(s2, a2), 0.f) * (param_8 - f);
        *s1 = s2 - node->n;
        *a1 = a2 - node->o;
        math::angle_normalize(*a1);
        *pos1 -= glm::vec3(math::lengthdir_vec(*s1, *a1), 0.f) * ((1.f - param_8) + f);
    }
}

void FUN_0043d3c0(LaserCurveListThing_t* node, glm::vec3* pos, float* speed, float* angle, float param_5)
{
    if (node->e == 0) {
        *pos = node->fgh * (param_5 - node->c) * node->m + node->ijk;
        *speed = node->m;
        *angle = node->l;
    }
    if (node->e == 1) {
        if (node->o <= -990.0 && node->o != -990.0) {
            *pos = node->fgh * (2 * node->m + node->n * (param_5 - node->c)) * (param_5 - node->c + 1.f) * 0.5f + node->ijk;
            *speed = node->n * (param_5 - node->c) + node->m;
            *angle = node->l;
            return;
        }
        glm::vec3 v = { math::lengthdir_vec(node->m, node->l) + math::lengthdir_vec(node->n, node->o), 0.f };
        *pos = v * (param_5 - node->c) + node->ijk;
        *speed = math::point_distance(0, 0, v.x, v.y);
        *angle = math::point_direction(0, 0, v.x, v.y);
    }
    if (node->e == 2) {
        *angle = node->l;
        *speed = node->m;
        *pos = node->ijk;
        for (int i = param_5 - node->c; i > 0; i--) {
            *angle += node->o;
            *speed += node->n;
            math::angle_normalize(*angle);
            *pos += glm::vec3(math::lengthdir_vec(*speed, *angle), 0.f);
        }
        *pos += (param_5 - node->c - (param_5 - node->c)) /*FUN_00497db0(param_5 - node->c))*/ * glm::vec3(math::lengthdir_vec(*speed, *angle), 0.f);
    }
}

int LaserCurve::initialize(void* arg)
{
    return 0;
    inner = *reinterpret_cast<LaserCurveInner_t*>(arg);

    bullet_type = inner.type;
    bullet_color = inner.color;
    kind = 2;
    __field_10__set_to_3_by_ex_delete = 2;

    vm1.destroy();
    if (bullet_type == 1) {
        // AnmLoaded::load_external_vm(LASER_MANAGER_PTR->bullet_anm, vm1, 0x142);
        vm1(AnmManagerN::getLoaded(7)->getPreloaded(0x142));
    } else {
        // AnmLoaded::load_external_vm(LASER_MANAGER_PTR->bullet_anm, vm1, bullet_type + 0x8e);
        vm1(AnmManagerN::getLoaded(7)->getPreloaded(bullet_type + 0x8e));
        vm1.on_set_sprite = [](AnmVM* vm, int spr) { return ((Laser*)vm->getEntity())->bullet_color + 0x20c; };
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
    vm2(AnmManagerN::getLoaded(7)->getPreloaded(inner.color + 0x38));
    vm2.parent = nullptr;
    // vm2.__root_vm__or_maybe_not = nullptr;
    vm2.update();
    vm2.interrupt(2);
    vm2.update();
    vm2.bitflags.blendmode = 1;
    vm2.bitflags.rendermode = 1;
    vm2.bitflags.originMode = 1;

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

    listNodes = new LaserCurveListThing_t[inner.laser_time_start];
    nodes = new LaserCurveNode_t[inner.laser_time_start];
    for (int i = 0; i < inner.laser_time_start; i++) {
        nodes[i].pos = laser_offset;
        nodes[i].v_speed = { 0, 0, 0 };
        nodes[i].angle = inner.ang_aim;
        nodes[i].speed = inner.spd_1;
    }

    LaserCurveListThing_t* ppiVar8 = &listThing;
    if (!inner.listThing) {
        listThing.next = nullptr;
        listThing.c = 0;
        listThing.d = 999999;
        listThing.e = 0;
        listThing.fgh = { cos(angle), sin(angle), 0.f };
        listThing.ijk = laser_offset;
        listThing.l = angle;
        listThing.m = speed;

        math::angle_normalize(listThing.l);
        et_ex_index = inner.ex_index;
    } else {
        listThing.next = inner.listThing->next;
        listThing.b = inner.listThing->b;
        listThing.c = inner.listThing->c;
        listThing.d = inner.listThing->d;
        listThing.e = inner.listThing->e;
        listThing.fgh = inner.listThing->fgh;
        listThing.ijk = inner.listThing->ijk;
        listThing.l = inner.listThing->l;
        listThing.m = inner.listThing->m;
        listThing.n = inner.listThing->n;
        listThing.o = inner.listThing->o;

        for (LaserCurveListThing_t *n = inner.listThing->next, *n2 = &listThing; n != NULL; n = n->next, n2 = n2->next) {
            n2->next = new LaserCurveListThing_t();
            n2->next->next = n->next;
            n2->next->b = n->b;
            n2->next->c = n->c;
            n2->next->d = n->d;
            n2->next->e = n->e;
            n2->next->fgh = n->fgh;
            n2->next->ijk = n->ijk;
            n2->next->l = n->l;
            n2->next->m = n->m;
            n2->next->n = n->n;
            n2->next->o = n->o;
        }

        inner.listThing = nullptr;
        et_ex_index = 99;
    }

    __timer_2c = 0;
    __timer_40 = inner.timer40_start;
    __timer_5a0 = 30;

    nodes[0].v_speed = laser_speed;

    for (int i = 0; i < inner.laser_time_start; i++) {
        for (LaserCurveListThing_t* node = &listThing; node != nullptr; node = node->next) {
            if ((node->c <= __timer_40 - i) && (__timer_40 - i < node->d)) {
                if (i == 0) {
                    FUN_0043d3c0(node, &nodes[i].pos, &nodes[i].speed, &nodes[i].angle, __timer_40 - i);
                } else {
                    FUN_0043d830(node, &nodes[i].pos, &nodes[i].speed, &nodes[i].angle, &nodes[i - 1].pos, nodes[i - 1].speed, nodes[i - 1].angle, __timer_40 - i);
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
    return 1;
    // float* pfVar8;
    // float* extraout_EDX;
    // double uStack_18;
    // float fStack_10;

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
            // ex_unassigned[4].timer --;
            // if (ex_unassigned[4].timer < 1) {
            // flags &= 0xfffffeff;
            // et_ex_done++
            //}
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
            if (__timer_40 - i < 0) {
                nodes[i].pos = inner.start_pos;
                nodes[i].v_speed = { 0, 0, 0 };
                nodes[i].angle = inner.ang_aim;
                nodes[i].speed = inner.spd_1;
            } else {
                LaserCurveListThing_t* thi = &listThing;
                if (thi == NULL) {
                } else {
                    do {
                        if ((thi->c <= __timer_40 - i) && (__timer_40 - i < thi->d)) {
                            if (i == 0.0) {
                                FUN_0043d3c0(thi, &nodes[i].pos, &nodes[i].speed, &nodes[i].angle, __timer_40 - i);
                                continue;
                            }
                            FUN_0043d830(thi, &nodes[i].pos, &nodes[i].speed, &nodes[i * 8].angle, &nodes[i - 1].pos, nodes[i - 1].speed, nodes[i - 1].angle, __timer_40 - i);
                            continue;
                        }
                        thi = thi->next;
                    } while (thi != NULL);
                }
            }
        }
    }

    if (__timer_5a0 > 0 || (flags & 0x100)) {
        __timer_5a0--;
        check_graze_or_kill(0);
        vm1.update();
        vm2.update();
        __timer_40++;
        return 0;
    }

    // pfVar8 = (float*)nodes;
    for (int i = 0; i < inner.laser_time_start; i++) {

        // polar_from_carthesian(, angle, laser_inf_current_length);
        // uStack_18 = (double)CONCAT44((float)((ulonglong)uStack_18 >> 0x20) + laser_offset.y,
        // laser_offset.x + (float)uStack_18);
        // fStack_10 = fStack_10 + laser_offset.z;
        { // if ((((-192.0 < laser_st_width + *extraout_EDX) && (*extraout_EDX - laser_st_width < 192.0)) && (0.0 < laser_st_width + extraout_EDX[1])) && (extraout_EDX[1] - laser_st_width < 448.0)) {
            check_graze_or_kill(0);
            vm1.update();
            vm2.update();
            __timer_40++;
            return 0;
        }
        // pfVar8 = extraout_EDX + 8;
    }

    return 1;
}

int LaserCurve::on_draw()
{return 1;}
#if false
    bool bVar1;
    zFloat3* pzVar2;
    int iVar3;
    int iVar4;
    float* extraout_EDX;
    int extraout_EDX_00;
    zLaserCurveListThing* list_thing;
    int i;
    zLaserCurveNode* pzVar6;
    float fVar7;
    float fVar8;
    float fVar11;
    float fVar12;
    float fVar13;
    float local_8;
    zAnmManager* anm_mgr;

    i = 0;
    local_8 = 0.0;
    list_thing = listNodes;
    pzVar6 = nodes;
    fVar13 = 1.570796;
    fVar12 = 0.5;
    for (int i = 0; i < inner.laser_time_start; i++) {
        listNodes->field5_0x14.x = local_8;
        listNodes->field3_0xc = 1.0;
        listNodes->field4_0x10 = -1;
        listNodes->field5_0x14.y = (vm1).prefix.uv_quad_of_sprite[0].y;
        fVar7 = pzVar6->angle + fVar13;
        if (i == 0) math::angle_normalize(fVar7);
        else {
            math::angle_normalize(fVar7);
            fVar12 = pzVar6[-1].angle + fVar13;
            math::angle_normalize(fVar12);
            fVar8 = fVar12 - fVar7;
            if (fVar8 <= PI) {
                if (PI < fVar7 - fVar12) {
                    fVar8 = fVar12 - (fVar7 - PI2);
                }
            } else {
                fVar8 = fVar12 - (fVar7 + PI2);
            }
            math::angle_normalize(fVar8);
            fVar8 *= 0.5;
            math::angle_normalize(fVar8);
            fVar7 += fVar8;
            math::angle_normalize(fVar7);
            fVar12 = 0.5;
        }
        polar_from_carthesian(listNodes, fVar7, inner.__bmgr_340__laser_new_arg_4 * fVar12);
        fVar8 = (pzVar6->pos).x + *extraout_EDX;
        *extraout_EDX = fVar8;
        fVar7 = (pzVar6->pos).y + extraout_EDX[1];
        extraout_EDX[1] = fVar7;
        extraout_EDX[2] = (pzVar6->pos).z + extraout_EDX[2];
        *extraout_EDX = (float)SURF_ORIGIN_ECL_X + fVar8;
        extraout_EDX[2] = 0.0;
        extraout_EDX[10] = 1.0;
        extraout_EDX[0xb] = -NAN;
        extraout_EDX[1] = (float)DAT_00524720 + fVar7;
        extraout_EDX[0xc] = local_8;
        extraout_EDX[0xd] = (vm1).prefix.uv_quad_of_sprite[2].y;
        fVar7 = pzVar6->angle - fVar13;
        if (i == 0) {
            math::angle_normalize(fVar7);
        } else {
            iVar4 = 0;
            do {
                if (fVar7 <= PI)
                    break;
                fVar7 = fVar7 - PI2;
                iVar3 = iVar4 + 1;
                bVar1 = iVar4 < 0x21;
                iVar4 = iVar3;
            } while (bVar1);
            do {
                if (-PI <= fVar7)
                    break;
                fVar7 = fVar7 + PI2;
                bVar1 = iVar4 < 0x21;
                iVar4 = iVar4 + 1;
            } while (bVar1);
            fVar12 = pzVar6[-1].angle - fVar13;
            iVar4 = 0;
            do {
                if (fVar12 <= PI)
                    break;
                fVar12 = fVar12 - PI2;
                iVar3 = iVar4 + 1;
                bVar1 = iVar4 < 0x21;
                iVar4 = iVar3;
            } while (bVar1);
            do {
                if (-PI <= fVar12)
                    break;
                fVar12 = fVar12 + PI2;
                bVar1 = iVar4 < 0x21;
                iVar4 = iVar4 + 1;
            } while (bVar1);
            fVar8 = fVar12 - fVar7;
            if (fVar8 <= PI) {
                if (PI < fVar7 - fVar12) {
                    fVar8 = fVar12 - (fVar7 - PI2);
                }
            } else {
                fVar8 = fVar12 - (fVar7 + PI2);
            }
            iVar4 = 0;
            do {
                if (fVar8 <= PI)
                    break;
                fVar8 = fVar8 - PI2;
                iVar3 = iVar4 + 1;
                bVar1 = iVar4 < 0x21;
                iVar4 = iVar3;
            } while (bVar1);
            do {
                if (-PI <= fVar8)
                    break;
                fVar8 = fVar8 + PI2;
                bVar1 = iVar4 < 0x21;
                iVar4 = iVar4 + 1;
            } while (bVar1);
            fVar8 = fVar8 * 0.5;
            iVar4 = 0;
            do {
                if (fVar8 <= PI)
                    break;
                fVar8 = fVar8 - PI2;
                iVar3 = iVar4 + 1;
                bVar1 = iVar4 < 0x21;
                iVar4 = iVar3;
            } while (bVar1);
            do {
                if (-PI <= fVar8)
                    break;
                fVar8 = fVar8 + PI2;
                bVar1 = iVar4 < 0x21;
                iVar4 = iVar4 + 1;
            } while (bVar1);
            fVar7 = fVar8 + fVar7;
            iVar4 = 0;
            do {
                if (fVar7 <= PI)
                    break;
                fVar7 = fVar7 - PI2;
                iVar3 = iVar4 + 1;
                bVar1 = iVar4 < 0x21;
                iVar4 = iVar3;
            } while (bVar1);
            do {
                if (-PI <= fVar7)
                    break;
                fVar7 = fVar7 + PI2;
                bVar1 = iVar4 < 0x21;
                iVar4 = iVar4 + 1;
            } while (bVar1);
            fVar12 = 0.5;
        }
        polar_from_carthesian(extraout_EDX + 7, fVar7, inner.__bmgr_340__laser_new_arg_4 * fVar12);
        i += 1;
        fVar7 = (pzVar6->pos).x + *(float*)(extraout_EDX_00 + 0x1c);
        *(float*)(extraout_EDX_00 + 0x1c) = fVar7;
        fVar8 = (pzVar6->pos).y + *(float*)(extraout_EDX_00 + 0x20);
        *(float*)(extraout_EDX_00 + 0x20) = fVar8;
        pzVar2 = &pzVar6->pos;
        pzVar6 = pzVar6 + 1;
        *(float*)(extraout_EDX_00 + 0x24) = pzVar2->z + *(float*)(extraout_EDX_00 + 0x24);
        *(float*)(extraout_EDX_00 + 0x1c) = (float)SURF_ORIGIN_ECL_X + fVar7;
        fVar7 = (float)DAT_00524720;
        *(undefined4*)(extraout_EDX_00 + 0x24) = 0;
        *(float*)(extraout_EDX_00 + 0x20) = fVar7 + fVar8;
        list_thing = (zLaserCurveListThing*)(extraout_EDX_00 + 0x38);
        local_8 = 1.0 / (float)(inner.laser_time_start + -1) + local_8;
    }
    AnmManager::draw_vm__mode_textureCircle(ANM_MANAGER_PTR, &vm1, listNodes, inner.laser_time_start * 2);
    if (__timer_40 <= inner.laser_time_start) {
        vm2.prefix.pos = nodes[inner.laser_time_start - 1].pos;
        vm2->draw();
    }
    return 0;
}

#endif
