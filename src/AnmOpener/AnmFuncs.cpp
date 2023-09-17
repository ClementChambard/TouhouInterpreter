#include "AnmFuncs.h"
#include "../Bullet.h"
#include "../Laser/Laser.h"
#include <math/Random.h>

int on_switch_anm_1(AnmVM* vm, int interrupt) {
    auto buff =
        reinterpret_cast<EFFECT_1_buffer_t*>(vm->special_vertex_buffer_data);
    switch (interrupt) {
    case 1:
        for (int i = 0; i < 4; i++) {
            AnmManager::getLoaded(8)->copyFromLoaded(&buff->vms[i], i + 0x7);
            buff->vms[i].parent_vm = nullptr;
            buff->vms[i].__root_vm__or_maybe_not = nullptr;
            buff->vms[i].update();
        }
        return 0;
    case 7:
        buff->i1 = 0;
        vm->bitflags.originMode = 0;
        vm->layer = 0x1e;
        break;
    case 8:
        buff->i1 = 1;
        vm->bitflags.originMode = 2;
        vm->layer = 0x17;
        break;
    case 9:
        buff->i1 = 0;
        vm->bitflags.originMode = 0;
        vm->layer = 0x24;
        break;
    case 10:
        buff->i1 = 3;
        vm->bitflags.originMode = 0;
        vm->layer = 0x1e;
        break;
    default:
        return 0;
    }
    vm->bitflags.resolutionMode = 1;
    buff->vms[0].entity_pos = {320, 240, 0};
    buff->vms[1].entity_pos = {320, 240, 0};
    buff->vms[2].entity_pos = {320, 240, 0};
    buff->vms[3].entity_pos = {320, 240, 0};
    return 0;
}

int on_switch_anm_2(AnmVM* vm, int sw) {
    if (sw != 1) return 0;
    reinterpret_cast<EFFECT_2_buffer_t*>(vm->special_vertex_buffer_data)
        ->timer.add(300);
    return 0;
}

int (*ANM_ON_SWITCH_FUNCS[5])(AnmVM*, int) = {
    nullptr,
    on_switch_anm_1,
    on_switch_anm_2,
    [](AnmVM*, int) -> int { return 0; },
    [](AnmVM*, int) -> int { return 0; },
};

int32_t on_sprite_set_bullet(AnmVM* vm, int32_t v) {
    auto b = reinterpret_cast<Bullet*>(vm->getEntity());
    if (b->sprite_data["colors"][0]["main_sprite_id"].asInt() < 0)
        return v;
    if (v == 0)
        return b->sprite_data["colors"][b->color]["main_sprite_id"].asInt();
    if (v == 1)
        return b->sprite_data["colors"][b->color]["spawn_sprite_id"].asInt();
    if (v == 2)
        return b->sprite_data["colors"][b->color]["cancel_sprite_id"].asInt();
    return v;
}

int32_t on_sprite_set_laser(AnmVM* vm, int32_t v) {
    auto b = reinterpret_cast<Laser*>(vm->getEntity());
    if (BULLET_TYPE_TABLE[b->bullet_type]["colors"][0]
        ["main_sprite_id"].asInt() < 0)
        return v;
    if (v == 0)
        return BULLET_TYPE_TABLE[b->bullet_type]["colors"][b->bullet_color]
        ["main_sprite_id"].asInt();
    if (v == 1)
        return BULLET_TYPE_TABLE[b->bullet_type]["colors"][b->bullet_color]
        ["spawn_sprite_id"].asInt();
    if (v == 2)
        return BULLET_TYPE_TABLE[b->bullet_type]["colors"][b->bullet_color]
        ["cancel_sprite_id"].asInt();
    return v;
}

int32_t on_sprite_set_laser_curve(AnmVM* vm, int32_t) {
    return reinterpret_cast<Laser*>(vm->getEntity())
        ->bullet_color + LASER_DATA["laser_curve"]["sprite_first"].asInt();
}

int32_t (*ANM_ON_SPRITE_SET_FUNCS[4])(AnmVM*, int32_t) = {
    nullptr,
    on_sprite_set_bullet,
    on_sprite_set_laser,
    on_sprite_set_laser_curve,
};

int on_func_return_0(AnmVM*) { return 0; }

