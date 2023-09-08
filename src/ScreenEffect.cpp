#include "./ScreenEffect.hpp"
#include "AnmOpener/AnmManager.h"
#include "GlobalData.h"
#include "Supervisor.h"

ScreenEffect::ScreenEffect(int _type, int _time, int colorRGB_maybe,
                           int color, int param_6, int on_draw_prio) {
  switch (_type) {
  case 0:
    on_tick = new UpdateFunc([this](){return this->f_on_tick_1();});
    on_tick->flags |= 2;
    UPDATE_FUNC_REGISTRY->register_on_tick(on_tick, 0x14);
    on_draw = new UpdateFunc([this](){return this->f_on_draw_3();});
    on_draw->flags |= 2;
    UPDATE_FUNC_REGISTRY->register_on_draw(on_draw, on_draw_prio);
    break;
  case 1:
    on_tick = new UpdateFunc([this](){return this->f_on_tick_2();});
    on_tick->flags |= 2;
    UPDATE_FUNC_REGISTRY->register_on_tick(on_tick, 0x14);
    break;
  case 2:
    on_tick = new UpdateFunc([this](){return this->f_on_tick_3();});
    on_tick->flags |= 2;
    UPDATE_FUNC_REGISTRY->register_on_tick(on_tick, 0x14);
    on_draw = new UpdateFunc([this](){return this->f_on_draw_1();});
    on_draw->flags |= 2;
    UPDATE_FUNC_REGISTRY->register_on_draw(on_draw, on_draw_prio);
    break;
  case 3:
    alpha = 255;
    on_tick = new UpdateFunc([this](){return this->f_on_tick_1();});
    on_tick->flags |= 2;
    UPDATE_FUNC_REGISTRY->register_on_tick(on_tick, 0x14);
    on_draw = new UpdateFunc([this](){return this->f_on_draw_1();});
    on_draw->flags |= 2;
    UPDATE_FUNC_REGISTRY->register_on_draw(on_draw, on_draw_prio);
    break;
  case 4:
    on_tick = new UpdateFunc([this](){return this->f_on_tick_4();});
    on_tick->flags |= 2;
    UPDATE_FUNC_REGISTRY->register_on_tick(on_tick, 0x14);
    on_draw = new UpdateFunc([this](){return this->f_on_draw_2();});
    on_draw->flags |= 2;
    UPDATE_FUNC_REGISTRY->register_on_draw(on_draw, on_draw_prio);
    break;
  case 5:
    on_tick = new UpdateFunc([this](){return this->f_on_tick_3();});
    on_tick->flags |= 2;
    UPDATE_FUNC_REGISTRY->register_on_tick(on_tick, 0x14);
    on_draw = new UpdateFunc([this](){return this->f_on_draw_3();});
    on_draw->flags |= 2;
    UPDATE_FUNC_REGISTRY->register_on_draw(on_draw, on_draw_prio);
    break;
  case 6:
    on_tick = new UpdateFunc([this](){return this->f_on_tick_5();});
    on_tick->flags |= 2;
    UPDATE_FUNC_REGISTRY->register_on_tick(on_tick, 0x14);
    on_draw = new UpdateFunc([this](){return this->f_on_draw_5();});
    on_draw->flags |= 2;
    UPDATE_FUNC_REGISTRY->register_on_draw(on_draw, on_draw_prio);
    break;
  case 7:
    on_tick = new UpdateFunc([this](){return this->f_on_tick_5();});
    on_tick->flags |= 2;
    UPDATE_FUNC_REGISTRY->register_on_tick(on_tick, 0x14);
    on_draw = new UpdateFunc([this](){return this->f_on_draw_4();});
    on_draw->flags |= 2;
    UPDATE_FUNC_REGISTRY->register_on_draw(on_draw, on_draw_prio);
    break;
  case 8:
    on_tick = new UpdateFunc([this](){return this->f_on_tick_6();});
    on_tick->flags |= 2;
    UPDATE_FUNC_REGISTRY->register_on_tick(on_tick, 0x14);
    break;
  case 9:
    on_tick = new UpdateFunc([this](){return this->f_on_tick_7();});
    on_tick->flags |= 2;
    UPDATE_FUNC_REGISTRY->register_on_tick(on_tick, 0x14);
    on_draw = new UpdateFunc([this](){return this->f_on_draw_5();});
    on_draw->flags |= 2;
    UPDATE_FUNC_REGISTRY->register_on_draw(on_draw, on_draw_prio);
    break;
  }
  on_tick->on_cleanup = [this](){ delete this; return 0; };
  time = 0;
  field_0x18 = _time;
  field_0x1c = colorRGB_maybe;
  field_0x20 = color;
  type = _type;
  field_0x24 = param_6;
  return;
}

