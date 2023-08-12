#include "AnmVM.h"
#include "../Supervisor.h"
#include "AnmBitflags.h"
#include "./AnmFuncs.h"
#include "AnmManager.h"
#include <NSEngine.h>
#include <NSlist.h>
#include <cstring>
#include <math/Random.h>
#include <vertex.h>

int AnmVM::cnt = 0;

void AnmVM::setLayer(uint32_t i) {
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

void AnmVM::interrupt(int i) {
  if (index_of_on_interrupt &&
      ANM_ON_SWITCH_FUNCS[index_of_on_interrupt]) {
    ANM_ON_SWITCH_FUNCS[index_of_on_interrupt](this, i);
  }
  pending_interrupt = i;
}

void AnmVM::interruptRun(int i) {
  if (index_of_on_interrupt &&
      ANM_ON_SWITCH_FUNCS[index_of_on_interrupt]) {
    ANM_ON_SWITCH_FUNCS[index_of_on_interrupt](this, i);
  }
  pending_interrupt = i;
  update(); // run
}

void AnmVM::interruptRec(int i) {
  interrupt(i);
  for (AnmVMList_t *child = list_of_children.next; child != nullptr;
       child = child->next) {
    if (child->value)
      child->value->interruptRec(i);
  }
}

void AnmVM::interruptRecRun(int i) {
  interruptRun(i);
  for (AnmVMList_t *child = list_of_children.next; child != nullptr;
       child = child->next) {
    if (child->value)
      child->value->interruptRecRun(i);
  }
}

AnmVM::AnmVM(uint32_t script_id, uint32_t anim_slot) {
  this->anm_loaded_index = anim_slot;
  this->script_id = script_id;

  // When creating via instruction vector, update frame -1 of the VM.
  time_in_script = -1;
  update();
}

enum class bytefields : uint32_t {};
std::ostream &operator<<(std::ostream &left, bytefields bf) {
  for (int i = 31; i >= 0; i--)
    left << (((uint32_t)bf & (1 << i)) != 0);
  return left;
}

AnmVM::~AnmVM() {
  if (special_vertex_buffer_data)
    delete[] reinterpret_cast<uint8_t*>(special_vertex_buffer_data);
  destroy();
}

AnmSprite AnmVM::getSprite() const {
  return AnmManager::loadedFiles[anm_loaded_index].getSprite(sprite_id);
}

void AnmVM::getParentData(glm::vec3 &pos, glm::vec3 &rot, glm::vec2 &scale) {
  if (parent_vm) {
    parent_vm->getParentData(pos, rot, scale);
    pos.x += parent_vm->pos.x + parent_vm->entity_pos.x;
    pos.y += -parent_vm->pos.y - parent_vm->entity_pos.y;
    pos.z += parent_vm->pos.z + parent_vm->entity_pos.z;
    rot += parent_vm->rotation;
    scale *= parent_vm->scale;
    // if (parent_vm->bitflags.resolutionMode == 2) {
    //     pos /= 2.f;
    //     scale /= 2.f;
    // }
    if (parent_vm->getFlags().originMode == 0) {
      pos.x -= 224.f;
      pos.y += 16.f;
    }
  }
}

int AnmVM::update(bool /*printInstr*/) {
  /* VM IS NOT RUNNING */
  if (bitflags.activeFlags == ANMVM_DELETE && time_in_script != -1)
    return 1;
  cnt++;

  if (bitflags.activeFlags == ANMVM_FROZEN)
    return 0;

  if (index_of_on_tick) {
    if (ANM_ON_TICK_FUNCS[index_of_on_tick](this)) {
      return 1;
    }
  }

  // SHOULD BE SOMEWHERE ELSE
  __timer_1c++;
  /* if hasGrowth flag is set */
  if (bitflags.hasGrowth)
    update_variables_growth();

  /* if flag_set_by_ins306 is set */
  if (bitflags.f530_15) entity_pos += SUPERVISOR.cameras[3].__vec3_104;

  /* if ins419 flag is set */
  if (bitflags.ins419) {
    glm::vec4 temp_quad[4];
    write_sprite_corners_2d(temp_quad);
    uv_quad_of_sprite[0].x = fmax(0.f, temp_quad[0].x / 640.0);
    uv_quad_of_sprite[0].y = fmax(0.f, temp_quad[0].y / 480.0);
    uv_quad_of_sprite[1].x = fmax(0.f, temp_quad[1].x / 640.0);
    uv_quad_of_sprite[1].y = fmax(0.f, temp_quad[1].y / 480.0);
    uv_quad_of_sprite[2].x = fmax(0.f, temp_quad[2].x / 640.0);
    uv_quad_of_sprite[2].y = fmax(0.f, temp_quad[2].y / 480.0);
    uv_quad_of_sprite[3].x = fmax(0.f, temp_quad[3].x / 640.0);
    uv_quad_of_sprite[3].y = fmax(0.f, temp_quad[3].y / 480.0);
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

  int8_t *instructions =
      AnmManager::loadedFiles[anm_loaded_index].getScript(script_id);
  if (instructions == nullptr)
    return 1;

  /* CHECK FOR INTERRUPTIONS */
  int offset_of_interrupt_m1 = -1;
  int time_of_interrupt_m1 = 0;
  bool interrupt_found = false;
  if (pending_interrupt != 0) {
    // find label
    int32_t offset = 0;
    while (instructions[offset] != -1) {
      uint16_t instype = *reinterpret_cast<uint16_t *>(&(instructions[offset]));
      uint16_t inslength =
          *reinterpret_cast<uint16_t *>(&(instructions[offset + 2]));
      int16_t instime =
          *reinterpret_cast<int16_t *>(&(instructions[offset + 4]));
      if (instype == 5 && *reinterpret_cast<int32_t *>(&(
                              instructions[offset + 8])) == -1) {
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
    if (instructions[instr_offset] == -1)
      return 1;
    int16_t instime = *reinterpret_cast<int16_t *>(&(instructions[instr_offset + 4]));
    while (instime <= time_in_script &&
           (bitflags.activeFlags == ANMVM_ACTIVE || time_in_script == -1)) {
      int ret = exec_instruction(&(instructions[instr_offset]));
      if (ret == 1)
        return 0;
      if (ret == 2)
        return 0;

      if (instructions[instr_offset] == -1)
        return 1;
      instime = *reinterpret_cast<int16_t *>(&(instructions[instr_offset + 4]));
    }
    time_in_script++;
  }

  return 0;
}
int AnmVM::getMode() const { return bitflags.rendermode; }
int AnmVM::getZdis() const {
  return bitflags.zwritedis || bitflags.blendmode != 0;
}

void AnmVM::getSpriteCorners2D(glm::vec2 *corners) {
  glm::vec4 pos4 = glm::vec4(pos, 0);

  auto s = AnmManager::loadedFiles[anm_loaded_index].getSprite(sprite_id);
  float XS = scale.x * scale_2.x;
  float YS = scale.y * scale_2.y;
  float l = bitflags.anchorX != 0 ? -(bitflags.anchorX - 1) : -0.5f;
  float r = l + 1;
  float t = bitflags.anchorY != 0 ? (bitflags.anchorY - 1) : 0.5f;
  float b = t - 1;
  glm::mat4 rotate;
  pos4 = glm::vec4(pos4.x + entity_pos.x, -pos4.y + entity_pos.y,
                   pos4.z + entity_pos.z, 0);
  pos4.z = 0;
  rotate = glm::rotate(glm::mat4(1.0f), -rotation.z, glm::vec3(0, 0, 1));
  corners[0] = {pos4 + rotate * glm::vec4(l * s.w * XS + anchor_offset.x,
                                          t * s.h * YS + anchor_offset.y, 0,
                                          0)};
  corners[1] = {pos4 + rotate * glm::vec4(r * s.w * XS + anchor_offset.x,
                                          t * s.h * YS + anchor_offset.y, 0,
                                          0)};
  corners[2] = {pos4 + rotate * glm::vec4(r * s.w * XS + anchor_offset.x,
                                          b * s.h * YS + anchor_offset.y, 0,
                                          0)};
  corners[3] = {pos4 + rotate * glm::vec4(l * s.w * XS + anchor_offset.x,
                                          b * s.h * YS + anchor_offset.y, 0,
                                          0)};
}

void AnmVM::destroy() {
  if (id.val && (id.val & AnmID::fastIdMask) != AnmID::fastIdMask) {
    // I am a fast vm
    AnmManager::fastArray[id.val & AnmID::fastIdMask].isAlive = false;
    ListUtil::listInsertAfter(
        &AnmManager::freelist_head,
        &AnmManager::fastArray[id.val & AnmID::fastIdMask].freelistNode);
  }

  bitflags = {};
  ListUtil::listRemoveNode(&node_in_global_list);
  ListUtil::listRemoveNode(&__node_as_child);

  interrupt_return_time = -99;
  interrupt_return_offset = 0;
  layer = 0;
  anm_loaded_index = 0;
  sprite_id = 0;
  script_id = 0;
  instr_offset = 0;
  pos = {0, 0, 0};
  rotation = {0, 0, 0};
  angular_velocity = {};
  scale = {1.f, 1.f};
  scale_2 = {1, 1};
  scale_growth = {};
  uv_scale = {1, 1};
  uv_scroll_pos = {};
  anchor_offset = {};
  uv_scroll_vel = {};
  pending_interrupt = 0;
  for (int i = 0; i < 4; i++) {
    int_script_vars[i] = 0;
    float_script_vars[i] = 0;
  }
  __script_vars_33_34_35 = {};
  __script_var_8 = 0;
  __script_var_9 = 0;
  rand_param_one = 1.f;
  rand_param_pi = PI;
  rand_param_int = 1;
  __pos_2 = {};
  mode_of_create_child = -1; // mode_of_create_child
  color_1 = {255, 255, 255, 255};
  color_2 = {255, 255, 255, 255};
  id = 0;
  fast_id = 0;
  time_in_script = 0;
  parent_vm = nullptr;
  entity_pos = {}; // pos3 ?
  associated_game_entity = nullptr;
}

void AnmVM::write_sprite_corners_2d(glm::vec4 *corners) {
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

void AnmVM::write_sprite_corners__without_rot(glm::vec4 &tl, glm::vec4 &tr,
                                              glm::vec4 &bl, glm::vec4 &br) {
  tl.x = bitflags.anchorX == 0 ? -0.5f : (bitflags.anchorX == 1 ? 0 : -1);
  tr.x = bitflags.anchorX == 0 ? 0.5f : (bitflags.anchorX == 1 ? 1 : 0);
  bl.x = bitflags.anchorX == 0 ? -0.5f : (bitflags.anchorX == 1 ? 0 : -1);
  br.x = bitflags.anchorX == 0 ? 0.5f : (bitflags.anchorX == 1 ? 1 : 0);
  tl.y = bitflags.anchorY == 0 ? 0.5f : (bitflags.anchorY == 2 ? 1 : 0);
  tr.y = bitflags.anchorY == 0 ? 0.5f : (bitflags.anchorY == 2 ? 1 : 0);
  bl.y = bitflags.anchorY == 0 ? -0.5f : (bitflags.anchorY == 2 ? 0 : -1);
  br.y = bitflags.anchorY == 0 ? -0.5f : (bitflags.anchorY == 2 ? 0 : -1);
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
  glm::vec4 p = {get_own_transformed_pos(), 0};
  p.y *= -1;
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

void AnmVM::write_sprite_corners__without_rot_o(glm::vec4 &tl, glm::vec4 &tr,
                                                glm::vec4 &bl, glm::vec4 &br) {
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
  glm::vec4 p = {get_own_transformed_pos_o(), 0};
  // p.y *= -1;
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

void AnmVM::write_sprite_corners__with_z_rot_o(glm::vec4 &tl, glm::vec4 &tr,
                                               glm::vec4 &bl, glm::vec4 &br) {
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
  // no rotation ?
  tl.x *= s.x;
  tl.y *= s.y;
  tr.x *= s.x;
  tr.y *= s.y;
  bl.x *= s.x;
  bl.y *= s.y;
  br.x *= s.x;
  br.y *= s.y;
  glm::vec3 p = entity_pos + pos + __pos_2;
  transform_coordinate_o(p);
  // p.y *= -1;
  glm::vec2 temp = {};
  temp.x = tl.x * cos(rotation.z) - tl.y * sin(rotation.z);
  temp.y = tl.y * cos(rotation.z) + tl.x * sin(rotation.z);
  tl = {temp.x, temp.y, tl.z, tl.w};
  temp.x = tr.x * cos(rotation.z) - tr.y * sin(rotation.z);
  temp.y = tr.y * cos(rotation.z) + tr.x * sin(rotation.z);
  tr = {temp.x, temp.y, tr.z, tr.w};
  temp.x = bl.x * cos(rotation.z) - bl.y * sin(rotation.z);
  temp.y = bl.y * cos(rotation.z) + bl.x * sin(rotation.z);
  bl = {temp.x, temp.y, bl.z, bl.w};
  temp.x = br.x * cos(rotation.z) - br.y * sin(rotation.z);
  temp.y = br.y * cos(rotation.z) + br.x * sin(rotation.z);
  br = {temp.x, temp.y, br.z, br.w};
  tl += glm::vec4(p, 0);
  tr += glm::vec4(p, 0);
  bl += glm::vec4(p, 0);
  br += glm::vec4(p, 0);
}

void AnmVM::write_sprite_corners__with_z_rot(glm::vec4 &tl, glm::vec4 &tr,
                                             glm::vec4 &bl, glm::vec4 &br) {
  tl.x = bitflags.anchorX == 0 ? -0.5f : (bitflags.anchorX == 1 ? 0 : -1);
  tr.x = bitflags.anchorX == 0 ? 0.5f : (bitflags.anchorX == 1 ? 1 : 0);
  bl.x = bitflags.anchorX == 0 ? -0.5f : (bitflags.anchorX == 1 ? 0 : -1);
  br.x = bitflags.anchorX == 0 ? 0.5f : (bitflags.anchorX == 1 ? 1 : 0);
  tl.y = bitflags.anchorY == 0 ? 0.5f : (bitflags.anchorY == 2 ? 1 : 0);
  tr.y = bitflags.anchorY == 0 ? 0.5f : (bitflags.anchorY == 2 ? 1 : 0);
  bl.y = bitflags.anchorY == 0 ? -0.5f : (bitflags.anchorY == 2 ? 0 : -1);
  br.y = bitflags.anchorY == 0 ? -0.5f : (bitflags.anchorY == 2 ? 0 : -1);
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
  // no rotation ?
  tl.x *= s.x;
  tl.y *= s.y;
  tr.x *= s.x;
  tr.y *= s.y;
  bl.x *= s.x;
  bl.y *= s.y;
  br.x *= s.x;
  br.y *= s.y;
  glm::vec3 p = entity_pos + pos + __pos_2;
  transform_coordinate(p);
  p.y *= -1;
  glm::vec2 temp = {};
  temp.x = tl.x * cos(-rotation.z) - tl.y * sin(-rotation.z);
  temp.y = tl.y * cos(-rotation.z) + tl.x * sin(-rotation.z);
  tl = {temp.x, temp.y, tl.z, tl.w};
  temp.x = tr.x * cos(-rotation.z) - tr.y * sin(-rotation.z);
  temp.y = tr.y * cos(-rotation.z) + tr.x * sin(-rotation.z);
  tr = {temp.x, temp.y, tr.z, tr.w};
  temp.x = bl.x * cos(-rotation.z) - bl.y * sin(-rotation.z);
  temp.y = bl.y * cos(-rotation.z) + bl.x * sin(-rotation.z);
  bl = {temp.x, temp.y, bl.z, bl.w};
  temp.x = br.x * cos(-rotation.z) - br.y * sin(-rotation.z);
  temp.y = br.y * cos(-rotation.z) + br.x * sin(-rotation.z);
  br = {temp.x, temp.y, br.z, br.w};
  tl += glm::vec4(p, 0);
  tr += glm::vec4(p, 0);
  bl += glm::vec4(p, 0);
  br += glm::vec4(p, 0);
}

int AnmVM::write_sprite_corners__mode_4_o() {
  glm::vec4 pp = SUPERVISOR.current_camera->projection_matrix *
                 SUPERVISOR.current_camera->view_matrix *
                 glm::vec4(pos + __pos_2 + entity_pos, 1.f);
  if (pp.z < 0 || pp.z > 1)
    return -1;
  glm::vec4 sprite_right =
      SUPERVISOR.current_camera->projection_matrix *
      SUPERVISOR.current_camera->view_matrix *
      glm::vec4((pos + __pos_2 + entity_pos) + SUPERVISOR.current_camera->right,
                1.f);
  float half_sprite_size =
      math::point_distance(glm::vec3(sprite_right), glm::vec3(pp)) * 0.5;
  float right = 0.f, left = 0.f, top = 0.f, bottom = 0.f;
  if (bitflags.anchorX == 0) {
    right = sprite_size.x * half_sprite_size * scale.x * scale_2.x * 0.5;
    left = sprite_size.x * half_sprite_size * scale.x * scale_2.x * -0.5;
  } else if (bitflags.anchorX == 1) {
    right = sprite_size.x * half_sprite_size * scale.x * scale_2.x;
    left = 0.0;
  } else if (bitflags.anchorX == 2) {
    right = 0.0;
    left = -sprite_size.x * half_sprite_size * scale.x * scale_2.x;
  }
  if (bitflags.anchorY == 0) {
    bottom = sprite_size.y * half_sprite_size * scale.y * scale_2.y * 0.5;
    top = sprite_size.y * half_sprite_size * scale.y * scale_2.y * -0.5;
  } else if (bitflags.anchorY == 1) {
    bottom = sprite_size.y * half_sprite_size * scale.y * scale_2.y;
    top = 0.0;
  } else if (bitflags.anchorY == 2) {
    bottom = 0.0;
    top = -sprite_size.y * half_sprite_size * scale.y * scale_2.y;
  }
  SPRITE_TEMP_BUFFER[0].transformed_pos = pp;
  SPRITE_TEMP_BUFFER[1].transformed_pos = pp;
  SPRITE_TEMP_BUFFER[2].transformed_pos = pp;
  SPRITE_TEMP_BUFFER[3].transformed_pos = pp;
  SPRITE_TEMP_BUFFER[0].transformed_pos.x +=
      left * cos(rotation.z) - top * sin(rotation.z);
  SPRITE_TEMP_BUFFER[1].transformed_pos.x +=
      right * cos(rotation.z) - top * sin(rotation.z);
  SPRITE_TEMP_BUFFER[2].transformed_pos.x +=
      left * cos(rotation.z) - bottom * sin(rotation.z);
  SPRITE_TEMP_BUFFER[3].transformed_pos.x +=
      right * cos(rotation.z) - bottom * sin(rotation.z);
  SPRITE_TEMP_BUFFER[0].transformed_pos.y +=
      left * sin(rotation.z) + top * cos(rotation.z);
  SPRITE_TEMP_BUFFER[1].transformed_pos.y +=
      right * sin(rotation.z) + top * cos(rotation.z);
  SPRITE_TEMP_BUFFER[2].transformed_pos.y +=
      left * sin(rotation.z) + bottom * cos(rotation.z);
  SPRITE_TEMP_BUFFER[3].transformed_pos.y +=
      right * sin(rotation.z) + bottom * cos(rotation.z);
  return 0;
}

int AnmVM::write_sprite_corners__mode_4() {
  glm::vec4 pp = SUPERVISOR.current_camera->projection_matrix *
                 SUPERVISOR.current_camera->view_matrix *
                 glm::vec4(pos + __pos_2 + entity_pos, 1.f);
  if (pp.z < 0 || pp.z > 1)
    return -1;
  glm::vec4 sprite_right =
      SUPERVISOR.current_camera->projection_matrix *
      SUPERVISOR.current_camera->view_matrix *
      glm::vec4((pos + __pos_2 + entity_pos) + SUPERVISOR.current_camera->right,
                1.f);
  float half_sprite_size =
      math::point_distance(glm::vec3(sprite_right), glm::vec3(pp)) * 0.5;
  float right = 0.f, left = 0.f, top = 0.f, bottom = 0.f;
  if (bitflags.anchorX == 0) {
    right = sprite_size.x * half_sprite_size * scale.x * scale_2.x * 0.5;
    left = sprite_size.x * half_sprite_size * scale.x * scale_2.x * -0.5;
  } else if (bitflags.anchorX == 1) {
    right = sprite_size.x * half_sprite_size * scale.x * scale_2.x;
    left = 0.0;
  } else if (bitflags.anchorX == 2) {
    right = 0.0;
    left = -sprite_size.x * half_sprite_size * scale.x * scale_2.x;
  }
  if (bitflags.anchorY == 0) {
    bottom = sprite_size.y * half_sprite_size * scale.y * scale_2.y * -0.5;
    top = sprite_size.y * half_sprite_size * scale.y * scale_2.y * 0.5;
  } else if (bitflags.anchorY == 1) {
    bottom = 0.0;
    top = sprite_size.y * half_sprite_size * scale.y * scale_2.y;
  } else if (bitflags.anchorY == 2) {
    bottom = -sprite_size.y * half_sprite_size * scale.y * scale_2.y;
    top = 0.0;
  }
  pp.y *= -1;
  SPRITE_TEMP_BUFFER[0].transformed_pos = pp;
  SPRITE_TEMP_BUFFER[1].transformed_pos = pp;
  SPRITE_TEMP_BUFFER[2].transformed_pos = pp;
  SPRITE_TEMP_BUFFER[3].transformed_pos = pp;
  SPRITE_TEMP_BUFFER[0].transformed_pos.x +=
      left * cos(rotation.z) - top * sin(rotation.z);
  SPRITE_TEMP_BUFFER[1].transformed_pos.x +=
      right * cos(rotation.z) - top * sin(rotation.z);
  SPRITE_TEMP_BUFFER[2].transformed_pos.x +=
      left * cos(rotation.z) - bottom * sin(rotation.z);
  SPRITE_TEMP_BUFFER[3].transformed_pos.x +=
      right * cos(rotation.z) - bottom * sin(rotation.z);
  SPRITE_TEMP_BUFFER[0].transformed_pos.y +=
      left * sin(rotation.z) + top * cos(rotation.z);
  SPRITE_TEMP_BUFFER[1].transformed_pos.y +=
      right * sin(rotation.z) + top * cos(rotation.z);
  SPRITE_TEMP_BUFFER[2].transformed_pos.y +=
      left * sin(rotation.z) + bottom * cos(rotation.z);
  SPRITE_TEMP_BUFFER[3].transformed_pos.y +=
      right * sin(rotation.z) + bottom * cos(rotation.z);
  return 0;
}

// TODO: set this
float SURF_ORIGIN_ECL_FULL_X_ = 0;
float SURF_ORIGIN_ECL_FULL_Y_ = 0;
float SURF_ORIGIN_ECL_X_ = 0;
float SURF_ORIGIN_ECL_Y_ = 0;
float SURF_ORIGIN_FULL_X_ = -224;
float SURF_ORIGIN_FULL_Y_ = -16;
extern float SURF_ORIGIN_ECL_X;
extern float SURF_ORIGIN_ECL_Y;
extern float SURF_ORIGIN_ECL_FULL_X;
extern float SURF_ORIGIN_ECL_FULL_Y;
void AnmVM::transform_coordinate_o(glm::vec3 &p) {
  /* if the resolution mode is set, position should increase */
  if (bitflags.resolutionMode == 1 || bitflags.resolutionMode == 3)
    p *= RESOLUTION_MULT;
  else if (bitflags.resolutionMode == 2 || bitflags.resolutionMode == 4)
    p *= RESOLUTION_MULT * 0.5;

  /* if no root mode or ignore parent */
  if (!__root_vm__or_maybe_not || bitflags.noParent) {
    if (bitflags.originMode == 0) {
    } else if (bitflags.originMode == 2) {
      /* if origin is ecl origin at full resolution */
      p.x += SURF_ORIGIN_ECL_FULL_X;
      p.y += SURF_ORIGIN_ECL_FULL_Y;
    } else if (bitflags.originMode == 1) {
      /* if origin is ecl origin at original resolution */
      p.x += SURF_ORIGIN_ECL_X;
      p.y += SURF_ORIGIN_ECL_Y;
    }
  } else {
    if (bitflags.parRotate) {
      /* the vm moves as the parent rotate */
      p.y = p.y * cos(__root_vm__or_maybe_not->rotation.z) +
            p.x * sin(__root_vm__or_maybe_not->rotation.z);
      p.x = p.x * cos(__root_vm__or_maybe_not->rotation.z) -
            p.y * sin(__root_vm__or_maybe_not->rotation.z);
    }
    p += __root_vm__or_maybe_not->get_own_transformed_pos();
  }
}

void AnmVM::transform_coordinate(glm::vec3 &p) {
  /* if the resolution mode is set, position should increase */
  if (bitflags.resolutionMode == 1 || bitflags.resolutionMode == 3)
    p *= RESOLUTION_MULT;
  else if (bitflags.resolutionMode == 2 || bitflags.resolutionMode == 4)
    p *= RESOLUTION_MULT * 0.5;

  /* if no root mode or ignore parent */
  if (!__root_vm__or_maybe_not || bitflags.noParent) {
    if (bitflags.originMode == 0) {
      /* if origin is top left */
      p.x += SURF_ORIGIN_FULL_X_;
      p.y += SURF_ORIGIN_FULL_Y_;
    } else if (bitflags.originMode == 2) {
      /* if origin is ecl origin at full resolution */
      p.x += SURF_ORIGIN_ECL_FULL_X_;
      p.y += SURF_ORIGIN_ECL_FULL_Y_;
    } else if (bitflags.originMode == 1) {
      /* if origin is ecl origin at original resolution */
      p.x += SURF_ORIGIN_ECL_X_;
      p.y += SURF_ORIGIN_ECL_Y_;
    }
  } else {
    if (bitflags.parRotate) {
      /* the vm moves as the parent rotate */
      p.y = p.y * cos(-__root_vm__or_maybe_not->rotation.z) +
            p.x * sin(-__root_vm__or_maybe_not->rotation.z);
      p.x = p.x * cos(-__root_vm__or_maybe_not->rotation.z) -
            p.y * sin(-__root_vm__or_maybe_not->rotation.z);
    }
    p += __root_vm__or_maybe_not->get_own_transformed_pos();
  }
}

glm::vec3 AnmVM::get_own_transformed_pos() {
  glm::vec3 p = pos + entity_pos + __pos_2;
  transform_coordinate(p);
  return p;
}

glm::vec3 AnmVM::get_own_transformed_pos_o() {
  glm::vec3 p = pos + entity_pos + __pos_2;
  transform_coordinate_o(p);
  return p;
}

#include "../GlobalData.h"

float getSlowdown(AnmVM *vm) {
  while (vm->__root_vm__or_maybe_not && !vm->bitflags.noParent)
    vm = vm->__root_vm__or_maybe_not;
  return vm->slowdown;
}

int AnmVM::check_interrupt() {
  // pzStack_120 = NULL;
  // pzStack_11c = NULL;
  // anm_loaded = get_anm_loaded(vm);
  // anm_script = (zAnmVM *)AnmLoaded::get_script(anm_loaded,script_id);
  // root_vm = local_114;
  // pzVar18 = NULL;
  // do {
  //   while (pzVar26 = anm_script, pzStack_118 = pzVar18, sVar5 = *(short
  //   *)&(pzVar26->prefix).interrupt_return_time.previous, sVar5 != 5)
  //   {
  //       if (sVar5 == -1) {
  //           goto PENDING_INTERRUPT_END;
  //       }
  //  IN_SEARCH_FOR_LABEL:
  //     uVar15 = (uint)*(ushort *)
  //     ((int)&(pzVar26->prefix).interrupt_return_time.previous + 2); pzVar18 =
  //     (zAnmVM *) ((int)&(pzStack_118->prefix).interrupt_return_time.previous
  //     + uVar15); anm_script = (zAnmVM *)
  //     ((int)&(pzVar26->prefix).interrupt_return_time.previous + uVar15);
  //   }
  //   fVar27 = (pzVar26->prefix).interrupt_return_time.current_f;
  //   if ((float)(vm->prefix).pending_interrupt == fVar27) {
  //     PENDING_INTERRUPT_END:
  //                         /* clear flag 14 */
  //         field_0x530 &= 0xffffbfff;
  //         pending_interrupt = 0;
  //         if (*(short *)&(pzVar24->prefix).interrupt_return_time.previous !=
  //         5) {
  //           if (pzStack_11c == NULL) {
  //             time_in_script--;
  //             pfVar17 = extraout_EDX;
  return 0;
  //          }
  //          pzStack_118 = pzStack_120;
  //          pzVar24 = pzStack_11c;
  //        }
  //        set_interrupt_return_time(vm,time_in_script);
  //        (root_vm->prefix).interrupt_return_offset =
  //        (root_vm->prefix).instr_offset; uVar11 = *(ushort
  //        *)((int)&(pzVar24->prefix).interrupt_return_time.previous + 2);
  //        in_stack_fffffebc = CONCAT412(0x467007,time_in_script);
  //        time_in_script = pzVar24->interrupt_return_time.current;
  //        field_0x530 |= 1;
  //        instr_offset =
  //        (int32_t)((int)&(pzStack_118->prefix.interrupt_return_time).previous
  //        + (uint)uVar11); goto START_SWITCH;
  //  }
  //  if (fVar27 != -1) goto IN_SEARCH_FOR_LABEL;
  //  uVar15 = (uint)*(ushort
  //  *)((int)&(pzVar26->prefix).interrupt_return_time.previous + 2)
  //  ;
  //  pzVar18 = (zAnmVM *)
  //            ((int)&(pzStack_118->prefix).interrupt_return_time.previous +
  //            uVar15);
  //  anm_script = (zAnmVM *)
  //               ((int)&(pzVar26->prefix).interrupt_return_time.previous +
  //               uVar15);
  //  pzStack_120 = pzStack_118;
  //  pzStack_11c = pzVar26;
  //} while( true );
}

void AnmVM::clear_flag_1_rec() {
  // Should be flag 1, not 0
  bitflags.visible = 0;
  for (auto node = list_of_children.next; node; node = node->next) {
    if (node->value)
      node->value->clear_flag_1_rec();
  }
}

void AnmVM::set_flag_1_rec() {
  // Should be flag 1, not 0
  bitflags.visible = 1;
  for (auto node = list_of_children.next; node; node = node->next) {
    if (node->value)
      node->value->set_flag_1_rec();
  }
}

void AnmVM::reset() {
  glm::vec3 old_entity_pos = entity_pos;
  int old_fast_id = fast_id;
  int old_layer = layer;
  memset(reinterpret_cast<char *>(this), 0, sizeof(AnmVM));
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
  __matrix_1 = glm::mat4(1.0);
  time_in_script = 0;
  // time_in_script.previous = -999999;
  // reset_neg999999
  __timer_1c = 0;
  // __timer_1c.previous = -999999;
  // reset_neg999999
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

int AnmVM::run() {
  float saved_game_speed = GAME_SPEED;
  if (bitflags.noSlowdown)
    GAME_SPEED = 1.0;
  if (0.0 < getSlowdown(this)) {
    GAME_SPEED = (GAME_SPEED - getSlowdown(this) * GAME_SPEED);
    if (GAME_SPEED < 0.0)
      GAME_SPEED = 0.0;
  }

  // if (index_of_on_tick && ANM_ON_TICK_FUNCS[index_of_on_tick](this)) {
  //     GAME_SPEED = saved_game_speed;
  //     return 1;
  // }

  if (instr_offset > -1 && !bitflags.f530_20) {
    __timer_1c++;
    if (pending_interrupt)
      check_interrupt();
    if (bitflags.activeFlags ==
        0b01 /*&& GAME_THREAD_PTR && (GAME_THREAD_PTR->flags & 2)*/) {
      GAME_SPEED = saved_game_speed;
      return 0;
    }
    // START_LOOP:
    while (true) {
      // get ins
      // if (time_in_script < ins.time)
      goto WAITING;
      // ins_exec can jump to these locations
      // --> 0 = loop
      // --> 1 = break
      // --> 2 = quit
    }
  }
  // check interrupt
  // if (pending_interrupt && check_interrupt()) {
  //    bitflags.f530_14 = 1;
  //    time_in_script--;
  //}

WAITING:
  /* if hasGrowth flag is set */
  if (bitflags.hasGrowth) {
    update_variables_growth();
  }
  /* if flag_set_by_ins306 is set */
  if (bitflags.f530_15) entity_pos += SUPERVISOR.cameras[3].__vec3_104;

  /* if ins419 flag is set */
  if (bitflags.ins419) {
    glm::vec4 temp_quad[4];
    write_sprite_corners_2d(temp_quad);
    uv_quad_of_sprite[0].x = fmax(0.f, temp_quad[0].x / 640.0);
    uv_quad_of_sprite[0].y = fmax(0.f, temp_quad[0].y / 480.0);
    uv_quad_of_sprite[1].x = fmax(0.f, temp_quad[1].x / 640.0);
    uv_quad_of_sprite[1].y = fmax(0.f, temp_quad[1].y / 480.0);
    uv_quad_of_sprite[2].x = fmax(0.f, temp_quad[2].x / 640.0);
    uv_quad_of_sprite[2].y = fmax(0.f, temp_quad[2].y / 480.0);
    uv_quad_of_sprite[3].x = fmax(0.f, temp_quad[3].x / 640.0);
    uv_quad_of_sprite[3].y = fmax(0.f, temp_quad[3].y / 480.0);
  }
  step_interpolators();
  write_texture_circle_vertices();
  // if (index_of_on_wait && ANM_ON_WAIT_FUNCS[index_of_on_wait](this)) {
  //     GAME_SPEED = saved_game_speed;
  //     return 1;
  // }
  time_in_script++;
  GAME_SPEED = saved_game_speed;
  return 0;
}

void AnmVM::step_interpolators() {
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

void AnmVM::update_variables_growth() {
  if (angular_velocity.x != 0.0) {
    rotation.x += angular_velocity.x * GAME_SPEED;
    math::angle_normalize(rotation.x);
    bitflags.rotated = true;
  }
  if (angular_velocity.y != 0.0) {
    rotation.y += angular_velocity.y * GAME_SPEED;
    math::angle_normalize(rotation.y);
    bitflags.rotated = true;
  }
  if (angular_velocity.z != 0.0) {
    rotation.z += angular_velocity.z * GAME_SPEED;
    math::angle_normalize(rotation.z);
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

AnmID AnmVM::add_child(int i, int mode) {
  // loaded->__field_134__some_kind_of_counter++;
  auto new_vm = AnmManager::getVM(
      AnmManager::SpawnVM(anm_loaded_index, i, mode & 4, mode & 2));
  new_vm->bitflags.randomMode = 0b1;
  new_vm->bitflags.colorizeChildren = bitflags.colorizeChildren;
  new_vm->parent_vm = this;
  new_vm->__root_vm__or_maybe_not =
      __root_vm__or_maybe_not ? __root_vm__or_maybe_not : this;
  if (!new_vm->layer)
    new_vm->layer = layer;
  new_vm->update();
  new_vm->__node_as_child.previous = &list_of_children;
  new_vm->__node_as_child.next = list_of_children.next;
  if (list_of_children.next)
    list_of_children.next->previous = &new_vm->__node_as_child;
  list_of_children.next = &new_vm->__node_as_child;
  new_vm->mode_of_create_child = mode;
  return new_vm->id;
}

AnmVM *AnmVM::search_children(int a, int b) {
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

static glm::vec4 lengthdir_vec4(float l, float a) {
  return {l * cos(a), l * sin(a), 0.f, 1.f};
}

void AnmVM::write_texture_circle_vertices() {
  switch (bitflags.rendermode) {
  case 9: {
    int n_segment = int_script_vars[0] - 1;
    float ang_inc = PI2 / n_segment;
    float v_inc = int_script_vars[1] / static_cast<float>(n_segment);
    glm::vec3 p = pos + entity_pos + __pos_2;
    transform_coordinate_o(p);
    auto pos = glm::vec4(p, 0);
    NSEngine::Color col2;
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
    auto cursor = reinterpret_cast<RenderVertex_t*>(special_vertex_buffer_data);
    for (int i = 0; i < n_segment; i++) {
      cursor[i * 2 + 0].transformed_pos =
          lengthdir_vec4(scale2, rotation.z + i * ang_inc) + pos;
      cursor[i * 2 + 0].diffuse_color = color_1;
      cursor[i * 2 + 0].texture_uv.x =
          uv_quad_of_sprite[0].x + uv_scroll_pos.x;
      cursor[i * 2 + 0].texture_uv.y = v_inc * i + uv_scroll_pos.y;
      cursor[i * 2 + 1].transformed_pos =
          lengthdir_vec4(scale1, rotation.z + i * ang_inc) + pos;
      cursor[i * 2 + 1].diffuse_color = col2;
      cursor[i * 2 + 1].texture_uv.x =
          uv_quad_of_sprite[1].x + uv_scroll_pos.x;
      cursor[i * 2 + 1].texture_uv.y = v_inc * i + uv_scroll_pos.y;
    }
    cursor[n_segment * 2 + 0] =
        reinterpret_cast<RenderVertex_t*>(special_vertex_buffer_data)[0];
    cursor[n_segment * 2 + 0].texture_uv.y =
        int_script_vars[1] + uv_scroll_pos.y;
    cursor[n_segment * 2 + 1] =
        reinterpret_cast<RenderVertex_t*>(special_vertex_buffer_data)[1];
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
    glm::vec3 p = pos + entity_pos + __pos_2;
    transform_coordinate_o(p);
    auto pos = glm::vec4(p, 0);
    if (bitflags.rendermode == 14) {
      start_ang = rotation.z;
    }
    NSEngine::Color col;
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

    auto cursor = reinterpret_cast<RenderVertex_t*>(special_vertex_buffer_data);
    for (int i = 0; i < npts; i++) {
      cursor[i * 2 + 0].transformed_pos =
          lengthdir_vec4(scale2, start_ang + ang_inc * i) + pos;
      cursor[i * 2 + 0].diffuse_color = col;
      cursor[i * 2 + 0].texture_uv.x =
          uv_quad_of_sprite[0].x + uv_scroll_pos.x;
      cursor[i * 2 + 0].texture_uv.y = v_inc * i + uv_scroll_pos.y;
      cursor[i * 2 + 1].transformed_pos =
          lengthdir_vec4(scale1, start_ang + ang_inc * i) + pos;
      cursor[i * 2 + 1].diffuse_color = col;
      cursor[i * 2 + 1].texture_uv.x =
          uv_quad_of_sprite[1].x + uv_scroll_pos.x;
      cursor[i * 2 + 1].texture_uv.y = v_inc * i + uv_scroll_pos.y;
    }
    }
    return;
  case 24:
  case 25: {
    int ptnb = int_script_vars[0];
    float ang_start =
        float_script_vars[3] - float_script_vars[0] / 2.f;
    float ang_inc = float_script_vars[0] / (ptnb - 0x1);
    float v_inc = int_script_vars[1] / static_cast<float>(ptnb - 0x1);
    NSEngine::Color col;
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

    auto cursor = reinterpret_cast<RenderVertex_t*>(special_vertex_buffer_data);
    // actually it uses vertices without w component (since it's untransformed)
    for (int i = 0; i < ptnb; i++) {
      glm::vec2 rotpos = math::lengthdir_vec(scale1, ang_start + i * ang_inc);
      cursor[i * 2 + 0].transformed_pos.x = rotpos.x;
      cursor[i * 2 + 0].transformed_pos.y = depth;
      cursor[i * 2 + 0].transformed_pos.z = rotpos.y;
      cursor[i * 2 + 0].transformed_pos.w = 1.f;
      cursor[i * 2 + 0].diffuse_color = col;
      cursor[i * 2 + 0].texture_uv.x =
          uv_quad_of_sprite[0].x + uv_scroll_pos.x;
      cursor[i * 2 + 0].texture_uv.y = i * v_inc + uv_scroll_pos.y;
      rotpos = math::lengthdir_vec(scale2, ang_start + i * ang_inc);
      cursor[i * 2 + 1].transformed_pos.x = rotpos.x;
      cursor[i * 2 + 1].transformed_pos.y = -depth;
      cursor[i * 2 + 1].transformed_pos.z = rotpos.y;
      cursor[i * 2 + 1].transformed_pos.w = 1.f;
      cursor[i * 2 + 1].diffuse_color = col;
      cursor[i * 2 + 1].texture_uv.x =
          uv_quad_of_sprite[1].x + uv_scroll_pos.x;
      cursor[i * 2 + 1].texture_uv.y = i * v_inc + uv_scroll_pos.y;
    }
    }
    return;
  default:
    return;
  }
}
