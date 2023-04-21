#ifndef ITEMMANAGER_H_
#define ITEMMANAGER_H_

#include "Item.h"
#include "UpdateFuncRegistry.h"

class ItemManager {
public:
    ItemManager();
    ~ItemManager();
    ItemManager(ItemManager const&) = delete;
    ItemManager& operator=(ItemManager const&) = delete;

    void reset();
    Item* spawn_item(int type, glm::vec3 const& pos, float angle, float speed, int32_t iframe, int32_t sfx);

private:
    int _on_tick();
    int _on_draw(bool b);
    uint32_t flags = 0;
    UpdateFunc* on_tick = nullptr;
    UpdateFunc* on_draw = nullptr;

public:
    // undefined4
    UpdateFunc* on_draw2 = nullptr;
    Item normal_items[600] {};
    Item cancel_items[4096] {};
    ItemListHead_t normal_items_freelist_head {};
    ItemListHead_t cancel_items_freelist_head {};
    float lolk_item_slowdown_factor = 1.f;
    int32_t num_items_onscreen = 0;
    int32_t total_items_created = 0;
    int32_t field_0xe4b980 = 0;
    int32_t field_0xe4b984 = 0;
};

extern ItemManager* ITEM_MANAGER_PTR;

extern void gen_items_from_et_cancel(glm::vec3 const& pos, int do_create_item);

#endif // ITEMMANAGER_H_
