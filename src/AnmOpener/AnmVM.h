#ifndef ANMVM_H_
#define ANMVM_H_

#include "./AnmBitflags.h"
#include "./AnmSprite.h"
#include "./anmOpener.h"

#include <Interp.hpp>
#include <SpriteBatch.h>
#include <vertex.h>
#include <math/math.h>
#include <glm/glm.hpp>
#include <functional>
#include <vector>

using NSEngine::Interp;

class AnmVM;
struct AnmFastVM;

struct AnmID {
  AnmID() {}
  AnmID(int i) { val = i; }
  operator int() { return val; }
  static constexpr uint32_t fastIdMask = 8191;
  static constexpr uint32_t disctOffset = 14;
  uint32_t val = 0;
  void setDiscriminator(int32_t discr) {
    val = (val & fastIdMask) | (discr << disctOffset);
  }
  void setFastId(int32_t fast) {
    val = (val & ~fastIdMask) | (fast & fastIdMask);
  }
  uint32_t getFastId() { return val & fastIdMask; }
  bool operator==(uint32_t other) { return val == other; }
  bool operator!=(uint32_t other) { return val != other; }
};

struct AnmVMList_t {
  AnmVM *value = nullptr;
  AnmVMList_t *next = nullptr;
  AnmVMList_t *previous = nullptr;
};

struct AnmFastVMList_t {
  AnmFastVM *value = nullptr;
  AnmFastVMList_t *next = nullptr;
  AnmFastVMList_t *previous = nullptr;
};

class AnmVM {

public:
  AnmVM() {}
  //AnmVM(AnmVM const &toCopy);
  //void operator()(AnmVM const &other);
  AnmVM(uint32_t script_id, uint32_t anim_slot);

  ~AnmVM();

  int update(bool printInstr = false);
  // void draw(NSEngine::SpriteBatch *sb = nullptr);
  void destroy();
  void interrupt(int i);
  void interruptRun(int i);
  void interruptRec(int i);
  void interruptRecRun(int i);
  void clear_flag_1_rec();
  void set_flag_1_rec();
  void reset();

  int &check_ref(int i);
  float &check_ref(float f);
  int check_val(int i);
  float check_val(float f);
  int exec_instruction(int8_t *ins);
  AnmVM* search_children(int a, int b);

  void setPos(float x, float y, float z) { pos = {x, y, z}; }
  void setEntityPos(float x, float y, float z) { entity_pos = {x, y, z}; }
  void setPos2(float x, float y, float z) { __pos_2 = {x, y, z}; }
  void setScale(float x, float y) { scale = {x, y}; }
  void setScale2(float x, float y) { scale_2 = {x, y}; }
  void setI(int i, int v) { int_script_vars[i] = v; }
  void setf(int i, int v) { float_script_vars[i] = v; }
  void setLayer(uint32_t i);
  void setEntity(void *e) { associated_game_entity = e; }
  void setRotz(float z) { rotation.z = z; }
  void setFlags(AnmVM_flags_t const &flags) { bitflags = flags; }
  AnmVM_flags_t getFlags() const { return bitflags; }
  int getMode() const;
  int getLayer() const { return layer; }
  int getZdis() const;
  glm::vec3 getRotation();
  void *getEntity() const { return associated_game_entity; }
  AnmVMList_t *getChild() { return list_of_children.next; }
  uint32_t getID() const { return id.val; }
  AnmSprite getSprite() const;
  void getSpriteCorners2D(glm::vec2 *corners);
  void getParentData(glm::vec3 &pos, glm::vec3 &rot, glm::vec2 &scale);

  int run();
  void step_interpolators();
  void update_variables_growth();

  void alloc_special_vertex_buffer(int size) {
    if (special_vertex_buffer_data)
      free(special_vertex_buffer_data);
    special_vertex_buffer_size = size;
    special_vertex_buffer_data = malloc(size);
  }
  void transform_coordinate(glm::vec3 &pos);
  glm::vec3 get_pos_with_root();
  glm::vec3 get_own_transformed_pos();
  void write_sprite_corners_2d(glm::vec4 *corners);
  void write_sprite_corners__without_rot(glm::vec4 &v1, glm::vec4 &v2,
                                         glm::vec4 &v3, glm::vec4 &v4);
  void write_sprite_corners__with_z_rot(glm::vec4 &v1, glm::vec4 &v2,
                                        glm::vec4 &v3, glm::vec4 &v4);
  int write_sprite_corners__mode_4();

  void write_texture_circle_vertices();
  static int cnt;

