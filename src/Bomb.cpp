#include "./Bomb.hpp"
#include "./AnmOpener/AnmManager.h"
#include "./AsciiManager.hpp"
#include "./BulletManager.h"
#include "./EnemyManager.h"
#include "./GlobalData.h"
#include "./GoastManager.h"
#include "./Gui.hpp"
#include "./Hardcoded.h"
#include "./Laser/LaserManager.h"
#include "./Player.h"
#include "./ScreenEffect.hpp"
#include "./Spellcard.h"
#include "./UpdateFuncRegistry.h"
#include <math/Random.h>

Bomb* BOMB_PTR = nullptr;

bool player_is_trying_to_bomb() {
    return (BOMB_PTR && !BOMB_PTR->active && GOAST_MANAGER_PTR &&
      !(GLOBALS.inner.HYPER_FLAGS & 6) && GUI_PTR &&
     /*!GUI_PTR->msg == NULL &&*/
      ENEMY_MANAGER_PTR && ENEMY_MANAGER_PTR->enemy_count_real);
}

Bomb::Bomb() {
    BOMB_PTR = this;
    on_tick = new UpdateFunc([this](){return this->f_on_tick();});
    UPDATE_FUNC_REGISTRY->register_on_tick(on_tick, 25);
    on_draw = new UpdateFunc([this](){return this->f_on_draw();});
    UPDATE_FUNC_REGISTRY->register_on_draw(on_draw, 41);
    timer_0x34 = 0;
    timer_0x7c = 0;
    field_0x78 = -1;
    field_0x74 = 0;
}

Bomb::~Bomb() {
    AnmManager::deleteVM(anmid_0x5c);
    AnmManager::deleteVM(anmid_0x60);
    AnmManager::deleteVM(anmid_0x64);
    if (on_tick) UPDATE_FUNC_REGISTRY->unregister(on_tick);
    if (on_draw) UPDATE_FUNC_REGISTRY->unregister(on_draw);
    if (ptr_0x70) free(ptr_0x70);
}

int Bomb::f_on_tick() {
    if (timer_0x34 < 0) {
        timer_0x34++;
    }

    if (active) {
        if (f_on_tick_()) {
            active = 0;
            return 1;
        }
        timer_0x34++;
    }
    return 1;
}

int Bomb::f_on_draw() {
    f_on_draw_();
    if (timer_0x7c < 1) {
      field_0x78 = -1.0;
      return 1;
    }
    if (timer_0x7c < 61) {
        if (field_0x78 < 0.0) {
            field_0x78 = field_0x74;
            field_0x94 = PLAYER_PTR->inner.pos;
            field_0x94.y -= 32.0;
        }
        static const NSEngine::Color TEXT_COLORS[] = {
            {0x60, 0x60, 0x60, 0x60},
            {0xa0, 0xb0, 0xb0, 0x80},
            {0xb0, 0xb8, 0xb8, 0x80},
            {0xc0, 0xc0, 0xc0, 0x80},
            {0xd0, 0xd0, 0xd0, 0x80},
            {0xe0, 0xe0, 0xe0, 0x80},
            {0xff, 0xff, 0xff, 0x30},
        };
        ASCII_MANAGER_PTR->color = TEXT_COLORS[field_0x90];
        ASCII_MANAGER_PTR->group = 1;
        ASCII_MANAGER_PTR->font_id = 2;
        ASCII_MANAGER_PTR->alignment_mode_h = 0;
        ASCII_MANAGER_PTR->alignment_mode_v = 2;
        ASCII_MANAGER_PTR->create_string_f(field_0x94, "+%d", (static_cast<int>(field_0x78) / 10) * 10);
        ASCII_MANAGER_PTR->color = c_white;
        ASCII_MANAGER_PTR->font_id = 0;
        ASCII_MANAGER_PTR->group = 0;
        ASCII_MANAGER_PTR->alignment_mode_h = 1;
        ASCII_MANAGER_PTR->alignment_mode_v = 1;
    }
    timer_0x7c++;
    return 1;
}

