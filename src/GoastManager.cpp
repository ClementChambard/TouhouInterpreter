#include "GoastManager.h"
#include "AnmOpener/AnmManager.h"
#include "Player.h"
#include "GlobalData.h"
#include <math/Random.h>

GoastManager* GOAST_MANAGER_PTR = nullptr;

GoastManager::GoastManager()
{
    flags |= 2;
    GOAST_MANAGER_PTR = this;
    //anm_file = AnmManager::preload_anm(0x1e,"beast.anm");
    //if (!anm_file) debug message & exit
    AnmManager::LoadFile(31, "beast.anm");

    on_tick = new UpdateFunc([this]() -> int { return this->_on_tick(); });
    on_tick->flags &= 0xfffffffd;
    UPDATE_FUNC_REGISTRY->register_on_tick(on_tick,31);

    on_draw = new UpdateFunc([]() -> int { return 1; });
    on_draw->flags = (on_draw->flags & 0xfffffffdU) | 1;
    UPDATE_FUNC_REGISTRY->register_on_draw(on_draw,34);

    //if (GAME_THREAD_PTR->field47_0xa8 == 0) {
    //    GLOBALS.inner._132_16_ = ZEXT816(0);
    //    GLOBALS.inner.TOKENS[3] = 0;
    //    GLOBALS.inner.TOKENS[4] = 0;
    //    GLOBALS.inner.__unknown_timer = 0;
    //    GLOBALS.inner.HYPER_TIME = 0;
    //    GLOBALS.inner.field43_0xdc = 0;
    //    GLOBALS.inner.field44_0xe0 = 0;
    //    GLOBALS.inner._228_8_ = 0;
    //}
}

GoastManager::~GoastManager()
{
  if (on_tick) UPDATE_FUNC_REGISTRY->unregister(on_tick);
  if (on_draw) UPDATE_FUNC_REGISTRY->unregister(on_draw);
  delete_tokens();
  //AnmManager::unload_file(30);
  GOAST_MANAGER_PTR = nullptr;
}

void GoastManager::delete_tokens()
{
    auto node = list_head.next;
    while (node) {
        auto ptr = node->entry;
        node = ptr->node.next;
        AnmManager::deleteVM(ptr->anm_id);
        delete ptr;
        token_count--;
    }
    list_head.entry = nullptr;
    list_head.next = nullptr;
    list_head.prev = nullptr;
    //GLOBALS.inner.HYPER_FLAGS |= 0x200;
}

int GoastManager::_on_tick()
{
    auto node = list_head.next;
    while (node) {
        auto ptr = node->entry;
        node = ptr->node.next;
        if (ptr->update()) {
            if (ptr->node.next) ptr->node.next->prev = ptr->node.prev;
            if (ptr->node.prev) ptr->node.prev->next = ptr->node.next;
            ptr->node.next = nullptr;
            ptr->node.prev = nullptr;
            AnmManager::deleteVM(ptr->anm_id);
            delete ptr;
            token_count--;
        }
    }

    if (!(GLOBALS.inner.HYPER_FLAGS & 1U)) {
        if (PLAYER_PTR && 400.0 < PLAYER_PTR->inner.pos.y && PLAYER_PTR->inner.pos.x < -64.0) {
            AnmManager::interrupt_tree(field_0x24,3);
            //GLOBALS.inner.HYPER_FLAGS |= 1;
        }
    } else if (PLAYER_PTR && (PLAYER_PTR->inner.pos.y < 384.0 || -64.0 < PLAYER_PTR->inner.pos.x)) {
        AnmManager::interrupt_tree(field_0x24,2);
        GLOBALS.inner.HYPER_FLAGS &= 0xfffffffe;
    }

    //if (field_0x3c != 0) FUN_0040e920(this);

    //if ((GLOBALS.inner.HYPER_FLAGS & 2U)) FUN_0040ee70(this);

    return 1;
}

