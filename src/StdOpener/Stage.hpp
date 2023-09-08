#ifndef STAGE_INCLUDED_H
#define STAGE_INCLUDED_H

#include "../AnmOpener/AnmFile.h"
#include "../Supervisor.h"
#include "../Timer.hpp"
#include "../UpdateFuncRegistry.h"
#include "./stdOpener.h"
#include "../Fog.hpp"

struct CameraSkyInterp_t {
  Interp<float> begin_dist;
  Interp<float> end_dist;
  Interp<glm::vec4> color;
  int32_t end_time = 0;

  void start(CameraSky_t const &begin, CameraSky_t const &end,
             int32_t time, int32_t mode);
  CameraSky_t step();
};

class Stage;

struct StageDistortion_t {
  Fog_t *fog_ptr = nullptr;
  float r_target = 0.f;
  float r = 0.f;
  int32_t field_0x0c = 0;
  Timer_t time = 0;
  float ox = 0.f;
  float oy = 0.f;
  int32_t mode = 0;
};

struct StageInner_t {
  Timer_t time_in_script = 0;
  int32_t instr_offset = 0;
  int32_t rocking_mode = 0;
  Timer_t rocking_timer = 0;
  Timer_t rocking_6_timer = 0;
  Interp<glm::vec3> camera_facing_i = {};
  Interp<glm::vec3> camera_pos_i = {};
  Interp<glm::vec3> camera_up_i = {};
  CameraSkyInterp_t camera_sky_i = {};
  Interp<float> camera_fov_i = {};
  Camera_t camera = {};
  Stage *full = nullptr;
  AnmVM vms[8] = {};
  int32_t anm_layers[8] = {};
  float draw_distance_sq = 0.f;
  StageDistortion_t distortion = {};
  NSEngine::Color some_bg_color_unrelated_to_ins_13 = {};

  void update_distortion();
};

class Stage {
public:
  Stage();
  ~Stage();

  int f_on_tick();
  int f_on_draw_1();
  int f_on_draw_2();
  void run_std();
  void draw_layer(int i);

  void create_face_vms();

  void interrupt(unsigned int i);

  // undefined4
  UpdateFunc *on_tick = nullptr;
  UpdateFunc *on_draw_1 = nullptr;
  StageInner_t inner = {};
  // copy of stage inner for LoLK
  AnmVM *faces_vms = nullptr;
  // copy of faces vms for LoLK
  void *std_file;
  StdOpener::std_entry_header_t **std_object_ptrs = nullptr;
  StdOpener::std_object_instance_t *std_faces = nullptr;
  StdOpener::std_instr_t *beginning_of_script = nullptr;
  AnmFile *stage_anm = nullptr;
  int32_t num_ticks_alive = 0;
  int32_t num_vms_outside_draw_distance = 0;
  int32_t num_vms_drawn = 0;
  int32_t flags = 0;
  Timer_t some_countdown_timer = 0;
  int32_t stage_num = 0;
  int32_t num_ticks_alive_2 = 0;
  StdOpener::std_header_10_t *std_file_data = nullptr;
  int32_t std_file_size = 0;
  UpdateFunc *on_draw_2 = nullptr;
};

extern Stage *STAGE_PTR;
extern Stage *ANOTHER_STAGE_PTR;

#endif // !STAGE_INCLUDED_H
