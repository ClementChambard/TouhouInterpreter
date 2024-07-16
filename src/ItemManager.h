#ifndef ITEMMANAGER_H_
#define ITEMMANAGER_H_

#include "./Item.h"
#include "./UpdateFuncRegistry.h"

class ItemManager {
public:
    ItemManager();
    ~ItemManager();
    ItemManager(ItemManager const&) = delete;
    ItemManager& operator=(ItemManager const&) = delete;

    void reset();
    Item* spawn_item(i32 type, ns::vec3 const& pos, f32 angle,
                     f32 speed, i32 iframe, i32 sfx);

private:
    i32 _on_tick();
    i32 _on_draw(bool b);
    u32 flags = 0;
    UpdateFunc* on_tick = nullptr;
    UpdateFunc* on_draw = nullptr;

public:
    // undefined4
    UpdateFunc* on_draw2 = nullptr;
    Item normal_items[600] {};
    Item cancel_items[4096] {};
    ItemListHead_t normal_items_freelist_head {};
    ItemListHead_t cancel_items_freelist_head {};
    f32 lolk_item_slowdown_factor = 1.f;
    i32 num_items_onscreen = 0;
    i32 total_items_created = 0;
    i32 field_0xe4b980 = 0;
    i32 field_0xe4b984 = 0;
};

extern ItemManager* ITEM_MANAGER_PTR;

extern void gen_items_from_et_cancel(ns::vec3 const& pos, int do_create_item);

#endif // ITEMMANAGER_H_
