#include "AnmFuncs.h"
#include "../Bullet.h"
#include "../Laser/Laser.h"

int on_switch_anm_1(AnmVM*, int) { return 0; }
int on_switch_anm_2(AnmVM* vm, int sw)
{
    if (sw == 1) {
        *(int*)((int64_t)vm->special_vertex_buffer_data + 0x1928) = *(int*)((int64_t)vm->special_vertex_buffer_data + 0x192c);
        *(float*)((int64_t)vm->special_vertex_buffer_data + 0x1930) = *(float*)((int64_t)vm->special_vertex_buffer_data + 0x1930) + 300.0;
        *(int*)((int64_t)vm->special_vertex_buffer_data + 0x192c) = (int)*(float*)((int64_t)vm->special_vertex_buffer_data + 0x1930) + 300.0;
    }
    return 0;
}

int (*ANM_ON_SWITCH_FUNCS[5])(AnmVM*, int) = {
    nullptr,
    on_switch_anm_1,
    on_switch_anm_2,
    [](AnmVM*, int) -> int { return 0; },
    [](AnmVM*, int) -> int { return 0; },
};

int32_t on_sprite_set_bullet(AnmVM* vm, int32_t v)
{
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

int32_t on_sprite_set_laser(AnmVM* vm, int32_t v)
{
    auto b = reinterpret_cast<Laser*>(vm->getEntity());
    if (BULLET_TYPE_TABLE[b->bullet_type]["colors"][0]["main_sprite_id"].asInt() < 0)
        return v;
    if (v == 0)
        return BULLET_TYPE_TABLE[b->bullet_type]["colors"][b->bullet_color]["main_sprite_id"].asInt();
    if (v == 1)
        return BULLET_TYPE_TABLE[b->bullet_type]["colors"][b->bullet_color]["spawn_sprite_id"].asInt();
    if (v == 2)
        return BULLET_TYPE_TABLE[b->bullet_type]["colors"][b->bullet_color]["cancel_sprite_id"].asInt();
    return v;
}

int32_t on_sprite_set_laser_curve(AnmVM* vm, int32_t)
{
    return reinterpret_cast<Laser*>(vm->getEntity())->bullet_color + LASER_DATA["laser_curve"]["sprite_first"].asInt();
}

int32_t (*ANM_ON_SPRITE_SET_FUNCS[6])(AnmVM*, int32_t) = {
    nullptr,
    on_sprite_set_bullet,
    on_sprite_set_laser,
    on_sprite_set_laser_curve,
    nullptr,
    nullptr
};

void on_draw_anm_1(AnmVM*) { }

void (*ANM_ON_DRAW_FUNCS[8])(AnmVM*) = {
    nullptr,
    on_draw_anm_1,
    on_draw_anm_1,
    on_draw_anm_1,
    on_draw_anm_1,
    on_draw_anm_1,
    on_draw_anm_1,
    on_draw_anm_1,
};

int (*ANM_ON_DESTROY_FUNCS[5])(AnmVM*) = {
    nullptr,
    [](AnmVM*) -> int { return 0; },
    [](AnmVM* vm) -> int { for (int i = 0; i < 200; i++) AnmManager::getVM(((uint*)vm->special_vertex_buffer_data)[i])->destroy(); return 0; },
    [](AnmVM*) -> int { return 0; },
    [](AnmVM*) -> int { return 0; }
};