void Bomb::initialize() {
    if (TOUHOU_VERSION != 17) new BombEmpty();
    if (GLOBALS.inner.CHARACTER == 0) new BombReimu();
    if (GLOBALS.inner.CHARACTER == 1) new BombMarisa();
    if (GLOBALS.inner.CHARACTER == 2) new BombYoumu();
}

void Bomb::start() {
  if (BOMB_PTR->active != 0) return;
  BOMB_PTR->active = 1;
  BOMB_PTR->timer_0x34 = 0;
  GLOBALS.inner.CURRENT_BOMBS--;
  if (0 > GLOBALS.inner.CURRENT_BOMBS) GLOBALS.inner.CURRENT_BOMBS = 0;
  if (8 < GLOBALS.inner.CURRENT_BOMBS) GLOBALS.inner.CURRENT_BOMBS = 8;
  if (GUI_PTR)
      GUI_PTR->set_bomb_meter(GLOBALS.inner.CURRENT_BOMBS, GLOBALS.inner.CURRENT_BOMB_PIECES);
  if (SPELLCARD_PTR->flags & 1) {
    if (0x3b < SPELLCARD_PTR->__timer_20) {
      BOMB_PTR->field_0x68 = 1;
    }
    if (SPELLCARD_PTR->spell_id == 100) {
      BULLET_MANAGER_PTR->cancel_radius(PLAYER_PTR->inner.pos, 0, 192.0);
    }
  }
  // SoundManager::play_sound_at_position(0x2c);
  BOMB_PTR->begin();
  ENEMY_MANAGER_PTR->can_still_capture_spell = 0;
}

void BombReimu::cleanup() {
    reinterpret_cast<Buffer_t*>(ptr_0x70)->explode_all();
    AnmManager::interrupt_tree(anmid_0x64, 1);
    if (ptr_0x70) {
        free(ptr_0x70);
        ptr_0x70 = nullptr;
    }
    new ScreenEffect(1, 8, 6, 6, 0, 0);
    active = 0;
}

int BombReimu::method_10() {
    auto buffer = reinterpret_cast<Buffer_t*>(ptr_0x70);
    for (int i = 0; i < 0x18; i++) {
        if (buffer->orbs[i].field28) {
            if (timer_0x34.current % 8 == (i % 8)) {
                BULLET_MANAGER_PTR->cancel_radius_as_bomb(buffer->orbs[i].pos.pos, 0, 64.0);
                LASER_MANAGER_PTR->cancel_in_radius(buffer->orbs[i].pos.pos, 0, 1, 64.0);
            }
        }
    }
    return 0;
}

void BombReimu::begin() {
    field_0x14 = PLAYER_PTR->inner.pos;
    // SoundManager::play_sound_centered(0x31);
    if (SPELLCARD_PTR->flags & 1) {
        if (SPELLCARD_PTR->__timer_20 >= 0x3c) {
            SPELLCARD_PTR->bonus = 0;
            SPELLCARD_PTR->flags &= 0xffffffdd;
        } else if (BOMB_PTR->active == 1) {
            SPELLCARD_PTR->flags |= 0x20;
        }
    }
    ENEMY_MANAGER_PTR->bomb_count++;
    if (ptr_0x70) {
        free(ptr_0x70);
        ptr_0x70 = nullptr;
    }
    ptr_0x70 = malloc(sizeof(Buffer_t));
    memset(ptr_0x70, 0, sizeof(Buffer_t));
    anmid_0x64 = PLAYER_PTR->playerAnm->createEffectPos(0x25, 0, PLAYER_PTR->inner.pos);
}

void BombReimu::Buffer_t::Orb_t::init(int id, glm::vec3 const& center_pos, int damage) {
    pos.velocity = center_pos;
    anmid = PLAYER_PTR->playerAnm->createEffectPos
        (0x17 - (GLOBALS.inner.SHOTTYPE != 1), 0, pos.pos);
    field28 = 1;
    time_alive = 0;
    this->id = id;
    ds = create_damage_source_3(pos.pos, 9999, damage, 56.0, 0.0);
    PLAYER_PTR->inner.damage_sources[ds - 1].flags |= 4;
    PLAYER_PTR->inner.damage_sources[ds - 1].field_0x80 = 3;
    return;
}