ScreenEffect::~ScreenEffect() {
  if (on_tick) UPDATE_FUNC_REGISTRY->unregister(on_tick);
  if (on_draw) UPDATE_FUNC_REGISTRY->unregister(on_draw);
}

int ScreenEffect::f_on_tick_1() {
  // if (!DAT_004b780c) {
    if (field_0x18 != 0) {
      alpha = math::max(0.f, 255.f - time.current_f * 255.f / field_0x18);
    }
    if (time < field_0x18) {
      time++;
      return 1;
    }
  // }
  return 7;
}

int ScreenEffect::f_on_tick_2() {
  // if (DAT_004b780c == 0) {
    time++;
    if (time < field_0x18) {
      float amt = ((float)(field_0x20 - field_0x1c) * time.current_f) /
              (float)field_0x18 + (float)field_0x1c;
      int r = rand() % 3;
      if (r == 0) {
        SUPERVISOR.cameras[1].screen_shake.x = 0.0;
        SUPERVISOR.cameras[3].screen_shake.x = 0.0;
      } else if (r == 1) {
        SUPERVISOR.cameras[1].screen_shake.x = RESOLUTION_MULT * amt;
        SUPERVISOR.cameras[3].screen_shake.x = amt;
      } else if (r == 2) {
        SUPERVISOR.cameras[1].screen_shake.x = RESOLUTION_MULT * amt;
        SUPERVISOR.cameras[3].screen_shake.x = -amt;
      }
      r = rand() % 3;
      if (r == 0) {
        SUPERVISOR.cameras[1].screen_shake.y = 0.0;
        SUPERVISOR.cameras[3].screen_shake.y = 0.0;
      } else if (r == 1) {
        SUPERVISOR.cameras[1].screen_shake.y = RESOLUTION_MULT * amt;
        SUPERVISOR.cameras[3].screen_shake.y = amt;
      } else if (r == 2) {
        SUPERVISOR.cameras[1].screen_shake.y = -RESOLUTION_MULT * amt;
        SUPERVISOR.cameras[3].screen_shake.y = -amt;
      }
      return 1;
    }
  // }
  return 7;
}

int ScreenEffect::f_on_tick_3() {
  // if (!DAT_004b780c) {
    if (field_0x18 != 0) {
      if (time < field_0x18) {
        alpha = math::max(0.f, time.current_f * 255.f / field_0x18);
      } else {
        alpha = 255;
      }
    }
    if (time < field_0x18 + 2) {
      // if (!GAME_THREAD_PTR || !((GAME_THREAD_PTR->flags >> 2 | GAME_THREAD_PTR->flags) & 1)) {
        time++;
      // }
      return 1;
    }
  // }
  return 7;
}

int ScreenEffect::f_on_tick_4() {
  // if (DAT_004b780c) return 7;
  if (time < field_0x18) {
  #define A(col) (col >> 0x18)
    alpha = math::max(A(field_0x20) - A(field_0x20) * time.current_f /
        field_0x18, 0.f);
  #undef A
  } else {
    field_0x1c--;
    alpha = 0;
    if (field_0x1c < 1) return 7;
    time = 0;
  }
  time++;
  return 1;
}


