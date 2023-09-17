#ifndef CAMERA_INCLUDED_H
#define CAMERA_INCLUDED_H

#include <glm/glm.hpp>
#include <color.h>

struct CameraSky_t {
    void init(float beg, float eng, float c0, float c1, float c2, float c3);
    float begin_distance = 0.f;
    float end_distance = 0.f;
    glm::vec4 color_components{};
    NSEngine::Color color{};
};

struct Viewport_t {
    uint32_t X = 0;
    uint32_t Y = 0;
    uint32_t Width = 0;
    uint32_t Height = 0;
    float MinZ = 0.f;
    float MaxZ = 0.f;
};

struct Camera_t {
    glm::vec3 position{};
    glm::vec3 facing{};
    glm::vec3 up{};
    glm::vec3 facing_normalized{};
    glm::vec3 right{};
    glm::vec3 __rocking_vector_1{};
    glm::vec3 __rocking_vector_2{};
    float fov_y = 0.f;
    glm::vec<2, int32_t> window_resolution{};
    glm::mat4 view_matrix = glm::mat4(1.f);
    glm::mat4 projection_matrix = glm::mat4(1.f);
    Viewport_t viewport{};
    int32_t camera_index = 0;
    glm::vec2 screen_shake{};
    glm::vec3 __vec3_104{};
    CameraSky_t sky{};

    Camera_t* as_2d_matrix();
    Camera_t* as_3d_matrix();
};

#endif  // !CAMERA_INCLUDED_H
