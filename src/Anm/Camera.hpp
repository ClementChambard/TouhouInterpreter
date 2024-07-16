#ifndef CAMERA_INCLUDED_H
#define CAMERA_INCLUDED_H

#include <color.h>
#include <math/types/types.hpp>

namespace anm {

struct CameraSky_t {
    void init(float beg, float eng, float c0, float c1, float c2, float c3);
    f32 begin_distance = 0.f;
    f32 end_distance = 0.f;
    ns::vec4 color_components{};
    ns::Color color{};
};

struct Viewport_t {
    u32 X = 0;
    u32 Y = 0;
    u32 Width = 0;
    u32 Height = 0;
    f32 MinZ = 0.f;
    f32 MaxZ = 0.f;
};

struct Camera_t {
    ns::vec3 position{};
    ns::vec3 facing{};
    ns::vec3 up{};
    ns::vec3 facing_normalized{};
    ns::vec3 right{};
    ns::vec3 __rocking_vector_1{};
    ns::vec3 __rocking_vector_2{};
    f32 fov_y = 0.f;
    ns::ivec2 window_resolution{};
    ns::mat4 view_matrix = ns::mat4(1.f);
    ns::mat4 projection_matrix = ns::mat4(1.f);
    Viewport_t viewport{};
    i32 camera_index = 0;
    ns::vec2 screen_shake{};
    ns::vec3 __vec3_104{};
    CameraSky_t sky{};

    Camera_t* as_2d_matrix();
    Camera_t* as_3d_matrix();
};

} // namespace anm

#endif  // !CAMERA_INCLUDED_H
