#include "Bullet.h"
#include "GlobalData.h"
#include "math/Random.h"
#include "BulletManager.h"
#include "Laser/LaserManager.h"
#include "Hardcoded.h"

float get_angle_to_player(glm::vec3 from)
{
    return math::point_direction(from.x, from.y, Globals::get()->playerX, Globals::get()->playerY);
}

glm::vec3 cartesian3_from_polar(float a, float r)
{
    return {r * cos(a), r * sin(a), 0.f};
}

int Bullet::on_tick()
{

                    /* Update timer (e68) */
  __timer_e68++;
                    /* delete if flag is set */
  if ((flags & 8) != 0) { _delete(); return -1; }

                    /* update hitbox size if the right et_ex is active */
                    /* seems like some flag tells if the hitbox has been resized */
  if ((active_ex_flags & 0x400000) != 0) {
    scale = scale_i.step();
    if (scale_i.end_time == 0) {
      active_ex_flags &= 0xffbfffff;
      if (scale == 1.0) flags &= 0xffffffbf;
    }
  }

  int ended_ex;
  float game_speed = 1.0f;
              /* seems like state 2 is spawn anim */
  if (state == 2) {
    pos += velocity * game_speed * 0.5f;
    if((__timer_e54 < 8 || true/*(check_player_collision(this, 0) != 1)*/) && (vm.int_vars[0] != 0))
      state = 1;
  }

  if (state == 3) { // just move ?? when is state3 activated ?
    pos += velocity * game_speed * 0.5f;
  }

                    /* state = 1 : normal ? */
  if (state == 1) {
    do {
      if ((active_ex_flags & 0x4000000) == 0) {
        run_et_ex();
      }
      if (active_ex_flags == 0) break;
      ended_ex = 0;
      if ((active_ex_flags & 0x00000001) != 0) {
        if ((ex_speedup).timer >= 0x11) {
          active_ex_flags &= 0xfffffffe;
          ended_ex++;
        }
        else {
          velocity = cartesian3_from_polar(angle,5.0 - ex_speedup.timer * 5.0 * 0.0625 + speed);
          ex_speedup.timer++;
        }
      }
      if ((active_ex_flags & 0x00000004) != 0) {
        if (ex_accel.timer >= ex_accel.duration) {
          active_ex_flags &= 0xfffffffb;
          ended_ex++;
        }
        else {
          speed += ex_accel.acceleration * game_speed;
          velocity += ex_accel.vec3_a14 * game_speed;
          if (fabs(velocity.x) > 0.0001 || fabs(velocity.y) > 0.0001) {
            speed = math::point_distance(0, 0, velocity.x, velocity.y);
            angle = math::point_direction(0, 0, velocity.x, velocity.y);
            math::angle_normalize(angle);
          }
          ex_accel.timer++;
        }
      }
      if ((active_ex_flags & 0x00000008) != 0) {
        if (ex_angleaccel.timer >= ex_angleaccel.duration) {
          active_ex_flags = active_ex_flags & 0xfffffff7;
          ended_ex++;
        }
        else {
          speed += ex_angleaccel.tangential_accel * game_speed;
          angle += ex_angleaccel.angular_velocity * game_speed;
          math::angle_normalize(angle);
          velocity = cartesian3_from_polar(angle, speed);
          ex_angleaccel.timer++;
        }
      }
      if ((active_ex_flags & 0x00000010) != 0) {
        float spd;
        if ((ex_angle).timer < ex_angle.duration) {
            spd = speed - (ex_angle.timer * speed) / (float)ex_angle.duration;
        }
        else {
            if (-1 < transform_sound) {} //SoundManager::play_sound_centered(transform_sound);
            ex_angle.turns_so_far++;
            switch((ex_angle).aim_type) {
            case 0:
            case 5:
                angle += ex_angle.angle;
                break;
            case 1:
            case 6:
                angle = get_angle_to_player(pos) + (ex_angle).angle;
                break;
            case 2:
            case 3:
            case 4:
                angle = (ex_angle).angle;
            }
            math::angle_normalize(angle);
            speed = ex_angle.speed;
            ex_angle.timer = 0;
            if (ex_angle.turns_so_far >= ex_angle.max_turns) {
                velocity = cartesian3_from_polar(angle, speed);
                active_ex_flags = active_ex_flags & 0xffffffef;
                ended_ex++;
            }
            spd = speed;
        }
        velocity = cartesian3_from_polar(angle,spd);
        ex_angle.timer++;
      }
      if ((active_ex_flags & 0x00000040) != 0) {
        bool bounced = false;
        float rect_h = BulletManager::GetInstance()->et_bounce_rect_h <= 0 ? ex_bounce.dim_y : BulletManager::GetInstance()->et_bounce_rect_h;
        float rect_w = BulletManager::GetInstance()->et_bounce_rect_w <= 0 ? ex_bounce.dim_x : BulletManager::GetInstance()->et_bounce_rect_w;

        if (!((pos.x >         rect_w * -0.5) && (pos.x < rect_w * 0.5        ) &&
            (pos.y > 224.0 - rect_h *  0.5) && (pos.y < rect_h * 0.5 + 224.0))) {
          if ((ex_bounce.wall_flags & 1) != 0) {
              if ((pos.y < 224.0 - rect_h * 0.5) && ((ex_bounce.wall_flags & 0x10) == 0)) {
              angle = -angle;
              math::angle_normalize(angle);
              pos.y = (448.0 - rect_h) - pos.y;
              bounced = true;
              }
          }
          if ((ex_bounce.wall_flags & 2) != 0) {
              if ((rect_h * 0.5 + 224.0 <= pos.y) && ((ex_bounce.wall_flags & 0x10) == 0)) {
              angle = -angle;
              math::angle_normalize(angle);
              pos.y = (448.0 + rect_h) - pos.y;
              bounced = true;
              }
          }
          if ((ex_bounce.wall_flags & 8) != 0) {
              if ((rect_w * 0.5 <= pos.x) && ((ex_bounce.wall_flags & 0x10) == 0)) {
              angle = -angle - PI;
              math::angle_normalize(angle);
              pos.x = rect_w - pos.x;
              bounced = true;
              }
          }
          if ((ex_bounce.wall_flags & 4) != 0) {
              if ((pos.x < rect_w * -0.5 && ((ex_bounce.wall_flags & 0x10) == 0))) {
              angle = -angle - PI;
              math::angle_normalize(angle);
              pos.x = -rect_w - pos.x;
              bounced = true;
              }
          }
          speed = ex_bounce.bounce_speed > -990.0 ? ex_bounce.bounce_speed : speed;
          velocity = cartesian3_from_polar(angle, speed);
          if (bounced) {
              ex_bounce.rebounds_so_far++;
              if (-1 < transform_sound) {}//SoundManager::play_sound_centered(transform_sound);
          }
          if ((ex_bounce).rebounds_so_far >= ex_bounce.max_rebounds) {
              active_ex_flags = active_ex_flags & 0xffffffbf;
              ended_ex++;
          }
        }
      }
      if ((active_ex_flags & 0x00000100) != 0) {
         ex_offscreen.timer--;
         if (0 >= ex_offscreen.timer) {
            active_ex_flags = active_ex_flags ^ 0x100;
            ended_ex++;
         }
      }
      if ((active_ex_flags & 0x00020000) != 0) {
        if (ex_move.__a <= ex_move.timer) {
            pos = ex_move.__rs__target_pos;
            speed = ex_move.__field_bf8__had_4;
            velocity = cartesian3_from_polar(angle, speed);
            active_ex_flags = active_ex_flags & 0xfffdffff;
            ended_ex++;
        }
        else {
          if (ex_move.timer == 0) pos_i.initial = pos_i.current;
          pos_i.step();
          velocity = pos_i.current - pos; velocity.z = 0.0;
          if ((0.0001 < fabs(velocity.x)) || (0.0001 < fabs(velocity.y))) {
            speed = math::point_distance(0, 0, velocity.x, velocity.y);
            angle = math::point_direction(0, 0, velocity.x, velocity.y);
            math::angle_normalize(angle);
          }
          ex_move.timer++;
        }
      }
      if ((active_ex_flags & 0x00080000) != 0) {
        if (ex_veladd.timer < ex_veladd.duration) {
          pos += ex_veladd.vel * game_speed;
          ex_veladd.timer = 0; //wierd...
        }
        else {
          active_ex_flags &= 0xfffffff6;
          ended_ex++;
        }
      }
      if ((active_ex_flags & 0x00200000) != 0) {
        if (ex_veltime.timer >= ex_veltime.duration) {
          active_ex_flags = active_ex_flags & 0xffdfffff;
          ended_ex++;
        }
        else {
          speed += ex_veltime.acceleration * game_speed;
          velocity += ex_veltime.acceleration_vector * game_speed;
          if (fabs(velocity.x) > 0.0001 || fabs(velocity.y) > 0.0001) {
            angle = math::point_direction(0, 0, velocity.x, velocity.y);
            math::angle_normalize(angle);
          }
          ex_veltime.timer++;
        }
      }
      if ((active_ex_flags & 0x04000000) != 0) {
        if (ex_delay.timer < 1) {
          active_ex_flags = active_ex_flags &= 0xfbffffff;
          flags &= 0xfffffdff;
          ended_ex += 1;
        }
        else {
          flags |= 0x200;
          ex_delay.timer--;
        }
      }
      if ((active_ex_flags & 0x40000000) != 0) {
        if (ex_homing.duration <= ex_homing.timer) {
          active_ex_flags = active_ex_flags & 0xbfffffff;
          ended_ex++;
        }
        else {
          float ang = ex_homing.angule + get_angle_to_player(pos);
          math::angle_normalize(ang);
          glm::vec3 velAdd = cartesian3_from_polar(ang, ex_homing.norm);
          velocity.x += (velAdd.x - (velocity).x) * (ex_homing).__m;
          velocity.y += (velAdd.y - (velocity).y) * (ex_homing).__m;
          velocity.z = 0.0;
          speed = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
          angle = math::point_direction(0, 0, velocity.x, velocity.y);
          math::angle_normalize(angle);
          ex_homing.timer++;
        }
      }
      if ((active_ex_flags & 0x80000000) != 0) {
        if (ex_wait.timer < 1) {
          active_ex_flags &= 0x7fffffff;
          ended_ex += 1;
        }
        else ex_wait.timer--;
      }

      if (ex_invuln__remaining_frames != 0) {
        ex_invuln__remaining_frames = ex_invuln__remaining_frames + -1;
      }
    } while (ended_ex != 0);

    if ((flags & 0x200) == 0) {
      pos += velocity * game_speed;
      //check_player_collision(0);
    }
  }

  // get sprite
  auto spr = vm.getSprite();

  if ((active_ex_flags & 0x1000) != 0 &&
      !((-192.0 < pos.x + spr.w * 0.5) && (pos.x - spr.w * 0.5 < 192.0) &&
            (0.0 < pos.y + spr.h * 0.5) && (pos.y - spr.h * 0.5 < 448.0))) {

    bool wraped = true;
    if      (((ex_wrap.wall_flags & 1) != 0) && (pos.y <    0.0)) pos.y += spr.h + 448.0;
    else if (((ex_wrap.wall_flags & 2) != 0) && (pos.y >  448.0)) pos.y -= spr.h + 448.0;
    else if (((ex_wrap.wall_flags & 4) != 0) && (pos.x < -192.0)) pos.x += spr.w + 384.0;
    else if (((ex_wrap.wall_flags & 8) != 0) && (pos.x >  192.0)) pos.x -= spr.w + 384.0;
    else wraped = false;

    if (wraped) {
      ex_wrap.wraps_count++;
      //if (transform_sound >= 0) SoundManager::play_sound_centered(transform_sound);
      if (ex_wrap.wraps_count >= ex_wrap.num_wraps) active_ex_flags = active_ex_flags ^ 0x1000;
    }
  }

  if (((active_ex_flags & 0x100) == 0) && (__field_664_had_5 < 1))
    if ((pos.x + spr.w * scale * 0.5 <= -192.0) || (pos.x - spr.w * scale * 0.5 >= 192.0) ||
        (pos.y + spr.h * scale * 0.5 <=  -64.0) || (pos.y - spr.h * scale * 0.5 >= 448.0)) {
      _delete();
      return -1;
    }


  if (ex_invuln__remaining_frames != 0) {
    ex_invuln__remaining_frames--;
  }

  if (__field_664_had_5 > 0) {
    __field_664_had_5--;
  }

  if ((flags & 0x200) != 0) return 0;
  if (vm.update() == 1) {
    _delete();
    return -1;
  }
  return 0;
}


