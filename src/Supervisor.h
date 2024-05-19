#ifndef SUPERVISOR_H_
#define SUPERVISOR_H_

#include "AnmOpener/AnmFile.h"
#include "AnmOpener/Camera.hpp"
#include <glm/glm.hpp>
#include <FrameBuffer.h>
#include <color.h>

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
    anm::Viewport_t __unknown_viewport = {};
    // present_parameters
    // lot undefined
    // display_mode
    ns::FrameBuffer* surface_atR_0 = nullptr;
    ns::FrameBuffer* surface_atR_1 = nullptr;
    // back_buffer
    // undefined4
    anm::VM* arcade_vm_0 = nullptr;
    anm::VM* arcade_vm_1 = nullptr;
    anm::VM* arcade_vm_2__handles_upscaling = nullptr;
    anm::VM* arcade_vm_3__handles_seija = nullptr;
    // undefined4
    // undefined4
    // config
    anm::Camera_t cameras[4];
    // moved to AnmManager
    //   Camera_t* current_camera = &cameras[0];
    // unused
    //   int32_t current_camera_index = 0;
    int32_t gamemode_current = 0;
    int32_t gamemode_to_switch_to = 0;
    int32_t gamemode_prev = 0;
    // lot undefined
    anm::File* text_anm = nullptr;
    // undefined4
    int32_t flags = 0;
    int32_t rng_seed_from_time = 0;
    // undefined4
    // caps
    // screenshot_stuff_that_shouldnt_be_global
    // thread
    // lot undefined
    // moved to AnmManager
    //   int32_t fog_enabled = 0;
    //   int32_t zwrite_enabled = 0;
    // lot undefined
    ns::Color background_color;

    void init_cameras();
    void setup_special_anms();
};

extern Supervisor_t SUPERVISOR;

extern float GAME_REGION_WIDTH;
extern float GAME_REGION_HEIGHT;
extern float GAME_REGION_X;
extern float GAME_REGION_Y;

#endif // SUPERVISOR_H_
