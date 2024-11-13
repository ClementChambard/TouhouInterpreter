#include "./Stage.hpp"
#include "../Anm/AnmManager.h"
#include "../GlobalData.h"
#include "../Hardcoded.h"
#include "../ScreenEffect.hpp"
#include "../Supervisor.h"
#include <memory.h>
#include <platform/filesystem.h>
#include <math/math.hpp>

Stage *STAGE_PTR = nullptr;
Stage *ANOTHER_STAGE_PTR = nullptr;

Stage::Stage() {
  STAGE_PTR = this;
  stage_num = GLOBALS.inner.STAGE_NUM;
  // if (std_file_data == NULL) {
  //   DAT_0052a810 = 0;
  //   pzVar4 = param_2;
  //   do {
  //     piVar6 = &pzVar4->field0_0x0;
  //     pzVar4 = (zStage *)((int)&pzVar4->field0_0x0 + 1);
  //   } while (*(char *)piVar6 != '\0');
  //   piVar6 = (int32_t *)0x52a80f;
  //   do {
  //     pcVar1 = (char *)((int)piVar6 + 1);
  //     piVar6 = (int32_t *)((int)piVar6 + 1);
  //   } while (*pcVar1 != '\0');
  //   pzVar5 = param_2;
  //   for (uVar2 = (uint)((int)pzVar4 - (int)param_2) >> 2; uVar2 != 0;
  //        uVar2 -= 1) {
  //     *piVar6 = pzVar5->field0_0x0;
  //     pzVar5 = (zStage *)&pzVar5->on_tick;
  //     piVar6 = piVar6 + 1;
  //   }
  //   for (uVar2 = (int)pzVar4 - (int)param_2 & 3; uVar2 != 0; uVar2 -= 1) {
  //     *(undefined *)piVar6 = *(undefined *)&pzVar5->field0_0x0;
  //     pzVar5 = (zStage *)((int)&pzVar5->field0_0x0 + 1);
  //     piVar6 = (int32_t *)((int)piVar6 + 1);
  //   }
  //   file_ptr = reads_file_into_new_allocation_4020d0(
  //       &DAT_0052a810, (char **)&this->std_file_size, 0);
  //   this->std_file_data = file_ptr;
  //   err_msg = extraout_ECX;
  //   if (file_ptr == NULL) {
  //     display_error_dialogue(err_msg, OUTPUT_BUF_FOR_STRINGS, &DAT_004a1334);
  //     return -1;
  //   }
  // }
  ns::fs::File f;
  ns::fs::open(STAGE_DATA_TABLE[stage_num]["std_filename"].asCString(), ns::fs::Mode::READ, true, &f);
  usize size;
  ns::fs::fsize(&f, &size);
  std_file = ns::alloc_raw(size);
  ns::fs::read_all_bytes(&f, reinterpret_cast<bytes>(std_file), &size);
  std_file_size = size;
  ns::fs::close(&f);
  this->std_file_data =
      reinterpret_cast<StdOpener::std_header_10_t *>(ns::alloc_raw(std_file_size));
  ns::mem_copy(std_file_data, std_file, std_file_size);
  /* load anm from string at offset 0x10 in std file */
  stage_anm = anm::loadFile(stage_num % 2 + 3, std_file_data->anm_name);
  // if (!stage_anm) {
  //   sub_4025a0_logs_debug_message(&OUTPUT_BUF_FOR_STRINGS,
  //                                 &DAT_004a136c);
  //   display_error_dialogue(&OUTPUT_BUF_FOR_STRINGS, &DAT_004a1334);
  //   return -1;
  // }

  std_object_ptrs =
      new StdOpener::std_entry_header_t *[std_file_data->nb_objects];
  for (int i = 0; i < std_file_data->nb_objects; i++) {
    std_object_ptrs[i] = reinterpret_cast<StdOpener::std_entry_header_t *>(
        reinterpret_cast<uint64_t>(std_file_data) +
        static_cast<uint32_t>(std_file_data->offsets[i]));
  }

  std_faces = reinterpret_cast<StdOpener::std_object_instance_t *>(
      reinterpret_cast<uint64_t>(std_file_data) + std_file_data->faces_offset);
  beginning_of_script = reinterpret_cast<StdOpener::std_instr_t *>(
      reinterpret_cast<uint64_t>(std_file_data) + std_file_data->script_offset);

  faces_vms = new anm::VM[std_file_data->nb_faces];
  // same for lolk
  this->inner.full = this;
  inner.camera = SUPERVISOR.cameras[3];
  inner.camera.position = {0.0, 0.0, -600.0};
  inner.camera.facing = {0.0, 300.0, 600.0};
  inner.camera.up = {0.0, 1.0, 0.0};
  inner.camera.__rocking_vector_1 = {0.0, 0.0, 0.0};
  inner.camera.__rocking_vector_2 = {0.0, 0.0, 0.0};
  inner.draw_distance_sq = 9610000.0;
  on_tick = new UpdateFunc([this]() { return this->f_on_tick(); });
  on_tick->flags &= 0xfffffffd;
  UPDATE_FUNC_REGISTRY.register_on_tick(on_tick, 18);
  on_draw_1 = new UpdateFunc([this]() { return this->f_on_draw_1(); });
  on_draw_1->flags &= 0xfffffffd;
  UPDATE_FUNC_REGISTRY.register_on_draw(on_draw_1, 3);
  on_draw_2 = new UpdateFunc([this]() { return this->f_on_draw_2(); });
  on_draw_2->flags &= 0xfffffffd;
  UPDATE_FUNC_REGISTRY.register_on_draw(on_draw_2, 6);
  num_ticks_alive_2 = 0;
  inner.time_in_script.set(0);
  flags |= 1;
  inner.camera_facing_i.end_time = 0;
  inner.camera_pos_i.end_time = 0;
  create_face_vms();
}

