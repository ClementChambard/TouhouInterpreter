#include "./Supervisor.h"
#include "AnmOpener/AnmManager.h"
#include "AsciiManager.hpp"
#include "Hardcoded.h"
#include "UpdateFuncRegistry.h"
#include <TextureManager.h>
#include <NSEngine.hpp>

Supervisor_t SUPERVISOR{};

float GAME_REGION_WIDTH = 0.0;
float GAME_REGION_HEIGHT = 0.0;
float GAME_REGION_X = 0.0;
float GAME_REGION_Y = 0.0;

void Supervisor_t::init() {  // temporary
  initialize();
  text_anm = anm::loadFile(0, "text.anm");
  anm::loadFile(1, "sig.anm");
}

Supervisor_t::~Supervisor_t() {
  if (arcade_vm_0) delete arcade_vm_0;
  if (arcade_vm_1) delete arcade_vm_1;
  if (arcade_vm_2__handles_upscaling) delete arcade_vm_2__handles_upscaling;
  if (arcade_vm_3__handles_seija) delete arcade_vm_3__handles_seija;
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
  cameras[2].viewport.Width = anm::BACK_BUFFER_SIZE.x;
  cameras[2].viewport.Height = anm::BACK_BUFFER_SIZE.y;
  cameras[2].viewport.MinZ = 0.0;
  cameras[2].viewport.MaxZ = 1.0;
  // fullscreen ?
  // if (DAT_00524700 & 0x3c == 8) {
  //   cameras[2].viewport.Height = 960;
  // }
  cameras[2].camera_index = 2;
  cameras[2].window_resolution.x = anm::BACK_BUFFER_SIZE.x;
  cameras[2].window_resolution.y = anm::BACK_BUFFER_SIZE.y;
  cameras[2].as_2d_matrix();
  cameras[0] = cameras[2];
  cameras[0].camera_index = 0;
  cameras[0].viewport.X = static_cast<int>(anm::RESOLUTION_MULT * 32.0);
  cameras[0].viewport.Y = static_cast<int>(anm::RESOLUTION_MULT * 16.0);
  cameras[0].viewport.Width = static_cast<int>(anm::RESOLUTION_MULT * 384.0);
  cameras[0].viewport.Height = static_cast<int>(anm::RESOLUTION_MULT * 448.0);
  cameras[0].as_2d_matrix();
  cameras[1] = cameras[2];
  cameras[1].camera_index = 1;
  cameras[1].viewport.X = static_cast<int>(anm::RESOLUTION_MULT * 128.0);
  cameras[1].viewport.Y = static_cast<int>(anm::RESOLUTION_MULT * 16.0);
  cameras[1].viewport.Width = static_cast<int>(anm::RESOLUTION_MULT * 384.0);
  cameras[1].viewport.Height = static_cast<int>(anm::RESOLUTION_MULT * 448.0);
  cameras[1].as_2d_matrix();
  cameras[3] = cameras[2];
  cameras[3].camera_index = 3;
  cameras[3].viewport.X = static_cast<int>((anm::BACK_BUFFER_SIZE.x - 408.0) * 0.5);
  cameras[3].viewport.Y = static_cast<int>((anm::BACK_BUFFER_SIZE.y - 472.0) * 0.5);
  cameras[3].viewport.Width = 408;
  cameras[3].viewport.Height = 472;
  cameras[3].as_2d_matrix();
  anm::origin(2).x = anm::BACK_BUFFER_SIZE.x / 2;
  anm::origin(2).y = static_cast<int>(anm::RESOLUTION_MULT * 16.0);
  anm::origin(1).x = anm::BACK_BUFFER_SIZE.x / 2;
  anm::origin(1).y = (anm::BACK_BUFFER_SIZE.y + -448) / 2;
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
  surface_atR_0 = ns::TextureManager::GetTexture(
    text_anm->textures.find("@R0")->second)->getFramebuffer();
  surface_atR_1 = ns::TextureManager::GetTexture(
    text_anm->textures.find("@R1")->second)->getFramebuffer();
  if (!arcade_vm_0->bitflags.visible) {
    int i = -1;
    if (anm::BACK_BUFFER_SIZE.x == 640) {
      i = 0;
    } else if (anm::BACK_BUFFER_SIZE.x == 960) {
      i = 1;
    } else if (anm::BACK_BUFFER_SIZE.x >= 1280) {
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
  if (anm::RESOLUTION_MULT == 1.5)
    arcade_vm_2__handles_upscaling->bitflags.resampleMode = 0;
  return;
}

static int supervisor_on_draw_01() {
  if (SUPERVISOR.surface_atR_0 == nullptr) {
    anm::clear_framebuffer(SUPERVISOR.background_color);
  } else {
    anm::clear_framebuffer(c_white);
    SUPERVISOR.surface_atR_0->bind();
    anm::set_viewport(SUPERVISOR.cameras[3]);
    anm::clear_framebuffer(SUPERVISOR.background_color);
    GAME_REGION_WIDTH = 384;
    GAME_REGION_HEIGHT = 448;
    anm::origin(1).x = anm::BACK_BUFFER_SIZE.x / 2;
    anm::origin(1).y = (anm::BACK_BUFFER_SIZE.y - 448) / 2;
    GAME_REGION_X = anm::origin(1).x - GAME_REGION_WIDTH / 2;
    GAME_REGION_Y = anm::origin(1).y - GAME_REGION_HEIGHT / 2;
  }
  anm::reset_batch_state();
  anm::set_camera(&SUPERVISOR.cameras[2]);
  return 1;
}

static int supervisor_on_draw_0e() {
  if (SUPERVISOR.surface_atR_0 == nullptr) return 1;
  anm::flush_vbos();
  SUPERVISOR.surface_atR_1->bind();
  anm::set_viewport(SUPERVISOR.cameras[3]);
  anm::clear_framebuffer_depth();
  return 1;
}

static int supervisor_on_draw_0f() {
  if (SUPERVISOR.surface_atR_0 == nullptr) return 1;
  anm::reset_blend_eq();
  anm::disable_atest();
  anm::disable_zwrite();
  anm::set_camera(&SUPERVISOR.cameras[3]);
  if (SUPERVISOR.arcade_vm_0->bitflags.anchorY != 0)
    SUPERVISOR.arcade_vm_0->bitflags.anchorY = SUPERVISOR.arcade_vm_0->bitflags.anchorY % 2 + 1;
  anm::drawVM(SUPERVISOR.arcade_vm_0);
  if (SUPERVISOR.arcade_vm_0->bitflags.anchorY != 0)
    SUPERVISOR.arcade_vm_0->bitflags.anchorY = SUPERVISOR.arcade_vm_0->bitflags.anchorY % 2 + 1;
  SUPERVISOR.arcade_vm_0->color_1 = c_white;
  anm::render_layer(34);
  anm::flush_vbos();
  anm::enable_atest();
  return 1;
}

static int supervisor_on_draw_19() {
  if (SUPERVISOR.surface_atR_0 == nullptr) return 1;
  anm::flush_vbos();
  SUPERVISOR.surface_atR_0->bind();
  anm::set_viewport(SUPERVISOR.cameras[3]);
  anm::clear_framebuffer_depth();
  return 1;
}

static int supervisor_on_draw_1a() {
  if (SUPERVISOR.surface_atR_0 == nullptr) return 1;
  anm::reset_blend_eq();
  anm::disable_atest();
  anm::disable_zwrite();
  anm::set_camera(&SUPERVISOR.cameras[3]);
  if (SUPERVISOR.arcade_vm_1->bitflags.anchorY != 0)
    SUPERVISOR.arcade_vm_1->bitflags.anchorY = SUPERVISOR.arcade_vm_1->bitflags.anchorY % 2 + 1;
  anm::drawVM(SUPERVISOR.arcade_vm_1);
  if (SUPERVISOR.arcade_vm_1->bitflags.anchorY != 0)
    SUPERVISOR.arcade_vm_1->bitflags.anchorY = SUPERVISOR.arcade_vm_1->bitflags.anchorY % 2 + 1;
  SUPERVISOR.arcade_vm_1->color_1 = c_white;
  anm::enable_atest();
  return 1;
}

static int supervisor_on_draw_2c() {
  if (SUPERVISOR.surface_atR_0 == nullptr) return 1;
  anm::flush_vbos();
  SUPERVISOR.surface_atR_1->bind();
  anm::set_viewport(SUPERVISOR.cameras[1]);
  anm::clear_framebuffer_depth();
  GAME_REGION_WIDTH = anm::RESOLUTION_MULT * 384.0;
  GAME_REGION_HEIGHT = anm::RESOLUTION_MULT * 448.0;
  GAME_REGION_X = anm::origin(2).x - GAME_REGION_WIDTH / 2;
  GAME_REGION_Y = anm::origin(2).y - GAME_REGION_HEIGHT / 2;
  return 1;
}

static int supervisor_on_draw_2d() {
  if (SUPERVISOR.surface_atR_0 == nullptr) return 1;
  anm::reset_blend_eq();
  anm::disable_atest();
  anm::disable_zwrite();
  anm::set_camera(&SUPERVISOR.cameras[1]);
  if (SUPERVISOR.arcade_vm_2__handles_upscaling->bitflags.anchorY != 0)
    SUPERVISOR.arcade_vm_2__handles_upscaling->bitflags.anchorY = SUPERVISOR.arcade_vm_2__handles_upscaling->bitflags.anchorY % 2 + 1;
  anm::drawVM(SUPERVISOR.arcade_vm_2__handles_upscaling);
  if (SUPERVISOR.arcade_vm_2__handles_upscaling->bitflags.anchorY != 0)
    SUPERVISOR.arcade_vm_2__handles_upscaling->bitflags.anchorY = SUPERVISOR.arcade_vm_2__handles_upscaling->bitflags.anchorY % 2 + 1;
  SUPERVISOR.arcade_vm_2__handles_upscaling->color_1 = c_white;
  anm::enable_atest();
  return 1;
}

static int supervisor_on_draw_39() {
  anm::flush_vbos();
  ns::FrameBuffer::unbindFramebuffer();
  anm::clear_framebuffer();
  return 1;
}

static int supervisor_on_draw_3a() {
  if (SUPERVISOR.surface_atR_0 == nullptr) return 1;
  anm::disable_atest();
  if (SUPERVISOR.arcade_vm_3__handles_seija->bitflags.anchorY != 0)
    SUPERVISOR.arcade_vm_3__handles_seija->bitflags.anchorY = SUPERVISOR.arcade_vm_3__handles_seija->bitflags.anchorY % 2 + 1;
  anm::drawVM(SUPERVISOR.arcade_vm_3__handles_seija);
  if (SUPERVISOR.arcade_vm_3__handles_seija->bitflags.anchorY != 0)
    SUPERVISOR.arcade_vm_3__handles_seija->bitflags.anchorY = SUPERVISOR.arcade_vm_3__handles_seija->bitflags.anchorY % 2 + 1;
  SUPERVISOR.arcade_vm_3__handles_seija->color_1 = c_white;
  anm::enable_atest();
  return 1;
}

static int supervisor_on_draw_56() {
  anm::flush_vbos();
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
    UPDATE_FUNC_REGISTRY.register_on_tick(on_tick, 2);
    TICK_COUNTER_PTR = this;
  }

  ~TickCounter() {
    if (on_tick)
      UPDATE_FUNC_REGISTRY.unregister(on_tick);
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
    UPDATE_FUNC_REGISTRY.register_on_draw(on_draw, 76);
    FPS_COUNTER_PTR = this;
  }

  ~FpsCounter() {
    if (on_draw)
      UPDATE_FUNC_REGISTRY.unregister(on_draw);
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

static int supervisor_on_tick() {
  if (FPS_COUNTER_PTR) FPS_COUNTER_PTR->current_fps = ns::getInstance()->fps();
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
//           piVar2 = anm::fullyLoadFile(ANM_MANAGER_PTR_,anmloaded);
//           if (piVar2 == NULL) {
//             return 4;
//           }
//           break;
//         }
//       }
//       else {
//         if (-1 < (int)uVar4) {
//           anm::_unload_file(ANM_MANAGER_PTR_,uVar4);
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
//     SURF_ORIGIN_ECL_X = anm::BACK_BUFFER_SIZE.x / 2;
//     SURF_ORIGIN_ECL_Y = (anm::BACK_BUFFER_SIZE.y + -448) / 2;
//   }
//   else if (pvVar1 == (void *)0x2) {
//     return 4;
//   }
//   return 1;
}


static int supervisor_on_registration() {
  // if (!OpenDatFile()) {
  //   ns::error("データファイルが存在しません");
  // } else {
  //   char str_buff[128];
  //   snprintf(str_buff, sizeof(str_buff), "th17_%.4x%c.ver", 256, 'b');
  //   SUPERVISOR.field107_0xa24 = reads_file_into_new_allocation_4020d0(str_buff,&local_8c,0);
  //   SUPERVISOR.field106_0xa20 = local_8c;
  //   if (SUPERVISOR.field107_0xa24 == nullptr)
  //     ns::error("データのバージョンが違います");
  // }
  ns::getInstance()->setGameSpeed(1.0);
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

  SUPERVISOR.arcade_vm_0 = new anm::VM();
  SUPERVISOR.arcade_vm_1 = new anm::VM();
  SUPERVISOR.arcade_vm_2__handles_upscaling = new anm::VM();
  SUPERVISOR.arcade_vm_3__handles_seija = new anm::VM();
  // SUPERVISOR_DRAW_0F_FNPTR = 0;
  // SUPERVISOR_ON_DRAW_1A_FNPTR = 0;
  return 0;
}

static void anmmanager_updatefuncs() {
  anm::set_3d_camera(&SUPERVISOR.cameras[3]);
  UpdateFunc* func;
  func = new UpdateFunc(anm::on_tick_world);
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_tick(func, 35);
  func = new UpdateFunc(anm::on_tick_ui);
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_tick(func, 10);
  func = new UpdateFunc([](){return anm::render_layer(0);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 5);
  func = new UpdateFunc([](){return anm::render_layer(1);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 7);
  func = new UpdateFunc([](){return anm::render_layer(2);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 9);
  func = new UpdateFunc([](){return anm::render_layer(4);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 11);
  func = new UpdateFunc([](){
    anm::set_camera(SUPERVISOR.cameras[3].as_2d_matrix());
    anm::disable_fog();
    return anm::render_layer(3);
  });
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 10);
  func = new UpdateFunc([](){return anm::render_layer(5);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 13);
  func = new UpdateFunc([](){return anm::render_layer(6);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 16);
  func = new UpdateFunc([](){return anm::render_layer(7);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 18);
  func = new UpdateFunc([](){return anm::render_layer(8);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 20);
  func = new UpdateFunc([](){return anm::render_layer(9);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 21);
  func = new UpdateFunc([](){return anm::render_layer(10);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 22);
  func = new UpdateFunc([](){return anm::render_layer(11);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 24);
  func = new UpdateFunc([](){return anm::render_layer(12);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 27);
  func = new UpdateFunc([](){return anm::render_layer(13);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 28);
  func = new UpdateFunc([](){return anm::render_layer(14);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 31);
  func = new UpdateFunc([](){return anm::render_layer(15);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 32);
  func = new UpdateFunc([](){return anm::render_layer(16);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 35);
  func = new UpdateFunc([](){return anm::render_layer(17);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 37);
  func = new UpdateFunc([](){return anm::render_layer(18);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 40);
  func = new UpdateFunc([](){return anm::render_layer(19);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 43);
  func = new UpdateFunc([](){
    anm::set_camera(&SUPERVISOR.cameras[1]);
    anm::disable_zwrite();
    anm::disable_ztest();
    return anm::render_layer(20);
  });
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 46);
  func = new UpdateFunc([](){return anm::render_layer(21);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 47);
  func = new UpdateFunc([](){return anm::render_layer(22);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 53);
  func = new UpdateFunc([](){return anm::render_layer(23);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 55);
  func = new UpdateFunc([](){return anm::render_layer(26);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 62);
  func = new UpdateFunc([](){return anm::render_layer(27);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 63);
  func = new UpdateFunc([](){return anm::render_layer(29);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 78);
  func = new UpdateFunc([](){return anm::render_layer(30);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 80);
  func = new UpdateFunc([](){return anm::render_layer(31);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 83);
  func = new UpdateFunc([](){return anm::render_layer(25);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 60);
  func = new UpdateFunc([](){
    anm::set_camera(&SUPERVISOR.cameras[2]);
    anm::disable_zwrite();
    anm::disable_ztest();
    anm::_set_cam_vec2_fc({0.0f, 0.0f});
    return anm::render_layer(24);
  });
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 59);
  func = new UpdateFunc([](){
    anm::set_camera(&SUPERVISOR.cameras[0]);
    return anm::render_layer(28);
    anm::set_camera(&SUPERVISOR.cameras[2]);
  });
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 65);
  func = new UpdateFunc([](){
    anm::set_camera(&SUPERVISOR.cameras[2]);
    anm::disable_zwrite();
    anm::disable_ztest();
    return anm::render_layer(36);
  });
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 56);
  func = new UpdateFunc([](){return anm::render_layer(37);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 61);
  func = new UpdateFunc([](){return anm::render_layer(38);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 64);
  func = new UpdateFunc([](){
    anm::set_camera(&SUPERVISOR.cameras[0]);
    return anm::render_layer(39);
    anm::set_camera(&SUPERVISOR.cameras[2]);
  });
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 66);
  func = new UpdateFunc([](){return anm::render_layer(40);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 79);
  func = new UpdateFunc([](){return anm::render_layer(41);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 81);
  func = new UpdateFunc([](){return anm::render_layer(42);});
  func->flags |= 2;
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 84);
}

int Supervisor_t::initialize() {
  UpdateFunc* func;
  SUPERVISOR.gamemode_current = -2;
  SUPERVISOR.gamemode_to_switch_to = 0;
  // SUPERVISOR.field60_0x6fc = 0;
  func = new UpdateFunc(supervisor_on_tick);
  func->on_registration = supervisor_on_registration;
  auto r = UPDATE_FUNC_REGISTRY.register_on_tick(func, 1);
  if (r) return r;
  func = new UpdateFunc(supervisor_on_draw_01);
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 1);
  func = new UpdateFunc(supervisor_on_draw_0e);
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 0xe);
  func = new UpdateFunc(supervisor_on_draw_0f);
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 0xf);
  func = new UpdateFunc(supervisor_on_draw_19);
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 0x19);
  func = new UpdateFunc(supervisor_on_draw_1a);
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 0x1a);
  func = new UpdateFunc(supervisor_on_draw_2c);
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 0x2c);
  func = new UpdateFunc(supervisor_on_draw_2d);
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 0x2d);
  func = new UpdateFunc(supervisor_on_draw_39);
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 0x39);
  func = new UpdateFunc(supervisor_on_draw_3a);
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 0x3a);
  func = new UpdateFunc(supervisor_on_draw_56);
  UPDATE_FUNC_REGISTRY.register_on_draw(func, 0x56);
  // get back_buffer ...
  anmmanager_updatefuncs();
  return 0;
}