int on_draw_anm_1(AnmVM* vm) {
    auto buff =
        reinterpret_cast<EFFECT_1_buffer_t*>(vm->special_vertex_buffer_data);
    if (buff->i1 != 0x2 && buff->i1 != 0) {
        for (int i = 0; i < 4; i++) {
            AnmManager::drawVM(&buff->vms[i]);
        }
        return 0;
    }
    RenderVertex_t vertexbuff[4];
    AnmManager::flush_vbos();
    // SUPERVISOR.d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    // SUPERVISOR.d3d_device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
    // SUPERVISOR.d3d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
    // SUPERVISOR.d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
    // SUPERVISOR.d3d_device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    // SUPERVISOR.d3d_device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
    // SUPERVISOR.d3d_device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO);
    // SUPERVISOR.d3d_device->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);
    if (buff->i1 == 0x2) {
        vertexbuff[0].transformed_pos = {128, 16, 0, 1};
        vertexbuff[0].diffuse_color = {0, 0, 0, 0};
        vertexbuff[1].transformed_pos = {512, 16, 0, 1};
        vertexbuff[1].diffuse_color = {0, 0, 0, 0};
        vertexbuff[2].transformed_pos = {128, 464, 0, 1};
        vertexbuff[2].diffuse_color = {0, 0, 0, 0};
        vertexbuff[3].transformed_pos = {512, 464, 0, 1};
        vertexbuff[3].diffuse_color = {0, 0, 0, 0};
    } else {
        vertexbuff[0].transformed_pos = {0, 0, 0, 1};
        vertexbuff[0].diffuse_color = {0, 0, 0, 0};
        vertexbuff[1].transformed_pos = {BACK_BUFFER_SIZE.x, 0, 0, 1};
        vertexbuff[1].diffuse_color = {0, 0, 0, 0};
        vertexbuff[2].transformed_pos = {0, BACK_BUFFER_SIZE.y, 0, 1};
        vertexbuff[2].diffuse_color = {0, 0, 0, 0};
        vertexbuff[3].transformed_pos = {BACK_BUFFER_SIZE, 0, 1};
        vertexbuff[3].diffuse_color = {0, 0, 0, 0};
    }
    // SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    // SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    // SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    // SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
    // SUPERVISOR.d3d_device->SetFVF(0x44);
    // SUPERVISOR.d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertexbuff, 0x14);
    // SUPERVISOR.d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    // SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    // SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    // SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    // SUPERVISOR.d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    // SUPERVISOR.d3d_device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_SRCALPHA);
    // SUPERVISOR.d3d_device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
    // SUPERVISOR.d3d_device->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);
    // ANM_MANAGER_PTR->last_used_blendmode = 10;
    for (int i = 0; i < 4; i++) {
        AnmManager::drawVM(&buff->vms[i]);
    }
    // SUPERVISOR.d3d_device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);
    AnmManager::drawVM(&buff->vms[4]);
    AnmManager::flush_vbos();
    return 0;
}

int on_draw_anm_3(AnmVM*) { return 0; }

#include "../Fog.hpp"

int on_draw_anm_4(AnmVM* vm) {
    reinterpret_cast<Fog_t*>(vm->associated_game_entity)->set_vm_vertices();
    return 0;
}

int (*ANM_ON_DRAW_FUNCS[8])(AnmVM*) = {
    nullptr,
    on_draw_anm_1,
    on_func_return_0,
    on_draw_anm_3,
    on_draw_anm_4,
    on_draw_anm_3,
    on_draw_anm_3,
    on_draw_anm_3,
};

int on_tick_anm_1(AnmVM* vm) {
    auto buff =
        reinterpret_cast<EFFECT_1_buffer_t*>(vm->special_vertex_buffer_data);
    int cnt_dead = 0;
    for (int i = 0; i < 4; i++) {
        if (buff->vms[i].update()) cnt_dead++;
    }
    if (cnt_dead < 0x4) {
        buff->vms[4].update();
        buff->i2++;
        return 0;
    }
    return 1;
}

static inline glm::vec3 rand_vec_3(float d, float a) {
    return math::lengthdir_vec3(Random::Float01() * d, Random::Floatm11() * a);
}

