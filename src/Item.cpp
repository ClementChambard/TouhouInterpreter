#include "./Item.h"

Item::Item() {
    anm_vm_1.id = 0;
    anm_vm_1.sprite_id = -1;
    anm_vm_1.instr_offset = -1;
    anm_vm_2.id = 0;
    anm_vm_2.sprite_id = -1;
    anm_vm_2.instr_offset = -1;
    field_0xc54 &= 0xfffffffe;
}

Item::~Item() {}

void Item::reset() {
    node_in_free_list = {};
    anm_vm_1.reset();
    anm_vm_2.reset();
    position = {};
    velocity = {};
    velocity_magnitude = 0.f;
    velocity_angle = 0.f;
    time = 0;
    field_0xc54 = 0;
    state = 0;
    item_type = 0;
    __field_c60__init_to_item_type_but_only_for_piv_items = 0;
    velocity_magnitude_towards_player = 0.f;
    intangibility_frames = 0;
    __might_be_unused__force_autocollect__from_th16 = 0;
    appear_sound = -1;
    anm_vm_1.id = 0;
    anm_vm_1.sprite_id = -1;
    anm_vm_1.instr_offset = -1;
    anm_vm_2.id = 0;
    anm_vm_2.sprite_id = -1;
    anm_vm_2.instr_offset = -1;
    field_0xc54 &= 0xfffffffe;
}

int Item::FUN_00434b80() {
    if (item_type == 4 || item_type == 6 || item_type == 15 ||
        item_type == 5 || item_type == 7) {
        AnmManager::getLoaded(8)->createEffectPos(0x65, 0, position);
        // if (item_type != 4 && item_type != 5)
        //     SoundManager::play_sound_centered(0x30);
        // else SoundManager::play_sound_centered(0x4a);
    } else if (item_type == 1 || item_type == 2) {
        AnmManager::getLoaded(8)->createEffectPos(0x65, 0, position);
        // if (appear_sound >= 0)
        //     SoundManager::play_sound_centered(appear_sound);
        return 0;
    }
    return 0;
}