Stage::~Stage() {
  if (on_tick)
    UPDATE_FUNC_REGISTRY.unregister(on_tick);
  if (on_draw_1)
    UPDATE_FUNC_REGISTRY.unregister(on_draw_1);
  if (on_draw_2)
    UPDATE_FUNC_REGISTRY.unregister(on_draw_2);
  /* destroy arrays */
  if (faces_vms)
    delete[] faces_vms;
  // same for lolk vms
  /* free spawned vms */
  if (std_file)
    ns::free_raw(std_file, std_file_size);
  if (std_file_data)
    ns::free_raw(std_file_data, std_file_size);
  // if (inner.distortion.fog_ptr)
  //   delete inner.distortion.fog_ptr;
  // same for lolk distortion

  delete[] std_object_ptrs;

  // if (!(GLOBALS.FLAGS & 1))
  //   ANM_MANAGER_PTR->_unload_file(this->stage_num % 2 + 3);
  // if (STAGE_PTR == this)
  //   STAGE_PTR = NULL;
  // if (ANOTHER_STAGE_PTR == this)
  //   ANOTHER_STAGE_PTR = NULL;
  return;
}

int Stage::f_on_tick() {
  if (flags & 8)
    return 1;
  if ((flags & 4) && this->some_countdown_timer >= 60)
    return 1;
  inner.camera.screen_shake = {0.f, 0.f};
  inner.camera.__vec3_104 = {0.f, 0.f, 0.f};
  inner.some_bg_color_unrelated_to_ins_13 = {128, 128, 128, 0};
  inner.camera.facing_normalized = ns::normalize(inner.camera.facing + inner.camera.__rocking_vector_2);
  if (!(flags & 4) || some_countdown_timer < 30) {
    for (int ent_id = 0; ent_id < std_file_data->nb_objects; ent_id++) {
      if (!(std_object_ptrs[ent_id]->vmAliveBits & 1))
        continue;
      num_ticks_alive = 0;
      for (auto obj = std_object_ptrs[ent_id]->quads;
           obj->unknown != static_cast<uint16_t>(-1);
           obj = reinterpret_cast<StdOpener::std_object_t *>(
               reinterpret_cast<uint64_t>(obj) + obj->size)) {
        faces_vms[obj->vmid].update();
        if (faces_vms[obj->vmid].instr_offset >= 0)
          num_ticks_alive++;
      }
      if (num_ticks_alive == 0)
        std_object_ptrs[ent_id]->vmAliveBits &= 0xfe;
    }
    run_std();
  }
  /* copy camera from this to supervisor 3 */
  SUPERVISOR.cameras[3] = inner.camera;

  /* update sprite vms */
  for (int i = 0; i < 8; i++)
    inner.vms[i].update();
  /* update distortion */
  inner.update_distortion();
  num_ticks_alive_2++;
  return 1;
}

