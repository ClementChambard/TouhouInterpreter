#include "./Supervisor.h"
#include "AnmOpener/AnmManager.h"
#include "GlobalData.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Supervisor_t SUPERVISOR{};

Supervisor_t::Supervisor_t() {
  text_anm = AnmManager::LoadFile(0, "text.anm");
  AnmManager::LoadFile(1, "sig.anm");
}

Supervisor_t::~Supervisor_t() {}

void Camera_t::as_2d_matrix() {
  glm::vec2 pos = {viewport.Width * 0.5 + viewport.X,
                   viewport.Height * 0.5 + viewport.Y};
  float z = (viewport.Height / 2.f) / tan(fov_y / 2);
  view_matrix = glm::lookAtLH(glm::vec3{pos.x, pos.y, z}, {pos.x, pos.y, 0.f},
                              {0.f, -1.f, 0.f});
  projection_matrix = glm::perspectiveLH(
      fov_y, static_cast<float>(viewport.Width) / viewport.Height, 1.f,
      10000.f);
  return;
}

void Camera_t::as_3d_matrix() {
  glm::vec3 pos = position + __rocking_vector_1;
  glm::vec3 look_at = pos + facing_normalized;
  view_matrix = glm::lookAtLH(pos, look_at, up);
  projection_matrix = glm::perspectiveLH(
      fov_y, static_cast<float>(viewport.Width) / viewport.Height, 30.f,
      8000.f);
  right.x = up.z * facing_normalized.y - up.y * facing_normalized.z;
  right.y = up.x * facing_normalized.z - up.z * facing_normalized.x;
  right.z = up.y * facing_normalized.x - up.x * facing_normalized.y;
  right = glm::normalize(right);
}

void Supervisor_t::set_camera_by_index(int cam_id, bool force) {
  if (current_camera_index == cam_id && !force)
    return;
  current_camera = &cameras[cam_id];
  current_camera_index = cam_id;
  AnmManager::flush_vbos();
  AnmManager::cam_vec2_fc_x = current_camera->screen_shake.x;
  AnmManager::cam_vec2_fc_y = current_camera->screen_shake.y;
  AnmManager::shader->start();
  AnmManager::shader->SetViewMatrix(current_camera->view_matrix);
  // d3d_device->SetTransform(D3DTS_VIEW, &current_camera->view_matrix);
  AnmManager::shader->SetProjectionMatrix(current_camera->projection_matrix);
  // d3d_device->SetTransform(D3DTS_PROJECTION,
  // &current_camera->projection_matrix);
  AnmManager::shader->SetCameraPosition(current_camera->position);
  glViewport(current_camera->viewport.X, current_camera->viewport.Y,
             current_camera->viewport.Width, current_camera->viewport.Height);
  // d3d_device->SetViewport(&this->current_camera->viewport);
  AnmManager::shader->stop();
  return;
}

void Supervisor_t::disable_d3d_fog(bool force) {
  if (fog_enabled || force) {
    AnmManager::flush_vbos();
    fog_enabled = false;
    // d3d_device->SetRenderState(D3DRS_FOGENABLE, false);
    AnmManager::shader->start();
    AnmManager::shader->SetFog(9999999, 9999999, {0, 0, 0, 0});
    AnmManager::shader->stop();
  }
}

void Supervisor_t::enable_d3d_fog(bool force) {
  if (!fog_enabled | force) {
    AnmManager::flush_vbos();
    fog_enabled = true;
    // d3d_device->SetRenderState(D3DRS_FOGENABLE, true);
    AnmManager::shader->start();
    AnmManager::shader->SetFog(current_camera->sky.begin_distance,
                              current_camera->sky.end_distance,
                              {current_camera->sky.color.r / 255.f,
                               current_camera->sky.color.g / 255.f,
                               current_camera->sky.color.b / 255.f,
                               current_camera->sky.color.a / 255.f});
    AnmManager::shader->stop();
  }
}

void Supervisor_t::disable_zwrite(bool force) {
  if (zwrite_enabled || force) {
    AnmManager::flush_vbos();
    zwrite_enabled = false;
    // d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, false);
    glDepthMask(false);
  }
}

void Supervisor_t::enable_zwrite(bool force) {
  if (!zwrite_enabled || force) {
    AnmManager::flush_vbos();
    zwrite_enabled = true;
    // d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, true);
    glDepthMask(true);
  }
}

void Supervisor_t::disable_ztest() {
  AnmManager::flush_vbos();
  // d3d_device->SetRenderState(D3DRS_ZFUNC, 8);
  glDisable(GL_DEPTH_TEST);
}

void Supervisor_t::enable_ztest() {
  AnmManager::flush_vbos();
  // d3d_device->SetRenderState(D3DRS_ZFUNC, 4);
  glEnable(GL_DEPTH_TEST);
}

float SURF_ORIGIN_ECL_FULL_X = 0.0;
float SURF_ORIGIN_ECL_FULL_Y = 0.0;
float SURF_ORIGIN_ECL_X = 0.0;
float SURF_ORIGIN_ECL_Y = 0.0;

