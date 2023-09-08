#ifndef SUPERVISOR_H_
#define SUPERVISOR_H_

#include "AnmOpener/AnmFile.h"
#include <glm/glm.hpp>
#include <FrameBuffer.h>
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

    void as_2d_matrix();
    void as_3d_matrix();
};

struct Supervisor_t {
    Supervisor_t() {}
    ~Supervisor_t();

    void init();

    int initialize();

    // hinstance
    // d3d
    // d3d_device
    // dinput
    // window_rect
    // keyboard_device
    // __joypad_device
    // undefined4
    // __joypad_caps__ithink
    // main_window
    glm::mat4 __unknown_view_matrix = glm::mat4(1.f);
    glm::mat4 __unknown_projection_matrix = glm::mat4(1.f);
    Viewport_t __unknown_viewport = {};
    // present_parameters
    // lot undefined
    // display_mode
    NSEngine::FrameBuffer* surface_atR_0 = nullptr;
    NSEngine::FrameBuffer* surface_atR_1 = nullptr;
    // back_buffer
    // undefined4
    AnmVM* arcade_vm_0 = nullptr;
    AnmVM* arcade_vm_1 = nullptr;
    AnmVM* arcade_vm_2__handles_upscaling = nullptr;
    AnmVM* arcade_vm_3__handles_seija = nullptr;
    // undefined4
    // undefined4
    // config
    Camera_t cameras[4];
    Camera_t* current_camera = &cameras[0];
    int32_t current_camera_index = 0;
    int32_t gamemode_current = 0;
    int32_t gamemode_to_switch_to = 0;
    int32_t gamemode_prev = 0;
    // lot undefined
    AnmFile* text_anm = nullptr;
    // undefined4
    int32_t flags = 0;
    int32_t rng_seed_from_time = 0;
    // undefined4
    // caps
    // screenshot_stuff_that_shouldnt_be_global
    // thread
    // lot undefined
    int32_t fog_enabled = 0;
    int32_t zwrite_enabled = 0;
    // lot undefined
    NSEngine::Color background_color;

    void set_camera_by_index(int cam_id, bool force = false);
    void disable_d3d_fog(bool force = false);
    void enable_d3d_fog(bool force = false);
    void disable_zwrite(bool force = false);
    void set_fog_params(NSEngine::Color col, float beg, float end);
    void enable_zwrite(bool force = false);
    void disable_ztest();
    void enable_ztest();
    void disable_atest();
    void enable_atest();
    void init_cameras();
    void setup_special_anms();
};

extern Supervisor_t SUPERVISOR;

#endif // SUPERVISOR_H_
