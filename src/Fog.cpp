#include "./Fog.hpp"
#include "Anm/AnmManager.h"
#include "Ecl/Enemy.h"
#include "Supervisor.h"
#include <NSEngine.hpp>
#include <memory.h>

#define GAME_SPEED ns::get_instance()->game_speed()

anm::ID fog_init_sub_anms(int count) {
  anm::VM *vm;
  anm::ID id = SUPERVISOR.text_anm->createEffect(59, 34, &vm);
  vm->alloc_special_vertex_buffer(count * 2 * sizeof(anm::RenderVertex_t));
  if (count < 3) {
    vm->bitflags.rendermode = 0;
  } else {
    vm->int_script_vars[0] = count;
    vm->bitflags.rendermode = 12;
    auto cursor = reinterpret_cast<anm::RenderVertex_t*>
      (vm->special_vertex_buffer_data);
    for (int i = 0; i < count * 2; i++) {
      cursor[i].transformed_pos.z = 0.f;
      cursor[i].transformed_pos.w = 1.f;
      cursor[i].diffuse_color = c_white;
    }
  }
  return id;
}

Fog_t::Fog_t(int count) {
  anmid = 0;
  if (SUPERVISOR.surface_atR_0 == nullptr) {
    vm_count = 0;
    vertex_count = 0;
    anmid = 0;
    anmid_array = nullptr;
    vm_array = nullptr;
    vertex_array = nullptr;
    pos_array = nullptr;
    return;
  }
  vm_count = 17;
  vertex_count = count;
  anmid_array = new anm::ID[vm_count - 1];
  vm_array = new anm::VM*[vm_count - 1];
  vertex_array = new anm::RenderVertex_t[vertex_count * vm_count];
  pos_array = new ns::vec3[vertex_count * vm_count];
  anm::VM* vm;
  anmid = SUPERVISOR.text_anm->createEffect(59, 34, &vm);
  // TODO: why mine is showing when it is not in the og
  vm->color_1.a = 0;
  vm->bitflags.rendermode = 0;
  vm->alloc_special_vertex_buffer(0x70);
  vm->index_of_on_draw = 4;
  vm->associated_game_entity = this;
  for (int i = 0; i < vm_count - 1; i++) {
    anmid_array[i] = fog_init_sub_anms(vertex_count);
    vm_array[i] = anm::getVM(anmid_array[i]);
    vm_array[i]->bitflags.blendmode &= 0b1100;
    vm_array[i]->bitflags.originMode = 0;
  }
  return;
}

Fog_t::~Fog_t() {
  anm::deleteVM(anmid);
  if (vertex_array) {
    delete[] vertex_array;
  }
  if (pos_array) {
    delete[] pos_array;
  }
  for (int i = 0; i < vm_count - 1; i++) {
    anm::deleteVM(anmid_array[i]);
  }
  if (anmid_array) {
    delete[] anmid_array;
  }
  if (vm_array) {
    delete[] vm_array;
  }
}

void Fog_t::set_vm_vertices() {
  if (vm_count != 0) {
    auto cursor = vertex_array;
    for (int i = 0; i + 1 < vm_count; i++) {
      auto vmverts = reinterpret_cast<anm::RenderVertex_t*>
          (vm_array[i]->special_vertex_buffer_data);
      for (int j = 0; j < vertex_count; j++) {
          vmverts[2*j+0] = cursor[0];
          vmverts[2*j+1] = cursor[vertex_count];
          cursor++;
      }
    }
  }
  return;
}

void Fog_t::reset_area(float some_x, float some_y,
                         float width, float height) {
  if (vm_count == 0) return;
  float pos_x = anm::origin(1).x + some_x;
  for (int i = 0; i < vm_count; i++) {
      float pos_y = anm::origin(1).y + some_y;
      for (int j = 0; j < vertex_count; j++) {
          auto& p = pos_array[j + i * vertex_count];
          auto& v = vertex_array[j + i * vertex_count];
          p.x = pos_x;
          p.y = pos_y;
          p.z = 0.0;
          v.transformed_pos.x = pos_x;
          v.transformed_pos.y = pos_y;
          v.transformed_pos.z = 0.0;
          v.transformed_pos.w = 1.0;
          // should invert:
          v.texture_uv.x = p.x / anm::BACK_BUFFER_SIZE.x;
          v.texture_uv.y = (anm::BACK_BUFFER_SIZE.y - p.y) / anm::BACK_BUFFER_SIZE.y;
          if (v.texture_uv.x < 0.0) v.texture_uv.x = 0.0;
          if (v.texture_uv.y < 0.0) v.texture_uv.y = 0.0;
          v.diffuse_color = c_white;
          pos_y += height / (vertex_count - 1);
      }
      pos_x += width / (vm_count - 1);
  }
  set_vm_vertices();
}

