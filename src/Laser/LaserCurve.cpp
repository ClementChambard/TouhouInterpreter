#include "LaserCurve.h"
#include "../BulletManager.h"
#include "../GlobalData.h"

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
    if (nodes)
        delete[] nodes;
    if (vertices)
        delete[] vertices;
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
    // return 0;
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
    vm2.layer = 14;

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
    __timer_40 = inner.timer40_start;
    __timer_5a0 = 30;

    nodes = new LaserCurveNode_t[inner.laser_time_start];
    for (int i = 0; i < inner.laser_time_start; i++) {
        nodes[i].pos = laser_offset;
        nodes[i].v_speed = { 0, 0, 0 };
        nodes[i].angle = inner.ang_aim;
        nodes[i].speed = inner.spd_1;
    }
    nodes[0].v_speed = laser_speed;

    vertices = new NSEngine::Vertex[inner.laser_time_start * 2];

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
                            if (i == 0.0)
                                FUN_0043d3c0(thi, &nodes[i].pos, &nodes[i].speed, &nodes[i].angle, __timer_40 - i);
                            else
                                FUN_0043d830(thi, &nodes[i].pos, &nodes[i].speed, &nodes[i].angle, &nodes[i - 1].pos, nodes[i - 1].speed, nodes[i - 1].angle, __timer_40 - i);
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

#include <NSEngine.h>
void _dzaww(AnmVM* vm, NSEngine::Vertex* vertices, int count)
{
    for (int i = 0; i < (count - 1) / 2; i++)
        NSEngine::engineData::layers[14]->getBatch()->draw(vm->getSprite().texID, vertices[i * 2], vertices[i * 2 + 1], vertices[i * 2 + 3], vertices[i * 2 + 2], vm->bitflags.blendmode);
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
        vertices[2 * i].uv.u = (float)i / (float)(inner.laser_time_start - 1);
        vertices[2 * i].uv.v = vm1.getSprite().v1; // vm1.uv_quad_of_sprite[0].y;
        vertices[2 * i].color = { 255, 255, 255, 255 };
        // vertices[2*i].position.x += SURF_ORIGIN_ECL_X;
        // vertices[2*i].position.y += DAT_00524720;

        vertices[2 * i + 1].position.x = nodes[i].pos.x - offset.x;
        vertices[2 * i + 1].position.y = -(nodes[i].pos.y - offset.y);
        vertices[2 * i + 1].position.z = 0.0;
        vertices[2 * i + 1].color = { 255, 255, 255, 255 };
        vertices[2 * i + 1].uv.u = (float)i / (float)(inner.laser_time_start - 1);
        vertices[2 * i + 1].uv.v = vm1.getSprite().v2; // vm1.uv_quad_of_sprite[2].y;
        // vertices[2*i+1].position.x += SURF_ORIGIN_ECL_X;
        // vertices[2*i+1].position.y += DAT_00524720;
    }
    // AnmManager::draw_vm__mode_textureCircle(ANM_MANAGER_PTR, &vm1, vertices, inner.laser_time_start * 2);
    _dzaww(&vm1, vertices, inner.laser_time_start * 2);
    if (__timer_40 <= inner.laser_time_start) {
        vm2.pos = nodes[inner.laser_time_start - 1].pos;
        vm2.draw();
    }
    return 0;
}