int ScreenEffect::f_on_tick_5() {
  if (field_0x28 == 0) {
    if (field_0x18 != 0 && time <= field_0x18) {
      alpha = time.current_f * 128.0 / field_0x18;
    }
  } else {
    if (8 < time) return 7;
    alpha = 0x80 - time.current_f * 128.0 * 0.125;
  }
  time++;
  return 1;
}

int ScreenEffect::f_on_tick_6() {
  float intensity;
  // if (DAT_004b780c) return 7;
  // if (!GAME_THREAD_PTR) return 1;
  // if ((GAME_THREAD_PTR->flags >> 2 | GAME_THREAD_PTR->flags) & 1) return 1;
  // if (GAME_THREAD_PTR->flags & 0x72) return 1;
  time++;
  if (time < field_0x1c) {
    intensity = time.current_f / static_cast<float>(field_0x1c) * field_0x18;
  } else if (time < field_0x20 + field_0x1c) {
    intensity = field_0x18;
  } else if (field_0x20 + field_0x24 + field_0x1c <= time) {
    return 7;
  } else {
    intensity = (field_0x20 + field_0x1c + field_0x24 - time.current_f) /
        field_0x24 * field_0x18;
  }
  int r = rand() % 3;
  if (r == 0) {
    SUPERVISOR.cameras[1].screen_shake.x = 0.0;
    SUPERVISOR.cameras[3].screen_shake.x = 0.0;
  } else if (r == 1) {
    SUPERVISOR.cameras[1].screen_shake.x = RESOLUTION_MULT * intensity;
    SUPERVISOR.cameras[3].screen_shake.x = intensity;
  } else if (r == 2) {
    SUPERVISOR.cameras[1].screen_shake.x = -RESOLUTION_MULT * intensity;
    SUPERVISOR.cameras[3].screen_shake.x = -intensity;
  }
  r = rand() % 3;
  if (r == 0) {
    SUPERVISOR.cameras[1].screen_shake.y = 0.0;
    SUPERVISOR.cameras[3].screen_shake.y = 0.0;
  } else if (r == 1) {
    SUPERVISOR.cameras[1].screen_shake.y = RESOLUTION_MULT * intensity;
    SUPERVISOR.cameras[3].screen_shake.y = intensity;
  } else if (r == 2) {
    SUPERVISOR.cameras[1].screen_shake.y = -RESOLUTION_MULT * intensity;
    SUPERVISOR.cameras[3].screen_shake.y = -intensity;
  }
  return 1;
}

int ScreenEffect::f_on_tick_7() {
  alpha = 255;
  if (field_0x18 <= time) return 7;
  time++;
  return 1;
}