#define IARG(id) (*reinterpret_cast<int32_t *>(&instr->args[id * 4]))
#define FARG(id) (*reinterpret_cast<float *>(&instr->args[id * 4]))
void Stage::run_std() {
  auto instr = reinterpret_cast<StdOpener::std_instr_t *>(
      reinterpret_cast<uint64_t>(beginning_of_script) + inner.instr_offset);
  while (static_cast<i32>(instr->time) <=
         inner.time_in_script
             .current) {   // conditionnal jump depends on unitialized value
    switch (instr->type) { // invalid read of size 2 ???
    case 0:                /* STOP */
      goto update_rest;
    case 1:                         /* JUMP */
      inner.instr_offset = IARG(0); // Before stage3 boss spawn th16 TODO: Use
                                    // of uninitialised value of size 8
      inner.time_in_script = IARG(1);
      instr = beginning_of_script + inner.instr_offset;
      continue; // invalid read of size 4 ???
    case 2:     /* POS */
      inner.camera.__vec3_104 = inner.camera.position;
      inner.camera.position = {FARG(0), FARG(1), FARG(2)};
      inner.camera.__vec3_104 = inner.camera.position - inner.camera.__vec3_104;
      break;
    case 3: /* POS TIME */
      inner.camera_pos_i.start(inner.camera.position,
                               {FARG(2), FARG(3), FARG(4)}, IARG(0), IARG(1));
      break;
    case 4: /* FACING */
      inner.camera.facing = {FARG(0), FARG(1), FARG(2)};
      break;
    case 5: /* FACING TIME */
      inner.camera_facing_i.start(
          inner.camera.facing, {FARG(2), FARG(3), FARG(4)}, IARG(0), IARG(1));
      break;
    case 6: /* UP */
      inner.camera.up = {FARG(0), FARG(1), FARG(2)};
      break;
    case 7: /* FOV */
      inner.camera.fov_y = FARG(0);
      break;
    case 8: /* FOG */
      inner.camera.sky.color_components[0] = (IARG(0) >> 0) & 0xff;
      inner.camera.sky.color_components[1] = (IARG(0) >> 8) & 0xff;
      inner.camera.sky.color_components[2] = (IARG(0) >> 16) & 0xff;
      inner.camera.sky.color_components[3] = (IARG(0) >> 24) & 0xff;
      inner.camera.sky.color.b = inner.camera.sky.color_components[0];
      inner.camera.sky.color.g = inner.camera.sky.color_components[1];
      inner.camera.sky.color.r = inner.camera.sky.color_components[2];
      inner.camera.sky.color.a = inner.camera.sky.color_components[3];
      inner.camera.sky.begin_distance = FARG(1);
      inner.camera.sky.end_distance = FARG(2);
      break;
    case 9: /* FOG TIME */
    {
      anm::CameraSky_t goal;
      int arg2 = IARG(2);
      goal.color_components[0] = (arg2 >> 0) & 0xff;
      goal.color_components[1] = (arg2 >> 8) & 0xff;
      goal.color_components[2] = (arg2 >> 16) & 0xff;
      goal.color_components[3] = (arg2 >> 24) & 0xff;
      goal.color.b = goal.color_components[0];
      goal.color.g = goal.color_components[1];
      goal.color.r = goal.color_components[2];
      goal.color.a = goal.color_components[3];
      goal.begin_distance = FARG(3);
      goal.end_distance = FARG(4);
      inner.camera_sky_i.start(inner.camera.sky, goal, IARG(0), IARG(1));
    } break;
    case 10: /* POS BEZIER */
      inner.camera_pos_i.start_bezier(
          inner.camera.position, {FARG(4), FARG(5), FARG(6)},
          {FARG(1), FARG(2), FARG(3)}, {FARG(7), FARG(8), FARG(9)}, IARG(0));
      break;
    case 11: /* FACING BEZIER */
      inner.camera_facing_i.start_bezier(
          inner.camera.facing, {FARG(4), FARG(5), FARG(6)},
          {FARG(1), FARG(2), FARG(3)}, {FARG(7), FARG(8), FARG(9)}, IARG(0));
      break;
    case 12: /* ROCKING MODE */
      inner.rocking_mode = IARG(0);
      if (inner.rocking_mode == 0)
        inner.camera.__rocking_vector_1 = {0.0, 0.0, 0.0};
      if (inner.rocking_mode == 6)
        inner.rocking_6_timer.set(0);
      else if (inner.rocking_mode == 2)
        inner.rocking_timer.set(0x200);
      else
        inner.rocking_timer.set(0);
      break;
    case 13: /* BG COLOR */
    {
      ns::Color c;
      c.b = (IARG(0) >> 0) & 0xff;
      c.g = (IARG(0) >> 8) & 0xff;
      c.r = (IARG(0) >> 16) & 0xff;
      c.a = (IARG(0) >> 24) & 0xff;
      SUPERVISOR.background_color = c;
    } break;
    case 14: /* SPRITE */
      if (IARG(1) == -2) {
        inner.vms[IARG(0)].bitflags.visible = false;
      } else if (IARG(1) == -1) {
        inner.vms[IARG(0)].instr_offset = -1;
        inner.vms[IARG(0)].bitflags.visible = false;
      } else if (IARG(1) >= 0) {
        stage_anm->copyFromLoaded(&inner.vms[IARG(0)], IARG(1));
        inner.vms[IARG(0)].parent_vm = NULL;
        inner.vms[IARG(0)].__root_vm__or_maybe_not = NULL;
        inner.vms[IARG(0)].run();
      }
      inner.anm_layers[IARG(0)] = IARG(2);
      break;
    case 17: /* DISTORTION */
      if (inner.distortion.fog_ptr)
        delete inner.distortion.fog_ptr;
      inner.distortion.fog_ptr = NULL;
      inner.distortion.r_target = 112.0;
      inner.distortion.r = 192.0;
      inner.distortion.field_0x0c = -1;
      inner.distortion.ox = 0.0;
      inner.distortion.oy = 0.0;
      inner.distortion.time = 0;
      inner.distortion.mode = IARG(0);
      inner.distortion.fog_ptr = new Fog_t(inner.distortion.mode != 1 ? 17 : 7);
      break;
    case 18: /* UP TIME */
      inner.camera_up_i.start(inner.camera.up, {FARG(2), FARG(3), FARG(4)},
                              IARG(0), IARG(1));
      break;
    case 19: /* INTERRUPT ANMS */
      for (int i = 0; i < 8; i++) {
        if (inner.vms[i].instr_offset < 0)
          continue;
        inner.vms[i].interrupt(IARG(0) + 7);
        inner.vms[i].run();
      }
      if (!faces_vms)
        break;
      for (int i = 0; i < std_file_data->nb_faces; i++) {
        faces_vms[i].interrupt(IARG(0) + 7);
        faces_vms[i].update();
      }
      break;
    case 20: /* DRAW DISTANCE */
      inner.draw_distance_sq = FARG(0) * FARG(0);
      break;
    case 21: /* FOV TIME */
      inner.camera_fov_i.start(inner.camera.fov_y, FARG(2), IARG(0), IARG(1));
      break;
    }
    inner.instr_offset += instr->size; // invalid read of size 2 ???
    instr = reinterpret_cast<StdOpener::std_instr_t *>(
        reinterpret_cast<uint64_t>(beginning_of_script) + inner.instr_offset);
  }
  inner.time_in_script++;
update_rest:

  if (inner.camera_pos_i.end_time != 0)
    inner.camera.position = inner.camera_pos_i.step();
  if (inner.camera_facing_i.end_time != 0)
    inner.camera.facing = inner.camera_facing_i.step();
  if (inner.camera_sky_i.end_time != 0)
    inner.camera.sky = inner.camera_sky_i.step();
  if (inner.camera_up_i.end_time != 0)
    inner.camera.up = inner.camera_up_i.step();
  if (inner.camera_fov_i.end_time != 0)
    (inner.camera).fov_y = inner.camera_fov_i.step();

  float intensity;
  switch (inner.rocking_mode) {
  case 1:
    intensity = ns::min(inner.rocking_timer.current_f / 512.f, 1.0f);
    inner.camera.__rocking_vector_1.x =
        ns::sin(inner.rocking_timer.current_f * ns::PI<f32> / 0x200) * -10.0 *
        intensity;
    inner.camera.__rocking_vector_1.z =
        ns::sin(inner.rocking_timer.current_f * ns::PI<f32> / 0x100) * -10.0 *
        intensity;
    inner.camera.__rocking_vector_2.x =
        ns::sin(inner.rocking_timer.current_f * ns::PI<f32> / 0x200) * 5.0 *
        intensity;
    inner.camera.__rocking_vector_2.z =
        ns::sin(inner.rocking_timer.current_f * ns::PI<f32> / 0x100) * 5.0 *
        intensity;
    inner.camera.up.x =
        ns::sin(inner.rocking_timer.current_f * ns::PI<f32> / 0x200) * -0.01 *
        intensity;
    inner.rocking_timer.increment();
    if (inner.rocking_timer.current >= 0x800)
      inner.rocking_timer.set(0x400);
    break;
  case 2:
    inner.camera.up.x =
        -ns::sin(inner.rocking_timer.current_f * ns::PI<f32> / 0x600);
    inner.camera.up.z =
        ns::cos(inner.rocking_timer.current_f * ns::PI<f32> / 0x600);
    inner.rocking_timer.increment();
    if (inner.rocking_timer.current >= 0xc00)
      inner.rocking_timer.set(0);
    break;
  case 3:
    intensity = ns::min(80.0f, inner.rocking_timer.current_f / 0x400 * 80.f);
    inner.camera.__rocking_vector_1.y = 0.0;
    inner.camera.__rocking_vector_1.z = 0.0;
    inner.camera.__rocking_vector_1.x =
        ns::sin((inner.rocking_timer.current % 0x400) * ns::PI<f32> / 0x200) *
        intensity;
    inner.camera.__rocking_vector_2.x =
        (ns::sin((inner.rocking_timer.current % 0x400) * ns::PI<f32> / 0x200) *
         intensity) /
        -10.0;
    inner.rocking_timer.increment();
    if (inner.rocking_timer.current >= 0x800)
      inner.rocking_timer.set(0x400);
    break;
  case 4:
    inner.camera.up.x =
        -ns::sin(inner.rocking_timer.current_f * ns::PI<f32> / 0x600);
    inner.camera.up.z =
        -ns::cos(inner.rocking_timer.current_f * ns::PI<f32> / 0x600);
    inner.rocking_timer.increment();
    if (inner.rocking_timer.current >= 0xc00)
      inner.rocking_timer.set(0);
    break;
  case 5:
    intensity = ns::min(1.f, inner.rocking_timer.current_f / 0x200);
    inner.camera.__rocking_vector_1.x =
        ns::sin(((inner.rocking_timer.current % 0x400) * ns::PI<f32> / 0x200)) *
        -5.0 * intensity;
    inner.camera.__rocking_vector_2.x =
        ns::sin(((inner.rocking_timer.current % 0x400) * ns::PI<f32> / 0x200)) *
        2.5 * intensity * intensity;
    inner.camera.up.x =
        ns::sin(((inner.rocking_timer.current % 0x400) * ns::PI<f32> / 0x200)) *
        -0.005 * intensity;
    inner.rocking_timer.increment();
    if (inner.rocking_timer.current >= 0x800)
      inner.rocking_timer.set(0x400);
    break;
  case 6:
    intensity =
        ns::max(0.0, (1.0 - inner.rocking_6_timer.current_f / 0x200) * 80.0);
    inner.camera.__rocking_vector_1.y = 0.0;
    inner.camera.__rocking_vector_1.z = 0.0;
    inner.camera.__rocking_vector_1.x =
        ns::sin((inner.rocking_timer.current % 0x400) * ns::PI<f32> / 0x200) *
        intensity;
    inner.camera.__rocking_vector_2.x =
        (ns::sin((inner.rocking_timer.current % 0x400) * ns::PI<f32> / 0x200) *
         intensity) /
        -10.0;
    inner.rocking_timer.increment();
    inner.rocking_6_timer.increment();
    if (inner.rocking_timer.current >= 0x800)
      inner.rocking_timer.set(0x400);
    break;
  case 9:
    inner.camera.__rocking_vector_1.x =
        ns::sin(((inner.rocking_timer.current_f * ns::PI<f32>) / 0x400 -
                 ns::PI<f32>)) *
        70.0;
    inner.camera.__rocking_vector_1.z =
        ns::sin(2 * ((inner.rocking_timer.current_f * ns::PI<f32>) / 0x400 -
                     ns::PI<f32>)) *
        200.0;
    inner.camera.up.x =
        ns::sin(((inner.rocking_timer.current_f * ns::PI<f32>) / 0x400 -
                 ns::PI<f32>)) *
        -0.1;
    inner.rocking_timer.increment();
    if (inner.rocking_timer.current >= 0x800)
      inner.rocking_timer.set(0);
    break;
  case 10:
    inner.camera.__rocking_vector_1.x =
        ns::sin((inner.rocking_timer.current_f * ns::PI<f32>) / 0x200 -
                ns::PI<f32>) *
        -50.0;
    inner.camera.up.x =
        ns::sin((inner.rocking_timer.current_f * ns::PI<f32>) / 0x200 -
                ns::PI<f32>) *
        -0.1;
    inner.rocking_timer.increment();
    if (inner.rocking_timer.current >= 0x400)
      inner.rocking_timer.set(0);
    break;
  case 11:
    inner.camera.__rocking_vector_1.x =
        ns::sin((inner.rocking_timer.current_f * ns::PI<f32>) / 0x100 -
                ns::PI<f32>) *
        -15.0;
    inner.camera.up.x =
        ns::sin((inner.rocking_timer.current_f * ns::PI<f32>) / 0x100 -
                ns::PI<f32>) *
        -0.01;
    inner.rocking_timer.increment();
    if (inner.rocking_timer.current >= 0x200)
      inner.rocking_timer.set(0);
    break;
  }
}

