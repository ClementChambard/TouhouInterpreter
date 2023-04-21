#ifndef SUPERVISOR_H_
#define SUPERVISOR_H_

#include <glm/glm.hpp>
#include <color.h>

struct CameraSky_t {
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
    glm::vec2 __vec2_fc{};
    glm::vec2 __vec2_104{};
    CameraSky_t sky{};
};

struct Supervisor_t {
    Camera_t cam1{};
    Camera_t* current_camera = &cam1;
};

extern Supervisor_t SUPERVISOR;

#endif // SUPERVISOR_H_
