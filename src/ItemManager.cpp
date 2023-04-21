#include "ItemManager.h"
#include "GlobalData.h"
#include "GoastManager.h"
#include "Hardcoded.h"
#include "Input.h"
#include "Player.h"
#include <math/Random.h>

ItemManager* ITEM_MANAGER_PTR = nullptr;

ItemManager::ItemManager()
{
    if (ITEM_MANAGER_PTR)
        delete ITEM_MANAGER_PTR;
    ITEM_MANAGER_PTR = this;

    flags |= 2;

    on_tick = new UpdateFunc([this]() -> int { /*Something with gamethread first*/ return this->_on_tick(); });
    on_tick->flags &= 0xfffffffd;
    UPDATE_FUNC_REGISTRY->register_on_tick(on_tick, 30);

    on_draw = new UpdateFunc([this]() -> int { /*Something with gamethread first*/ return this->_on_draw(1); });
    on_draw->flags &= 0xfffffffd;
    UPDATE_FUNC_REGISTRY->register_on_draw(on_draw, 33);

    on_draw2 = new UpdateFunc([this]() -> int { /*Something with gamethread first*/ return this->_on_draw(0); });
    on_draw2->flags &= 0xfffffffd;
    UPDATE_FUNC_REGISTRY->register_on_draw(on_draw2, 19);

    reset();
}

ItemManager::~ItemManager()
{
    if (on_tick)
        UPDATE_FUNC_REGISTRY->unregister(on_tick);
    if (on_draw)
        UPDATE_FUNC_REGISTRY->unregister(on_draw);
    if (on_draw2)
        UPDATE_FUNC_REGISTRY->unregister(on_draw2);

    ITEM_MANAGER_PTR = nullptr;
}

void ItemManager::reset()
{
    total_items_created = 0;
    field_0xe4b984 = 0;

    normal_items_freelist_head.entry = nullptr;
    normal_items_freelist_head.next = nullptr;
    normal_items_freelist_head.prev = nullptr;

    for (int i = 0; i < 600; i++) {
        normal_items[i].reset();
        normal_items[i].node_in_free_list.next = nullptr;
        normal_items[i].node_in_free_list.prev = nullptr;
        normal_items[i].node_in_free_list.list_head = &normal_items_freelist_head;
        if (normal_items_freelist_head.next) {
            normal_items[i].node_in_free_list.next = normal_items_freelist_head.next;
            normal_items_freelist_head.next->node_in_free_list.prev = &normal_items[i];
        }
        normal_items_freelist_head.next = &normal_items[i];
        normal_items[i].node_in_free_list.prev = (Item*)&normal_items_freelist_head;
    }

    cancel_items_freelist_head.entry = nullptr;
    cancel_items_freelist_head.next = nullptr;
    cancel_items_freelist_head.prev = nullptr;

    for (int i = 0; i < 0x1000; i++) {
        cancel_items[i].reset();
        cancel_items[i].node_in_free_list.next = nullptr;
        cancel_items[i].node_in_free_list.prev = nullptr;
        cancel_items[i].node_in_free_list.list_head = &cancel_items_freelist_head;
        if (cancel_items_freelist_head.next) {
            cancel_items[i].node_in_free_list.next = cancel_items_freelist_head.next;
            cancel_items_freelist_head.next->node_in_free_list.prev = &cancel_items[i];
        }
        cancel_items_freelist_head.next = &cancel_items[i];
        cancel_items[i].node_in_free_list.prev = (Item*)&cancel_items_freelist_head;
    }

    lolk_item_slowdown_factor = 1.0;
    return;
}