void BombReimu::Buffer_t::Orb_t::explode() {
    if (field35 == 0) {
        if (field28) {
            // SoundManager::play_sound_at_position(0x1b);
            BULLET_MANAGER_PTR->cancel_radius_as_bomb(pos.pos, 0, 128.0);
            LASER_MANAGER_PTR->cancel_in_radius(pos.pos, 1, 1, 128);
            int ds = create_damage_source_3(pos.pos, 0xb, 100, 64.0, 8.0);
            PLAYER_PTR->inner.damage_sources[ds - 1].flags |= 4;
        }
        AnmManager::interrupt_tree(anmid, 1);
        field28 = 0;
        if (ds) {
            PLAYER_PTR->inner.damage_sources[ds - 1].flags &= 0xfe;
        }
        ds = 0;
    } else {
        AnmManager::deleteVM(anmid);
        anmid = 0;
    }
}

void BombReimu::Buffer_t::Orb_t::update() {
    if (time_alive.current != time_alive.previous) {
        if (time_alive < 0x5a) {
            pos.velocity = PLAYER_PTR->inner.pos;
            pos.circle_radius += 1.5;
            pos.angle += field36;
        } else if (time_alive < ((id + 9) * 10)) {
            pos.velocity = PLAYER_PTR->inner.pos;
            pos.angle += field36;
        } else if (time_alive == ((id + 9) * 10)) {
            pos.flags &= 0xfffffff0;
            pos.angle = math::point_direction({0,0}, velocity);
            pos.speed = math::point_distance(0, 0, velocity.x, velocity.y);
        } else {
            closest_enemy_id = ENEMY_MANAGER_PTR->closest_enemy_id(pos.pos);
            if (closest_enemy_id) {
                enm = ENEMY_MANAGER_PTR->EnmFind(closest_enemy_id);
                if (!(enm->enemy.flags & 0xc000021)) {
                    float dir = math::point_direction(pos.pos, enm->enemy.final_pos.pos);
                    float angle_diff = dir - pos.angle;
                    if (angle_diff > PI) {
                        angle_diff -= PI2;
                    } else if (-angle_diff > PI) {
                        angle_diff += PI2;
                    }
                    if (abs(angle_diff) < 0.7853982) {
                        if (pos.speed - 0.7 >= 1.0) {
                            pos.speed -= 0.7;
                        } else {
                            pos.speed = 1.0;
                        }
                    } else if (abs(angle_diff) < 0.2617994) {
                        if (pos.speed + 0.2 <= 8.0) {
                            pos.speed += 0.2;
                        } else {
                            pos.speed = 8.0;
                        }
                    }
                    pos.angle += angle_diff * 0.1;
                    math::angle_normalize(pos.angle);
                    if ((-160.0 > pos.pos.x) || (pos.pos.x > 160.0) ||
                        (32.0 > pos.pos.y) || (pos.pos.y > 416.0))
                        pos.speed *= 0.9;
                }
            }
        }
    }
    glm::vec3 oldpos = pos.pos;
    pos.update_velocities();
    pos.update_position();
    if (auto vm = AnmManager::getVM(anmid); vm) {
        vm->entity_pos = pos.pos;
    }
    velocity = pos.pos - oldpos;
    time_alive++;
}

void BombReimu::Buffer_t::explode_all() {
  for (int i = 0; i < 0x18; i++) {
      orbs[i].explode();
  }
}

