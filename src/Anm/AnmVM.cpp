#include "./AnmVM.h"
#include "./AnmBitflags.h"
#include "./AnmFuncs.h"
#include "./AnmManager.h"
#include "./anmTypes.h"
#include <NSEngine.hpp>
#include <math/math.hpp>

#include <memory.h>

namespace anm {

#define GAME_SPEED ns::get_instance()->game_speed()

void VM::setLayer(u32 i) {
  layer = i;
  if (i < 3) {
    bitflags.originMode = 0;
  } else if (i < 20) {
    bitflags.originMode = 1;
  } else if (i < 24) {
    bitflags.originMode = 2;
  } else {
    bitflags.originMode = 0;
  }

  if ((layer > 19 && layer < 32) || (layer > 35 && layer < 43)) {
    bitflags.resolutionMode = 1;
  }
}

void VM::interrupt(int i) {
  if (index_of_on_interrupt && Funcs::on_switch(index_of_on_interrupt)) {
    Funcs::on_switch(index_of_on_interrupt)(this, i);
  }
  pending_interrupt = i;
}

void VM::interruptRun(int i) {
  if (index_of_on_interrupt && Funcs::on_switch(index_of_on_interrupt)) {
    Funcs::on_switch(index_of_on_interrupt)(this, i);
  }
  pending_interrupt = i;
  update(); // run
}

void VM::interruptRec(int i) {
  interrupt(i);
  for (List_t *child = list_of_children.next; child != nullptr;
       child = child->next) {
    if (child->value)
      child->value->interruptRec(i);
  }
}

void VM::interruptRecRun(int i) {
  interruptRun(i);
  for (List_t *child = list_of_children.next; child != nullptr;
       child = child->next) {
    if (child->value)
      child->value->interruptRecRun(i);
  }
}

VM::VM(u32 script_id, u32 anim_slot) {
  reset();
  anm_loaded_index = anim_slot;
  script_id_2 = script_id;
  this->script_id = script_id;
  instr_offset = 0;
  bitflags.flip_x = false;
  bitflags.flip_y = false;
  bitflags.visible = false;
  bitflags.f534_14_15 = 0b10;

  // When creating via instruction vector, update frame -1 of the VM.
  time_in_script = -1;
  __timer_1c = -1;
  update();
}

VM::~VM() {
  if (special_vertex_buffer_data)
    ns::free_raw(special_vertex_buffer_data, special_vertex_buffer_size);
}

Sprite VM::getSprite() const {
  return anm::getLoaded(anm_loaded_index)->getSprite(sprite_id);
}

int VM::update() {
  /* VM IS NOT RUNNING */
  if (bitflags.activeFlags == ANMVM_DELETE && !time_in_script.had_value(-1))
    return 1;

  if (bitflags.activeFlags == ANMVM_FROZEN || !bitflags.f530_1)
    return 0;

  if (index_of_on_tick && Funcs::on_tick(index_of_on_tick)) {
    if (Funcs::on_tick(index_of_on_tick)(this)) {
      return 1;
    }
  }

  // SHOULD BE SOMEWHERE ELSE
  __timer_1c++;
  /* if hasGrowth flag is set */
  if (bitflags.hasGrowth)
    update_variables_growth();

  /* if flag_set_by_ins306 is set */
  // I believe it will teleport the anm when the camera teleports
  if (bitflags.f530_15) {
    entity_pos += anm::get_3d_camera()->__vec3_104;
  }

  /* if ins419 flag is set */
  if (bitflags.ins419) {
    ns::vec4 temp_quad[4];
    write_sprite_corners_2d(temp_quad);
    uv_quad_of_sprite[0].x = ns::max(0.f, temp_quad[0].x / 640.f);
    uv_quad_of_sprite[0].y = ns::max(0.f, temp_quad[0].y / 480.f);
    uv_quad_of_sprite[1].x = ns::max(0.f, temp_quad[1].x / 640.f);
    uv_quad_of_sprite[1].y = ns::max(0.f, temp_quad[1].y / 480.f);
    uv_quad_of_sprite[2].x = ns::max(0.f, temp_quad[2].x / 640.f);
    uv_quad_of_sprite[2].y = ns::max(0.f, temp_quad[2].y / 480.f);
    uv_quad_of_sprite[3].x = ns::max(0.f, temp_quad[3].x / 640.f);
    uv_quad_of_sprite[3].y = ns::max(0.f, temp_quad[3].y / 480.f);
  }
  step_interpolators();
  write_texture_circle_vertices();

  // if (index_of_on_wait && ANM_ON_WAIT_FUNCS[index_of_on_wait](this)) {
  //     // GAME_SPEED = saved_game_speed;
  //     return 1;
  // }
  // time_in_script++;
  // // GAME_SPEED = saved_game_speed;
  // return 0;

  bytes instructions = anm::getLoaded(anm_loaded_index)->getScript(script_id);
  if (instructions == nullptr)
    return 1;

  /* CHECK FOR INTERRUPTIONS */
  int offset_of_interrupt_m1 = -1;
  int time_of_interrupt_m1 = 0;
  bool interrupt_found = false;
  if (pending_interrupt != 0) {
    // find label
    int32_t offset = 0;
    while (instructions[offset] != 0xff) {
      uint16_t instype = *reinterpret_cast<uint16_t *>(&(instructions[offset]));
      uint16_t inslength =
          *reinterpret_cast<uint16_t *>(&(instructions[offset + 2]));
      int16_t instime =
          *reinterpret_cast<int16_t *>(&(instructions[offset + 4]));
      if (instype == 5 &&
          *reinterpret_cast<int32_t *>(&(instructions[offset + 8])) == -1) {
        offset_of_interrupt_m1 = offset;
        time_of_interrupt_m1 = instime;
      }
      if (instype == 5 && *reinterpret_cast<int32_t *>(&(
                              instructions[offset + 8])) == pending_interrupt) {
        interrupt_return_offset = instr_offset;
        interrupt_return_time = time_in_script;
        instr_offset = offset;
        time_in_script = instime;
        bitflags.visible = 1;
        interrupt_found = true;
        break;
      }
      offset += inslength;
    }
    if (!interrupt_found && offset_of_interrupt_m1 >= 0) {
      interrupt_return_offset = instr_offset;
      interrupt_return_time = time_in_script;
      instr_offset = offset_of_interrupt_m1;
      time_in_script = time_of_interrupt_m1;
      bitflags.visible = 1;
    }
    pending_interrupt = 0;
  }

  /* VM IS STOPPED */
  if (time_in_script < -9999)
    return 0;

  /* RUN INSTRUCTIONS */
  if (instr_offset >= 0) {
    if (instructions[instr_offset] == 0xff)
      return 1;
    int16_t instime =
        *reinterpret_cast<int16_t *>(&(instructions[instr_offset + 4]));
    while (instime <= time_in_script && (bitflags.activeFlags == ANMVM_ACTIVE ||
                                         time_in_script.had_value(-1))) {
      int ret = exec_instruction(&(instructions[instr_offset]));
      if (ret == 1)
        return 0;
      if (ret == 2)
        return 0;

      if (instructions[instr_offset] == 0xff)
        return 1;
      instime = *reinterpret_cast<int16_t *>(&(instructions[instr_offset + 4]));
    }
    time_in_script++;
  }

  return 0;
}

void VM::getSpriteCorners2D(ns::vec2 *corners) {
  ns::vec4 pos4 = ns::vec4(pos, 0);

  auto s = anm::getLoaded(anm_loaded_index)->getSprite(sprite_id);
  float XS = scale.x * scale_2.x;
  float YS = scale.y * scale_2.y;
  float l = bitflags.anchorX != 0 ? -(bitflags.anchorX - 1) : -0.5f;
  float r = l + 1;
  float t = bitflags.anchorY != 0 ? (bitflags.anchorY - 1) : 0.5f;
  float b = t - 1;
  ns::mat4 rotate;
  pos4 = ns::vec4(pos4.x + entity_pos.x, -pos4.y + entity_pos.y,
                  pos4.z + entity_pos.z, 0);
  pos4.z = 0;
  rotate = ns::mat::mk4_rotate_z(-rotation.z);
  corners[0] = {pos4 + rotate * ns::vec4(l * s.w * XS + anchor_offset.x,
                                         t * s.h * YS + anchor_offset.y, 0, 0)};
  corners[1] = {pos4 + rotate * ns::vec4(r * s.w * XS + anchor_offset.x,
                                         t * s.h * YS + anchor_offset.y, 0, 0)};
  corners[2] = {pos4 + rotate * ns::vec4(r * s.w * XS + anchor_offset.x,
                                         b * s.h * YS + anchor_offset.y, 0, 0)};
  corners[3] = {pos4 + rotate * ns::vec4(l * s.w * XS + anchor_offset.x,
                                         b * s.h * YS + anchor_offset.y, 0, 0)};
}

ns::vec3 VM::get_pos_with_root() {
  ns::vec3 p = pos + entity_pos + __pos_2;
  auto root = __root_vm__or_maybe_not;
  if (root && !bitflags.noParent) {
    if (bitflags.parRotate) {
      auto savedpy = p.y;
      p.y = p.y * ns::cos(root->rotation.z) + p.x * ns::sin(root->rotation.z);
      p.x =
          p.x * ns::cos(root->rotation.z) - savedpy * ns::sin(root->rotation.z);
    }
    p += root->get_pos_with_root();
  }
  return p;
}

void VM::write_sprite_corners_2d(ns::vec4 *corners) {
  switch (bitflags.rendermode) {
  case 0:
  case 2:
  case 3:
    write_sprite_corners__without_rot(corners[0], corners[1], corners[2],
                                      corners[3]);
    break;
  case 1:
    write_sprite_corners__with_z_rot(corners[0], corners[1], corners[2],
                                     corners[3]);
    return;
  }
  return;
}

void VM::write_sprite_corners__without_rot(ns::vec4 &tl, ns::vec4 &tr,
                                           ns::vec4 &bl, ns::vec4 &br) {
  tl.x = bitflags.anchorX == 0 ? -0.5f : (bitflags.anchorX == 1 ? 0 : -1);
  tr.x = bitflags.anchorX == 0 ? 0.5f : (bitflags.anchorX == 1 ? 1 : 0);
  bl.x = bitflags.anchorX == 0 ? -0.5f : (bitflags.anchorX == 1 ? 0 : -1);
  br.x = bitflags.anchorX == 0 ? 0.5f : (bitflags.anchorX == 1 ? 1 : 0);
  tl.y = bitflags.anchorY == 0 ? -0.5f : (bitflags.anchorY == 2 ? -1 : 0);
  tr.y = bitflags.anchorY == 0 ? -0.5f : (bitflags.anchorY == 2 ? -1 : 0);
  bl.y = bitflags.anchorY == 0 ? 0.5f : (bitflags.anchorY == 2 ? 0 : 1);
  br.y = bitflags.anchorY == 0 ? 0.5f : (bitflags.anchorY == 2 ? 0 : 1);
  tl.x *= sprite_size.x;
  tl.y *= sprite_size.y;
  tr.x *= sprite_size.x;
  tr.y *= sprite_size.y;
  bl.x *= sprite_size.x;
  bl.y *= sprite_size.y;
  br.x *= sprite_size.x;
  br.y *= sprite_size.y;
  tl.x -= anchor_offset.x;
  tl.y -= anchor_offset.y;
  tr.x -= anchor_offset.x;
  tr.y -= anchor_offset.y;
  bl.x -= anchor_offset.x;
  bl.y -= anchor_offset.y;
  br.x -= anchor_offset.x;
  br.y -= anchor_offset.y;
  if (bitflags.resolutionMode == 1) {
    tl *= RESOLUTION_MULT;
    tr *= RESOLUTION_MULT;
    bl *= RESOLUTION_MULT;
    br *= RESOLUTION_MULT;
  } else if (bitflags.resolutionMode == 2) {
    tl *= RESOLUTION_MULT * 0.5;
    tr *= RESOLUTION_MULT * 0.5;
    bl *= RESOLUTION_MULT * 0.5;
    br *= RESOLUTION_MULT * 0.5;
  }
  auto s = scale_2 * scale;
  if (parent_vm && !bitflags.noParent)
    s *= parent_vm->scale_2 * parent_vm->scale;
  tl.x *= s.x;
  tl.y *= s.y;
  tr.x *= s.x;
  tr.y *= s.y;
  bl.x *= s.x;
  bl.y *= s.y;
  br.x *= s.x;
  br.y *= s.y;
  ns::vec4 p = {get_own_transformed_pos(), 0};
  tl += p;
  tr += p;
  bl += p;
  br += p;
  br.z = entity_pos.z + pos.z + __pos_2.z;
  bl.z = entity_pos.z + pos.z + __pos_2.z;
  tr.z = entity_pos.z + pos.z + __pos_2.z;
  tl.z = entity_pos.z + pos.z + __pos_2.z;
  return;
}

ns::vec3 VM::getRotation() {
  __rotation_related = rotation;
  if (__root_vm__or_maybe_not && !bitflags.noParent) {
    ns::vec3 parentRot = __root_vm__or_maybe_not->getRotation();
    __rotation_related += parentRot;
    // ns::angle_normalize(rotation.x);
    // ns::angle_normalize(rotation.y);
    // ns::angle_normalize(rotation.z);
  }
  return __rotation_related;
}

void VM::write_sprite_corners__with_z_rot(ns::vec4 &tl, ns::vec4 &tr,
                                          ns::vec4 &bl, ns::vec4 &br) {
  tl.x = bitflags.anchorX == 0 ? -0.5f : (bitflags.anchorX == 1 ? 0 : -1);
  tr.x = bitflags.anchorX == 0 ? 0.5f : (bitflags.anchorX == 1 ? 1 : 0);
  bl.x = bitflags.anchorX == 0 ? -0.5f : (bitflags.anchorX == 1 ? 0 : -1);
  br.x = bitflags.anchorX == 0 ? 0.5f : (bitflags.anchorX == 1 ? 1 : 0);
  tl.y = bitflags.anchorY == 0 ? -0.5f : (bitflags.anchorY == 2 ? -1 : 0);
  tr.y = bitflags.anchorY == 0 ? -0.5f : (bitflags.anchorY == 2 ? -1 : 0);
  bl.y = bitflags.anchorY == 0 ? 0.5f : (bitflags.anchorY == 2 ? 0 : 1);
  br.y = bitflags.anchorY == 0 ? 0.5f : (bitflags.anchorY == 2 ? 0 : 1);
  tl.z = bl.z = tr.z = br.z = 0;
  tl.x *= sprite_size.x;
  tl.y *= sprite_size.y;
  tr.x *= sprite_size.x;
  tr.y *= sprite_size.y;
  bl.x *= sprite_size.x;
  bl.y *= sprite_size.y;
  br.x *= sprite_size.x;
  br.y *= sprite_size.y;
  tl.x -= anchor_offset.x;
  tl.y -= anchor_offset.y;
  tr.x -= anchor_offset.x;
  tr.y -= anchor_offset.y;
  bl.x -= anchor_offset.x;
  bl.y -= anchor_offset.y;
  br.x -= anchor_offset.x;
  br.y -= anchor_offset.y;
  if (bitflags.resolutionMode == 1) {
    tl *= RESOLUTION_MULT;
    tr *= RESOLUTION_MULT;
    bl *= RESOLUTION_MULT;
    br *= RESOLUTION_MULT;
  } else if (bitflags.resolutionMode == 2) {
    tl *= RESOLUTION_MULT * 0.5;
    tr *= RESOLUTION_MULT * 0.5;
    bl *= RESOLUTION_MULT * 0.5;
    br *= RESOLUTION_MULT * 0.5;
  }
  auto s = scale_2 * scale;
  if (parent_vm && !bitflags.noParent)
    s *= parent_vm->scale_2 * parent_vm->scale;
  tl.x *= s.x;
  tl.y *= s.y;
  tr.x *= s.x;
  tr.y *= s.y;
  bl.x *= s.x;
  bl.y *= s.y;
  br.x *= s.x;
  br.y *= s.y;
  ns::vec3 p = entity_pos + pos + __pos_2;
  transform_coordinate(p);
  // p.y *= -1;
  float rot = getRotation().z;
  ns::vec2 temp = {};
  temp.x = tl.x * ns::cos(rot) - tl.y * ns::sin(rot);
  temp.y = tl.y * ns::cos(rot) + tl.x * ns::sin(rot);
  tl = {temp.x, temp.y, tl.z, tl.w};
  temp.x = tr.x * ns::cos(rot) - tr.y * ns::sin(rot);
  temp.y = tr.y * ns::cos(rot) + tr.x * ns::sin(rot);
  tr = {temp.x, temp.y, tr.z, tr.w};
  temp.x = bl.x * ns::cos(rot) - bl.y * ns::sin(rot);
  temp.y = bl.y * ns::cos(rot) + bl.x * ns::sin(rot);
  bl = {temp.x, temp.y, bl.z, bl.w};
  temp.x = br.x * ns::cos(rot) - br.y * ns::sin(rot);
  temp.y = br.y * ns::cos(rot) + br.x * ns::sin(rot);
  br = {temp.x, temp.y, br.z, br.w};
  tl += ns::vec4(p, 0);
  tr += ns::vec4(p, 0);
  bl += ns::vec4(p, 0);
  br += ns::vec4(p, 0);
}

int VM::write_sprite_corners__mode_4() {
  ns::vec3 pp = {pos + __pos_2 + entity_pos};
  ns::vec2 s = scale * scale_2 * sprite_size / 2.f;

  // Good enough for now
  auto &vwmx = anm::get_camera()->view_matrix;
  ns::vec3 ViewZ = ns::vec3(vwmx[0][2], vwmx[1][2], vwmx[2][2]);
  ns::mat4 rot = ns::mat::mk4_rotate(rotation.z, ViewZ);
  ns::vec3 ViewX =
      (ns::vec3)(rot * ns::vec4(vwmx[0][0], vwmx[1][0], vwmx[2][0], 1.f));
  ns::vec3 ViewY =
      (ns::vec3)(rot * ns::vec4(vwmx[0][1], vwmx[1][1], vwmx[2][1], 1.f));
  ns::vec3 postl = pp - s.x / 2 * ViewX + s.y / 2 * ViewY;
  ns::vec3 postr = pp + s.x / 2 * ViewX + s.y / 2 * ViewY;
  ns::vec3 posbr = pp + s.x / 2 * ViewX - s.y / 2 * ViewY;
  ns::vec3 posbl = pp - s.x / 2 * ViewX - s.y / 2 * ViewY;

  /*
  ns::vec3 f = ns::normalize(SUPERVISOR.current_camera->position - pp);
  ns::vec3 cam_up = ns::normalize(SUPERVISOR.current_camera->up);
  ns::vec3 r = ns::cross(cam_up, f);
  ns::vec3 u = ns::cross(f, r);
  ns::vec4 p = {pp, 1};

  ns::vec3 right{}, left{}, top{}, bottom{};

  if (bitflags.anchorX == 0) {
    right = r * s.x / 2.f;
    left = -r * s.x / 2.f;
  } else if (bitflags.anchorX == 1) {
    right = r * s.x;
  } else if (bitflags.anchorX == 2) {
    left = -r * s.x;
  }
  if (bitflags.anchorY == 0) {
    bottom = -u * s.y / 2.f;
    top = u * s.y / 2.f;
  } else if (bitflags.anchorY == 1) {
    bottom = -u * s.y;
  } else if (bitflags.anchorY == 2) {
    top = u * s.y;
  }

  ns::mat4 rotmat = ns::mat4(1.f); ns::rotate(ns::mat4(1.f),
      rotation.z, f);

  SPRITE_TEMP_BUFFER[0].transformed_pos = p + rotmat * ns::vec4{top + left, 0};
  SPRITE_TEMP_BUFFER[1].transformed_pos = p + rotmat * ns::vec4{top + right, 0};
  SPRITE_TEMP_BUFFER[2].transformed_pos = p + rotmat * ns::vec4{bottom + left,
  0}; SPRITE_TEMP_BUFFER[3].transformed_pos = p + rotmat * ns::vec4{bottom +
  right, 0};
*/
  SPRITE_TEMP_BUFFER[0].transformed_pos = ns::vec4(postl, 1.f);
  SPRITE_TEMP_BUFFER[1].transformed_pos = ns::vec4(postr, 1.f);
  SPRITE_TEMP_BUFFER[2].transformed_pos = ns::vec4(posbl, 1.f);
  SPRITE_TEMP_BUFFER[3].transformed_pos = ns::vec4(posbr, 1.f);
  return 0;
  // ns::vec4 pp = SUPERVISOR.current_camera->projection_matrix *
  //                SUPERVISOR.current_camera->view_matrix *
  //                ns::vec4(pos + __pos_2 + entity_pos, 1.f);
  // if (pp.z < 0 || pp.z > 1)
  //   return -1;
  // ns::mat4 inv_view_proj = ns::inverse(
  //   SUPERVISOR.current_camera->projection_matrix *
  //   SUPERVISOR.current_camera->view_matrix);
  // ns::vec4 sprite_right =
  //     SUPERVISOR.current_camera->projection_matrix *
  //     SUPERVISOR.current_camera->view_matrix *
  //     ns::vec4((pos + __pos_2 + entity_pos) +
  //     SUPERVISOR.current_camera->right,
  //               1.f);
  // float half_sprite_size =
  //     ns::point_distance(ns::vec3(sprite_right), ns::vec3(pp)) * 0.5;
  // float right = 0.f, left = 0.f, top = 0.f, bottom = 0.f;
  // if (bitflags.anchorX == 0) {
  //   right = sprite_size.x * half_sprite_size * scale.x * scale_2.x * 0.5;
  //   left = sprite_size.x * half_sprite_size * scale.x * scale_2.x * -0.5;
  // } else if (bitflags.anchorX == 1) {
  //   right = sprite_size.x * half_sprite_size * scale.x * scale_2.x;
  //   left = 0.0;
  // } else if (bitflags.anchorX == 2) {
  //   right = 0.0;
  //   left = -sprite_size.x * half_sprite_size * scale.x * scale_2.x;
  // }
  // if (bitflags.anchorY == 0) {
  //   bottom = sprite_size.y * half_sprite_size * scale.y * scale_2.y * 0.5;
  //   top = sprite_size.y * half_sprite_size * scale.y * scale_2.y * -0.5;
  // } else if (bitflags.anchorY == 1) {
  //   bottom = sprite_size.y * half_sprite_size * scale.y * scale_2.y;
  //   top = 0.0;
  // } else if (bitflags.anchorY == 2) {
  //   bottom = 0.0;
  //   top = -sprite_size.y * half_sprite_size * scale.y * scale_2.y;
  // }
  // SPRITE_TEMP_BUFFER[0].transformed_pos = pp;
  // SPRITE_TEMP_BUFFER[1].transformed_pos = pp;
  // SPRITE_TEMP_BUFFER[2].transformed_pos = pp;
  // SPRITE_TEMP_BUFFER[3].transformed_pos = pp;
  // SPRITE_TEMP_BUFFER[0].transformed_pos.x +=
  //     left * ns::cos(rotation.z) - top * ns::sin(rotation.z);
  // SPRITE_TEMP_BUFFER[1].transformed_pos.x +=
  //     right * ns::cos(rotation.z) - top * ns::sin(rotation.z);
  // SPRITE_TEMP_BUFFER[2].transformed_pos.x +=
  //     left * ns::cos(rotation.z) - bottom * ns::sin(rotation.z);
  // SPRITE_TEMP_BUFFER[3].transformed_pos.x +=
  //     right * ns::cos(rotation.z) - bottom * ns::sin(rotation.z);
  // SPRITE_TEMP_BUFFER[0].transformed_pos.y +=
  //     left * ns::sin(rotation.z) + top * ns::cos(rotation.z);
  // SPRITE_TEMP_BUFFER[1].transformed_pos.y +=
  //     right * ns::sin(rotation.z) + top * ns::cos(rotation.z);
  // SPRITE_TEMP_BUFFER[2].transformed_pos.y +=
  //     left * ns::sin(rotation.z) + bottom * ns::cos(rotation.z);
  // SPRITE_TEMP_BUFFER[3].transformed_pos.y +=
  //     right * ns::sin(rotation.z) + bottom * ns::cos(rotation.z);
  // return 0;
}

void VM::transform_coordinate(ns::vec3 &p) {
  /* if the resolution mode is set, position should increase */
  if (bitflags.resolutionMode == 1 || bitflags.resolutionMode == 3)
    p *= RESOLUTION_MULT;
  else if (bitflags.resolutionMode == 2 || bitflags.resolutionMode == 4)
    p *= RESOLUTION_MULT * 0.5;

  /* if no root mode or ignore parent */
  if (!__root_vm__or_maybe_not || bitflags.noParent) {
    p.x += anm::origin(bitflags.originMode).x;
    p.y += anm::origin(bitflags.originMode).y;
  } else {
    if (bitflags.parRotate) {
      /* the vm moves as the parent rotate */
      auto oldp = p;
      p.y = oldp.y * ns::cos(__root_vm__or_maybe_not->rotation.z) +
            oldp.x * ns::sin(__root_vm__or_maybe_not->rotation.z);
      p.x = oldp.x * ns::cos(__root_vm__or_maybe_not->rotation.z) -
            oldp.y * ns::sin(__root_vm__or_maybe_not->rotation.z);
    }
    p += __root_vm__or_maybe_not->get_own_transformed_pos();
  }
}

ns::vec3 VM::get_own_transformed_pos() {
  ns::vec3 p = pos + entity_pos + __pos_2;
  transform_coordinate(p);
  return p;
}

float getSlowdown(VM *vm) {
  if (!vm->__root_vm__or_maybe_not || vm->bitflags.noParent) {
    return vm->slowdown;
  } else {
    while (vm->__root_vm__or_maybe_not && !vm->bitflags.noParent)
      vm = vm->__root_vm__or_maybe_not;
    return vm->slowdown;
  }
}

void VM::clear_flag_1_rec() {
  bitflags.f530_1 = 0;
  for (auto node = list_of_children.next; node; node = node->next) {
    if (node->value)
      node->value->clear_flag_1_rec();
  }
}

void VM::set_flag_1_rec() {
  bitflags.f530_1 = 1;
  for (auto node = list_of_children.next; node; node = node->next) {
    if (node->value)
      node->value->set_flag_1_rec();
  }
}

void VM::reset() {
  ns::vec3 old_entity_pos = entity_pos;
  int old_fast_id = fast_id;
  int old_layer = layer;
  ns::mem_zero(this, sizeof(VM));
  entity_pos = old_entity_pos;
  fast_id = old_fast_id;
  layer = old_layer;
  bitflags.visible = 1;
  bitflags.f530_1 = 1;
  bitflags.rotated = 1;
  bitflags.f534_14_15 = 0b01;
  scale = {1.0, 1.0};
  scale_2 = {1.0, 1.0};
  uv_scale = {1.0, 1.0};
  color_1 = {255, 255, 255, 255};
  __matrix_1 = ns::mat4(1.0);
  time_in_script.reset_neg999999();
  __timer_1c.reset_neg999999();
  pos_i.end_time = 0;
  rgb1_i.end_time = 0;
  alpha1_i.end_time = 0;
  rotate_i.end_time = 0;
  scale_i.end_time = 0;
  scale_2_i.end_time = 0;
  uv_scale_i.end_time = 0;
  rgb2_i.end_time = 0;
  alpha2_i.end_time = 0;
  u_vel_i.end_time = 0;
  v_vel_i.end_time = 0;
  rand_param_one = 1.0;
  rand_param_pi = 3.141593;
  rand_param_int = 0x10000;
  __root_vm__or_maybe_not = nullptr;
  parent_vm = nullptr;
  node_in_global_list = {this, nullptr, nullptr};
  __node_as_child = {this, nullptr, nullptr};
  list_of_children = {this, nullptr, nullptr};
  __wierd_list = {this, nullptr, nullptr};
}

static constexpr int SUCCESS = 0;
static constexpr int FAILURE = 1;
static constexpr int WAIT = 2;
static constexpr int CONTINUE = 3;

int VM::run() {
  return update();
  float gamespeed_real = GAME_SPEED;
  if (bitflags.noSlowdown)
    ns::get_instance()->set_game_speed(1.0);
  float slow = getSlowdown(this);
  if (slow > 0.0) {
    ns::get_instance()->set_game_speed(gamespeed_real - slow * gamespeed_real);
    if (GAME_SPEED < 0)
      ns::get_instance()->set_game_speed(0.0);
  }
  if (index_of_on_tick && Funcs::on_tick(index_of_on_tick)(this)) {
    ns::get_instance()->set_game_speed(gamespeed_real);
    return FAILURE;
  }
  if (instr_offset < 0x0 || bitflags.f530_20) {
    ns::get_instance()->set_game_speed(gamespeed_real);
    return SUCCESS;
  }
  __timer_1c++;
  using Ins = opener::anm_instr_t;
  if (pending_interrupt) {
    int offset = 0;
    Ins *scr = reinterpret_cast<Ins *>(
        anm::getLoaded(anm_loaded_index)->getScript(script_id));
    Ins *m1IntIns = nullptr;
    int m1IntOffset = 0;
    while (1) {
      if (scr->type == static_cast<uint16_t>(-1))
        break;
      if (scr->type == 5) {
        if (pending_interrupt == *reinterpret_cast<int32_t *>(&scr->data[0]))
          break;
        if (*reinterpret_cast<int32_t *>(&scr->data[0]) == -0x1) {
          m1IntIns = scr;
          m1IntOffset = offset;
        }
      }
      offset += scr->length;
      scr =
          reinterpret_cast<Ins *>(reinterpret_cast<int64_t>(scr) + scr->length);
    }
    bitflags.f530_14 = 0;
    pending_interrupt = 0;
    if (scr->type != 5) {
      scr = m1IntIns;
      if (scr == 0) {
        time_in_script--;
        return wait(gamespeed_real);
      }
      offset = m1IntOffset;
    }
    interrupt_return_time = time_in_script;
    // dword ptr [EBX + 0x14] = dword ptr [EBX + 0x28];
    time_in_script = scr->time;
    instr_offset = offset + scr->length;
    bitflags.visible = true;
  } else {
    // if (bitflags.f534_14_15 == 1 && GAME_THREAD_PTR &&
    //     GAME_THREAD_PTR->field_0x8c & 0x2) {
    //    GAME_SPEED = local_20;
    //    return 0;
    // }
  }
  while (true) {
    auto instr =
        &anm::getLoaded(anm_loaded_index)->getScript(script_id)[instr_offset];

    if (reinterpret_cast<Ins *>(instr)->time > time_in_script)
      return wait(gamespeed_real);
    auto res = exec_instruction(instr);

    if (res == CONTINUE) {
      continue;
    } else if (res == WAIT) {
      return wait(gamespeed_real);
    } else {
      ns::get_instance()->set_game_speed(gamespeed_real);
      return res;
    }
  }
}

void VM::step_interpolators() {
  if (pos_i.end_time) {
    if (bitflags.alt_pos)
      __pos_2 = pos_i.step();
    else
      pos = pos_i.step();
  }
  if (rotate_i.end_time) {
    rotation = rotate_i.step();
    bitflags.rotated = true;
  }
  if (rotate_2d_i.end_time) {
    rotation.z = rotate_2d_i.step();
    bitflags.rotated = true;
  }
  if (scale_i.end_time) {
    scale = scale_i.step();
    bitflags.scaled = true;
  }
  if (scale_2_i.end_time) {
    scale_2 = scale_2_i.step();
    bitflags.scaled = true;
  }
  if (rgb1_i.end_time) {
    auto res = rgb1_i.step();
    color_1.r = res.r;
    color_1.g = res.g;
    color_1.b = res.b;
  }
  if (alpha1_i.end_time)
    color_1.a = alpha1_i.step();
  if (rgb2_i.end_time) {
    auto res = rgb2_i.step();
    color_2.r = res.r;
    color_2.g = res.g;
    color_2.b = res.b;
  }
  if (alpha2_i.end_time)
    color_2.a = alpha2_i.step();
  if (uv_scale_i.end_time) {
    uv_scale = uv_scale_i.step();
    bitflags.zoomed = true;
  }
  if (u_vel_i.end_time)
    uv_scroll_vel.x = u_vel_i.step();
  if (v_vel_i.end_time)
    uv_scroll_vel.y = v_vel_i.step();
}

void VM::update_variables_growth() {
  if (angular_velocity.x != 0.0) {
    rotation.x += angular_velocity.x * GAME_SPEED;
    ns::angle_normalize(rotation.x);
    bitflags.rotated = true;
  }
  if (angular_velocity.y != 0.0) {
    rotation.y += angular_velocity.y * GAME_SPEED;
    ns::angle_normalize(rotation.y);
    bitflags.rotated = true;
  }
  if (angular_velocity.z != 0.0) {
    rotation.z += angular_velocity.z * GAME_SPEED;
    ns::angle_normalize(rotation.z);
    bitflags.rotated = true;
  }
  if (scale_growth.x != 0.0) {
    scale.x += scale_growth.x * GAME_SPEED;
    bitflags.scaled = true;
  }
  if (scale_growth.y != 0.0) {
    scale.y += scale_growth.y * GAME_SPEED;
    bitflags.scaled = true;
  }
  if (uv_scroll_vel.x != 0.0) {
    uv_scroll_pos.x += uv_scroll_vel.x * GAME_SPEED;
    if (uv_scroll_pos.x < 0.0)
      uv_scroll_pos.x += 2.0;
    else if (uv_scroll_pos.x > 2.0)
      uv_scroll_pos.x -= 2.0;
  }
  if (uv_scroll_vel.y != 0.0) {
    uv_scroll_pos.y += uv_scroll_vel.y * GAME_SPEED;
    if (uv_scroll_pos.y < 0.0)
      uv_scroll_pos.y += 2.0;
    else if (uv_scroll_pos.y > 2.0)
      uv_scroll_pos.y -= 2.0;
  }
}

VM *VM::search_children(int a, int b) {
  auto child_node = &list_of_children;
  do {
    if (!child_node)
      return nullptr;
    auto child = child_node->value;
    if (child && child != this) {
      // should only check script_id_2
      if (child->script_id_2 == a || child->script_id == a || a == -1) {
        if (b == 0) {
          return child;
        }
        b--;
      }
      if (child->list_of_children.next) {
        if (auto c = child->search_children(a, b); child)
          return c;
      }
      if (child->script_id_2 == -2 && !child_node->next) {
        return child_node->value;
      }
    }
    child_node = child_node->next;
  } while (true);
}

static ns::vec4 lengthdir_vec4(float l, float a) {
  return {l * ns::cos(a), l * ns::sin(a), 0.f, 1.f};
}

void VM::write_texture_circle_vertices() {
  switch (bitflags.rendermode) {
  case 9: {
    int n_segment = int_script_vars[0] - 1;
    float ang_inc = ns::PI_2<f32> / n_segment;
    float v_inc = int_script_vars[1] / static_cast<float>(n_segment);
    ns::vec3 p = pos + entity_pos + __pos_2;
    transform_coordinate(p);
    auto pos = ns::vec4(p, 0);
    ns::Color col2;
    if (bitflags.colmode) {
      col2 = color_2;
    } else {
      col2 = color_1;
    }
    float scale2 = scale.y + scale.x / 2.f;
    float scale1 = scale.y - scale.x / 2.f;
    if (parent_vm && !bitflags.noParent) {
      scale2 *= parent_vm->scale.x;
      scale1 *= parent_vm->scale.y;
    }
    if (bitflags.resolutionMode == 1) {
      scale2 *= RESOLUTION_MULT;
      scale1 *= RESOLUTION_MULT;
    } else if (bitflags.resolutionMode == 2) {
      scale2 *= RESOLUTION_MULT / 2.f;
      scale1 *= RESOLUTION_MULT / 2.f;
    }
    auto cursor =
        reinterpret_cast<RenderVertex_t *>(special_vertex_buffer_data);
    for (int i = 0; i < n_segment; i++) {
      cursor[i * 2 + 0].transformed_pos =
          lengthdir_vec4(scale2, rotation.z + i * ang_inc) + pos;
      cursor[i * 2 + 0].diffuse_color = color_1;
      cursor[i * 2 + 0].texture_uv.x = uv_quad_of_sprite[0].x + uv_scroll_pos.x;
      cursor[i * 2 + 0].texture_uv.y = v_inc * i + uv_scroll_pos.y;
      cursor[i * 2 + 1].transformed_pos =
          lengthdir_vec4(scale1, rotation.z + i * ang_inc) + pos;
      cursor[i * 2 + 1].diffuse_color = col2;
      cursor[i * 2 + 1].texture_uv.x = uv_quad_of_sprite[1].x + uv_scroll_pos.x;
      cursor[i * 2 + 1].texture_uv.y = v_inc * i + uv_scroll_pos.y;
    }
    cursor[n_segment * 2 + 0] =
        reinterpret_cast<RenderVertex_t *>(special_vertex_buffer_data)[0];
    cursor[n_segment * 2 + 0].texture_uv.y =
        int_script_vars[1] + uv_scroll_pos.y;
    cursor[n_segment * 2 + 1] =
        reinterpret_cast<RenderVertex_t *>(special_vertex_buffer_data)[1];
    cursor[n_segment * 2 + 1].texture_uv.y =
        int_script_vars[1] + uv_scroll_pos.y;

    return;
  }
  case 13:
  case 14: {
    int npts = int_script_vars[0];
    float start_ang = rotation.z - rotation.x / 2.f;
    float v_inc = int_script_vars[1] / static_cast<float>(npts - 0x1);
    float ang_inc = rotation.x / (npts - 0x1);
    ns::vec3 p = pos + entity_pos + __pos_2;
    transform_coordinate(p);
    auto pos = ns::vec4(p, 0);
    if (bitflags.rendermode == 14) {
      start_ang = rotation.z;
    }
    ns::Color col;
    if (bitflags.colmode) {
      col = color_2;
    } else {
      col = color_1;
    }
    float scale1 = scale.y - scale.x / 2.f;
    float scale2 = scale.y + scale.x / 2.f;
    if (parent_vm && !bitflags.noParent) {
      scale1 *= parent_vm->scale.y;
      scale2 *= parent_vm->scale.x;
    }
    if (bitflags.resolutionMode == 1) {
      scale1 *= RESOLUTION_MULT;
      scale2 *= RESOLUTION_MULT;
    } else if (bitflags.resolutionMode == 2) {
      scale1 *= RESOLUTION_MULT / 2.f;
      scale2 *= RESOLUTION_MULT / 2.f;
    }

    auto cursor =
        reinterpret_cast<RenderVertex_t *>(special_vertex_buffer_data);
    for (int i = 0; i < npts; i++) {
      cursor[i * 2 + 0].transformed_pos =
          lengthdir_vec4(scale2, start_ang + ang_inc * i) + pos;
      cursor[i * 2 + 0].diffuse_color = col;
      cursor[i * 2 + 0].texture_uv.x = uv_quad_of_sprite[0].x + uv_scroll_pos.x;
      cursor[i * 2 + 0].texture_uv.y = v_inc * i + uv_scroll_pos.y;
      cursor[i * 2 + 1].transformed_pos =
          lengthdir_vec4(scale1, start_ang + ang_inc * i) + pos;
      cursor[i * 2 + 1].diffuse_color = col;
      cursor[i * 2 + 1].texture_uv.x = uv_quad_of_sprite[1].x + uv_scroll_pos.x;
      cursor[i * 2 + 1].texture_uv.y = v_inc * i + uv_scroll_pos.y;
    }
  }
    return;
  case 24:
  case 25: {
    int ptnb = int_script_vars[0];
    float ang_start = float_script_vars[3] - float_script_vars[0] / 2.f;
    float ang_inc = float_script_vars[0] / (ptnb - 0x1);
    float v_inc = int_script_vars[1] / static_cast<float>(ptnb - 0x1);
    ns::Color col;
    if (bitflags.colmode) {
      col = color_2;
    } else {
      col = color_1;
    }
    float scale1 = float_script_vars[2];
    float scale2 = float_script_vars[2];
    float depth = float_script_vars[1] / 2.f;
    if (bitflags.rendermode == 25) {
      scale1 -= float_script_vars[1] / 2.f;
      depth = 0;
      scale2 += float_script_vars[1] / 2.f;
    }

    auto cursor =
        reinterpret_cast<RenderVertex_t *>(special_vertex_buffer_data);
    // actually it uses vertices without w component (since it's untransformed)
    for (int i = 0; i < ptnb; i++) {
      ns::vec2 rotpos = ns::lengthdir_vec(scale1, ang_start + i * ang_inc);
      cursor[i * 2 + 0].transformed_pos.x = rotpos.x;
      cursor[i * 2 + 0].transformed_pos.y = depth;
      cursor[i * 2 + 0].transformed_pos.z = rotpos.y;
      cursor[i * 2 + 0].transformed_pos.w = 1.f;
      cursor[i * 2 + 0].diffuse_color = col;
      cursor[i * 2 + 0].texture_uv.x = uv_quad_of_sprite[0].x + uv_scroll_pos.x;
      cursor[i * 2 + 0].texture_uv.y = i * v_inc + uv_scroll_pos.y;
      rotpos = ns::lengthdir_vec(scale2, ang_start + i * ang_inc);
      cursor[i * 2 + 1].transformed_pos.x = rotpos.x;
      cursor[i * 2 + 1].transformed_pos.y = -depth;
      cursor[i * 2 + 1].transformed_pos.z = rotpos.y;
      cursor[i * 2 + 1].transformed_pos.w = 1.f;
      cursor[i * 2 + 1].diffuse_color = col;
      cursor[i * 2 + 1].texture_uv.x = uv_quad_of_sprite[1].x + uv_scroll_pos.x;
      cursor[i * 2 + 1].texture_uv.y = i * v_inc + uv_scroll_pos.y;
    }
  }
    return;
  default:
    return;
  }
}

int VM::wait(float old_game_speed) {
  if (bitflags.hasGrowth) {
    update_variables_growth();
  }
  if (bitflags.f530_15) {
    entity_pos += anm::get_3d_camera()->__vec3_104;
  }
  if (bitflags.ins419) {
    ns::vec4 temp_quad[4];
    write_sprite_corners_2d(temp_quad);
    uv_quad_of_sprite[0].x = ns::max(0.f, temp_quad[0].x / 640.f);
    uv_quad_of_sprite[0].y = ns::max(0.f, temp_quad[0].y / 480.f);
    uv_quad_of_sprite[1].x = ns::max(0.f, temp_quad[1].x / 640.f);
    uv_quad_of_sprite[1].y = ns::max(0.f, temp_quad[1].y / 480.f);
    uv_quad_of_sprite[2].x = ns::max(0.f, temp_quad[2].x / 640.f);
    uv_quad_of_sprite[2].y = ns::max(0.f, temp_quad[2].y / 480.f);
    uv_quad_of_sprite[3].x = ns::max(0.f, temp_quad[3].x / 640.f);
    uv_quad_of_sprite[3].y = ns::max(0.f, temp_quad[3].y / 480.f);
  }
  step_interpolators();
  write_texture_circle_vertices();
  if (index_of_on_wait && Funcs::on_wait(index_of_on_wait) &&
      Funcs::on_wait(index_of_on_wait)(this) == 0) {
    ns::get_instance()->set_game_speed(old_game_speed);
    return 1;
  }
  time_in_script++;
  ns::get_instance()->set_game_speed(old_game_speed);
  return 0;
}

void VM::alloc_special_vertex_buffer(i32 size) {
  if (special_vertex_buffer_data)
    ns::free_raw(special_vertex_buffer_data, special_vertex_buffer_size);
  special_vertex_buffer_size = size;
  special_vertex_buffer_data = ns::alloc_raw(size);
}

} // namespace anm
