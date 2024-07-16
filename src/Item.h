#ifndef ITEM_H_
#define ITEM_H_

#include "./Anm/AnmVM.h"

class Item;
struct ItemListHead_t {
    ptr entry = nullptr;
    Item* next = nullptr;
    ptr prev = nullptr;
};

struct ItemList_t {
    ItemListHead_t* list_head = nullptr;
    Item* next = nullptr;
    Item* prev = nullptr;
};

class Item {
public:
    Item();
    ~Item();

    void reset();

    int FUN_00434b80();

public:
    ItemList_t node_in_free_list{};
    anm::VM anm_vm_1{};
    anm::VM anm_vm_2{};
    ns::vec3 position{};
    ns::vec3 velocity{};
    f32 velocity_magnitude = 0.f;
    f32 velocity_angle = 0.f;
    ns::Timer_t time = 0;
    // undefined4[4]
    i32 field_0xc54 = 0;
    i32 state = 0;
    i32 item_type = 0;
    i32 __field_c60__init_to_item_type_but_only_for_piv_items = 0;
    f32 velocity_magnitude_towards_player = 0.f;
    i32 intangibility_frames = 0;
    i32 field_0xc6c = 0;
    i32 __might_be_unused__force_autocollect__from_th16 = 0;
    i32 appear_sound = -1;
};

#endif // ITEM_H_