void CameraSkyInterp_t::start(anm::CameraSky_t const &begin,
                              anm::CameraSky_t const &end, int32_t time,
                              int32_t mode) {
  begin_dist.start(begin.begin_distance, end.begin_distance, time, mode);
  end_dist.start(begin.end_distance, end.end_distance, time, mode);
  color.start(
      ns::vec4(begin.color.r, begin.color.g, begin.color.b, begin.color.a),
      ns::vec4(end.color.r, end.color.g, end.color.b, end.color.a), time, mode);
  end_time = time;
}

ns::vec4 color_components = ns::vec4();
ns::Color color{};
anm::CameraSky_t CameraSkyInterp_t::step() {
  if (begin_dist.end_time > 0)
    begin_dist.step();
  if (end_dist.end_time > 0)
    end_dist.step();
  if (color.end_time > 0)
    color.step();
  end_time = begin_dist.end_time;
  anm::CameraSky_t out = {};
  out.begin_distance = begin_dist.current;
  out.end_distance = end_dist.current;
  out.color.r = static_cast<uint8_t>(color.current.r);
  out.color.g = static_cast<uint8_t>(color.current.g);
  out.color.b = static_cast<uint8_t>(color.current.b);
  out.color.a = static_cast<uint8_t>(color.current.a);
  out.color_components =
      ns::vec4(out.color.b, out.color.g, out.color.r, out.color.a);
  return out;
}

