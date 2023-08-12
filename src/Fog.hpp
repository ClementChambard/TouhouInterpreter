#ifndef FOG_INCLUDED_H
#define FOG_INCLUDED_H

#include <cstdint>
#include "AnmOpener/AnmVM.h"

struct Fog_t {
    explicit Fog_t(int i);
    ~Fog_t();

    int32_t some_count = 0;
    // undefined4
    int32_t text_script_59_anmid = 0;
    AnmID* anmid_array = nullptr;
    AnmVM* vm_array = nullptr;
    void* owned_ptr_14 = nullptr;
    void* owned_ptr_18 = nullptr;
};

#endif // !FOG_INCLUDED_H