void LaserCurve::run_ex()
{
    uint32_t* puVar1;
    int32_t* piVar2;
    uint8_t* puVar3;
    LaserCurveListThing_t* pzVar5;
    LaserCurveListThing_t* pzVar10;
    LaserCurveNode_t* pzVar11;
    LaserCurveNode_t* pzVar12;
    LaserCurveListThing_t* pzVar13;
    LaserCurveNode_t* pzVar14;
    void* pvVar15;
    AnmVM* vm;
    int iVar16;
    int extraout_EDX;
    int iVar17;
    int extraout_EDX_00;
    int iVar18;
    EtEx_t* pzVar19;
    float* pfVar20;
    float fVar21;
    uint8_t auStack_3c4[4];
    LaserCurveListThing_t* local_3c0;
    LaserCurveNode_t* local_3bc;
    double local_3b8;
    AnmID zStack_39c;
    uint64_t local_398;

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
            case 4:
                pzVar5 = &listThing;
                for (pzVar10 = listThing.next; pzVar10 != nullptr; pzVar10 = pzVar10->next) {
                    pzVar5 = pzVar10;
                }
                // pvVar15 = new LaserCurveListThing_t();
                // pzVar5->next = pvVar15;
                // pzVar5->field3_0xc = inner.ex[et_ex_index].b;
                //*(zLaserCurveNode **)((int)pvVar15 + 8) = inner.ex[et_ex_index].b;
                //*(undefined4 *)pzVar5->field0_0x0 = 0;
                //*(zLaserCurveListThing **)((int)pzVar5->field0_0x0 + 4) = pzVar5;
                // local_3bc = (zLaserCurveNode *)pzVar5->field0_0x0;
                // pzVar5 = (zLaserCurveListThing *)(local_3bc->pos).y;
                // pzVar12 = local_3bc + 1;
                //(local_3bc->v_speed).y = 1.401298e-45;
                // FUN_0043d3c0(pzVar5,&local_3bc[1].pos,&local_3bc[1].v_speed.y,&(pzVar12->v_speed).x,
                // pzVar5->field3_0xc);
                // fVar21 = (pzVar12->v_speed).x;
                // local_3bc[1].pos.z = 0.0;
                // zFloat2::from_polar((zFloat2 *)&(local_3bc->v_speed).z,fVar21,1.0);
                //*(undefined4 *)(extraout_EDX + 0x1c) = 0;
                //*(float *)(extraout_EDX + 0x34) = inner.ex[et_ex_index].r;
                //*(float *)(extraout_EDX + 0x38) = inner.ex[et_ex_index].s;
                // iVar16 = inner.ex[et_ex_index].a;
                // iVar17 = extraout_EDX;
                // if (iVar16 < 0) {
                //*(undefined4 *)(iVar17 + 0xc) = 999999;
                //}
                // else {
                // local_3bc = (zLaserCurveNode *)((float)iVar16 + (float)inner.ex[et_ex_index].b);
                //*(zLaserCurveNode **)(extraout_EDX + 0xc) = local_3bc;
                // pzVar10 = listThing.next;
                // pzVar5 = &listThing;
                // while (pzVar13 = pzVar10, pzVar13 != NULL) {
                // pzVar5 = pzVar13;
                // pzVar10 = pzVar13->next;
                //}
                // pvVar15 = operator_new(0x3c);
                // pzVar5->next = pvVar15;
                // pzVar5->field3_0xc = (float)local_3bc;
                //*(zLaserCurveNode **)((int)pvVar15 + 8) = local_3bc;
                //*(undefined4 *)pzVar5->field0_0x0 = 0;
                //*(zLaserCurveListThing **)((int)pzVar5->field0_0x0 + 4) = pzVar5;
                // pvVar15 = pzVar5->field0_0x0;
                //*(undefined4 *)((int)pvVar15 + 0x10) = 0;
                // FUN_0043d3c0(*(zLaserCurveListThing **)((int)pvVar15 + 4),
                //(zFloat3 *)((int)pvVar15 + 0x20),(float *)((int)pvVar15 + 0x30),
                //(float *)((int)pvVar15 + 0x2c),
                //(*(zLaserCurveListThing **)((int)pvVar15 + 4))->field3_0xc);
                //*(undefined4 *)((int)pvVar15 + 0x28) = 0;
                // zFloat2::from_polar((zFloat2 *)((int)pvVar15 + 0x14),*(float *)((int)pvVar15 + 0x2c),
                // 1.0);
                //*(undefined4 *)((int)pvVar15 + 0x1c) = 0;
                //*(undefined4 *)((int)pvVar15 + 0xc) = 0x497423f0;
            //}
            break;
            case 8:
                // local_3c0 = (zLaserCurveListThing *)
                // pzVar5 = (zLaserCurveListThing *)&listThing;
                // for (pzVar10 = listThing->next; pzVar10 != nullptr; pzVar10 = pzVar10->next) {
                // pzVar5 = pzVar10;
                //}
                // pvVar15 = operator_new(0x3c);
                // local_3b8 = (double)((ulonglong)local_3b8 & 0xffffffff00000000 | ZEXT48(pvVar15));
                // pzVar5->field0_0x0 = pvVar15;
                // pzVar5->field3_0xc = inner.ex[et_ex_index].b;
                //*(zLaserCurveListThing **)((int)pvVar15 + 8) = inner.ex[et_ex_index].b;
                //*(undefined4 *)pzVar5->field0_0x0 = 0;
                //*(zLaserCurveListThing **)((int)pzVar5->field0_0x0 + 4) = pzVar5;
                // local_3c0 = (zLaserCurveListThing *)pzVar5->field0_0x0;
                // pfVar20 = &local_3c0->field7_0x2c;
                //(&local_3c0->field3_0xc)[1] = 2.802597e-45;
                // FUN_0043d3c0((zLaserCurveListThing *)(&local_3c0->field0_0x0)[1],&local_3c0->field6_0x20
                //,&local_3c0->field8_0x30,pfVar20,
                //((zLaserCurveListThing *)(&local_3c0->field0_0x0)[1])->field3_0xc);
                // fVar21 = *pfVar20;
                //(&(local_3c0->field6_0x20).x)[2] = 0.0;
                // zFloat2::from_polar((zFloat2 *)(&local_3c0->field3_0xc + 2),fVar21,1.0);
                //*(undefined4 *)(extraout_EDX_00 + 0x1c) = 0;
                //*(float *)(extraout_EDX_00 + 0x34) = inner.ex[et_ex_index].r;
                //*(float *)(extraout_EDX_00 + 0x38) = inner.ex[et_ex_index].s;
                // if (inner.ex[et_ex_index].a < 0) {
                //*(undefined4 *)(extraout_EDX_00 + 0xc) = 999999;
                // break;
                //}
                // local_3c0 = (zLaserCurveListThing *)(inner.ex[et_ex_index].a; + (float)inner.ex[et_ex_index].b);
                //*(zLaserCurveListThing **)(extraout_EDX_00 + 0xc) = local_3c0;
                // pzVar14 = (zLaserCurveNode *)(listThing.pos).x;
                // pzVar12 = &listThing;
                // while (pzVar11 = pzVar14, pzVar11 != NULL) {
                // pzVar12 = pzVar11;
                // pzVar14 = (zLaserCurveNode *)(pzVar11->pos).x;
                //}
                // pvVar15 = operator_new(0x3c);
                // local_3b8 = (double)((ulonglong)local_3b8 & 0xffffffff00000000 | ZEXT48(pvVar15));
                //(pzVar12->pos).x = (float)pvVar15;
                //(pzVar12->v_speed).x = (float)local_3c0;
                //*(zLaserCurveListThing **)((int)pvVar15 + 8) = local_3c0;
                //*(undefined4 *)(pzVar12->pos).x = 0;
                //*(zLaserCurveNode **)((int)(pzVar12->pos).x + 4) = pzVar12;
                // fVar21 = (pzVar12->pos).x;
                //*(undefined4 *)((int)fVar21 + 0x10) = 0;
                // FUN_0043d3c0(*(zLaserCurveListThing **)((int)fVar21 + 4),(zFloat3 *)((int)fVar21 + 0x20)
                //,(float *)((int)fVar21 + 0x30),(float *)((int)fVar21 + 0x2c),
                //(*(zLaserCurveListThing **)((int)fVar21 + 4))->field3_0xc);
                //*(undefined4 *)((int)fVar21 + 0x28) = 0;
                // zFloat2::from_polar((zFloat2 *)((int)fVar21 + 0x14),*(float *)((int)fVar21 + 0x2c),1.0);
                //*(undefined4 *)((int)fVar21 + 0x1c) = 0;
                //*(undefined4 *)((int)fVar21 + 0xc) = 0x497423f0;
                break;
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
            // flags |= 0x100;
            // ex_unassigned[4].timer = inner.ex[et_ex_index].a;
            // ex_unassigned[4].field13_0x34 = inner.ex[et_ex_index].b;
        }

        if (inner.ex[et_ex_index].type == 0x200) {
            vm1(AnmManagerN::getLoaded(7)->getPreloaded(BULLET_TYPE_DEFINITIONS[inner.ex[et_ex_index].a].script + inner.ex[et_ex_index].b));
            vm1.parent = nullptr;
            // vm1.__root_vm__or_maybe_not = nullptr;
            vm1.update();
        }

        if (inner.ex[et_ex_index].type == 0x100000) {
            if (inner.ex[et_ex_index].a == 0) vm1.bitflags.blendmode = 0;
            else vm1.bitflags.blendmode = 1;
        }

        if (inner.ex[et_ex_index].type == 0x10000000) {
            some_flags = some_flags ^ (inner.ex[et_ex_index].a << 4 ^ some_flags) & 0x10;
        }

        if (inner.ex[et_ex_index].type == 0x80000000) {
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
