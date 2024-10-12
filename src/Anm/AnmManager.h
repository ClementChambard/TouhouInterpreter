#ifndef ANMMANAGER_H_
#define ANMMANAGER_H_

#include "./AnmFile.h"
#include "./AnmVM.h"
#include "./Camera.hpp"
#include "./RenderVertex.hpp"
#include <defines.h>

namespace ns { struct Vertex; }

namespace anm {

// DEBUG ========================
struct ListIter {
  void next();
  VM* get();
  void* it_ptr;
};
ListIter iter_world_list();
// ==============================

extern f32 RESOLUTION_MULT;
extern ns::vec2 BACK_BUFFER_SIZE;

void init();
void cleanup();
i32 render_layer(u32 layer);
i32 on_tick_world();
i32 on_tick_ui();

File *loadFile(usize slot, cstr filename);
File *getLoaded(i32 i);

void recreate_vm(ID &id, i32 new_script);
ID createVM508(i32 i, VM *root);

VM *get_vm(ID id);
bool is_alive(ID id);
void delete_vm(ID id);
void delete_vm(VM *vm);

void interrupt_tree(ID id, i32 interrupt);
void interrupt_tree_run(ID id, i32 interrupt);

void draw_vm(VM *vm);
void draw_vm_as_quad(VM *vm);
void draw_vm_as_quad_no_rot(VM *vm);
void draw_vm_as_triangle_strip(VM *vm, RenderVertex_t* vertex, usize vertex_count);

void flush_vbos();

void set_camera(Camera_t *camera);
Camera_t *get_camera();
void enable_fog();
void disable_fog();
void set_fog_params(ns::Color col, f32 beg, f32 end);
void enable_atest();
void disable_atest();
void enable_zwrite();
void disable_zwrite();
void enable_ztest();
void disable_ztest();
void set_viewport(Viewport_t viewport);
void set_viewport(Camera_t &camera);
void clear_framebuffer_color(ns::Color c = c_black);
void clear_framebuffer_depth(f32 d = 1.f);
void clear_framebuffer(ns::Color c = c_black, f32 d = 1.f);
void enable_blending(bool val = true);
void enable_culling(bool val = true);
void reset_blend_eq();
void reset_texture();
void reset_batch_state();
void draw_rect_col(ns::vec4 const &rect, int a, int rgb);
void draw_rect_col(ns::vec4 const &rect, ns::Color c);
void raw_batch_draw(u32 texture, RenderVertex_t tl, RenderVertex_t tr, RenderVertex_t br, RenderVertex_t bl, u8 blendmode = 0);
void raw_batch_draw(ns::Texture* texture, RenderVertex_t tl, RenderVertex_t tr, RenderVertex_t br, RenderVertex_t bl, u8 blendmode = 0);
ns::vec2 &origin(usize i); // 4 origins for now
Camera_t *get_3d_camera();
void set_3d_camera(Camera_t *c);
void use_custom_color(ns::Color c);
void unuse_custom_color();

void _set_cam_vec2_fc(ns::vec2 v);

void set_effect_508(usize i, i16 anm_loaded_index, i16 script_index,
                    i32 index_of_on_create, i32 index_of_on_tick,
                    i32 index_of_on_draw, i32 index_of_on_destroy,
                    i32 index_of_on_interrupt,
                    i32 index_of_on_copy_1__disused = 0,
                    i32 index_of_on_copy_2__disused = 0);

struct Stats {
  usize n_vm_ticked;
  usize n_vm_ticked_world;
  usize n_vm_ticked_ui;
  usize n_vm_drawn;
  usize n_vm_drawn_world;
  usize n_vm_drawn_ui;
  usize n_total_initiated_draws;
  usize n_flush_vbos;
  usize n_non_fast_vm_alive = 0;
};

void get_stats(Stats& stats);

}; // namespace AnmManager

#endif // ANMMANAGER_H_
