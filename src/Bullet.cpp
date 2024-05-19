#include "./Bullet.h"
#include "./BulletHandler.h"
#include "./BulletManager.h"
#include "./ItemManager.h"
#include "AnmOpener/AnmManager.h"

Bullet::Bullet() {}
Bullet::~Bullet() {}

void Bullet::Update() {
    on_tick();
    vm.setEntityPos(pos.x, pos.y, pos.z);
    if (vm.bitflags.autoRotate)
        vm.rotation.z = angle+PI1_2;
    vm.setScale2(scale, scale);
}

void Bullet::Reset() {
    vm.reset();
    flags = 0;
    ex_invuln__remaining_frames = 0;
    pos = {0, 0, 0};
    velocity = {0, 0, 0};
    speed = 0.f;
    angle = 0.f;
    hitbox_diameter = 0.f;
    __hitbox_diameter_copy = 0.f;
    active_ex_flags = 0;
    state = 1;
    scale = {1};
    ex_offscreen__remaining_frames = 0;
    ex_slot = 0;
    ex_index = 0;
    for (int i = 0; i < 18; i++) {
        et_ex[i].a = -999999;
        et_ex[i].b = -999999;
        et_ex[i].c = -999999;
        et_ex[i].d = -999999;
        et_ex[i].r = -999999.f;
        et_ex[i].s = -999999.f;
        et_ex[i].m = -999999.f;
        et_ex[i].n = -999999.f;
        et_ex[i].slot = 0;
        et_ex[i].type = 0;
        et_ex[i].string = "";
    }
    for (int i = 0; i < 14; i++) {
        // ex_state[i].ex_id = -1;
        // ex_state[i].remainingTime = 0;
        // ex_state[i].startAngle = 0;
        // ex_state[i].startSpeed = 0;
    }
}

void Bullet::_delete() {
    if (state != 0) {
        state = 0;
        __timer_e54 = 0;
        __timer_e68 = 0;
        __timer_e24 = 0;
        __timer_e38 = 0;
        BulletManager *bul_mgr = BulletManager::GetInstance();
        flags = flags & 0xfffffcbe;
        // field22_0x680 = 0;
        __ex_react_a = 0;
                        /* add in the freelist */
        if (bul_mgr->freelist_head.next != nullptr) {
          freelist_node.next = bul_mgr->freelist_head.next;
          (bul_mgr->freelist_head.next)->previous = &freelist_node;
        }
        bul_mgr->freelist_head.next = &freelist_node;
        freelist_node.previous = &bul_mgr->freelist_head;
                        /* remove from the tick list */
        if (tick_list_node.next)
                tick_list_node.next->previous = tick_list_node.previous;
        if (tick_list_node.previous)
                tick_list_node.previous->next = tick_list_node.next;
        tick_list_node.next = nullptr;
        tick_list_node.previous = nullptr;
    }
}

int Bullet::cancel(bool item) {
    vm.interrupt(1);
    vm.update();
    anm::interrupt_tree(anm_extra_id, 1);
    if ((flags & 0x200) == 0) {
        if (-1 < cancel_sprite_id) {
            anm::VM* vm;
            BULLET_MANAGER_PTR->bullet_anm->createEffectPos(
                    cancel_sprite_id, 0, pos, -1, &vm);
            // BULLET_MANAGER_PTR->anm_ids[index_in_bullet_array] = id;
        }
        // SoundManager::play_sound_at_position(0x47);
        gen_items_from_et_cancel(pos, item);
    }
    state = 4;
    pos += velocity * 0.5f;
    __timer_e54 = 0;
    return 0;
}
