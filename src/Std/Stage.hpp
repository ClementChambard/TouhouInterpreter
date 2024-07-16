#ifndef STAGE_INCLUDED_H
#define STAGE_INCLUDED_H

#include "../Anm/AnmFile.h"
#include <Timer.hpp>
#include "../UpdateFuncRegistry.h"
#include "./stdOpener.h"
#include "../Fog.hpp"
#include "../Anm/Camera.hpp"

struct CameraSkyInterp_t {
  Interp<f32> begin_dist;
  Interp<f32> end_dist;
  Interp<ns::vec4> color;
  i32 end_time = 0;

  void start(anm::CameraSky_t const &begin, anm::CameraSky_t const &end,
             i32 time, i32 mode);
  anm::CameraSky_t step();
};

class Stage;

struct StageDistortion_t {
  Fog_t *fog_ptr = nullptr;
  f32 r_target = 0.f;
  f32 r = 0.f;
  i32 field_0x0c = 0;
  ns::Timer_t time = 0;
  f32 ox = 0.f;
  f32 oy = 0.f;
  i32 mode = 0;
};

struct StageInner_t {
  ns::Timer_t time_in_script = 0;
  i32 instr_offset = 0;
  i32 rocking_mode = 0;
  ns::Timer_t rocking_timer = 0;
  ns::Timer_t rocking_6_timer = 0;
  Interp<ns::vec3> camera_facing_i = {};
  Interp<ns::vec3> camera_pos_i = {};
  Interp<ns::vec3> camera_up_i = {};
  CameraSkyInterp_t camera_sky_i = {};
  Interp<f32> camera_fov_i = {};
  anm::Camera_t camera = {};
  Stage *full = nullptr;
  anm::VM vms[8] = {};
  i32 anm_layers[8] = {};
  f32 draw_distance_sq = 0.f;
  StageDistortion_t distortion = {};
  ns::Color some_bg_color_unrelated_to_ins_13 = {};

  void update_distortion();
};

class Stage {
public:
  Stage();
  ~Stage();

  i32 f_on_tick();
  i32 f_on_draw_1();
  i32 f_on_draw_2();
  void run_std();
  void draw_layer(i32 i);

  void create_face_vms();

  void interrupt(u32 i);

  // undefined4
  UpdateFunc *on_tick = nullptr;
  UpdateFunc *on_draw_1 = nullptr;
  StageInner_t inner = {};
  // copy of stage inner for LoLK
  anm::VM *faces_vms = nullptr;
  // copy of faces vms for LoLK
  void *std_file;
  StdOpener::std_entry_header_t **std_object_ptrs = nullptr;
  StdOpener::std_object_instance_t *std_faces = nullptr;
  StdOpener::std_instr_t *beginning_of_script = nullptr;
  anm::File *stage_anm = nullptr;
  i32 num_ticks_alive = 0;
  i32 num_vms_outside_draw_distance = 0;
  i32 num_vms_drawn = 0;
  i32 flags = 0;
  ns::Timer_t some_countdown_timer = 0;
  i32 stage_num = 0;
  i32 num_ticks_alive_2 = 0;
  StdOpener::std_header_10_t *std_file_data = nullptr;
  i32 std_file_size = 0;
  UpdateFunc *on_draw_2 = nullptr;
};

extern Stage *STAGE_PTR;
extern Stage *ANOTHER_STAGE_PTR;

#endif // !STAGE_INCLUDED_H
