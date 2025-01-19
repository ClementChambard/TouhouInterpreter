#include "AnmManager.h"
#include <GL/glew.h>

namespace anm {

void flush_vbos();

void enable_blending(bool val) {
    if (val) {
        glEnable(GL_BLEND);
    } else {
        glDisable(GL_BLEND);
    }
}

void enable_culling(bool val) {
    if (val) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }
}

// void enable_fog();
// void enable_atest();
// void enable_zwrite();
// void enable_ztest();
// void disable_fog();
// void disable_atest();
// void disable_zwrite();
// void disable_ztest();
//
// void set_fog_params(ns::Color col, f32 beg, f32 end);
// void set_viewport(Viewport_t viewport);
// void set_viewport(Camera_t &camera);
// void set_camera(Camera_t *camera);
// void set_3d_camera(Camera_t *c);
// void _set_cam_vec2_fc(ns::vec2 v);
// Camera_t *get_camera();
// Camera_t *get_3d_camera();
// void reset_blend_eq();
// void reset_texture();
// void reset_batch_state();
// void clear_framebuffer_color(ns::Color c = c_black);
// void clear_framebuffer_depth(f32 d = 1.f);
// void clear_framebuffer(ns::Color c = c_black, f32 d = 1.f);
// void use_custom_color(ns::Color c);
// void use_texture(ns::Texture *tex);
// void use_default_texture();
// void unuse_custom_color();

} // namespace anm