void draw_rect_col(glm::vec4 const& rect, NSEngine::Color c) {
  AnmManager::flush_vbos();
  //(*(SUPERVISOR.d3d_device)->vtable->SetTextureStageState)(SUPERVISOR.d3d_device,0,D3DTSS_ALPHAOP,2) ;
  //(*(SUPERVISOR.d3d_device)->vtable->SetTextureStageState)(SUPERVISOR.d3d_device,0,D3DTSS_COLOROP,2) ;
  //(*(SUPERVISOR.d3d_device)->vtable->SetTextureStageState)(SUPERVISOR.d3d_device,0,D3DTSS_ALPHAARG1,0);
  //(*(SUPERVISOR.d3d_device)->vtable->SetTextureStageState)(SUPERVISOR.d3d_device,0,D3DTSS_COLORARG1,0);
  //(*(SUPERVISOR.d3d_device)->vtable->SetRenderState)(SUPERVISOR.d3d_device,D3DRS_DESTBLEND,6);
  //(*(SUPERVISOR.d3d_device)->vtable->SetFVF)(SUPERVISOR.d3d_device,0x44);
  //(*(SUPERVISOR.d3d_device)->vtable->DrawPrimitiveUP)(SUPERVISOR.d3d_device,D3DPT_TRIANGLESTRIP,2,vertices,0x14);
  AnmManager::batch->draw(1,
    {{rect.x, rect.y, 0.f}, c, {0.f, 0.f}},
    {{rect.z, rect.y, 0.f}, c, {1.f, 0.f}},
    {{rect.z, rect.w, 0.f}, c, {1.f, 1.f}},
    {{rect.x, rect.w, 0.f}, c, {0.f, 1.f}}, 0);
  AnmManager::flush_vbos();
  AnmManager::field_0x18607c9 = 255;
  AnmManager::field_0x18607ca = 255;
  // ANM_MGR->some_positions[0] = 0.0;
  AnmManager::last_used_texture = -1;
  AnmManager::last_used_blendmode = 10;
  AnmManager::field_0x18607cb = 255;
  //(*(SUPERVISOR.d3d_device)->vtable->SetTextureStageState)(SUPERVISOR.d3d_device,0,D3DTSS_ALPHAOP,4) ;
  //(*(SUPERVISOR.d3d_device)->vtable->SetTextureStageState)(SUPERVISOR.d3d_device,0,D3DTSS_COLOROP,4) ;
  //(*(SUPERVISOR.d3d_device)->vtable->SetTextureStageState)(SUPERVISOR.d3d_device,0,D3DTSS_ALPHAARG1,2);
  //(*(SUPERVISOR.d3d_device)->vtable->SetTextureStageState)(SUPERVISOR.d3d_device,0,D3DTSS_COLORARG1,2);
  return;
}

void draw_rect_col(glm::vec4 const& rect, int a, int rgb) {
  NSEngine::Color c = {
    static_cast<uint8_t>((rgb >> 0x10) & 0xff),
    static_cast<uint8_t>((rgb >> 0x08) & 0xff),
    static_cast<uint8_t>((rgb >> 0x00) & 0xff),
    static_cast<uint8_t>(a & 0xff)
  };
  draw_rect_col(rect, c);
}

int ScreenEffect::f_on_draw_1() {
  draw_rect_col(
    {0, 0, BACK_BUFFER_SIZE.x, BACK_BUFFER_SIZE.y},
    alpha, field_0x1c);
  return 1;
}


int ScreenEffect::f_on_draw_2() {
  draw_rect_col(
    {GAME_REGION_X, GAME_REGION_Y,
     GAME_REGION_WIDTH + GAME_REGION_X,
     GAME_REGION_HEIGHT + GAME_REGION_Y},
    alpha, field_0x20);
  return 1;
}


int ScreenEffect::f_on_draw_3() {
  AnmManager::flush_vbos();
  SUPERVISOR.__unknown_viewport.Width = BACK_BUFFER_SIZE.x;
  SUPERVISOR.__unknown_viewport.Height = BACK_BUFFER_SIZE.y;
  SUPERVISOR.__unknown_viewport.X = 0;
  SUPERVISOR.__unknown_viewport.Y = 0;
  glViewport(0, 0, BACK_BUFFER_SIZE.x, BACK_BUFFER_SIZE.y);
  draw_rect_col(
    {0, 0, BACK_BUFFER_SIZE.x, BACK_BUFFER_SIZE.y},
    alpha, field_0x1c);
  return 1;
}


int ScreenEffect::f_on_draw_4() {
  draw_rect_col(
    {128.0, 16.0, 512.0, 464.0},
    alpha, field_0x1c);
  return 1;
}

int ScreenEffect::f_on_draw_5() {
  draw_rect_col(
    {0, 0, BACK_BUFFER_SIZE.x, BACK_BUFFER_SIZE.y},
    alpha, field_0x1c);
  return 1;
}
