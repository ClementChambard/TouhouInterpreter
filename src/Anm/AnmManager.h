#ifndef ANMMANAGER_H_
#define ANMMANAGER_H_

#include "./AnmFile.h"
#include "./AnmVM.h"
#include "./Camera.hpp"
#include "./RenderVertex.hpp"
#include <defines.h>

namespace ns { struct Vertex; }

namespace anm {

extern f32 RESOLUTION_MULT;
extern ns::vec2 BACK_BUFFER_SIZE;

void init();
void cleanup();

void recreate_vm(ID &id, i32 new_script);

VM *getVM(u32 id);
VM *getVM(ID id);

VM *allocate_vm();
ID insert_in_world_list_back(VM *vm);
ID insert_in_world_list_front(VM *vm);
ID insert_in_ui_list_back(VM *vm);
ID insert_in_ui_list_front(VM *vm);
void prepare_vm_for_delete(VM *vm, VMList_t *list);
i32 destroy_possibly_managed_vm(VM *vm);

void _destroyFastVM(VM *vm);
void deleteVM(u32 id);
void deleteVM(VM *vm);
void delete_of_file(File *f);
void killAll();
bool isAlive(u32 id);
File *loadFile(usize slot, cstr filename);
ID createVM508(i32 i, VM *root);

void flush_vbos();
void setup_render_state_for_vm(VM *vm);
void write_sprite(const RenderVertex_t *buffer);
void calc_mat_world(VM *vm);
void drawVM(VM *vm);
void draw_vm__modes_0_1_2_3(VM *vm, i32 i);
i32 draw_vm__mode_6(VM *vm);
void draw_vm__mode_8_15(VM *vm);
void draw_vm_triangle_fan(VM *vm, RenderVertex_t *vertexData, i32 nVert);
void draw_vm_triangle_strip(VM *vm, RenderVertex_t *vertexData, i32 nVert);
i32 draw_vm__ins_603(f32 x, f32 y, f32 width, f32 height, f32 rot_z, ns::Color col1, ns::Color col2, i32 anchorX, i32 anchorY);
i32 draw_vm__ins_607(f32 x, f32 y, f32 width, f32 height, f32 rot_z, ns::Color col1, ns::Color col2, i32 anchorX, i32 anchorY);
i32 draw_vm__ins_612(f32 x, f32 y, f32 width, f32 height, f32 rot_z, ns::Color col1, ns::Color col2, i32 anchorX, i32 anchorY);
i32 draw_vm__ins_613(f32 x, f32 y, f32 width, f32 rot_z, ns::Color col1, ns::Color col2, i32 anchor);
i32 draw_vm__mode_19__drawRing(f32 x, f32 y, f32 angle_0, f32 radius, f32 thickness, i32 vertex_count, ns::Color col);
i32 draw_vm__mode_18__drawCircleBorder(f32 x, f32 y, f32 angle_0, f32 radius, i32 nb_vertex, ns::Color col);
i32 draw_vm__mode_17__drawCircle(f32 x, f32 y, f32 angle_0, f32 radius, i32 vertex_count, ns::Color color_1, ns::Color color_2);
void draw_vm_mode_24_25(VM *vm, void *buff, i32 cnt);

void update(bool printInstr = false);

i32 render_layer(u32 layer);
i32 on_tick_world();
i32 on_tick_ui();
void on_draw(u32 layer);

i32 getFreeAnm();

File *getLoaded(i32 i);

void interrupt_tree(ID id, i32 interrupt);
void interrupt_tree_run(ID id, i32 interrupt);

void bindBuffer();
void drawBuffer(RenderVertex_t *start, u32 count);
void unbindBuffer();

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

}; // namespace AnmManager

#endif // ANMMANAGER_H_