void StageInner_t::update_distortion() {
  if (distortion.fog_ptr == nullptr)
    return;
  if (distortion.mode == 1) {
    // if (SPELLCARD_PTR && !(SPELLCARD_PTR->flags & 1)) {
    //   distortion.time++;
    //   return;
    // }
    distortion.fog_ptr->reset_area(-192.0, 320.0, 384.0, 128.0);
    float magnitude = ns::min(6.f, (distortion.time.current_f * 6.f) / 60.f);
    float ox = distortion.ox;
    float oy = distortion.oy;
    for (int j = 0; j < distortion.fog_ptr->vm_count; j++) {
      for (int i = 0; i < distortion.fog_ptr->vertex_count; i++) {
        auto &v = distortion.fog_ptr
                      ->vertex_array[i + j * distortion.fog_ptr->vertex_count];
        v.diffuse_color.a = 128;
        if (j != 0 && i != 0 && j != distortion.fog_ptr->vm_count - 1 &&
            i != distortion.fog_ptr->vertex_count - 1) {
          float strength =
              (i * magnitude) / (distortion.fog_ptr->vertex_count - 1);
          v.transformed_pos.x += ns::sin(ox) * strength;
          v.transformed_pos.y += ns::sin(oy) * strength;
        }
        ox += 0.668424;
        ns::angle_normalize(ox);
      }
      oy -= 1.495997;
      ns::angle_normalize(oy);
    }
    distortion.ox += 0.04908739;
    ns::angle_normalize(distortion.ox);
    distortion.oy += 0.03926991;
    ns::angle_normalize(distortion.oy);
  } else if (distortion.mode == 2) {
    if (distortion.r_target <= distortion.r)
      distortion.r -= 2.0;
    distortion.fog_ptr->reset_area(-distortion.r, 224.0 - distortion.r,
                                   2 * distortion.r, 2 * distortion.r);
    float ox = distortion.ox;
    float oy = distortion.oy;
    for (int i = 0; i < distortion.fog_ptr->vm_count; i++) {
      for (int j = 0; j < distortion.fog_ptr->vertex_count; j++) {
        auto &p = distortion.fog_ptr
                      ->pos_array[j + i * distortion.fog_ptr->vertex_count];
        auto &v = distortion.fog_ptr
                      ->vertex_array[j + i * distortion.fog_ptr->vertex_count];
        ns::vec2 local_18 = {p.x - anm::BACK_BUFFER_SIZE.x / 2.f,
                             p.y - anm::BACK_BUFFER_SIZE.y / 2.f};
        float distin = distortion.r * distortion.r - ns::length_sq(local_18);
        if (distin < 0.0) {
          v.diffuse_color.a = 0;
        } else {
          float s = distin / (distortion.r * distortion.r);
          local_18 = ns::normalize(local_18) * s * 32.f;
          local_18.x += ns::sin(ox) * s * 8.0;
          local_18.y += ns::sin(oy) * s * 8.0;
          v.diffuse_color = {255, 255, 255, 96};
          v.transformed_pos.x += local_18.x;
          v.transformed_pos.y += local_18.y;
        }
        ox += 1.570796;
        ns::angle_normalize(ox);
        oy -= 0.6981317;
        ns::angle_normalize(oy);
      }
    }
    distortion.ox += 0.04908739;
    ns::angle_normalize(distortion.ox);
    distortion.oy += (ns::frand() * ns::PI<f32>) / 40.0 + 0.03926991;
    ns::angle_normalize(distortion.oy);
  }
  distortion.time++;
}