void EnemyData::update_distorsion() {
  if (fog.fog_ptr == nullptr) return;

  if (fog.r < fog.fog_radius) {
    fog.r += GAME_SPEED * 2.0;
  }
  fog.fog_ptr->reset_area(
               (final_pos.pos.x - fog.r) - 20.0,
               (final_pos.pos.y - fog.r) - 20.0,
               2 * fog.r + 40.0, 2 * fog.r + 40.0);
  float osc_x = fog.oscillate_x;
  float osc_y = fog.oscillate_y;
  for (int i = 0; i <fog.fog_ptr->vm_count; i++) {
      for (int j = 0; j < fog.fog_ptr->vertex_count; j++) {
          auto& v = fog.fog_ptr->vertex_array
              [i * fog.fog_ptr->vertex_count + j];
          auto& p = fog.fog_ptr->pos_array
              [i * fog.fog_ptr->vertex_count + j];
          // vertex position recentered so that it is centered around 0
          ns::vec2 local_18 = {
             p.x - anm::origin(1).x - final_pos.pos.x,
             p.y - anm::origin(1).y - final_pos.pos.y,
          };
          float dst_in_circle = fog.r * fog.r - local_18.length_sq();
          if (dst_in_circle < 0.0) {
            v.diffuse_color.a = 0;
          } else {
            // 0 if on the edge of the circle, 1 if in the middle
            float mul = dst_in_circle / (fog.r * fog.r);
            v.diffuse_color.a = 255;
            v.diffuse_color.r = 255 - (255 - fog.fog_color.r) * mul;
            v.diffuse_color.g = 255 - (255 - fog.fog_color.g) * mul;
            v.diffuse_color.b = 255 - (255 - fog.fog_color.b) * mul;
            local_18 = local_18.normalized() * mul * 32.f;
            local_18.x += ns::sin(osc_x) * mul * 8.0;
            local_18.y += ns::sin(osc_y) * mul * 8.0;
            v.transformed_pos.x += local_18.x;
            v.transformed_pos.y += local_18.y;
          }

          osc_x += 0.09817477;
          math::angle_normalize(osc_x);
          osc_y -= 0.04908739;
          math::angle_normalize(osc_y);
          const float GAME_REGION_X = anm::origin(1).x - 384.0 / 2.f;
          if (p.x <= GAME_REGION_X) {
            v.transformed_pos.x = GAME_REGION_X + 1.0;
            p.x = GAME_REGION_X + 1.0;
          } else if (p.x >= GAME_REGION_X + 384.0) {
            v.transformed_pos.x = GAME_REGION_X + 383.0;
            p.x = GAME_REGION_X + 383.0;
          }
          if (p.y <= anm::origin(1).y) {
            v.transformed_pos.y = anm::origin(1).y + 1.0;
            p.y = anm::origin(1).y + 1.0;
          } else if (p.y >= anm::origin(1).y + 448.0) {
            v.transformed_pos.y = anm::origin(1).y + 447.0;
            p.y = anm::origin(1).y + 447.0;
          }
          v.texture_uv.x = p.x / anm::BACK_BUFFER_SIZE.x;
          v.texture_uv.y = (anm::BACK_BUFFER_SIZE.y - p.y) / anm::BACK_BUFFER_SIZE.y;
          if (v.texture_uv.x < 0.0) v.texture_uv.x = 0.0;
          if (v.texture_uv.y < 0.0) v.texture_uv.y = 0.0;
      }
  }
  fog.oscillate_x += GAME_SPEED * 0.1963495;
  math::angle_normalize(fog.oscillate_x);
  fog.oscillate_y += GAME_SPEED * 0.09817477;
  math::angle_normalize(fog.oscillate_y);
}
