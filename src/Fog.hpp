#ifndef FOG_INCLUDED_H
#define FOG_INCLUDED_H

#include <defines.h>
#include "Anm/AnmVM.h"
#include "Anm/RenderVertex.hpp"

struct Fog_t {
    explicit Fog_t(i32 i);
    ~Fog_t();

    void reset_area(f32 some_x, f32 some_y, f32 width, f32 height);

    void set_vm_vertices();

    i32 vm_count = 0;
    i32 vertex_count = 0;
    i32 anmid = 0;
    anm::ID* anmid_array = nullptr;
    anm::VM** vm_array = nullptr;
    anm::RenderVertex_t* vertex_array = nullptr;
    ns::vec3* pos_array = nullptr;
};

#endif // !FOG_INCLUDED_H