int BombReimu::f_on_tick_() {
    PLAYER_PTR->inner.iframes = 40;
    if (auto vm = AnmManager::getVM(anmid_0x64); vm) {
        vm->entity_pos = PLAYER_PTR->inner.pos;
    }
    auto buffer = reinterpret_cast<Buffer_t*>(ptr_0x70);
    if (timer_0x34 > 0x77) {
        int i = 0;
        AnmVM* vm;
        while (vm = AnmManager::getVM(buffer->orbs[i].anmid), !vm) {
            buffer->orbs[i].anmid = 0;
            i++;
            if (i > 0x17) {
                AnmManager::interrupt_tree(anmid_0x64, 1);
                if (ptr_0x70) {
                    free(ptr_0x70);
                    ptr_0x70 = 0;
                }
                return -1;
            }
        }
    }
    if (timer_0x34.current == ((GLOBALS.inner.SHOTTYPE == 1) * 200 + 200)) {
      buffer->explode_all();
      AnmManager::interrupt_tree(anmid_0x64, 1);
      new ScreenEffect(1, 8, 6, 6, 0, 0);
      return 0;
    }
    if (timer_0x34.current != timer_0x34.previous) {
        if (timer_0x34.current == 0) {
            for (int i = 0; i < 8; i++) {
                auto orb = &buffer->orbs[i];
                orb->init(i, PLAYER_PTR->inner.pos, 15);
                orb->pos.flags &= 0xfffffff2;
                orb->pos.flags |= 2;
                orb->pos.circle_radius = 0;
                orb->pos.angle = i + PI1_4;
                orb->pos.circle_radial_speed = PI / 64.f;
                orb->field36 = PI / 30.f;
                PLAYER_PTR->inner.damage_sources[orb->ds - 1].field_0x7c = 400;
            }
        } else if (GLOBALS.inner.SHOTTYPE == 1) {
            if (timer_0x34 == 0x50) {
                // SoundManager::play_sound_at_position(0x2c);
                for (int i = 0; i < 8; i++) {
                    auto orb = &buffer->orbs[i + 8];
                    orb->init(i, PLAYER_PTR->inner.pos, 7);
                    orb->pos.flags &= 0xfffffff2;
                    orb->pos.flags |= 2;
                    orb->pos.circle_radius = 0;
                    orb->pos.angle = i + PI1_4;
                    orb->pos.circle_radial_speed = PI / 64.f;
                    orb->field36 = -PI / 30.f;
                    PLAYER_PTR->inner.damage_sources[orb->ds - 1].field_0x7c = 400;
                }
            } else if (timer_0x34 == 0xa0) {
                // SoundManager::play_sound_at_position(0x2c);
                for (int i = 0; i < 8; i++) {
                    auto orb = &buffer->orbs[i + 16];
                    orb->init(i, PLAYER_PTR->inner.pos, 7);
                    orb->pos.flags &= 0xfffffff2;
                    orb->pos.flags |= 2;
                    orb->pos.circle_radius = 0;
                    orb->pos.angle = i + PI1_4;
                    orb->pos.circle_radial_speed = PI / 64.f;
                    orb->field36 = PI / 30.f;
                    PLAYER_PTR->inner.damage_sources[orb->ds - 1].field_0x7c = 400;
                }
            }
        }
    }
    for (int i = 0; i < 0x18; i++) {
        auto orb = &buffer->orbs[i];
        if (orb->field28) {
            orb->update();
            if (PLAYER_PTR->inner.damage_sources[orb->ds - 1].field_0x78 < 400) {
                PLAYER_PTR->inner.damage_sources[orb->ds - 1].pos.pos = orb->pos.pos;
            } else {
                orb->explode();
                // SoundManager::play_sound_at_position(0x1b);
                new ScreenEffect(1, 8, 6, 6, 0, 0);
            }
        }
    }
    method_10();
    return 0;
}


