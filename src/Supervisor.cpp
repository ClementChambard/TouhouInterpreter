#include "./Supervisor.h"
#include "AnmOpener/AnmManager.h"
#include "AsciiManager.hpp"
#include "GlobalData.h"
#include "Hardcoded.h"
#include "UpdateFuncRegistry.h"
#include <TextureManager.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Supervisor_t SUPERVISOR{};

void CameraSky_t::init(float beg, float end, float c0, float c1, float c2, float c3) {
    begin_distance = beg;
    end_distance = end;
    color_components[0] = c0;
    color_components[1] = c1;
    color_components[2] = c2;
    color_components[3] = c3;
    color.b = color_components[0];
    color.g = color_components[1];
    color.r = color_components[2];
    color.a = color_components[3];
}

void Supervisor_t::init() {  // temporary
  initialize();
  text_anm = AnmManager::LoadFile(0, "text.anm");
  AnmManager::LoadFile(1, "sig.anm");
}

Supervisor_t::~Supervisor_t() {
  if (arcade_vm_0) delete arcade_vm_0;
  if (arcade_vm_1) delete arcade_vm_1;
  if (arcade_vm_2__handles_upscaling) delete arcade_vm_2__handles_upscaling;
  if (arcade_vm_3__handles_seija) delete arcade_vm_3__handles_seija;
}

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

void Supervisor_t::set_camera_by_index(int cam_id, bool) {
  current_camera = &cameras[cam_id];
  current_camera_index = cam_id;
  AnmManager::flush_vbos();
  AnmManager::cam_vec2_fc_x = current_camera->screen_shake.x;
  AnmManager::cam_vec2_fc_y = current_camera->screen_shake.y;
  // d3d_device->SetTransform(D3DTS_VIEW, &current_camera->view_matrix);
  // d3d_device->SetTransform(D3DTS_PROJECTION,
  // &current_camera->projection_matrix);
  AnmManager::shader->start();
  AnmManager::shader->SetViewMatrix(current_camera->view_matrix);
  AnmManager::shader->SetProjectionMatrix(current_camera->projection_matrix);
  AnmManager::shader->stop();
  AnmManager::fshader->start();
  AnmManager::fshader->SetViewMatrix(current_camera->view_matrix);
  AnmManager::fshader->SetProjectionMatrix(current_camera->projection_matrix);
  AnmManager::fshader->SetCameraPosition(current_camera->position);
  AnmManager::shader->stop();
  AnmManager::bshader->start();
  AnmManager::bshader->SetViewMatrix(current_camera->view_matrix);
  AnmManager::bshader->SetProjectionMatrix(current_camera->projection_matrix);
  AnmManager::bshader->stop();
  // d3d_device->SetViewport(&this->current_camera->viewport);
  glViewport(current_camera->viewport.X, current_camera->viewport.Y,
             current_camera->viewport.Width, current_camera->viewport.Height);
  return;
}

void Supervisor_t::disable_d3d_fog(bool force) {
  if (fog_enabled || force) {
    AnmManager::flush_vbos();
    fog_enabled = false;
    // d3d_device->SetRenderState(D3DRS_FOGENABLE, false);
    AnmManager::curr_shader = AnmManager::shader;
  }
}

void Supervisor_t::enable_d3d_fog(bool force) {
  if (!fog_enabled || force) {
    AnmManager::flush_vbos();
    fog_enabled = true;
    // d3d_device->SetRenderState(D3DRS_FOGENABLE, true);
    AnmManager::curr_shader = AnmManager::fshader;
  }
}

void Supervisor_t::set_fog_params(NSEngine::Color col, float beg, float end) {
  if (AnmManager::curr_shader == AnmManager::fshader) AnmManager::flush_vbos();
  AnmManager::fshader->start();
  AnmManager::fshader->SetFog(beg, end,
                {col.r / 255.f, col.g / 255.f, col.b / 255.f, col.a / 255.f});
  AnmManager::fshader->stop();
}