int ItemManager::_on_tick()
{
    field_0xe4b980 = 0;
    num_items_onscreen = 0;

    for (int i = 0; i < 0x1257; i++) {
        Item* item = &(i < 600 ? normal_items : cancel_items)[i < 600 ? i : i - 600];
        if (item->state == 0)
            continue;
        if (item->state == 5) {
            item->intangibility_frames--;
            if (item->intangibility_frames < 0) {
                item->state = 2;
                // BULLET_MANAGER_PTR->bullet_anm->anm_init_copy_vm_from_loaded(&item->anm_vm_1, ITEM_ANM_SCRIPT_IDS[item->item_type].id_1);
                item->anm_vm_1(AnmManager::getLoaded(7)->getPreloaded(ITEM_ANM_SCRIPT_IDS[item->item_type]["id_1"].asInt()));
                item->anm_vm_1.parent_vm = nullptr;
                item->anm_vm_1.__root_vm__or_maybe_not = nullptr;
                item->anm_vm_1.update();
                item->anm_vm_2.bitflags.visible = false;
                item->anm_vm_2.instr_offset = -1;
            }
            continue;
        } else if (item->state == 1) {
            if (item->intangibility_frames < 1) {
                // POC
                if ((PLAYER_PTR->inner.state != 2 && PLAYER_PTR->inner.state != 4 && PLAYER_PTR->inner.pos.y < ((GLOBALS.inner.CHARACTER == 1) ? 0x94 : 0x80)) // TODO: no hardcode
                                                                                                                                                               //|| (((BOMB_PTR->active == 1) && ((int)BOMB_PTR->field14_0x38 < 0x3c)) || GUI_PTR->msg) // TODO
                ) {
                    item->velocity_magnitude_towards_player = PLAYER_PTR->sht_file->header.grazebox_u;
                    item->state = 3;
                    item->velocity = { math::lengthdir_vec(item->velocity_magnitude_towards_player, math::point_direction(item->position.x, item->position.y, PLAYER_PTR->inner.pos.x, PLAYER_PTR->inner.pos.y)), 0 };
                    item->position += item->velocity * GAME_SPEED;

                    if (item->velocity_magnitude_towards_player < 12.0)
                        item->velocity_magnitude_towards_player += 0.2;

                    if (PLAYER_PTR->inner.state == 4) {
                        item->state = 1;
                        item->velocity.y = 0.0;
                        item->velocity.x = 0.0;
                    }

                } else {
                    // Movement
                    item->position += item->velocity * GAME_SPEED * lolk_item_slowdown_factor;
                    item->velocity.y += 0.03 * GAME_SPEED * lolk_item_slowdown_factor;
                    if (item->velocity.y >= 0)
                        item->velocity.x = 0.0;
                    if (item->velocity.y > 2)
                        item->velocity.y = 2.0;

                    if (item->time >= 32) {
                        if (lolk_item_slowdown_factor >= 1.f) {
                            item->anm_vm_1.color_1.r = 0xff;
                            item->anm_vm_1.color_1.g = 0xff;
                        } else {
                            item->anm_vm_1.pos = { Random::Floatm11(), Random::Floatm11(), 0 };
                            item->anm_vm_1.color_1.r = 0xff;
                            item->anm_vm_1.color_1.g = 0xa0;
                        }
                        item->anm_vm_1.color_1.a = 0xff;
                        item->anm_vm_1.color_1.b = 0xff;
                    }

                    if (item->position.y > 472.0 || abs(item->position.x) >= 200.0) {
                        // Destroy
                        item->state = 0;
                        if (item->node_in_free_list.list_head->next) {
                            item->node_in_free_list.next = item->node_in_free_list.list_head->next;
                            item->node_in_free_list.list_head->next->node_in_free_list.prev = item;
                        }
                        item->node_in_free_list.list_head->next = item;
                        item->node_in_free_list.prev = reinterpret_cast<Item*>(item->node_in_free_list.list_head);
                        continue;
                    }
                }
            } else {
                if (--item->intangibility_frames < 1)
                    item->FUN_00434b80();
                continue;
            }
        } else if (item->state == 2) {
            item->position += item->velocity * GAME_SPEED;
            item->velocity.y += GAME_SPEED * 0.03;
            if (item->velocity.y < 0.0) {
                if (item->position.y <= 472.0 && abs(item->position.x) < 200.0) {
                    if (item->anm_vm_1.bitflags.visible)
                        item->anm_vm_1.update();
                    if (item->anm_vm_2.bitflags.visible)
                        item->anm_vm_2.update();
                    item->time++;
                    num_items_onscreen++;
                    continue;
                }

                // Destroy
                item->state = 0;
                if (item->node_in_free_list.list_head->next) {
                    item->node_in_free_list.next = item->node_in_free_list.list_head->next;
                    item->node_in_free_list.list_head->next->node_in_free_list.prev = item;
                }
                item->node_in_free_list.list_head->next = item;
                item->node_in_free_list.prev = reinterpret_cast<Item*>(item->node_in_free_list.list_head);
                continue;
            }
            item->velocity_magnitude_towards_player = PLAYER_PTR->sht_file->header.grazebox_u;
            item->state = 3;
            item->velocity = { math::lengthdir_vec(item->velocity_magnitude_towards_player, math::point_direction(item->position.x, item->position.y, PLAYER_PTR->inner.pos.x, PLAYER_PTR->inner.pos.y)), 0 };
            item->position += item->velocity * GAME_SPEED;

            if (item->velocity_magnitude_towards_player < 12.0)
                item->velocity_magnitude_towards_player += 0.2;

            if (PLAYER_PTR->inner.state == 4) {
                item->state = 1;
                item->velocity.y = 0.0;
                item->velocity.x = 0.0;
            }

        } else if (item->state == 3) {
            item->velocity = { math::lengthdir_vec(item->velocity_magnitude_towards_player, math::point_direction(item->position.x, item->position.y, PLAYER_PTR->inner.pos.x, PLAYER_PTR->inner.pos.y)), 0 };
            item->position += item->velocity * GAME_SPEED;
            if (item->velocity_magnitude_towards_player < 12.0)
                item->velocity_magnitude_towards_player += 0.2;

            if (PLAYER_PTR->inner.state == 4) {
                item->state = 1;
                item->velocity.y = 0.0;
                item->velocity.x = 0.0;
            }
        } else if (item->state == 4) {
            if ((PLAYER_PTR->inner.state != 2 && PLAYER_PTR->inner.state != 4 && PLAYER_PTR->inner.pos.y < ((GLOBALS.inner.CHARACTER == 1) ? 0x94 : 0x80)) // TODO: no hardcode
                                                                                                                                                           //|| (((BOMB_PTR->active == 1) && ((int)BOMB_PTR->field14_0x38 < 0x3c)) || GUI_PTR->msg) // TODO
            ) {
                item->velocity_magnitude_towards_player = PLAYER_PTR->sht_file->header.grazebox_u;
                item->state = 3;
            }

            item->velocity = { math::lengthdir_vec(item->velocity_magnitude_towards_player, math::point_direction(item->position.x, item->position.y, PLAYER_PTR->inner.pos.x, PLAYER_PTR->inner.pos.y)), 0 };
            item->position += item->velocity * GAME_SPEED;

            if (item->velocity_magnitude_towards_player < 12.0)
                item->velocity_magnitude_towards_player += 0.2;

            if (PLAYER_PTR->inner.state == 4) {
                item->state = 1;
                item->velocity.y = 0.0;
                item->velocity.x = 0.0;
            }
        }

        if (PLAYER_PTR->inner.state != 2) {
            if (item->position.x >= PLAYER_PTR->item_collect_box.min_pos.x && item->position.y >= PLAYER_PTR->item_collect_box.min_pos.y && item->position.x <= PLAYER_PTR->item_collect_box.max_pos.x && item->position.y <= PLAYER_PTR->item_collect_box.max_pos.y) {
                switch (item->item_type) {
                case 1:
                    // FUN_00434200(item);
                    break;
                case 2:
                    // FUN_00434400(item);
                    break;
                case 3:
                    if (GLOBALS.inner.CURRENT_POWER < GLOBALS.inner.MAXIMUM_POWER) {
                        GLOBALS.inner.CURRENT_POWER += GLOBALS.inner.POWER_PER_LEVEL;
                        GLOBALS.inner.CURRENT_SCORE = fmin(999999999, GLOBALS.inner.CURRENT_SCORE + 10);
                        if (GLOBALS.inner.MAXIMUM_POWER < GLOBALS.inner.CURRENT_POWER) {
                            GLOBALS.inner.CURRENT_POWER = GLOBALS.inner.MAXIMUM_POWER;
                            // FUN_0042f8a0(0,2);
                        }
                        if ((GLOBALS.inner.CURRENT_POWER - GLOBALS.inner.POWER_PER_LEVEL) / GLOBALS.inner.POWER_PER_LEVEL != GLOBALS.inner.CURRENT_POWER / GLOBALS.inner.POWER_PER_LEVEL) {
                            // FUN_00449630(&PLAYER_PTR->inner);
                            // SoundManager::play_sound_at_position(0xd);
                            // PopupManager::generate_small_score_popup(&item->position,-1,-0xc0);
                        }
                    } else {
                        GLOBALS.inner.CURRENT_SCORE = fmin(999999999, GLOBALS.inner.CURRENT_SCORE + 4000);
                        // PopupManager::generate_small_score_popup(&item->position,20000,-0x7f7f80);
                        // SoundManager::play_sound_at_position(0xd);
                    }
                    break;
                case 4:
                    // if (GLOBALS.inner.CURRENT_LIVES < 8) {
                    // GLOBALS.inner.field26_0x68 += 1;
                    // if (GLOBALS.inner.DIFFICULTY == 4) {
                    // iVar7 = *(int *)(&DAT_004a0d64 + GLOBALS.inner.field27_0x6c * 4);
                    //}
                    // else {
                    // iVar7 = (&ITEM_ANM_SCRIPT_IDS[0x11].id_1)[GLOBALS.inner.field27_0x6c];
                    //}
                    // while (iVar7 <= GLOBALS.inner.field26_0x68) {
                    // GLOBALS.inner.field26_0x68 -= iVar7;
                    // if (GLOBALS.inner.CURRENT_LIVES < 8) {
                    // GLOBALS.inner.CURRENT_LIVES = fmin(8, GLOBALS.inner.CURRENT_LIVES + 1);
                    // FUN_0042fc60(GUI_PTR,GLOBALS.inner.CURRENT_LIVES,GLOBALS.inner.field26_0x68);
                    // SoundManager::play_sound_centered(0x11);
                    // FUN_0042f8a0(0,4);
                    //}
                    // GLOBALS.inner.field27_0x6c += 1;
                    //}
                    // FUN_0042fc60(GUI_PTR,GLOBALS.inner.CURRENT_LIVES,GLOBALS.inner.field26_0x68);
                    //}
                    // else {
                    // FUN_00443700(GLOBALS.inner);
                    // GLOBALS.inner.field26_0x68 = 0;
                    //}
                    break;
                case 5:
                    // if (GLOBALS.inner.CURRENT_LIVES < 8) {
                    // GLOBALS.inner.CURRENT_LIVES = fmin(8, GLOBALS.inner.CURRENT_LIVES + 1);
                    // FUN_0042fc60(GUI_PTR,GLOBALS.inner.CURRENT_LIVES,GLOBALS.inner.field26_0x68);
                    // SoundManager::play_sound_centered(0x11);
                    // FUN_0042f8a0(0,4);
                    //}
                    break;
                case 6:
                    // FUN_00443700(&GLOBALS.inner);
                    break;
                case 7:
                    // GLOBALS.inner.CURRENT_BOMBS += 1;
                    // if (GLOBALS.inner.CURRENT_BOMBS < 9) SoundManager::play_sound_centered(0x2e);
                    // else GLOBALS.inner.CURRENT_BOMBS = 8;
                    // FUN_0042fd50(GUI_PTR,GLOBALS.inner.CURRENT_BOMBS,GLOBALS.inner.field29_0x74);
                    break;
                case 8:
                    if (GLOBALS.inner.MAXIMUM_POWER <= GLOBALS.inner.CURRENT_POWER) {
                        // GLOBALS.inner.CURRENT_PIV = fmin(GLOBALS.inner.MAXIMUM_PIV, GLOBALS.inner.CURRENT_PIV + 10000);
                        // PopupManager::generate_small_score_popup(&item->position,100,-0xbf00c0);
                        // SoundManager::play_sound_at_position(0xd);
                        /*if (GLOBALS.inner.MAXIMUM_POWER <= GLOBALS.inner.CURRENT_POWER)*/ break;
                    }
                    GLOBALS.inner.CURRENT_POWER += GLOBALS.inner.MAXIMUM_POWER;
                    if (GLOBALS.inner.MAXIMUM_POWER < GLOBALS.inner.CURRENT_POWER) {
                        GLOBALS.inner.CURRENT_POWER = GLOBALS.inner.MAXIMUM_POWER;
                        // FUN_0042f8a0(0,2);
                    }
                    if ((GLOBALS.inner.CURRENT_POWER - GLOBALS.inner.MAXIMUM_POWER) / GLOBALS.inner.POWER_PER_LEVEL != GLOBALS.inner.CURRENT_POWER / GLOBALS.inner.POWER_PER_LEVEL) {
                        // FUN_00449630(&PLAYER_PTR->inner);
                        // PopupManager::generate_small_score_popup(&item->position,-1,-0xc0);
                        // SoundManager::play_sound_at_position(0xd);
                    }
                    break;
                case 9:
                case 10:
                case 0xb:
                case 0xc:
                case 0xd:
                case 0xe:
                    break;
                    // GLOBALS.inner.CURRENT_PIV += (int)(*(float *)(&DAT_004a0d6c + item->item_type * 4) * 100.0);
                    // if (GLOBALS.inner.MAXIMUM_PIV < GLOBALS.inner.CURRENT_PIV) {
                    // GLOBALS.inner.CURRENT_PIV = GLOBALS.inner.MAXIMUM_PIV;
                    //}
                    // GLOBALS.inner.CURRENT_SCORE += (GLOBALS.inner.CURRENT_PIV / 100 - (GLOBALS.inner.CURRENT_PIV / 100) % 10) / 100;
                    // if (999999999 < (uint)GLOBALS.inner.CURRENT_SCORE) {
                    // GLOBALS.inner.CURRENT_SCORE = 999999999;
                    //}
                }
                // SoundManager::play_sound_at_position(0x25);
                item->state = 0;
                if (item->node_in_free_list.list_head->next) {
                    item->node_in_free_list.next = item->node_in_free_list.list_head->next;
                    item->node_in_free_list.list_head->next->node_in_free_list.prev = item;
                }
                item->node_in_free_list.list_head->next = item;
                item->node_in_free_list.prev = reinterpret_cast<Item*>(item->node_in_free_list.list_head);
                continue;
            }
            if ((item->state != 4 && item->state != 3) && (((INPUT_STRUCT.input & 8U) && item->position.x >= PLAYER_PTR->item_attract_box_focused.min_pos.x && item->position.y >= PLAYER_PTR->item_attract_box_focused.min_pos.y && item->position.x <= PLAYER_PTR->item_attract_box_focused.max_pos.x && item->position.y <= PLAYER_PTR->item_attract_box_focused.max_pos.y) ||

                    (!(INPUT_STRUCT.input & 8) && PLAYER_PTR->item_attract_box_unfocused.min_pos.x <= item->position.x && PLAYER_PTR->item_attract_box_unfocused.min_pos.y <= item->position.y && item->position.x <= PLAYER_PTR->item_attract_box_unfocused.max_pos.x && item->position.y <= PLAYER_PTR->item_attract_box_unfocused.max_pos.y))
                && (item->item_type < 9 || item->item_type > 14)) {
                item->state = 4;
                item->velocity_magnitude_towards_player = PLAYER_PTR->sht_file->header.grazebox_u / 3.0;
            }
        }
        if (item->anm_vm_1.bitflags.visible)
            item->anm_vm_1.update();
        if (item->anm_vm_2.bitflags.visible)
            item->anm_vm_2.update();
        item->time++;
        num_items_onscreen++;
    }
    if (lolk_item_slowdown_factor < 1.0)
        lolk_item_slowdown_factor += 0.1;
    return 1;
}