void BombMarisa::begin() {
    field_0x14 = PLAYER_PTR->inner.pos;
    field_0x2c = -PI1_2;
    // SoundManager::play_sound_centered(0x31);
    anmid_0x5c = PLAYER_PTR->playerAnm->createEffectPos(
        0x16 + (GLOBALS.inner.SHOTTYPE == 1) * 7, 0, field_0x14);
    if (SPELLCARD_PTR->flags & 1) {
        if (SPELLCARD_PTR->__timer_20 < 0x3c) {
            SPELLCARD_PTR->bonus = 0;
            SPELLCARD_PTR->flags &= 0xffffffdd;
        } else if (BOMB_PTR->active == 1) {
            SPELLCARD_PTR->flags |= 0x20;
        }
    }
    PLAYER_PTR->inner.iframes = 120;
    ENEMY_MANAGER_PTR->bomb_count++;
    new ScreenEffect(8, 3, 0x3c,
        GLOBALS.inner.SHOTTYPE != 1 ? 0xf0 : 0x1a4, 0x1e, 0);
    anmid_0x64 = PLAYER_PTR->playerAnm->createEffectPos(0x24, 0, field_0x14);
    PLAYER_PTR->flags |= 4;
}

int BombMarisa::method_10() {
    int i = 0;
    int scr = 28 + (GLOBALS.inner.SHOTTYPE == 1) * 7;
    AnmVM* vm;
    while (vm = AnmManager::getVM(anmid_0x5c), vm) {
        vm = vm->search_children(scr, i);
        if (!vm) return 0;
        glm::vec2 scale = vm->scale * glm::vec2(48.0, 160.0);
        glm::vec3 pos = vm->get_pos_with_root();
        BULLET_MANAGER_PTR->cancel_rectangle_as_bomb(field_0x2c, pos, scale, 0);
        LASER_MANAGER_PTR->cancel_in_rectangle(pos, scale, field_0x2c);
        i++;
    }
    anmid_0x5c = 0;
    return 0;
}

int BombMarisa::f_on_tick_() {
    PLAYER_PTR->inner.iframes = 40;
    auto vm = AnmManager::getVM(anmid_0x5c);
    if (vm == NULL) {
        anmid_0x5c = 0;
        AnmManager::interrupt_tree(anmid_0x64, 1);
        return -1;
    }
    int time = (300 + (GLOBALS.inner.SHOTTYPE == 1) * 150);
    if (timer_0x34 > time) return 0;
    if (timer_0x34 == time) {
        AnmManager::interrupt_tree(anmid_0x5c, 1);
        AnmManager::interrupt_tree(anmid_0x64, 1);
        PLAYER_PTR->flags &= 0xfffffffb;
        PLAYER_PTR->speed_multiplier = 1.0;
    }
    vm->bitflags.rotated = true;
    vm->rotation.z = field_0x2c;
    if (PLAYER_PTR->last_movement.x < 0) {
        field_0x2c -= 0.002617994;
    } else if (PLAYER_PTR->last_movement.x > 0) {
        field_0x2c += 0.002617994;
    }
    PLAYER_PTR->speed_multiplier = 0.5;
    field_0x14 = PLAYER_PTR->inner.pos;
    if (timer_0x34.current != timer_0x34.previous &&
          (timer_0x34.current % 3) == 0) {
        glm::vec3 pos = math::lengthdir_vec3(208, field_0x2c) + field_0x14;
        int ds = create_damage_source(pos, field_0x2c, 0, 50, 512.0, 32.0);
        PLAYER_PTR->inner.damage_sources[ds - 1].flags |= 4;
        pos = math::lengthdir_vec3(240, field_0x2c) + field_0x14;
        ds = create_damage_source(pos, field_0x2c, 0, 15, 512.0, 128.0);
        PLAYER_PTR->inner.damage_sources[ds - 1].flags |= 4;
        pos = math::lengthdir_vec3(304, field_0x2c) + field_0x14;
        int damage = 15 + (GLOBALS.inner.SHOTTYPE == 1) * 25;
        ds = create_damage_source(pos, field_0x2c, 0, damage, 512.0, 256.0);
        PLAYER_PTR->inner.damage_sources[ds - 1].flags |= 4;
        if (GLOBALS.inner.SHOTTYPE == 1) {
            PLAYER_PTR->inner.damage_sources[ds - 1].ds_on_hit = 3;
        }
    }
    if (auto vm = AnmManager::getVM(anmid_0x5c); vm) {
        vm->entity_pos = field_0x14;
    }
    if (auto vm = AnmManager::getVM(anmid_0x64); vm) {
        vm->entity_pos = field_0x14;
    }
    method_10();
    return 0;
}

