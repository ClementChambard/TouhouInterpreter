#include "./HardcodedFuncs.hpp"
#include "./AnmOpener/AnmFuncs.h"
#include "./AnmOpener/AnmManager.h"
#include "./Fog.hpp"
#include "./Hardcoded.h"

#include <math/Random.h>

struct EFFECT_1_buffer_t {
  AnmVM vms[5] = {};
  int32_t i1 = 0;
  int32_t i2 = 0;
};

struct EFFECT_2_buffer_t {
  AnmID ids[200] = {};
  glm::vec3 endpos[200] = {};
  glm::vec3 endbez[200] = {};
  int32_t phase[200] = {};
  glm::vec3 midpoint = {};
  glm::vec3 startpoint = {};
  glm::vec3 endpoint = {};
  NSEngine::Timer_t timer = {};
};

static int on_switch_anm_1(AnmVM* vm, int interrupt) {
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

static int on_switch_anm_2(AnmVM* vm, int sw) {
    if (sw != 1) return 0;
    reinterpret_cast<EFFECT_2_buffer_t*>(vm->special_vertex_buffer_data)
        ->timer.add(300);
    return 0;
}

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

int on_draw_anm_4(AnmVM* vm) {
    reinterpret_cast<Fog_t*>(vm->associated_game_entity)->set_vm_vertices();
    return 0;
}

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

int on_destroy_anm_2(AnmVM* vm) {
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
}

int on_create_anm_0(AnmVM *vm) {
    vm->bitflags.originMode = 0;
    vm->layer = 0x28;
    vm->bitflags.resolutionMode = 1;
    vm->special_vertex_buffer_size = sizeof(EFFECT_1_buffer_t);
    vm->special_vertex_buffer_data = malloc(sizeof(EFFECT_1_buffer_t));
    memset(vm->special_vertex_buffer_data, 0, sizeof(EFFECT_1_buffer_t));
    auto buff =
        reinterpret_cast<EFFECT_1_buffer_t*>(vm->special_vertex_buffer_data);
    for (int i = 0; i < 4; i++) {
        AnmManager::getLoaded(8)->copyFromLoaded(&buff->vms[i], i + 3);
        buff->vms[i].parent_vm = nullptr;
        buff->vms[i].__root_vm__or_maybe_not = nullptr;
        buff->vms[i].update();
        buff->vms[i].entity_pos = {320, 240, 0};
    }
    AnmManager::getLoaded(8)->copyFromLoaded(&buff->vms[4], 0xc2);
    buff->vms[4].parent_vm = nullptr;
    buff->vms[4].__root_vm__or_maybe_not = nullptr;
    buff->vms[4].update();
    return 0;
}

int on_create_anm_1(AnmVM* vm) {
    vm->special_vertex_buffer_size = sizeof(EFFECT_2_buffer_t);
    vm->special_vertex_buffer_data = malloc(sizeof(EFFECT_2_buffer_t));
    memset(vm->special_vertex_buffer_data, 0, sizeof(EFFECT_2_buffer_t));
    reinterpret_cast<EFFECT_2_buffer_t*>(vm->special_vertex_buffer_data)
      ->timer.reset();
    return 0;
}

int on_create_anm_2(AnmVM* vm) {
    std::cout << "[WARNING] anm: effect 2 is not implemented\n";
    vm->bitflags.activeFlags = ANMVM_DELETE;
    return 0;
}

int on_create_anm_3(AnmVM* vm) {
    std::cout << "[WARNING] anm: effect 3 is not implemented\n";
    vm->bitflags.activeFlags = ANMVM_DELETE;
    return 0;
}

int on_create_anm_4(AnmVM* vm) {
    std::cout << "[WARNING] anm: effect 4 is not implemented\n";
    vm->bitflags.activeFlags = ANMVM_DELETE;
    return 0;
}

int on_create_anm_5(AnmVM* vm) {
    std::cout << "[WARNING] anm: effect 5 is not implemented\n";
    vm->bitflags.activeFlags = ANMVM_DELETE;
    return 0;
}

int on_create_anm_6(AnmVM* vm) {
    std::cout << "[WARNING] anm: effect 6 is not implemented\n";
    vm->bitflags.activeFlags = ANMVM_DELETE;
    return 0;
}

int on_create_anm_7(AnmVM* vm) {
    std::cout << "[WARNING] anm: effect 7 is not implemented\n";
    vm->bitflags.activeFlags = ANMVM_DELETE;
    return 0;
}

int on_create_anm_8(AnmVM* vm) {
    std::cout << "[WARNING] anm: effect 8 is not implemented\n";
    vm->bitflags.activeFlags = ANMVM_DELETE;
    return 0;
}

void HardcodedFuncsInit(int) {
    AnmFuncs::set_on_switch(1, on_switch_anm_1);
    AnmFuncs::set_on_switch(2, on_switch_anm_2);
    AnmFuncs::set_on_draw(1, on_draw_anm_1);
    AnmFuncs::set_on_draw(4, on_draw_anm_4);
    AnmFuncs::set_on_tick(1, on_tick_anm_1);
    AnmFuncs::set_on_tick(2, on_tick_anm_2);
    AnmFuncs::set_on_destroy(2, on_destroy_anm_2);
    AnmFuncs::set_on_create(0, on_create_anm_0);
    AnmFuncs::set_on_create(1, on_create_anm_1);
    AnmFuncs::set_on_create(2, on_create_anm_2);
    AnmFuncs::set_on_create(3, on_create_anm_3);
    AnmFuncs::set_on_create(4, on_create_anm_4);
    AnmFuncs::set_on_create(5, on_create_anm_5);
    AnmFuncs::set_on_create(6, on_create_anm_6);
    AnmFuncs::set_on_create(7, on_create_anm_7);
    AnmFuncs::set_on_create(8, on_create_anm_8);
    AnmManager::set_effect_508(0, 0, 0, 0, 1, 1, 1, 1, 0, 0);
    AnmManager::set_effect_508(1, 0, 0, 1, 2, 2, 2, 2, 1, 1);
    AnmManager::set_effect_508(2, 0, 0, 2, 3, 3, 3, 3, 0, 0);
    AnmManager::set_effect_508(3, 0, 0, 3, 3, 3, 3, 3, 0, 0);
    AnmManager::set_effect_508(4, 0, 0, 4, 5, 7, 4, 4, 0, 0);
    AnmManager::set_effect_508(5, 0, 0, 5, 5, 7, 4, 4, 0, 0);
    AnmManager::set_effect_508(6, 0, 0, 6, 5, 7, 4, 4, 0, 0);
    AnmManager::set_effect_508(7, 0, 0, 7, 5, 7, 4, 4, 0, 0);
    AnmManager::set_effect_508(8, 0, 0, 8, 5, 7, 4, 4, 0, 0);
}