#include <DrawFuncs.h>
#include <NSEngine.h>
int ItemManager::_on_draw(bool a)
{
    NSEngine::draw_set_layer(19);
    for (int i = 0; i < 600; i++) {
        if (normal_items[i].state && normal_items[i].anm_vm_1.bitflags.visible && normal_items[i].intangibility_frames < 1 && a) {
            normal_items[i].anm_vm_1.pos = normal_items[i].position;
            normal_items[i].anm_vm_2.pos = normal_items[i].position;
            if (normal_items[i].anm_vm_1.pos.y >= -8) {
                normal_items[i].anm_vm_1.layer = 15;
                normal_items[i].anm_vm_1.draw();
                normal_items[i].__field_c60__init_to_item_type_but_only_for_piv_items = 0;
            } else {
                if (normal_items[i].anm_vm_2.bitflags.visible) {
                    if (normal_items[i].anm_vm_2.pos.y + 8.0 < 32.0)
                        normal_items[i].anm_vm_2.color_1.a = (normal_items[i].anm_vm_2.pos.y + 8.0) * 0.03125 * 255.0;
                    else
                        normal_items[i].anm_vm_2.color_1.a = 0xff;
                    normal_items[i].anm_vm_2.pos.y = 8.0;
                    normal_items[i].anm_vm_2.layer = 15;
                    normal_items[i].anm_vm_2.draw();
                }
                normal_items[i].__field_c60__init_to_item_type_but_only_for_piv_items = 1;
            }
        }
    }
    for (int i = 0; i < 0x1000; i++) {
        if (cancel_items[i].state && cancel_items[i].anm_vm_1.bitflags.visible && cancel_items[i].intangibility_frames < 1 && a) {
            cancel_items[i].anm_vm_1.pos = cancel_items[i].position;
            cancel_items[i].anm_vm_2.pos = cancel_items[i].position;
            if (cancel_items[i].anm_vm_1.pos.y >= -8) {
                cancel_items[i].anm_vm_1.layer = 15;
                cancel_items[i].anm_vm_1.draw();
                cancel_items[i].__field_c60__init_to_item_type_but_only_for_piv_items = 0;
            } else {
                if (cancel_items[i].anm_vm_2.bitflags.visible) {
                    if (cancel_items[i].anm_vm_2.pos.y + 8.0 < 32.0)
                        cancel_items[i].anm_vm_2.color_1.a = (cancel_items[i].anm_vm_2.pos.y + 8.0) * 0.03125 * 255.0;
                    else
                        cancel_items[i].anm_vm_2.color_1.a = 0xff;
                    cancel_items[i].anm_vm_2.pos.y = 8.0;
                    cancel_items[i].anm_vm_2.layer = 15;
                    cancel_items[i].anm_vm_2.draw();
                }
                cancel_items[i].__field_c60__init_to_item_type_but_only_for_piv_items = 1;
            }
        }
    }
    return 1;
}