int Stage::f_on_draw_1() {
  anm::enable_culling(false);
  anm::enable_blending();
  ns::vec4 viewport_rect;
  ns::Color color;

  if (flags & 8)
    return 1;
  if (!(flags & 4) || some_countdown_timer < 60) {
    anm::flush_vbos();
    inner.camera.screen_shake = SUPERVISOR.cameras[3].screen_shake;
    SUPERVISOR.cameras[3] = inner.camera;
    anm::set_camera(SUPERVISOR.cameras[3].as_3d_matrix());
    anm::enable_zwrite();
    anm::enable_ztest();
    anm::set_fog_params(inner.camera.sky.color, inner.camera.sky.begin_distance,
                        inner.camera.sky.end_distance);
    if (!(flags & 4) || 33 < num_ticks_alive_2)
      color = inner.camera.sky.color;
    else
      color = {0, 0, 0, 0};
    viewport_rect.x = SUPERVISOR.cameras[3].viewport.X;
    viewport_rect.y = SUPERVISOR.cameras[3].viewport.Y;
    viewport_rect.z =
        SUPERVISOR.cameras[3].viewport.X + SUPERVISOR.cameras[3].viewport.Width;
    viewport_rect.w = SUPERVISOR.cameras[3].viewport.Y +
                      SUPERVISOR.cameras[3].viewport.Height;
    // SUPERVISOR.d3d_device->Clear(1, &viewport_rect, 3, color, 1.0f, 0);
    anm::clear_framebuffer({color.r, color.g, color.b, 255});
  }

  if (flags & 4) {
    if (some_countdown_timer < 30) {
      new ScreenEffect(3, 30, 0, 0, 0, 10);
      flags |= 1;
      some_countdown_timer = 1;
    } else {
      inner.some_bg_color_unrelated_to_ins_13.a = 0;
      flags &= 0xfffffffe;
    }
  }
  if (inner.some_bg_color_unrelated_to_ins_13.a != 0) {
    anm::use_custom_color(inner.some_bg_color_unrelated_to_ins_13);
    inner.some_bg_color_unrelated_to_ins_13.a = 0;
  }
  num_ticks_alive = 0;
  num_vms_outside_draw_distance = 0;
  num_vms_drawn = 0;
  if (flags & 1) {
    anm::enable_fog();
    draw_layer(0);
    draw_layer(1);
    draw_layer(2);
    draw_layer(3);
    draw_layer(4);
    draw_layer(5);
    draw_layer(6);
    draw_layer(7);
    anm::flush_vbos();
  }
  anm::unuse_custom_color();
  anm::disable_zwrite();
  anm::disable_ztest();
  return 1;
}