static NSEngine::ShaderProgram* last_shader_before_no_atest_save = nullptr;
void Supervisor_t::disable_atest() {
  if (AnmManager::curr_shader != AnmManager::bshader) {
    AnmManager::flush_vbos();
    last_shader_before_no_atest_save = AnmManager::curr_shader;
    AnmManager::curr_shader = AnmManager::bshader;
  }
}
void Supervisor_t::enable_atest() {
  if (AnmManager::curr_shader == AnmManager::bshader) {
    AnmManager::flush_vbos();
    AnmManager::curr_shader = last_shader_before_no_atest_save;
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
  if (surface_atR_0) {
    cameras[3] = cameras[0];
    return;
  }
  // if (!back_buffer) {
  //   d3d_device->GetBackBuffer(0, 0, 0, &back_buffer);
  // }
  // text_anm->d3d[2].texture->GetSurfaceLevel(0, &surface_atR_0);
  // text_anm->d3d[3].texture->GetSurfaceLevel(0, &surface_atR_1);
  surface_atR_0 = NSEngine::TextureManager::GetTexture(
    text_anm->textures.find("@R0")->second)->getFramebuffer();
  surface_atR_1 = NSEngine::TextureManager::GetTexture(
    text_anm->textures.find("@R1")->second)->getFramebuffer();
  if (!arcade_vm_0->bitflags.visible) {
    int i = -1;
    if (BACK_BUFFER_SIZE.x == 640) {
      i = 0;
    } else if (BACK_BUFFER_SIZE.x == 960) {
      i = 1;
    } else if (BACK_BUFFER_SIZE.x >= 1280) {
      i = 2;
    }
    if (i >= 0) {
      int a = 59;
      if (TOUHOU_VERSION == 18) a = 66;
      text_anm->copyFromLoaded(arcade_vm_0, a + i);
      arcade_vm_0->parent_vm = nullptr;
      arcade_vm_0->__root_vm__or_maybe_not = nullptr;
      arcade_vm_0->update();
      a = 65;
      if (TOUHOU_VERSION == 18) a = 72;
      text_anm->copyFromLoaded(arcade_vm_1, a + i);
      arcade_vm_1->parent_vm = nullptr;
      arcade_vm_1->__root_vm__or_maybe_not = nullptr;
      arcade_vm_1->update();
      a = 62;
      if (TOUHOU_VERSION == 18) a = 69;
      text_anm->copyFromLoaded(arcade_vm_2__handles_upscaling, a + i);
      arcade_vm_2__handles_upscaling->parent_vm = nullptr;
      arcade_vm_2__handles_upscaling->__root_vm__or_maybe_not = nullptr;
      arcade_vm_2__handles_upscaling->update();
      a = 68;
      if (TOUHOU_VERSION == 18) a = 75;
      text_anm->copyFromLoaded(arcade_vm_3__handles_seija, a + i);
      arcade_vm_3__handles_seija->parent_vm = nullptr;
      arcade_vm_3__handles_seija->__root_vm__or_maybe_not = nullptr;
      arcade_vm_3__handles_seija->update();

      arcade_vm_0->scale_2.y = -1;
      arcade_vm_1->scale_2.y = -1;
      arcade_vm_2__handles_upscaling->scale_2.y = -1;
      arcade_vm_3__handles_seija->scale_2.y = -1;
    }
  }
  if (RESOLUTION_MULT == 1.5)
    arcade_vm_2__handles_upscaling->bitflags.resampleMode = 0;
  return;
}

static int supervisor_on_draw_01() {
  if (SUPERVISOR.surface_atR_0 == nullptr) {
    glClearColor(SUPERVISOR.background_color.r / 255.f,
                 SUPERVISOR.background_color.g / 255.f,
                 SUPERVISOR.background_color.b / 255.f,
                 SUPERVISOR.background_color.a / 255.f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  } else {
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SUPERVISOR.surface_atR_0->bind();
    glViewport(SUPERVISOR.cameras[3].viewport.X,
               SUPERVISOR.cameras[3].viewport.Y,
               SUPERVISOR.cameras[3].viewport.Width,
               SUPERVISOR.cameras[3].viewport.Height);
    glClearColor(SUPERVISOR.background_color.r / 255.f,
                 SUPERVISOR.background_color.g / 255.f,
                 SUPERVISOR.background_color.b / 255.f,
                 SUPERVISOR.background_color.a / 255.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GAME_REGION_WIDTH = 384;
    GAME_REGION_HEIGHT = 448;
    SURF_ORIGIN_ECL_X = BACK_BUFFER_SIZE.x / 2;
    SURF_ORIGIN_ECL_Y = (BACK_BUFFER_SIZE.y - 448) / 2;
  }
  AnmManager::some_positions[0] = 0.0;
  AnmManager::last_used_texture = -1;
  AnmManager::last_used_blendmode = 10;
  AnmManager::field_0x18607c9 = 255;
  AnmManager::field_0x18607cb = 255;
  AnmManager::field_0x18607cc = 255;
  AnmManager::use_custom_color_1c90a4c = 0;
  AnmManager::custom_color_1c90a48.b = 128;
  AnmManager::custom_color_1c90a48.g = 128;
  AnmManager::custom_color_1c90a48.r = 128;
  AnmManager::custom_color_1c90a48.a = 128;
  AnmManager::last_used_resamplemode = 255;
  AnmManager::field_0x18607cf = 255;
  AnmManager::cam_vec2_fc_y = 0.0;
  AnmManager::cam_vec2_fc_x = 0.0;
  AnmManager::field_0x18607ca = 255;
  SUPERVISOR.set_camera_by_index(2);
  return 1;
}

static int supervisor_on_draw_0e() {
  if (SUPERVISOR.surface_atR_0 == nullptr) return 1;
  AnmManager::flush_vbos();
  SUPERVISOR.surface_atR_1->bind();
  glViewport(SUPERVISOR.cameras[3].viewport.X,
             SUPERVISOR.cameras[3].viewport.Y,
             SUPERVISOR.cameras[3].viewport.Width,
             SUPERVISOR.cameras[3].viewport.Height);
  glClearDepth(1.0f);
  glClear(GL_DEPTH_BUFFER_BIT);
  return 1;
}

static int supervisor_on_draw_0f() {
  if (SUPERVISOR.surface_atR_0 == nullptr) return 1;
  glBlendEquation(GL_FUNC_ADD);
  SUPERVISOR.disable_atest();
  SUPERVISOR.disable_zwrite();
  SUPERVISOR.set_camera_by_index(3);
  if (SUPERVISOR.arcade_vm_0->bitflags.anchorY != 0)
    SUPERVISOR.arcade_vm_0->bitflags.anchorY = SUPERVISOR.arcade_vm_0->bitflags.anchorY % 2 + 1;
  AnmManager::drawVM(SUPERVISOR.arcade_vm_0);
  if (SUPERVISOR.arcade_vm_0->bitflags.anchorY != 0)
    SUPERVISOR.arcade_vm_0->bitflags.anchorY = SUPERVISOR.arcade_vm_0->bitflags.anchorY % 2 + 1;
  SUPERVISOR.arcade_vm_0->color_1 = c_white;
  AnmManager::render_layer(34);
  AnmManager::flush_vbos();
  SUPERVISOR.enable_atest();
  return 1;
}

static int supervisor_on_draw_19() {
  if (SUPERVISOR.surface_atR_0 == nullptr) return 1;
  AnmManager::flush_vbos();
  SUPERVISOR.surface_atR_0->bind();
  glViewport(SUPERVISOR.cameras[3].viewport.X,
             SUPERVISOR.cameras[3].viewport.Y,
             SUPERVISOR.cameras[3].viewport.Width,
             SUPERVISOR.cameras[3].viewport.Height);
  glClearDepth(1.0f);
  glClear(GL_DEPTH_BUFFER_BIT);
  return 1;
}

static int supervisor_on_draw_1a() {
  if (SUPERVISOR.surface_atR_0 == nullptr) return 1;
  glBlendEquation(GL_FUNC_ADD);
  SUPERVISOR.disable_atest();
  SUPERVISOR.disable_zwrite();
  SUPERVISOR.set_camera_by_index(3);
  if (SUPERVISOR.arcade_vm_1->bitflags.anchorY != 0)
    SUPERVISOR.arcade_vm_1->bitflags.anchorY = SUPERVISOR.arcade_vm_1->bitflags.anchorY % 2 + 1;
  AnmManager::drawVM(SUPERVISOR.arcade_vm_1);
  if (SUPERVISOR.arcade_vm_1->bitflags.anchorY != 0)
    SUPERVISOR.arcade_vm_1->bitflags.anchorY = SUPERVISOR.arcade_vm_1->bitflags.anchorY % 2 + 1;
  SUPERVISOR.arcade_vm_1->color_1 = c_white;
  SUPERVISOR.enable_atest();
  return 1;
}

static int supervisor_on_draw_2c() {
  if (SUPERVISOR.surface_atR_0 == nullptr) return 1;
  AnmManager::flush_vbos();
  SUPERVISOR.surface_atR_1->bind();
  glViewport(SUPERVISOR.cameras[1].viewport.X,
             SUPERVISOR.cameras[1].viewport.Y,
             SUPERVISOR.cameras[1].viewport.Width,
             SUPERVISOR.cameras[1].viewport.Height);
  glClearDepth(1.0f);
  glClear(GL_DEPTH_BUFFER_BIT);
  GAME_REGION_WIDTH = RESOLUTION_MULT * 384.0;
  GAME_REGION_HEIGHT = RESOLUTION_MULT * 448.0;
  return 1;
}

static int supervisor_on_draw_2d() {
  if (SUPERVISOR.surface_atR_0 == nullptr) return 1;
  glBlendEquation(GL_FUNC_ADD);
  SUPERVISOR.disable_atest();
  SUPERVISOR.disable_zwrite();
  SUPERVISOR.set_camera_by_index(1);
  if (SUPERVISOR.arcade_vm_2__handles_upscaling->bitflags.anchorY != 0)
    SUPERVISOR.arcade_vm_2__handles_upscaling->bitflags.anchorY = SUPERVISOR.arcade_vm_2__handles_upscaling->bitflags.anchorY % 2 + 1;
  AnmManager::drawVM(SUPERVISOR.arcade_vm_2__handles_upscaling);
  if (SUPERVISOR.arcade_vm_2__handles_upscaling->bitflags.anchorY != 0)
    SUPERVISOR.arcade_vm_2__handles_upscaling->bitflags.anchorY = SUPERVISOR.arcade_vm_2__handles_upscaling->bitflags.anchorY % 2 + 1;
  SUPERVISOR.arcade_vm_2__handles_upscaling->color_1 = c_white;
  SUPERVISOR.enable_atest();
  return 1;
}

static int supervisor_on_draw_39() {
  AnmManager::flush_vbos();
  NSEngine::FrameBuffer::unbindFramebuffer();
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClearDepth(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  return 1;
}

static int supervisor_on_draw_3a() {
  if (SUPERVISOR.surface_atR_0 == nullptr) return 1;
  SUPERVISOR.disable_atest();
  if (SUPERVISOR.arcade_vm_3__handles_seija->bitflags.anchorY != 0)
    SUPERVISOR.arcade_vm_3__handles_seija->bitflags.anchorY = SUPERVISOR.arcade_vm_3__handles_seija->bitflags.anchorY % 2 + 1;
  AnmManager::drawVM(SUPERVISOR.arcade_vm_3__handles_seija);
  if (SUPERVISOR.arcade_vm_3__handles_seija->bitflags.anchorY != 0)
    SUPERVISOR.arcade_vm_3__handles_seija->bitflags.anchorY = SUPERVISOR.arcade_vm_3__handles_seija->bitflags.anchorY % 2 + 1;
  SUPERVISOR.arcade_vm_3__handles_seija->color_1 = c_white;
  SUPERVISOR.enable_atest();
  return 1;
}

static int supervisor_on_draw_56() {
  AnmManager::flush_vbos();
  SUPERVISOR.cameras[3].screen_shake.x = 0.0;
  SUPERVISOR.cameras[3].screen_shake.y = 0.0;
  SUPERVISOR.cameras[1].screen_shake.x = 0.0;
  SUPERVISOR.cameras[1].screen_shake.y = 0.0;
  return 1;
}


static struct TickCounter* TICK_COUNTER_PTR;

struct TickCounter {
  // unused 4 (flags)
  UpdateFunc* on_tick = nullptr;
  // unused 4 (on_draw)
  int nTick = 0;

  TickCounter() {
    on_tick = new UpdateFunc(f_on_tick);
    UPDATE_FUNC_REGISTRY->register_on_tick(on_tick, 2);
    TICK_COUNTER_PTR = this;
  }

  ~TickCounter() {
    if (on_tick)
      UPDATE_FUNC_REGISTRY->unregister(on_tick);
  }

  static int f_on_tick() {
    TICK_COUNTER_PTR->nTick++;
    return  1;
  }
};

static struct FpsCounter* FPS_COUNTER_PTR;

struct FpsCounter {
  // unused 4 (flags)
  // unused 4 (on_tick)
  UpdateFunc* on_draw = nullptr;
  // undefined4
  double dbl1 = 0.0;
  int inte = 0;
  unsigned int uint = 0;
  double dbl2 = 0.0;
  double dbl3 = 0.0;
  float current_fps = 0.0f;
  // undefined4

  FpsCounter() {
    on_draw = new UpdateFunc(f_on_draw);
    UPDATE_FUNC_REGISTRY->register_on_draw(on_draw, 76);
    FPS_COUNTER_PTR = this;
  }

  ~FpsCounter() {
    if (on_draw)
      UPDATE_FUNC_REGISTRY->unregister(on_draw);
  }

  static int f_on_draw() {
    if ((SUPERVISOR.gamemode_to_switch_to != 0xf) &&
        (SUPERVISOR.gamemode_to_switch_to != 4)) {
      float fps = FPS_COUNTER_PTR->current_fps;
      if (ASCII_MANAGER_PTR) {
        if (fps < 30) {
          ASCII_MANAGER_PTR->color = {80, 80, 0xff, 0xff};
        } else if (fps < 40.0) {
          ASCII_MANAGER_PTR->color = {0xa0, 0xa0, 0xff, 0xff};
        } else {
          ASCII_MANAGER_PTR->color = c_white;
        }
        auto fid = ASCII_MANAGER_PTR->font_id;
        ASCII_MANAGER_PTR->alignment_mode_h = 1;
        ASCII_MANAGER_PTR->font_id = 1;
        ASCII_MANAGER_PTR->create_string_f({588, 470, 0},
                                           "%2.1ffps", fps + 0.05);
        ASCII_MANAGER_PTR->font_id = fid;
        ASCII_MANAGER_PTR->color = c_white;
      }
    }
    return 1;
  }
};

#include <Engine.hpp>

static int supervisor_on_tick() {
  if (FPS_COUNTER_PTR) FPS_COUNTER_PTR->current_fps = NSEngine::getInstance()->fps();
  return 1;
//   void *pvVar1;
//   zAnmLoaded *anmloaded;
//   int *piVar2;
//   int iVar3;
//   uint uVar4;
//   zAnmPreloadedArray *pzVar5;
//   zAnmManager *ANM_MANAGER_PTR_;
//   
//   if (((SUPERVISOR.flags & 0x180U) == 0x80) && ((SUPERVISOR.__thread_998).__bool_10 == 0)) {
//     SUPERVISOR.gamemode_to_switch_to = 3;
//   }
//   sound_related_465660();
//   pvVar1 = DAT_00529e88;
//   if (DAT_00529e88 != NULL) {
//     if (*(int *)((int)DAT_00529e88 + 0x20) == 1) {
//       *(int *)((int)DAT_00529e88 + 0x18) = *(int *)((int)DAT_00529e88 + 0x18) + -1;
//       if (*(int *)((int)pvVar1 + 0x18) < 1) {
//         *(undefined4 *)((int)pvVar1 + 0x20) = 0;
//         (**(code **)(***(int ***)((int)pvVar1 + 8) + 0x48))(**(int ***)((int)pvVar1 + 8));
//       }
//       else {
//         FUN_00477ca0(pvVar1,(*(int *)((int)pvVar1 + 0x18) * 5000) / *(int *)((int)pvVar1 + 0x1c) +
//                             -5000);
//       }
//     }
//     pvVar1 = DAT_00529e88;
//     if (*(int *)((int)DAT_00529e88 + 0x20) == 2) {
//       *(int *)((int)DAT_00529e88 + 0x18) = *(int *)((int)DAT_00529e88 + 0x18) + -1;
//       if (*(int *)((int)pvVar1 + 0x18) < 1) {
//         *(undefined4 *)((int)pvVar1 + 0x20) = 0;
//       }
//       else {
//         FUN_00477ca0(pvVar1,(*(int *)((int)pvVar1 + 0x18) * -5000) / *(int *)((int)pvVar1 + 0x1c));
//       }
//     }
//     pvVar1 = DAT_00529e88;
//     if (*(int *)((int)DAT_00529e88 + 0x20) == 4) {
//       *(int *)((int)DAT_00529e88 + 0x18) = *(int *)((int)DAT_00529e88 + 0x18) + -1;
//       if (*(int *)((int)pvVar1 + 0x18) < 1) {
//         *(undefined4 *)((int)pvVar1 + 0x20) = 0;
//       }
//       else {
//         FUN_00477ca0(pvVar1,(*(int *)((int)pvVar1 + 0x18) * 1000) / *(int *)((int)pvVar1 + 0x1c) +
//                             -1000);
//       }
//     }
//     pvVar1 = DAT_00529e88;
//     if (*(int *)((int)DAT_00529e88 + 0x20) == 3) {
//       *(int *)((int)DAT_00529e88 + 0x18) = *(int *)((int)DAT_00529e88 + 0x18) + -1;
//       if (*(int *)((int)pvVar1 + 0x18) < 1) {
//         *(undefined4 *)((int)pvVar1 + 0x20) = 0;
//       }
//       else {
//         FUN_00477ca0(pvVar1,(*(int *)((int)pvVar1 + 0x18) * -1000) / *(int *)((int)pvVar1 + 0x1c));
//       }
//     }
//   }
//   read_keyboard_input();
//   ANM_MANAGER_PTR_ = ANM_MANAGER_PTR;
//   uVar4 = 0;
//   pzVar5 = &ANM_MANAGER_PTR->preloaded;
//   do {
//     anmloaded = (pzVar5->by_name).text;
//     if (anmloaded != NULL) {
//       if (anmloaded->field11_0x12c == 0) {
//         if (anmloaded->load_wait != 0) {
//           piVar2 = AnmManager::fullyLoadFile(ANM_MANAGER_PTR_,anmloaded);
//           if (piVar2 == NULL) {
//             return 4;
//           }
//           break;
//         }
//       }
//       else {
//         if (-1 < (int)uVar4) {
//           AnmManager::_unload_file(ANM_MANAGER_PTR_,uVar4);
//           anmloaded = (pzVar5->by_name).text;
//         }
//         anmloaded->field11_0x12c = 0;
//       }
//     }
//     uVar4 += 1;
//     pzVar5 = (zAnmPreloadedArray *)((int)pzVar5 + 4);
//   } while (uVar4 < 0x1f);
//   if (DAT_00524704 != 0) {
//     DAT_00524704 += -1;
//   }
//   pvVar1 = (this->__thread_998).func;
//   if (pvVar1 == NULL) {
//     iVar3 = switch_gamemodes(this);
//     if (iVar3 != 1) {
//       return iVar3;
//     }
//     SURF_ORIGIN_ECL_X = BACK_BUFFER_SIZE.x / 2;
//     SURF_ORIGIN_ECL_Y = (BACK_BUFFER_SIZE.y + -448) / 2;
//   }
//   else if (pvVar1 == (void *)0x2) {
//     return 4;
//   }
//   return 1;
}


static int supervisor_on_registration() {
  // if (!OpenDatFile()) {
  //   NSEngine::error("データファイルが存在しません");
  // } else {
  //   char str_buff[128];
  //   snprintf(str_buff, sizeof(str_buff), "th17_%.4x%c.ver", 256, 'b');
  //   SUPERVISOR.field107_0xa24 = reads_file_into_new_allocation_4020d0(str_buff,&local_8c,0);
  //   SUPERVISOR.field106_0xa20 = local_8c;
  //   if (SUPERVISOR.field107_0xa24 == nullptr)
  //     NSEngine::error("データのバージョンが違います");
  // }
  GAME_SPEED = 1.0;
  SUPERVISOR.background_color = {0, 0, 0, 255};
  SUPERVISOR.init_cameras();
  // SUPERVISOR.rng_seed_from_time = timeGetTime();
  // REPLAY_UNSAFE_RNG._0_2_ = SUPERVISOR.rng_seed_from_time;
  // REPLAY_SAFE_RNG._0_2_ = SUPERVISOR.rng_seed_from_time;
  // hHandle_00529ea0 =
  //      CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)&lpStartAddress_00465030,&SOUND_MANAGER,0,
  //                   (LPDWORD)&local_8c);
  new FpsCounter;
  new TickCounter;
  // SUPERVISOR.create_vertex_buffers();
  // SUPERVISOR.load_fonts();

  SUPERVISOR.arcade_vm_0 = new AnmVM();
  SUPERVISOR.arcade_vm_1 = new AnmVM();
  SUPERVISOR.arcade_vm_2__handles_upscaling = new AnmVM();
  SUPERVISOR.arcade_vm_3__handles_seija = new AnmVM();
  // SUPERVISOR_DRAW_0F_FNPTR = 0;
  // SUPERVISOR_ON_DRAW_1A_FNPTR = 0;
  return 0;
}

int Supervisor_t::initialize() {
  UpdateFunc* func;
  SUPERVISOR.gamemode_current = -2;
  SUPERVISOR.gamemode_to_switch_to = 0;
  // SUPERVISOR.field60_0x6fc = 0;
  func = new UpdateFunc(supervisor_on_tick);
  func->on_registration = supervisor_on_registration;
  auto r = UPDATE_FUNC_REGISTRY->register_on_tick(func, 1);
  if (r) return r;
  func = new UpdateFunc(supervisor_on_draw_01);
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 1);
  func = new UpdateFunc(supervisor_on_draw_0e);
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 0xe);
  func = new UpdateFunc(supervisor_on_draw_0f);
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 0xf);
  func = new UpdateFunc(supervisor_on_draw_19);
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 0x19);
  func = new UpdateFunc(supervisor_on_draw_1a);
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 0x1a);
  func = new UpdateFunc(supervisor_on_draw_2c);
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 0x2c);
  func = new UpdateFunc(supervisor_on_draw_2d);
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 0x2d);
  func = new UpdateFunc(supervisor_on_draw_39);
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 0x39);
  func = new UpdateFunc(supervisor_on_draw_3a);
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 0x3a);
  func = new UpdateFunc(supervisor_on_draw_56);
  UPDATE_FUNC_REGISTRY->register_on_draw(func, 0x56);
  // get back_buffer ...
  return 0;
}
