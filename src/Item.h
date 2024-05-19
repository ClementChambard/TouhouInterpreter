#ifndef ITEM_H_
#define ITEM_H_

#include "./AnmOpener/AnmVM.h"

class Item;
struct ItemListHead_t {
    void* entry = nullptr;
    Item* next = nullptr;
    void* prev = nullptr;
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
    glm::vec3 position{};
    glm::vec3 velocity{};
    float velocity_magnitude = 0.f;
    float velocity_angle = 0.f;
    int32_t time = 0;
    // undefined4[4]
    int32_t field_0xc54 = 0;
    int32_t state = 0;
    int32_t item_type = 0;
    int32_t __field_c60__init_to_item_type_but_only_for_piv_items = 0;
    float velocity_magnitude_towards_player = 0.f;
    int32_t intangibility_frames = 0;
    int32_t field_0xc6c = 0;
    int32_t __might_be_unused__force_autocollect__from_th16 = 0;
    int32_t appear_sound = -1;
};

#endif // ITEM_H_