void BombYoumu::begin() {
    field_0x14 = PLAYER_PTR->inner.pos;
    // SoundManager::play_sound_centered(0x31);
    if (SPELLCARD_PTR->flags & 1) {
        if (SPELLCARD_PTR->__timer_20 < 0x3c) {
            SPELLCARD_PTR->bonus = 0;
            SPELLCARD_PTR->flags &= 0xffffffdd;
        } else if (BOMB_PTR->active == 1) {
            SPELLCARD_PTR->flags |= 0x20;
        }
    }
    PLAYER_PTR->inner.iframes = 120;
    ENEMY_MANAGER_PTR->bomb_count++;
    new ScreenEffect(8, 1, 6,
        GLOBALS.inner.SHOTTYPE != 1 ? 0xe6 : 0x140, 0x1e, 0);
    AnmVM* vm;
    anmid_0x64 = PLAYER_PTR->playerAnm->createEffectPos(0x1e,
                                          0, field_0x14, -1, &vm);
    vm->color_1.a = 0xe0;
    vm->alpha2_i.bezier_2 = 0xe0;
    field2080 = 0;
    field2084 = 15 + (GLOBALS.inner.SHOTTYPE != 1) * 10;
    timer2088 = 0;
    timer_0x34 = 0;
}

int BombYoumu::f_on_tick_() {
    PLAYER_PTR->inner.iframes = 40;
    if (0x104 + 0x5a * (GLOBALS.inner.SHOTTYPE == 1) < timer_0x34) {
        AnmManager::interrupt_tree(anmid_0x64, 1);
        return -1;
    }
    static const float FLOATS_004a0434[] = {
        224.0, 192.0, 320.0,
        224.0, 64.0, 80.0,
        384.0, 320.0, 96.0,
        128.0, 224.0,
    };
    if ((timer_0x34.current != timer_0x34.previous) &&
       timer2088.current < 1) {
        glm::vec3 pos = {};
        pos.y = FLOATS_004a0434[field2080 % 0xb] + Random::Floatm11() * 48.0;
        floats2[field2080] = pos.y;
        int scr;
        if (field2080 % 2 == 0) {
            floats[field2080] = -192.0;
            scr = (GLOBALS.inner.SHOTTYPE == 1) * 2 + 0x16;
        } else {
            floats[field2080] = 192.0;
            scr = (GLOBALS.inner.SHOTTYPE == 1) * 2 + 0x17;
        }
        anmids[field2080] =
            PLAYER_PTR->playerAnm->createEffectPos(scr, 0, pos);
        timers[field2080] = 0;
        field2080++;
        if (((GLOBALS.inner.SHOTTYPE != 1) * 2 + 6) < field2084) {
            field2084 -= 2;
        }
        timer2088 = field2084;
        // SoundManager::play_sound_centered(0x2c);
        // SoundManager::play_sound_centered(0x1b);
    }
    timer2088--;
    for (int i = 0; i < 0x104; i++) {
        auto vm = AnmManager::getVM(anmids[i]);
        if (vm == NULL) {
            anmids[i] = 0;
        } else {
            glm::vec3 pos = {0, floats2[i], 0};
            glm::vec3 addpos =
                math::lengthdir_vec3(256, vm->rotation.z + PI1_2);
            if (floats[i] < 0) {
                pos -= addpos;
            } else {
                pos += addpos;
            }
            glm::vec2 hbsize = {96, timers[i].current_f * 128.0};
            int ds = create_damage_source(pos,
                  vm->rotation.z, 0, 0x1e, hbsize.x, hbsize.y);
            PLAYER_PTR->inner.damage_sources[ds - 1].flags |= 4;
            BULLET_MANAGER_PTR->cancel_rectangle_as_bomb(vm->rotation.z,
                                                         pos, hbsize, 0);
            LASER_MANAGER_PTR->cancel_in_rectangle(pos, hbsize, vm->rotation.z);
            timers[i]++;
        }
    }
    method_10();
    return 0;
}