int Token_t::update()
{
    if (flags & 1) {
        speed = (PLAYER_PTR->inner.pos - pos) * 0.25f;
        pos += speed;
        if (__timer_30 >= 10) {
            AnmManager::deleteVM(anm_id);
            anm_id = 0;
            return 1;
        } else {
            if (auto vm = AnmManager::getVM(anm_id); vm) vm->entity_pos = pos;
            __timer_30++;
            return 0;
        }
    }
    if ((pos.y - PLAYER_PTR->inner.pos.y) * (pos.y - PLAYER_PTR->inner.pos.y) + (pos.x - PLAYER_PTR->inner.pos.x) * (pos.x - PLAYER_PTR->inner.pos.x) < 576.0) {
        //SoundManager::play_sound_at_position(0x3f);
        //FUN_0040f980(token_type);
        flags |= 1;
        AnmManager::interrupt_tree(anm_id, 1);
        __timer_30 = 0;
    }

    float d2_to_pl = (PLAYER_PTR->inner.pos.x - pos.x) * (PLAYER_PTR->inner.pos.x - pos.x) + (PLAYER_PTR->inner.pos.y - pos.y) * (PLAYER_PTR->inner.pos.y - pos.y);

    if (flags & 2U) {
        if (d2_to_pl <= 3600.0) {
            if (!(flags & 4U)) AnmManager::interrupt_tree(anm_id,8);
            if (__timer_58 < 0x3c) __timer_58 = 0x5a;
            flags |= 4;
        }
        else {
            __timer_58--;
            if (flags & 4) {
                if (__timer_58 < 0x3d) {
                    AnmManager::interrupt_tree(anm_id,2);
                    flags &= 0xfffffffb;
                } else {
                    AnmManager::interrupt_tree(anm_id,3);
                    flags &= 0xfffffffb;
                }
            }
            else flags &= 0xfffffffb;
        }

        if (__timer_58 == 0x3c) {
            AnmManager::interrupt_tree(anm_id,2);
        } else if (__timer_58 < 1) {
            __timer_58 = 0xb4;
            token_type = (token_type % 3) + 1;
            AnmManager::deleteVM(anm_id);
            //anm_id = GOAST_MANAGER_PTR->anm_loaded->create_effect(nullptr,token_type - 1,-1,NULL);
            anm_id = AnmManager::SpawnVM(31, token_type - 1);
            if (0x1e77 < __timer_30) AnmManager::interrupt_tree(anm_id,9);
            //SoundManager::play_sound_at_position(0x4e);
        }
    }
    if (abs(pos.x) > 208.0 || abs(pos.y - 224.0) > 240.0) {
        __timer_44++;
        if (0x77 < __timer_44 || 0x81 < __timer_30) {
            AnmManager::deleteVM(anm_id);
            anm_id = 0;
            return 1;
        }
    } else __timer_44 = 0;
    pos += ((d2_to_pl <= 3600.0) ? 0.5f : 1.0f) * speed;
    if (__timer_30 == 0x1e78) AnmManager::interrupt_tree(anm_id,9);
    if (__timer_30 < 0x20d0) {
        bool bounced = false;
        if (pos.x <= -180.0 && speed.x < 0) { speed.x *= -1; bounced = true; }
        if (pos.x >= 180.0 && speed.x > 0) { speed.x *= -1; bounced = true; }
        if (pos.y <= 128.0 && speed.y < 0) { speed.y *= -1; bounced = true; }
        if (pos.y >= 384.0 && speed.y > 0) { speed.y *= -1; bounced = true; }
        if (bounced) {
            if (abs(speed.x) < 0.2) speed.x = (speed.x <= 0) ? Random::Float01() * -0.3 - 0.5 : Random::Float01() * 0.3 + 0.5;
            if (abs(speed.y) < 0.2) speed.y = (speed.y <= 0) ? Random::Float01() * -0.3 - 0.5 : Random::Float01() * 0.3 + 0.5;
            GOAST_MANAGER_PTR->choose_angle(this, math::point_direction(0,0,speed.x, speed.y));
        }
    }
    if (auto vm = AnmManager::getVM(anm_id); vm) vm->entity_pos = pos;
    __timer_30++;
    return 0;
}

void GoastManager::choose_angle(Token_t* token, float angle)
{
    token->speed = {math::lengthdir_vec(1, angle), 0};

    for (auto node = list_head.next; node; node = node->entry->node.next) {
        auto other = node->entry;
        if (other != token && math::point_distance_sq(other->pos, token->pos) < 576.0 && glm::dot(other->speed, token->speed) > 0.96)
        {
            angle += 3.883222;
            token->speed = {math::lengthdir_vec(1, angle), 0};
        }
    }
}

int GoastManager::spawn_token(glm::vec3 const& pos,int type,float param_4)
{
    if (token_count > 0x27) return -1;
    auto token = new Token_t();
    token->anm_id = 0;
    token->node.entry = token;
    token->node.next = list_head.next;
    token->node.prev = &list_head;
    if (list_head.next) list_head.next->prev = &token->node;
    list_head.next = &token->node;
    token->__timer_30 = 0;
    token->__timer_44 = 0;
    if (type - 15 < 3 && type - 15 >= 0) {
        //token->anm_id = anm_file->create_effect(nullptr,type - 15,-1,nullptr);
        token->anm_id = AnmManager::SpawnVM(31, type - 15);
        token->token_type = type - 14;
        token->flags = (token->flags & 0xfffffffb) | 2;
        token->__timer_58 = 0xf0;
    } else {
        //token->anm_id = anm_file->create_effect(nullptr,type - 1,-1,nullptr);
        token->anm_id = AnmManager::SpawnVM(31, type - 1);
        if (type - 1 < 3) AnmManager::interrupt_tree(token->anm_id, 7);
        token->token_type = type;
    }
    token->pos = pos;
    if (auto vm = AnmManager::getVM(token->anm_id); vm) vm->entity_pos = pos;
    choose_angle(token, param_4);
    token_count++;
    if (type - 8 < 7 && type - 8 > 0) {
        //if (GAME_THREAD_PTR->field47_0xa8 == 0) FUN_0045d8a0(type + 0x19);
        //SoundManager::play_sound_at_position(0x4a);
        return 0;
    }
    //SoundManager::play_sound_at_position(0x3b);
    return 0;
}
