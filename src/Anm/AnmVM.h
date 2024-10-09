#ifndef ANMVM_H_
#define ANMVM_H_

#include "./AnmBitflags.h"
#include "./AnmSprite.h"

#include <Interp.hpp>
#include <color.h>
#include <math/math.hpp>

using ns::Interp;

namespace anm {

struct VM;
struct FastVM;

struct ID {
  ID() {}
  ID(u32 i) { val = i; }
  operator u32() { return val; }
  static constexpr u32 fast_id_mask = 8191;
  union {
    u32 val = 0;
    struct {
      u32 fast_id : 13;
      u32 what : 1;
      u32 discriminator : 18;
    };
  };
  bool operator==(ID other) { return val == other.val; }
  bool operator!=(ID other) { return val != other.val; }
};

struct VM {

  struct List_t {
    VM *value = nullptr;
    List_t *next = nullptr;
    List_t *previous = nullptr;
  };

  VM() {}
  VM(u32 script_id, u32 anim_slot);

  ~VM();

  i32 update();
  i32 run();
  void interrupt(i32 i);
  void interruptRun(i32 i);
  void interruptRec(i32 i);
  void interruptRecRun(i32 i);
  void clear_flag_1_rec();
  void set_flag_1_rec();
  void reset();

  // private
  i32 &check_ref(i32 i);
  f32 &check_ref(f32 f);
  i32 check_val(i32 i);
  f32 check_val(f32 f);
  i32 exec_instruction(bytes ins);
  void step_interpolators();
  void update_variables_growth();
  void write_texture_circle_vertices();

  VM *search_children(i32 a, i32 b);

  void setLayer(u32 i);
  ns::vec3 getRotation();
  Sprite getSprite() const;
  void getSpriteCorners2D(ns::vec2 *corners);

  void alloc_special_vertex_buffer(i32 size);
  void transform_coordinate(ns::vec3 &pos);
  ns::vec3 get_pos_with_root();
  ns::vec3 get_own_transformed_pos();
  void write_sprite_corners_2d(ns::vec4 *corners);
  void write_sprite_corners__without_rot(ns::vec4 &v1, ns::vec4 &v2, ns::vec4 &v3, ns::vec4 &v4);
  void write_sprite_corners__with_z_rot(ns::vec4 &v1, ns::vec4 &v2, ns::vec4 &v3, ns::vec4 &v4);
  i32 write_sprite_corners__mode_4();

  i32 wait(f32 old_gamespeed = 1.f);

  // PREFIX
  ns::Timer_t interrupt_return_time = -99;
  i32 interrupt_return_offset = 0;
  u32 layer = 0;
  i32 anm_loaded_index = 0;
  i32 sprite_id = 0;
  i32 script_id = 0;
  i32 instr_offset = 0;
  ns::vec3 pos = {};
  ns::vec3 rotation = {};
  ns::vec3 angular_velocity = {};
  ns::vec2 scale = {1.f, 1.f};
  ns::vec2 scale_2 = {1.f, 1.f};
  ns::vec2 scale_growth = {};
  ns::vec2 uv_scale = {1.f, 1.f};
  ns::vec2 sprite_size = {};
  ns::vec2 uv_scroll_pos = {};
  ns::vec2 anchor_offset = {};
  // undefined4
  Interp<ns::vec3> pos_i = {};
  Interp<ns::vec3> rgb1_i = {};
  Interp<i32> alpha1_i = {};
  Interp<ns::vec3> rotate_i = {};
  Interp<f32> rotate_2d_i = {};
  Interp<ns::vec2> scale_i = {};
  Interp<ns::vec2> scale_2_i = {};
  Interp<ns::vec2> uv_scale_i = {};
  Interp<ns::vec3> rgb2_i = {};
  Interp<i32> alpha2_i = {};
  Interp<f32> u_vel_i = {};
  Interp<f32> v_vel_i = {};
  ns::vec2 uv_quad_of_sprite[4] = {};
  ns::vec2 uv_scroll_vel = {};
  ns::mat4 __matrix_1 = {};
  ns::mat4 __matrix_2 = {};
  ns::mat4 __matrix_3 = {};
  i32 pending_interrupt = 0;
  i32 __time_of_last_sprite_set__unused = 0;
  // undefined4
  i16 script_id_2 = 0;
  // undefined2
  i32 int_script_vars[4] = {0, 0, 0, 0};
  f32 float_script_vars[4] = {0, 0, 0, 0};
  ns::vec3 __script_vars_33_34_35 = {}; /* rotation related vars 33 34 35 */
  i32 __script_var_8 = 0;
  i32 __script_var_9 = 0;
  f32 rand_param_one = 1.f;
  f32 rand_param_pi = ns::PI<f32>;
  i32 rand_param_int = 1;
  ns::vec3 __pos_2 = {};
  ns::vec3 last_rendered_quad_in_surface_space[4];
  i32 mode_of_create_child = -1;
  ns::Color color_1 = {255, 255, 255, 255};
  ns::Color color_2 = {0, 0, 0, 0};
  ns::Color mixed_inherited_color = {255, 255, 255, 255};
  u8 font_dims[2] = {};

  // undefined2
  VM_flags_t bitflags;
  f32 cam_fade_begin = 0.f;
  f32 cam_fade_end = 0.f;

  // SUFFIX
  ID id = 0;
  i32 fast_id = 0;
  ns::Timer_t time_in_script = 0;
  ns::Timer_t __timer_1c = 0;
  List_t node_in_global_list = {this, nullptr, nullptr};
  List_t __node_as_child = {this, nullptr, nullptr};
  List_t list_of_children = {};
  List_t __wierd_list = {this, nullptr, nullptr};
  VM *next_in_layer__disused = nullptr;
  i32 new_field_added_in_1_00b = 0;
  VM *__root_vm__or_maybe_not = nullptr;
  VM *parent_vm = nullptr;
  f32 slowdown = 0.f;
  void *special_vertex_buffer_data = nullptr;
  i32 special_vertex_buffer_size = 0;
  i32 index_of_on_wait = 0;
  i32 index_of_on_tick = 0;
  i32 index_of_on_draw = 0;
  i32 index_of_on_destroy = 0;
  i32 index_of_on_interrupt = 0;
  i32 index_of_on_copy_1__disused = 0;
  i32 index_of_on_copy_2__disused = 0;
  i32 index_of_sprite_mapping_func = 0;
  ns::vec3 entity_pos = {};
  void *associated_game_entity = nullptr;
  ns::vec3 __rotation_related = {};
};

struct FastVM {
  struct List_t {
    FastVM *value = nullptr;
    List_t *next = nullptr;
    List_t *previous = nullptr;
  };

  VM vm;
  List_t freelistNode = {this, nullptr, nullptr};
  bool isAlive = false;
  ID fastID = 0;
};

} // namespace anm

#endif // ANMVM_H_