int Bullet::run_et_ex()
{
  ///zLaserBaseClass *pzVar5;
  ///bool bVar6;
  ///zLaserManager *pzVar8;
  ///undefined auVar9 [16];
  ///undefined4 *puVar11;
  ///zLaserBaseClass *pzVar12;
  ///int iVar13;
  ///int iVar14;
  ///int32_t iVar15;
  ///float *pfVar17;
  ///undefined in_XMM0 [16];
  ///undefined auVar20 [16];
  ///float fVar21;
///
  ///undefined8 local_a60;
  ///undefined local_a54 [16];
  ///undefined local_a44 [12];
  ///undefined4 uStack2616;
  ///float local_a34;
  ///undefined local_a30 [16];
  ///undefined local_a20 [16];
  ///uint local_a10;
  ///int32_t local_a0c;
  ///int32_t local_a08;
  ///uint local_a04;
  ///float local_a00 [198];
  ///undefined8 local_6e8;
  ///float local_6e0;
  ///undefined local_6dc [8];
  ///undefined8 local_6cc;
  ///int32_t local_6c4;
  ///int32_t local_6c0;
  ///undefined8 local_6bc;
  ///uint local_6b4;
  ///float local_6b0 [198];
  ///undefined8 local_398;

  uint cur_ex_id = ex_index;
  while (cur_ex_id < 20) {
    cur_ex_id = ex_index;
    EtEx_t *et_ex = this->et_ex + cur_ex_id;
    uint ex_type = et_ex->type;

    if ((ex_type == 0) || (((et_ex->slot == 0 && ((active_ex_flags & 0xfffffeff) != 0)) || (ex_type & active_ex_flags) != 0))) break;

        /* Speedup */
    if (ex_type == 1) {
      active_ex_flags |= 1;
      ex_speedup.timer = 0;
      ex_speedup.unused_0 = 0.f;
    }

    if (ex_type == 2) {
      vm.interrupt(et_ex->a + 7);
      state = 2;
      pos -= velocity * 4.f;
    }

    if (ex_type == 4) {
      active_ex_flags |= 4;
      ex_accel.acceleration = et_ex->r;
      ex_accel.duration = et_ex->a;
      ex_accel.timer = 0;

      float dir = et_ex->s;
      if (-999990.0 < dir) {
        if (999990.0 <= dir) {
          dir = et_ex->m + get_angle_to_player(pos);
        }
        math::angle_normalize(dir);
      }
      else dir = angle;
      ex_accel.angle = dir;
      ex_accel.vec3_a14 = cartesian3_from_polar(ex_accel.angle, ex_accel.acceleration);
      if ((ex_index != 0) && (-1 < transform_sound)) {}//SoundManager::play_sound_centered(transform_sound);
    }

    if (ex_type == 8) {
      active_ex_flags |= 8;
      ex_angleaccel.tangential_accel = et_ex->r;
      ex_angleaccel.angular_velocity = et_ex->s;
      ex_angleaccel.timer = 0;
      ex_angleaccel.duration = et_ex->a;
      if ((ex_index != 0) && (-1 < transform_sound)) {} //SoundManager::play_sound_centered(transform_sound);
    }

    if (ex_type == 0x10) {
      active_ex_flags |= 0x10;
      ex_angle.speed = et_ex->s <= -999990.f ? speed : et_ex->s;
float ang = et_ex->r;
      if (-999990.0 < ang) {
        if (999990.0 <= ang) {
          ang = et_ex->m + get_angle_to_player(pos);
        }
        math::angle_normalize(ang);
      }
      else ang = angle;

      switch(et_ex->c) {
      case 2:
        ang += get_angle_to_player(pos);
        math::angle_normalize(ang);
      case 0:
      case 1:
      case 4:
        ex_angle.angle = ang;
        break;
      case 3:
        ang += ex_save.saved_angle;
        math::angle_normalize(ang);
        ex_angle.angle = ang;
        break;
      case 5:
      case 6:
        ex_angle.angle = Random::Floatm11() * et_ex->r;
        break;
      case 7:
        ang = et_ex->r;
        if (-999990.0 < ang) {
          if (ang >= 990.0) ang = get_angle_to_player(pos);
          math::angle_normalize(ang);
        }
        else ang = angle;
        ex_angle.angle = ang;
        ex_angle.speed += Random::Floatm11() * et_ex->s;
      }
      ex_angle.timer = 0;
      ex_angle.duration = et_ex->a;
      ex_angle.max_turns = et_ex->b;
      ex_angle.turns_so_far = 0;
      ex_angle.aim_type = et_ex->c;
      ex_angle.__d = et_ex->d;
    }

    if (ex_type == 0x40) {
      active_ex_flags |= 0x40;
      ex_bounce.bounce_speed = et_ex->r;
      ex_bounce.max_rebounds = et_ex->a;
      ex_bounce.rebounds_so_far = 0;
      ex_bounce.wall_flags = et_ex->b;
      if ((et_ex->b & 0x20) == 0) {
        ex_bounce.dim_x = 384.0;
        ex_bounce.dim_y = 448.0;
      }
      else {
        ex_bounce.dim_x = et_ex->s;
        ex_bounce.dim_y = et_ex->m;
      }
    }

    if (ex_type == 0x80) {
      ex_invuln__remaining_frames = et_ex->a;
    }

    if (ex_type == 0x100) {
      active_ex_flags |= 0x100;
      ex_offscreen.timer = et_ex->a;
      ex_offscreen.__b__unknown = et_ex->b;
    }

    if (ex_type == 0x200) {
      type = et_ex->a;
      color = et_ex->b & 0x7fff;
      sprite_data = BULLET_TYPE_TABLE[et_ex->a];
      __hitbox_diameter_copy = sprite_data["default_radius"].asFloat();
      hitbox_diameter = __hitbox_diameter_copy;
      layer = sprite_data["default_layer"].asInt();
      flags = flags | 0x10;
      vm(AnmManagerN::getLoaded(7)->getPreloaded(sprite_data["script"].asInt()));
      vm.setEntity((void*)this);
      vm.setLayer(15);
      vm.on_set_sprite = [](AnmVM* vm, int spr) {
          auto b = (Bullet*) vm->getEntity();
          if (b->sprite_data["colors"][0]["main_sprite_id"].asInt() < 0) return spr;
          if (spr == 0) return b->sprite_data["colors"][b->color]["main_sprite_id"].asInt();
          if (spr == 1) return b->sprite_data["colors"][b->color]["spawn_sprite_id"].asInt();
          if (spr == 2) return b->sprite_data["colors"][b->color]["cancel_sprite_id"].asInt();
          return spr;
      };
      vm.refreshSprite();
      vm.bitflags.originMode = 0b01;

      //zAnmVM__delete_by_id(anmExtraId);
      // Spawn anim wierd and cancel id
      //anmExtraId = 0;
      //if (sprite_data["__field_114"].asInt() != 0) {
        //zAnmId* id = zAnmLoaded__create_4112b0
                            //(BULLET_MANAGER_PTR->bullet_anm,nullptr,
                             //sprite_data["__field_114"].asInt(),
                             //&pos,0,-1,nullptr);
        //anmExtraId = id->value;
      //}
      switch(sprite_data["__field_10c"].asInt()) {
      case 0:
        cancel_sprite_id = color * 2 + 4;
        break;
      case 1:
        cancel_sprite_id = -1;//BULLET_ADDITIONAL_CANCEL_SCR[color];
        break;
      case 2:
        cancel_sprite_id = -1;
        break;
      case 3:
        cancel_sprite_id = 0x10;
        break;
      case 4:
        cancel_sprite_id = 6;
        break;
      case 5:
        cancel_sprite_id = 0xc;
        break;
      case 6:
        cancel_sprite_id = sprite_data["colors"][color]["cancel_script"].asInt();
        break;
      case 7:
        cancel_sprite_id = 0x104;
        break;
      case 8:
        cancel_sprite_id = 0x107;
        break;
      case 9:
        cancel_sprite_id = 0x10a;
        break;
      case 10:
        cancel_sprite_id = 0x113;
      }
      if ((et_ex->b & 0x8000U) != 0) vm.interrupt(2);
    }

    if (ex_type == 0x400) {
      if (et_ex->a == 1) cancel_sprite_id = -1;
      cancel(0);
      //BulletManager::GetInstance()->RemoveBullet(this);
      return -1;
    }

    if (ex_type == 0x800) {
      //SoundManager::play_sound_at_position(et_ex->a);
    }

        /* Wrap */
    if (ex_type == 0x1000) {
      active_ex_flags |= 0x1000;
      ex_wrap.num_wraps = et_ex->a;
      ex_wrap.wraps_count = 0;
      ex_wrap.wall_flags = et_ex->b;
    }

    if (ex_type == 0x2000) { // automatically checks for 0x4000
      EnemyBulletShooter_t bullet_shooter = {};
      //memset(&bullet_shooter,0,0x380);
      bullet_shooter.__vec3_8 = pos;
      bullet_shooter.aim_type = et_ex->a;
      bullet_shooter.__start_transform = et_ex->b;
      bullet_shooter.cnt_count = et_ex->c;
      bullet_shooter.cnt_layers = et_ex->d;
      bullet_shooter.__shot_transform_sfx = -1;
      bullet_shooter.ang_aim = (et_ex->r >= 999990.0) ? get_angle_to_player(pos) : ((et_ex->r <= -999990.0) ? angle : et_ex->r);
      bullet_shooter.ang_bullet_dist = et_ex->s;
      bullet_shooter.spd1 = et_ex->m > -999990.0 ? et_ex->m : speed;
      bullet_shooter.spd2 = et_ex->n;
      math::angle_normalize(bullet_shooter.ang_aim);

      // next et ex should be 0x4000
      ex_index = cur_ex_id + 1;
      et_ex = this->et_ex + ex_index;

      bullet_shooter.type = et_ex->a;
      bullet_shooter.__color = et_ex->b;
      bullet_shooter.sfx_flag = 0;

      for (int i = 0; i < 20; i++) bullet_shooter.ex[i] = this->et_ex[i];

      BulletManager::GetInstance()->Shoot(&bullet_shooter);

      if (et_ex->c != 0) {
        cancel(0);
        return -1;
        //BulletManager::GetInstance()->RemoveBullet(this);
      }
    }

    if (ex_type == 0x8000) {
      __ex_react_a = et_ex->a;
    }

           /* Jump TODO: check */
    if (ex_type == 0x10000) {
      if (et_ex->b > 0) {
        if (__ex_goto_b_loop_count == 0) {
          __ex_goto_b_loop_count = et_ex->b;
          ex_index = et_ex->a;
          continue;
        }
        if (__ex_goto_b_loop_count == 1) {
          __ex_goto_b_loop_count = 0;
          ex_index = ex_index + 1;
          continue;
        }
        __ex_goto_b_loop_count = __ex_goto_b_loop_count + -1;
      }
      ex_index = et_ex->a;
      continue;
    }

            /* move
                use interpolator */
    if (ex_type == 0x20000) {
      active_ex_flags |= 0x20000;
      ex_move.__field_bf8__had_4 = speed;
      ex_move.__a = et_ex->a;
      ex_move.__b__mode = et_ex->b;
      ex_move.timer = 0;
      ex_move.__rs__target_pos.x = et_ex->r;
      ex_move.__rs__target_pos.y = et_ex->s;
      if ((et_ex->b & 0x100U) != 0) ex_move.__rs__target_pos += glm::vec3(pos.x, pos.y, 0.f);
      ex_move.__rs__target_pos.z = 0.f;
      pos_i.start(pos, ex_move.__rs__target_pos, et_ex->a, et_ex->b);
    }

          /* set vel */
    if (ex_type == 0x40000) {
      if (et_ex->r >= 990.f) angle = (et_ex->r - 999.0) + get_angle_to_player(pos);
      else if (et_ex->r > -990.f) angle = et_ex->r;
      math::angle_normalize(angle);
      if (et_ex->s >= -990.0) speed = et_ex->s;
      velocity = cartesian3_from_polar(angle, speed);
    }

          /* vel add */
    if (ex_type == 0x80000) {
      active_ex_flags |= 0x80000;
      ex_veladd.vel = cartesian3_from_polar(et_ex->r,et_ex->s);
      ex_veladd.angle = et_ex->r;
      ex_veladd.magnitude = et_ex->s;
      ex_veladd.duration = et_ex->a;
      ex_veladd.timer = 0;
    }

              /* brightness effect
                  Changes the blendmode of the anm */
    if (ex_type == 0x100000) {
      if (et_ex->a == 2) vm.bitflags.blendmode = 2;
      else if (et_ex->a == 1) vm.bitflags.blendmode = 1;
      else vm.bitflags.blendmode = 0;
    }

    if (ex_type == 0x200000) {
      active_ex_flags |= 0x200000;
      ex_veltime.acceleration = (et_ex->r - speed) / (float)et_ex->a;
      ex_veltime.field2_0x18 = et_ex->s;
      if (et_ex->s >= 999990.f) ex_veltime.field2_0x18 = et_ex->m + get_angle_to_player(pos);
      else if (et_ex->s <= -999990.f) ex_veltime.field2_0x18 = angle;
      math::angle_normalize(ex_veltime.field2_0x18);
      ex_veltime.timer = 0;
      ex_veltime.duration = et_ex->a;
      ex_veltime.acceleration_vector = cartesian3_from_polar(ex_veltime.field2_0x18, ex_veltime.acceleration);
      if ((ex_index != 0) && (-1 < transform_sound)) {}//SoundManager::play_sound_centered(transform_sound);
    }

            /* Scale time */
    if (ex_type == 0x400000) {
      active_ex_flags |= 0x400000;
      scale_i.start(et_ex->r, et_ex->s, et_ex->a, et_ex->b);
      flags |= 0x40;
    }

          /* Save bullet pos angle and speed */
    if (ex_type == 0x800000) {
      ex_save.saved_pos = pos;
      ex_save.saved_angle = angle;
      ex_save.saved_speed = speed;
    }

         /* spawn enemy TODO */
    if (ex_type == 0x1000000) {
      //zEnemySpawnData spawn;
      //spawn.pos = pos;
      //spawn.score_reward = 0;
      //spawn.main_drop = 0;
      //spawn.life = 10000;
      //spawn.__in_flag_18 = 0;
      //spawn.__in_flag_1c = 0;
      //spawn.ecl_vars_int[0] = et_ex->a;
      //spawn.ecl_vars_int[1] = et_ex->b;
      //spawn.ecl_vars_int[2] = et_ex->c;
      //spawn.ecl_vars_int[3] = et_ex->d;
      //for (int i = 0; i < 8; i++) spawn.ecl_vars_float[i] = 0.f;
      //spawn.parent_enemy_id = 0;
      //zEnemyManager__allocate_new_enemy(ENEMY_MANAGER_PTR, et_ex->string, &spawn);
    }

        /* Set layer */
    if (ex_type == 0x2000000) {
      layer = et_ex->a;
    }

        /* Delay */
    if (ex_type == 0x4000000) {
      if (et_ex->a > 0) {
        active_ex_flags |= 0x4000000;
        ex_delay.timer = et_ex->a;
      }
    }

        /* Shoot laser TODO */
    if (ex_type == 0x8000000) {
        if (et_ex->a == 1) {
            LaserInfiniteInner_t inner;
            for (int i = 0; i < 20; i++) inner.ex[i] = this->et_ex[i];
            inner.spd_1 = 8.0;
            inner.start_pos = pos;
            inner.flags = et_ex->d & 0xfd | 2;
            inner.type = et_ex->b;
            inner.color = et_ex->c;
            inner.et_ex_index = et_ex->d >> 8 & 0xff;
            inner.ang_aim = et_ex->r;
            inner.spd_1 = et_ex->s;
            if (et_ex->r <= -999990.0) inner.ang_aim = angle;
            if (et_ex->r > 999990.0) inner.ang_aim = math::point_direction(pos.x, pos.y, Globals::get()->playerX, Globals::get()->playerY);
            if (inner.spd_1 <= -999990.0) inner.spd_1 = speed;
            inner.laser_new_arg1 = et_ex->m;
            inner.laser_new_arg2 = et_ex->n;
            ex_index++;
            inner.laser_new_arg4 = et_ex[1].r;
            inner.laser_time_start = et_ex[1].a;
            inner.laser_trans_1 = et_ex[1].b;
            inner.laser_duration = et_ex[1].c;
            inner.laser_trans_2 = et_ex[1].d;
            inner.distance = et_ex[1].s;
            inner.shot_sfx = 18;
            inner.shot_transform_sfx = -1;
            allocate_new_laser(1,&inner);
            if ((et_ex->d & 0x10000) != 0) {
              cancel(0);
            }
        }
        if (et_ex->a == 0) {
            LaserLineInner_t inner;
            for (int i = 0; i < 20; i++) inner.et_ex[i] = this->et_ex[i];
            inner.start_pos = pos;
            inner.bullet_type = et_ex->b;
            inner.bullet_color = et_ex->c;
            inner.ang_aim = et_ex->r;
            inner.spd_1 = et_ex->s;
            inner.laser_new_arg_1 = et_ex->m;
            inner.laser_new_arg_2 = et_ex->n;
            inner.flags |= 1;
            if (et_ex->r <= -999990.0) inner.ang_aim = angle;
            if (et_ex->r > 999990.0) inner.ang_aim = math::point_direction(pos.x, pos.y, Globals::get()->playerX, Globals::get()->playerY);
            if (et_ex->s <= -999990.0) {
                inner.spd_1 = speed;
            }
            ex_index++;
            inner.laser_new_arg_3 = et_ex[1].r;
            inner.laser_new_arg_4 = et_ex[1].s;
            inner.distance = et_ex[1].m;
            inner.shot_sfx = et_ex[1].a;
            inner.shot_transform_sfx = et_ex[1].b;
            inner.et_ex_index = et_ex[1].c;
            allocate_new_laser(0, &inner);
            if (et_ex->d != 0) {
                cancel(0);
            }
        }
    }

            /* Set hitbox size */
    if (ex_type == 0x20000000) {
      __hitbox_diameter_copy = et_ex->r < 0.f ? sprite_data["default_radius"].asFloat() : et_ex->r;
      hitbox_diameter = __hitbox_diameter_copy;
    }

            /* Homing */
    if (ex_type == 0x40000000) {
      active_ex_flags |= 0x40000000;
      ex_homing.norm = et_ex->r;
      ex_homing.angule = et_ex->s;
      ex_homing.__m = et_ex->m;
      ex_homing.timer = 0;
      ex_homing.duration = et_ex->a;
    }
          /* Wait */
    if (ex_type == 0x80000000) {
      if (et_ex->a > 0) {
        active_ex_flags |= 0x80000000;
        ex_wait.timer = et_ex->a;
      }
    }

    ex_index++;
  }
  return 0;
}
