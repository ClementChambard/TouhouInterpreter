#ifndef FOG_INCLUDED_H
#define FOG_INCLUDED_H

#include <cstdint>
#include <glm/fwd.hpp>
#include "AnmOpener/AnmManager.h"
#include "AnmOpener/AnmVM.h"

struct Fog_t {
    explicit Fog_t(int i);
    ~Fog_t();

    void reset_area(float some_x, float some_y, float width, float height);

    void set_vm_vertices();

    int32_t vm_count = 0;
    int32_t vertex_count = 0;
    int32_t anmid = 0;
    AnmID* anmid_array = nullptr;
    AnmVM** vm_array = nullptr;
    RenderVertex_t* vertex_array = nullptr;
    glm::vec3* pos_array = nullptr;
};

#endif // !FOG_INCLUDED_H