  // PREFIX
  int32_t interrupt_return_time = -99;
  int32_t interrupt_return_offset = 0;
  uint32_t layer = 0;
  int32_t anm_loaded_index = 0;
  int32_t sprite_id = 0;
  int32_t script_id = 0;
  int32_t instr_offset = 0;
  glm::vec3 pos = {};
  glm::vec3 rotation = {};
  glm::vec3 angular_velocity = {};
  glm::vec2 scale = {1.f, 1.f};
  glm::vec2 scale_2 = {1.f, 1.f};
  glm::vec2 scale_growth = {};
  glm::vec2 uv_scale = {1.f, 1.f};
  glm::vec2 sprite_size = {};
  glm::vec2 uv_scroll_pos = {};
  glm::vec2 anchor_offset = {};
  // undefined 4
  Interp<glm::vec3> pos_i = {};
  Interp<glm::vec3> rgb1_i = {};
  Interp<int> alpha1_i = {};
  Interp<glm::vec3> rotate_i = {};
  Interp<float> rotate_2d_i = {};
  Interp<glm::vec2> scale_i = {};
  Interp<glm::vec2> scale_2_i = {};
  Interp<glm::vec2> uv_scale_i = {};
  Interp<glm::vec3> rgb2_i = {};
  Interp<int> alpha2_i = {};
  Interp<float> u_vel_i = {};
  Interp<float> v_vel_i = {};
  glm::vec2 uv_quad_of_sprite[4] = {};
  glm::vec2 uv_scroll_vel = {};
  glm::mat4 __matrix_1 = {};
  glm::mat4 __matrix_2 = {};
  glm::mat4 __matrix_3 = {};
  int32_t pending_interrupt = 0;
  int32_t __time_of_last_sprite_set__unused = 0;
  // undefined4
  int16_t script_id_2 = 0;
  // undefined2
  int32_t int_script_vars[4] = {0, 0, 0, 0};
  float float_script_vars[4] = {0, 0, 0, 0};
  glm::vec3 __script_vars_33_34_35 = {}; /* rotation related vars 33 34 35 */
  int32_t __script_var_8 = 0;
  int32_t __script_var_9 = 0;
  float rand_param_one = 1.f;
  float rand_param_pi = PI;
  int32_t rand_param_int = 1;
  glm::vec3 __pos_2 = {};
  glm::vec3 last_rendered_quad_in_surface_space[4];
  int32_t mode_of_create_child = -1;
  NSEngine::Color color_1 = {255, 255, 255, 255};
  NSEngine::Color color_2 = {0, 0, 0, 0};
  NSEngine::Color mixed_inherited_color = {255, 255, 255, 255};
  uint8_t font_dims[2] = {};

  // undefined2
  AnmVM_flags_t bitflags;
  float cam_fade_begin = 0.f;
  float cam_fade_end = 0.f;

  // SUFFIX
  AnmID id = 0;
  int32_t fast_id = 0;
  int32_t time_in_script = 0;  // zTimer
  int32_t __timer_1c = 0;      // zTimer
  AnmVMList_t node_in_global_list = {this, nullptr, nullptr};
  AnmVMList_t __node_as_child = {this, nullptr, nullptr};
  AnmVMList_t list_of_children = {};
  AnmVMList_t __wierd_list = {this, nullptr, nullptr};
  AnmVM *next_in_layer__disused = nullptr;
  int32_t new_field_added_in_1_00b = 0;
  AnmVM *__root_vm__or_maybe_not = nullptr;
  AnmVM *parent_vm = nullptr;
  float slowdown = 0.f;
  void *special_vertex_buffer_data = nullptr;
  int32_t special_vertex_buffer_size = 0;
  int32_t index_of_on_wait = 0;
  int32_t index_of_on_tick = 0;
  int32_t index_of_on_draw = 0;
  int32_t index_of_on_destroy = 0;
  int32_t index_of_on_interrupt = 0;
  int32_t index_of_on_copy_1__disused = 0;
  int32_t index_of_on_copy_2__disused = 0;
  int32_t index_of_sprite_mapping_func = 0;
  glm::vec3 entity_pos = {};
  void *associated_game_entity = nullptr;
  glm::vec3 __rotation_related = {};

  friend class AnmManager;
  friend class AnmManager;
  friend class AnmFile;
  friend class StdFile;
};

struct AnmFastVM {
  AnmVM vm;
  AnmFastVMList_t freelistNode = {this, nullptr, nullptr};
  bool isAlive = false;
  AnmID fastID = 0;
};

#endif // ANMVM_H_