void Supervisor_t::init_cameras() {
  cameras[2].position = {};
  cameras[2].facing = {};
  cameras[2].__rocking_vector_1 = {};
  cameras[2].__rocking_vector_2 = {};
  cameras[2].up = {0.0, 1.0, 0.0};
  cameras[2].fov_y = 0.5235988;
  cameras[2].viewport.X = 0;
  cameras[2].viewport.Y = 0;
  cameras[2].viewport.Width = BACK_BUFFER_SIZE.x;
  cameras[2].viewport.Height = BACK_BUFFER_SIZE.y;
  cameras[2].viewport.MinZ = 0.0;
  cameras[2].viewport.MaxZ = 1.0;
  // fullscreen ?
  // if (DAT_00524700 & 0x3c == 8) {
  //   cameras[2].viewport.Height = 960;
  // }
  cameras[2].camera_index = 2;
  cameras[2].window_resolution.x = BACK_BUFFER_SIZE.x;
  cameras[2].window_resolution.y = BACK_BUFFER_SIZE.y;
  cameras[2].as_2d_matrix();
  cameras[0] = cameras[2];
  cameras[0].camera_index = 0;
  cameras[0].viewport.X = static_cast<int>(RESOLUTION_MULT * 32.0);
  cameras[0].viewport.Y = static_cast<int>(RESOLUTION_MULT * 16.0);
  cameras[0].viewport.Width = static_cast<int>(RESOLUTION_MULT * 384.0);
  cameras[0].viewport.Height = static_cast<int>(RESOLUTION_MULT * 448.0);
  cameras[0].as_2d_matrix();
  cameras[1] = cameras[2];
  cameras[1].camera_index = 1;
  cameras[1].viewport.X = static_cast<int>(RESOLUTION_MULT * 128.0);
  cameras[1].viewport.Y = static_cast<int>(RESOLUTION_MULT * 16.0);
  cameras[1].viewport.Width = static_cast<int>(RESOLUTION_MULT * 384.0);
  cameras[1].viewport.Height = static_cast<int>(RESOLUTION_MULT * 448.0);
  cameras[1].as_2d_matrix();
  cameras[3] = cameras[2];
  cameras[3].camera_index = 3;
  cameras[3].viewport.X = static_cast<int>((BACK_BUFFER_SIZE.x - 408.0) * 0.5);
  cameras[3].viewport.Y = static_cast<int>((BACK_BUFFER_SIZE.y - 472.0) * 0.5);
  cameras[3].viewport.Width = 408;
  cameras[3].viewport.Height = 472;
  cameras[3].as_2d_matrix();
  SURF_ORIGIN_ECL_FULL_X = BACK_BUFFER_SIZE.x / 2;
  SURF_ORIGIN_ECL_FULL_Y = static_cast<int>(RESOLUTION_MULT * 16.0);
  SURF_ORIGIN_ECL_X = BACK_BUFFER_SIZE.x / 2;
  SURF_ORIGIN_ECL_Y = (BACK_BUFFER_SIZE.y + -448) / 2;
  return;
}

void Supervisor_t::setup_special_anms() {
  // if (surface_atR_0) {
  //   cameras[3] = cameras[0];
  //   return;
  // }
  // if (!back_buffer) {
  //   d3d_device->GetBackBuffer(0, 0, 0, &back_buffer);
  // }
  // text_anm->d3d[2].texture->GetSurfaceLevel(0, &surface_atR_0);
  // text_anm->d3d[3].texture->GetSurfaceLevel(0, &surface_atR_1);
  if (!arcade_vm_0->bitflags.visible) {
    int i = -1;
    if (BACK_BUFFER_SIZE.x == 640) {
      i = 0;
    } else if (BACK_BUFFER_SIZE.x == 960) {
      i = 1;
    } else if (BACK_BUFFER_SIZE.x == 1280) {
      i = 2;
    }
    if (i >= 0) {
      text_anm->copyFromLoaded(arcade_vm_0, 0x3b + i);
      arcade_vm_0->parent_vm = nullptr;
      arcade_vm_0->__root_vm__or_maybe_not = nullptr;
      arcade_vm_0->update();
      text_anm->copyFromLoaded(arcade_vm_1, 0x41 + i);
      arcade_vm_1->parent_vm = nullptr;
      arcade_vm_1->__root_vm__or_maybe_not = nullptr;
      arcade_vm_1->update();
      text_anm->copyFromLoaded(arcade_vm_2__handles_upscaling, 0x3e + i);
      arcade_vm_2__handles_upscaling->parent_vm = nullptr;
      arcade_vm_2__handles_upscaling->__root_vm__or_maybe_not = nullptr;
      arcade_vm_2__handles_upscaling->update();
      text_anm->copyFromLoaded(arcade_vm_3__handles_seija, 0x44 + i);
      arcade_vm_3__handles_seija->parent_vm = nullptr;
      arcade_vm_3__handles_seija->__root_vm__or_maybe_not = nullptr;
      arcade_vm_3__handles_seija->update();
    }
  }
  if (RESOLUTION_MULT == 1.5)
    arcade_vm_2__handles_upscaling->bitflags.resampleMode = 0;
  return;
}
