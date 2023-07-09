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
    anm_vm_1.destroy();
    anm_vm_2.destroy();
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
        // EFFECT_MANAGER_PTR->effect_anm->__field_134__some_kind_of_counter++;
        // vm = AnmManager::allocate_vm();
        // anm_init_copy_vm_from_loaded(EFFECT_MANAGER_PTR->effect_anm,vm,0x65);
        auto vm = AnmManager::getVM(AnmManager::SpawnVM(8, 0x65));
        vm->bitflags.randomMode = true;
        // TODO(ClementChambard): SHOULD BE ENTITY_POS
        vm->pos = position;
        vm->rotation.z = 0.0;
        vm->update();
        // vm->mode_of_create_child = 0;
        // AnmManager::insert_in_world_list_back(&local_8,vm);
        // if (item_type != 4 && item_type != 5)
        //     SoundManager::play_sound_centered(0x30);
        // else SoundManager::play_sound_centered(0x4a);
    } else if (item_type == 1 || item_type == 2) {
        // EFFECT_MANAGER_PTR->effect_anm->__field_134__some_kind_of_counter++;
        // vm = AnmManager::allocate_vm();
        // anm_init_copy_vm_from_loaded(EFFECT_MANAGER_PTR->effect_anm,vm,0x65);
        auto vm = AnmManager::getVM(AnmManager::SpawnVM(8, 0x65));
        vm->bitflags.randomMode = true;
        // TODO(ClementChambard): SHOULD BE ENTITY_POS
        vm->pos = position;
        vm->rotation.z = 0.0;
        vm->update();
        // vm->mode_of_create_child = 0;
        // AnmManager::insert_in_world_list_back(&local_8,vm);
        // if (appear_sound >= 0)
        //     SoundManager::play_sound_centered(appear_sound);
        return 0;
    }
    return 0;
}