int Stage::f_on_draw_2() {
  if (flags & 8)
    return 1;
  if (!(flags & 4) || some_countdown_timer < 60) {
    anm::flush_vbos();
    inner.camera.screen_shake = SUPERVISOR.cameras[3].screen_shake;
    SUPERVISOR.cameras[3] = inner.camera;
    anm::set_camera(SUPERVISOR.cameras[3].as_3d_matrix());
    anm::disable_fog();
    anm::disable_zwrite();
    anm::disable_ztest();
    anm::render_layer(32);
    anm::enable_ztest();
    anm::render_layer(33);
    anm::set_fog_params(inner.camera.sky.color, inner.camera.sky.begin_distance,
                        inner.camera.sky.end_distance);
  }
  if ((flags & 4) && some_countdown_timer >= 30)
    inner.some_bg_color_unrelated_to_ins_13.a = 0;
  if (flags & 1) {
    anm::enable_zwrite();
    anm::enable_fog();
    draw_layer(8);
    draw_layer(9);
    draw_layer(10);
    draw_layer(11);
    anm::flush_vbos();
  }
  anm::unuse_custom_color();
  if (some_countdown_timer > 0) {
    some_countdown_timer--;
    if (some_countdown_timer < 1) {
      if (flags & 2)
        flags |= 8;
      inner.some_bg_color_unrelated_to_ins_13 = {255, 255, 255, 0};
      flags &= 0xfffffff9;
    }
  }
  anm::disable_zwrite();
  anm::disable_ztest();
  anm::disable_fog();
  return 1;
}

const float SCREEN_START_X = 0.0;
const float SCREEN_START_Y = 0.0;
bool shouldBeCulledAt(StdOpener::std_entry_header_t *ent,
                      ns::vec3 const &face_pos, anm::Camera_t const &cam,
                      float draw_dstce) {
  return false;
  ns::vec3 fcam = (face_pos + ns::vec3(ent->x, ent->y, ent->z)) -
                  (cam.position + cam.__rocking_vector_1);
  if (fcam.x * fcam.x + fcam.y * fcam.y + fcam.z * fcam.z > draw_dstce)
    return true;
  ns::vec3 in_verts[16];
  in_verts[0].x = ent->x + ent->width * 0.5;
  in_verts[0].y = ent->y + ent->height * 0.5;
  in_verts[0].z = ent->z + ent->depth * 0.5;
  in_verts[1].x = ent->x + ent->width * 0.5;
  in_verts[1].y = ent->y + ent->height * 0.5;
  in_verts[1].z = ent->z - ent->depth * 0.5;
  in_verts[2].x = ent->x + ent->width * 0.5;
  in_verts[2].y = ent->y - ent->height * 0.5;
  in_verts[2].z = ent->z + ent->depth * 0.5;
  in_verts[3].x = ent->x + ent->width * 0.5;
  in_verts[3].y = ent->y - ent->height * 0.5;
  in_verts[3].z = ent->z - ent->depth * 0.5;
  in_verts[4].x = ent->x - ent->width * 0.5;
  in_verts[4].y = ent->y + ent->height * 0.5;
  in_verts[4].z = ent->z + ent->depth * 0.5;
  in_verts[5].x = ent->x - ent->width * 0.5;
  in_verts[5].y = ent->y + ent->height * 0.5;
  in_verts[5].z = ent->z - ent->depth * 0.5;
  in_verts[6].x = ent->x - ent->width * 0.5;
  in_verts[6].y = ent->y - ent->height * 0.5;
  in_verts[6].z = ent->z + ent->depth * 0.5;
  in_verts[7].x = ent->x - ent->width * 0.5;
  in_verts[7].y = ent->y - ent->height * 0.5;
  in_verts[7].z = ent->z - ent->depth * 0.5;
  in_verts[8].x = ent->x;
  in_verts[8].y = ent->y - ent->height * 0.5;
  in_verts[8].z = ent->z - ent->depth * 0.5;
  in_verts[9].x = ent->x;
  in_verts[9].y = ent->y + ent->height * 0.5;
  in_verts[9].z = ent->z - ent->depth * 0.5;
  in_verts[10].x = ent->x;
  in_verts[10].y = ent->y - ent->height * 0.5;
  in_verts[10].z = ent->z + ent->depth * 0.5;
  in_verts[11].x = ent->x;
  in_verts[11].y = ent->y + ent->height * 0.5;
  in_verts[11].z = ent->z + ent->depth * 0.5;
  in_verts[12].x = ent->x;
  in_verts[12].y = ent->y - ent->height * 0.5;
  in_verts[12].z = ent->z;
  in_verts[13].x = ent->x;
  in_verts[13].y = ent->y + ent->height * 0.5;
  in_verts[13].z = ent->z;
  in_verts[14].x = ent->x;
  in_verts[14].y = ent->y - ent->height * 0.5;
  in_verts[14].z = ent->z - ent->depth * 0.25;
  in_verts[15].x = ent->x;
  in_verts[15].y = ent->y + ent->height * 0.5;
  in_verts[15].z = ent->z + ent->depth * 0.25;
  ns::mat4 world_matrix = ns::mat::mk4_translate(face_pos);
  ns::mat4 mat = cam.projection_matrix * cam.view_matrix * world_matrix;
  ns::vec3 out_verts[16];
  for (int i = 0; i < 16; i++) {
    out_verts[i] = mat * ns::vec4(in_verts[i], 1.f);
  }
  float min_x = SCREEN_START_X + 384.0 + 8.0;
  float min_y = SCREEN_START_Y + 448.0 + 8.0;
  float max_x = SCREEN_START_X - 8.0;
  float max_y = SCREEN_START_Y - 8.0;
  /* Check if at least one point is in view */
  for (int i = 0; i < 16; i++) {
    auto p = &out_verts[i];
    if ((0.0 > p->z) || (p->z > 1.0))
      continue;
    if (p->x > max_x) {
      max_x = p->x;
    }
    if (p->x < min_x) {
      min_x = p->x;
    }
    if (p->y > max_y) {
      max_y = p->y;
    }
    if (p->y < min_y) {
      min_y = p->y;
    }
  }

  if ((((SCREEN_START_X <= max_x) && (min_x <= SCREEN_START_X + 384.0)) &&
       (SCREEN_START_Y <= max_y)) &&
      (min_y <= SCREEN_START_Y + 448.0)) {
    return false;
  }
  return true;
}