int on_tick_anm_2(AnmVM* vm) {
    using math::lengthdir_vec3;
    auto buff =
        reinterpret_cast<EFFECT_2_buffer_t*>(vm->special_vertex_buffer_data);
    buff->endpoint = vm->entity_pos;
    buff->startpoint =
        vm->entity_pos + lengthdir_vec3(300.0, vm->rotation.z);
    buff->midpoint = vm->entity_pos
        + lengthdir_vec3(150.0, vm->rotation.z + vm->__script_vars_33_34_35.z);

    if (buff->timer.current != buff->timer.previous &&
        buff->timer.current < 0x32) {
        int i = buff->timer.current * 4;
        buff->ids[i+0] = AnmManager::getLoaded(8)->createEffect(EFFECT["eff2_startanm"].asInt());
        buff->ids[i+1] = AnmManager::getLoaded(8)->createEffect(EFFECT["eff2_startanm"].asInt());
        buff->ids[i+2] = AnmManager::getLoaded(8)->createEffect(EFFECT["eff2_startanm"].asInt());
        buff->ids[i+3] = AnmManager::getLoaded(8)->createEffect(EFFECT["eff2_startanm"].asInt()+1);
        auto vvm = AnmManager::getVM(buff->ids[i+0]);
        vvm->color_1 = vm->color_1;
        vvm->int_script_vars[0] = vm->int_script_vars[0];
        vvm = AnmManager::getVM(buff->ids[i+1]);
        vvm->color_1 = vm->color_1;
        vvm->int_script_vars[0] = vm->int_script_vars[0];
        vvm = AnmManager::getVM(buff->ids[i+2]);
        vvm->color_1 = vm->color_1;
        vvm->int_script_vars[0] = vm->int_script_vars[0];
        vvm = AnmManager::getVM(buff->ids[i+3]);
        vvm->color_1.r = 0x2c - vm->color_1.r;
        vvm->color_1.g = 0x2c - vm->color_1.g;
        vvm->color_1.b = 0x2c - vm->color_1.b;
        vvm->color_1.a = vm->color_1.a;
        vvm->int_script_vars[0] = vm->int_script_vars[0];
    }

    int nb_alive = 0;
    for (int i = 0; i < 0xc8; i++) {
        auto vvm = AnmManager::getVM(buff->ids[i]);
        if (vvm == 0) {
            buff->ids[i] = 0;
            continue;
        }
        // vvm->slowdown = vm->get_root_slowdown();
        if (buff->phase[i] == 0) {
            glm::vec3 rand_start = buff->startpoint + rand_vec_3(150.0, PI);
            glm::vec3 rand_end = buff->midpoint + rand_vec_3(50.0, PI);
            glm::vec3 path_dir = glm::normalize(rand_end - rand_start);
            glm::vec3 path2_dir = glm::normalize(buff->endpoint - rand_end);
            glm::vec3 bez_end = glm::normalize(path_dir + path2_dir)
                * (Random::Float01() * 200.f + 200.f);
            glm::vec3 bez_start =
                path_dir * (Random::Float01() * 100.f + 100.f);
            vvm->pos_i.start_bezier(rand_start, rand_end, bez_start
                                    , bez_end, vm->int_script_vars[0]);

            buff->endpos[i] = rand_end;
            buff->endbez[i] = bez_end;
            buff->phase[i] = 1;
        } else if (vvm->__timer_1c > vm->int_script_vars[0] &&
                   buff->phase[i] == 1) {
            vvm->pos_i.start_bezier(buff->endpos[i],
                                    vm->entity_pos + rand_vec_3(20.0, PI),
                                    buff->endbez[i], rand_vec_3(20.0, PI),
                                    vm->int_script_vars[0]);
            buff->phase[i] = 2;
        }
        nb_alive++;
    }
    if (nb_alive == 0)
        return -1;
    buff->timer++;
    return 0;
}

int (*ANM_ON_TICK_FUNCS[7])(AnmVM*) = {
    nullptr,
    on_tick_anm_1,
    on_tick_anm_2,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
};

int (*ANM_ON_WAIT_FUNCS[1])(AnmVM*) = {
    nullptr
};

int (*ANM_ON_DESTROY_FUNCS[5])(AnmVM*) = {
    nullptr,
    [](AnmVM*) -> int { return 0; },
    [](AnmVM* vm) -> int {
        auto buff =
          reinterpret_cast<EFFECT_2_buffer_t*>(vm->special_vertex_buffer_data);
        for (int i = 0; i < 200; i++) {
            auto subvm = AnmManager::getVM(buff->ids[i]);
            if (!subvm) {
                buff->ids[i] = 0;
            } else {
                subvm->bitflags.visible = false;
                subvm->instr_offset = -1;
            }
        }
        return 0;
    },
    [](AnmVM*) -> int { return 0; },
    [](AnmVM*) -> int { return 0; }
};