Item* ItemManager::spawn_item(int type, glm::vec3 const& pos, float angle, float speed, int32_t iframe, int32_t sfx)
{
    total_items_created++;

    if (type == 33)
        type = rand() % 3 + 30; // Random goast ?

    if (type >= 9 && type <= 14) {
        Item* item = cancel_items_freelist_head.next;
        if (item) {
            // item->field_0xc6c = field_0xe4b984;
            item->state = 5;
            item->item_type = type;
            item->__field_c60__init_to_item_type_but_only_for_piv_items = type;
            if (field_0xe4b980 >= 0x400)
                item->intangibility_frames = abs(total_items_created) % 32;
            else if (field_0xe4b980 >= 0x200)
                item->intangibility_frames = abs(total_items_created) % 16;
            else if (field_0xe4b980 >= 0x100)
                item->intangibility_frames = abs(total_items_created) % 8;
            else
                item->intangibility_frames = abs(total_items_created) % 4;
            item->position = pos;
            item->velocity = { math::lengthdir_vec(speed, angle), 0 };
            item->time = 0;
            item->velocity_angle = angle;
            item->velocity_magnitude = speed;
            item->__might_be_unused__force_autocollect__from_th16 = 0;
            if (item->node_in_free_list.next)
                item->node_in_free_list.next->node_in_free_list.prev = item->node_in_free_list.prev;
            if (item->node_in_free_list.prev)
                item->node_in_free_list.prev->node_in_free_list.next = item->node_in_free_list.next;
            item->node_in_free_list.next = nullptr;
            item->node_in_free_list.prev = nullptr;
            item->appear_sound = sfx;
        }
        return item;
    }
    if (0x10 < type - 0x10U) {
        Item* item = normal_items_freelist_head.next;
        if (item) {
            item->state = 1;
            item->position = pos;
            if (item->position.x <= -192.f)
                item->position.x = -192.f;
            if (item->position.x >= 192.f)
                item->position.x = 192.0;
            // if (type == 15) type = 6; // always false ?
            item->velocity = { math::lengthdir_vec(speed, angle), 0 };
            item->time = 0;
            item->velocity_magnitude = 0.0;
            item->velocity_magnitude_towards_player = 0.0;
            item->intangibility_frames = iframe;
            item->item_type = type;
            item->appear_sound = sfx;
            if (!iframe)
                item->FUN_00434b80();
            item->__field_c60__init_to_item_type_but_only_for_piv_items = 0;
            // anm_init_copy_vm_from_loaded(BULLET_MANAGER_PTR->bullet_anm, &item->anm_vm_1, ITEM_ANM_SCRIPT_IDS[type].id_1);
            item->anm_vm_1(AnmManager::getLoaded(7)->getPreloaded(ITEM_ANM_SCRIPT_IDS[type]["id_1"].asInt()));
            item->anm_vm_1.parent_vm = nullptr;
            item->anm_vm_1.__root_vm__or_maybe_not = nullptr;
            item->anm_vm_1.update();
            // anm_init_copy_vm_from_loaded(BULLET_MANAGER_PTR->bullet_anm, &item->anm_vm_2, ITEM_ANM_SCRIPT_IDS[type].id_2);
            item->anm_vm_2(AnmManager::getLoaded(7)->getPreloaded(ITEM_ANM_SCRIPT_IDS[type]["id_2"].asInt()));
            item->anm_vm_2.parent_vm = nullptr;
            item->anm_vm_2.__root_vm__or_maybe_not = nullptr;
            item->anm_vm_2.update();
            item->anm_vm_1.color_1 = { 255, 255, 255, 255 };
            item->__might_be_unused__force_autocollect__from_th16 = 0;
            if (item->node_in_free_list.next)
                item->node_in_free_list.next->node_in_free_list.prev = item->node_in_free_list.prev;
            if (item->node_in_free_list.prev)
                item->node_in_free_list.prev->node_in_free_list.next = item->node_in_free_list.next;
            item->node_in_free_list.next = nullptr;
            item->node_in_free_list.prev = nullptr;
        }
        return item;
    }

    float fVar8 = math::point_direction(PLAYER_PTR->inner.pos.x, PLAYER_PTR->inner.pos.y, pos.x, pos.y);

    float fVar9 = Random::Floatm11() * 1.047198 + fVar8;
    int i = 0;
    while (((((fVar9 < -0.3490658) || (0.3490658 < fVar9)) && ((-2.792527 < fVar9 && (fVar9 < 2.792527)))) && (((fVar9 < -1.919862 || (-1.22173 < fVar9)) && ((fVar9 < 1.22173 || (1.919862 < fVar9)))))) || (99 < i)) {
        fVar9 = Random::Floatm11() * 1.047198 + fVar8;
        math::angle_normalize(fVar9);
        i++;
    }

    if (GOAST_MANAGER_PTR)
        GOAST_MANAGER_PTR->spawn_token(pos, type - 15, fVar9);
    return nullptr;
}

void gen_items_from_et_cancel(glm::vec3 const& pos, int do_create_item)
{
    if (do_create_item == 1 && -192.0 < pos.x + 32.0 && pos.x - 32.0 < 192.0 && 0.0 < pos.y + 32.0 && pos.y - 32.0 < 448.0) {
        // BULLET_MANAGER_PTR->__related_to_cancels++;
        ITEM_MANAGER_PTR->spawn_item(9, pos, Random::Floatm11() * 0.1745329 - 1.570796, 2.2, 0, -1);
    }
}