void Stage::draw_layer(int layer) {
  anm::set_camera(SUPERVISOR.cameras[3].as_2d_matrix());
  anm::disable_fog();
  anm::disable_zwrite();
  anm::disable_ztest();
  for (int i = 0; i < 8; i++) {
    if (inner.anm_layers[i] != layer)
      continue;
    // if (this->inner.vms[i].sprite_id > 0)
    //   continue;
    anm::draw_vm(&this->inner.vms[i]);
    // inner.vms[i].draw();
  }
  anm::enable_ztest();
  anm::enable_zwrite();
  anm::enable_fog();
  anm::set_camera(SUPERVISOR.cameras[3].as_3d_matrix());
  // anm::field_0x18607cc = 1; TODO: Find out what that does
  for (auto face = std_faces; face->object_id != static_cast<uint16_t>(-1);
       face++) {
    if (std_object_ptrs[face->object_id]->layer != layer)
      continue;
    if (shouldBeCulledAt(std_object_ptrs[face->object_id],
                         {face->x, face->y, face->z}, SUPERVISOR.cameras[3],
                         inner.draw_distance_sq)) {
      num_vms_outside_draw_distance++;
      face->unknown1 &= 0xfffe;
      continue;
    }
    this->std_object_ptrs[face->object_id]->vmAliveBits |= 2;
    for (auto obj = std_object_ptrs[face->object_id]->quads;
         obj->unknown != static_cast<uint16_t>(-1);
         obj = reinterpret_cast<StdOpener::std_object_t *>(
             reinterpret_cast<uint64_t>(obj) + obj->size)) {
      auto vm = &this->faces_vms[obj->vmid];
      if (obj->unknown != 0)
        continue;
      if (vm->bitflags.rendermode >= 4) {
        vm->entity_pos = {obj->x + face->x, obj->y + face->y, obj->z + face->z};
        if (obj->width != 0) {
          vm->bitflags.scaled = true;
          vm->scale.x = obj->width / vm->getSprite().w;
        }
        if (obj->height != 0) {
          vm->bitflags.scaled = true;
          vm->scale.y = obj->height / vm->getSprite().h;
        }
      }
      // 3d plane && 3d cylinder (no 3d ring(25) ?)
      if (vm->bitflags.rendermode == 8 || vm->bitflags.rendermode == 24) {
        anm::enable_fog();
      } else {
        anm::disable_fog();
      }
      if (vm->bitflags.zwritedis) {
        anm::disable_zwrite();
      } else {
        anm::enable_zwrite();
      }

      anm::draw_vm(vm);
      // vm->draw();
      num_vms_drawn++;
    }
    face->unknown1 |= 1;
    num_ticks_alive++;
  }

  anm::disable_zwrite();
  return;
}

void Stage::create_face_vms() {
  int anm_i = 0;
  for (int i = 0; i < STAGE_PTR->std_file_data->nb_objects; i++) {
    STAGE_PTR->std_object_ptrs[i]->vmAliveBits = 0x1;
    for (auto obj = STAGE_PTR->std_object_ptrs[i]->quads;
         obj->unknown != static_cast<uint16_t>(-1);
         obj = reinterpret_cast<StdOpener::std_object_t *>(
             reinterpret_cast<uint64_t>(obj) + obj->size)) {
      STAGE_PTR->stage_anm->copyFromLoaded(&STAGE_PTR->faces_vms[anm_i],
                                           obj->script_index);
      STAGE_PTR->faces_vms[anm_i].parent_vm = nullptr;
      STAGE_PTR->faces_vms[anm_i].__root_vm__or_maybe_not = nullptr;
      STAGE_PTR->faces_vms[anm_i].run();
      obj->vmid = anm_i;
      anm_i++;
    }
  }
}

void Stage::interrupt(unsigned int i) {
  int off = 0;
  auto instr = reinterpret_cast<StdOpener::std_instr_t *>(
      reinterpret_cast<uint64_t>(beginning_of_script) + off);
  while (static_cast<int32_t>(instr->time) > -1) {
    if (instr->type == 0x10 && instr->args[0] == i) {
      inner.instr_offset = reinterpret_cast<uint64_t>(instr) -
                           reinterpret_cast<uint64_t>(beginning_of_script);
      inner.time_in_script = instr->time;
      return;
    }
    off += instr->size;
    instr = reinterpret_cast<StdOpener::std_instr_t *>(
        reinterpret_cast<uint64_t>(beginning_of_script) + off);
  }
}
