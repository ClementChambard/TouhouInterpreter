#ifndef __INCLUDE_PARTS__
#include "../Bullet/BulletManager.h"
#include "../Anm/AnmManager.h"
#include "./EclContext.h"
#include "./EclInstrMacros.h"
#include "./Enemy.h"
#include "../Laser/LaserManager.h"
#include <math/math.hpp>
inline int Enemy::execInstr(EclRunContext_t *cont, const EclRawInstr_t *instr) {
  _insNop
#endif

      _ins(700, laserNew) _S(etid) _f(i_l) _f(f_l) _f(arg3) _f(w)
          _args enemy.BSs[etid]
              .__vec3_8 = {i_l, f_l, arg3};
  enemy.BSs[etid].__laser_new_arg_4 = w;

  _ins(701, laserTiming) _S(etid) _S(start_t) _S(tr1_t) _S(dur) _S(tr2_t)
      _S(end_t) _args enemy.BSs[etid]
          .__laser_timing_start = start_t;
  enemy.BSs[etid].__laser_timing_trans1 = tr1_t;
  enemy.BSs[etid].__laser_timing_duration = dur;
  enemy.BSs[etid].__laser_timing_trans2 = tr2_t;
  enemy.BSs[etid].__laser_timing_end = end_t;

  _ins(702, laserOn) _S(etid) _args LaserLineInner_t inner;

  for (int i = 0; i < 20; i++)
    inner.et_ex[i] = enemy.BSs[etid].ex[i];

  if (enemy.bulletOrigins[etid].z <= 0.9f) {
    inner.start_pos = enemy.final_pos.pos + enemy.bulletOffsets[etid];
  } else {
    inner.start_pos = {
        enemy.bulletOrigins[etid].x + enemy.bulletOffsets[etid].x,
        enemy.bulletOrigins[etid].y + enemy.bulletOffsets[etid].y, 0.f};
  }

  inner.ang_aim = enemy.BSs[etid].ang_aim;
  inner.laser_new_arg_2 = enemy.BSs[etid].__vec3_8.y;
  inner.laser_new_arg_1 = enemy.BSs[etid].__vec3_8.x;
  inner.laser_new_arg_3 = enemy.BSs[etid].__vec3_8.z;
  inner.laser_new_arg_4 = enemy.BSs[etid].__laser_new_arg_4;
  inner.spd_1 = enemy.BSs[etid].spd1;
  inner.bullet_type = enemy.BSs[etid].type;
  inner.bullet_color = enemy.BSs[etid].__color;
  inner.distance = enemy.BSs[etid].distance;
  inner.flags = enemy.BSs[etid].__laser_timing_end | 1;
  inner.shot_sfx = enemy.BSs[etid].shot_sfx;
  inner.shot_transform_sfx = enemy.BSs[etid].__shot_transform_sfx;

  allocate_new_laser(0, &inner);

  _ins(703, laserStOn) _S(etid) _S(id) _args LaserInfiniteInner_t inner;

  for (int i = 0; i < 20; i++)
    inner.ex[i] = enemy.BSs[etid].ex[i];

  if (enemy.bulletOrigins[etid].z <= 0.9f) {
    inner.start_pos = enemy.final_pos.pos + enemy.bulletOffsets[etid];
  } else {
    inner.start_pos = {
        enemy.bulletOrigins[etid].x + enemy.bulletOffsets[etid].x,
        enemy.bulletOrigins[etid].y + enemy.bulletOffsets[etid].y, 0.f};
  }

  inner.type = enemy.BSs[etid].type;
  inner.color = enemy.BSs[etid].__color;
  inner.ang_aim = enemy.BSs[etid].ang_aim;
  inner.laser_time_start = enemy.BSs[etid].__laser_timing_start;
  inner.laser_trans_1 = enemy.BSs[etid].__laser_timing_trans1;
  inner.laser_duration = enemy.BSs[etid].__laser_timing_duration;
  inner.laser_trans_2 = enemy.BSs[etid].__laser_timing_trans2;
  inner.spd_1 = enemy.BSs[etid].spd1;
  inner.flags = enemy.BSs[etid].__laser_timing_end | 2;
  inner.laser_new_arg2 = enemy.BSs[etid].__vec3_8.y;
  inner.laser_new_arg1 = enemy.BSs[etid].__vec3_8.x;
  inner.laser_new_arg4 = enemy.BSs[etid].__laser_new_arg_4;
  inner.shot_sfx = enemy.BSs[etid].shot_sfx;
  inner.distance = enemy.BSs[etid].distance;
  inner.shot_transform_sfx = enemy.BSs[etid].__shot_transform_sfx;
  inner.laser_st_on_arg1 = id;

  allocate_new_laser(1, &inner);

  _ins(704, laserOffset) _S(id) _f(x) _f(y) _args Laser *l =
      find_laser_by_id(id);
  if (l)
    l->set_offset(x, y, 0.f);

  _ins(705, laserTrajectory) _S(id) _f(xs) _f(ys) _args Laser *l =
      find_laser_by_id(id);
  if (l)
    l->set_speed(xs, ys, 0.f);

  _ins(706, laserStLength) _S(id) _f(s) _args Laser *l = find_laser_by_id(id);
  if (l)
    l->set_speed(s);

  _ins(707, laserStWidth) _S(id) _f(w) _args Laser *l = find_laser_by_id(id);
  if (l)
    l->set_width(w);

  _ins(708, laserStAngle) _S(id) _f(a) _args Laser *l = find_laser_by_id(id);
  if (l)
    l->set_angle(a);

  _ins(709, laserStRotation) _S(id) _f(avel) _args Laser *l =
      find_laser_by_id(id);
  if (dynamic_cast<LaserInfinite *>(l))
    dynamic_cast<LaserInfinite *>(l)->getInner().laser_inf_angular_velocity =
        avel;

  _ins(710, laserStEnd) _S(id) _args Laser *l = find_laser_by_id(id);
  while (l) {
    l->cancel(0, 0);
    l->set_id(0);
    l = find_laser_by_id(id);
  }

  _ins(711, laserCuOn) _S(etid) _args LaserCurveInner_t inner;

  for (int i = 0; i < 20; i++)
    inner.ex[i] = enemy.BSs[etid].ex[i];

  if (enemy.bulletOrigins[etid].z <= 0.9f) {
    inner.start_pos = enemy.final_pos.pos + enemy.bulletOffsets[etid];
  } else {
    inner.start_pos = {
        enemy.bulletOrigins[etid].x + enemy.bulletOffsets[etid].x,
        enemy.bulletOrigins[etid].y + enemy.bulletOffsets[etid].y, 0.f};
  }

  inner.type = enemy.BSs[etid].type;
  inner.color = enemy.BSs[etid].__color;
  inner.ang_aim = enemy.BSs[etid].ang_aim;
  inner.laser_time_start = enemy.BSs[etid].__laser_timing_start;
  inner.init_1 = 1;
  inner.shot_sfx = enemy.BSs[etid].shot_sfx;
  inner.shot_transform_sfx = enemy.BSs[etid].__shot_transform_sfx;
  inner.spd_1 = enemy.BSs[etid].spd1;
  inner.laser_new_arg4 = enemy.BSs[etid].__laser_new_arg_4;
  inner.distance = enemy.BSs[etid].distance;

  allocate_new_laser(2, &inner);

  _ins(712, etCancelRect) _f(w) _f(h) _args
  auto vm = anm::get_vm(enemy.anmIds[0]);
  float z_rot = 0.f;
  if (vm) z_rot = vm->rotation.z;
  BULLET_MANAGER_PTR->cancel_rectangle_as_bomb(
    z_rot, ns::vec2(enemy.final_pos.pos), {w, h}, 1);

  _ins(713, LaserBeOn) _S(a) _S(b) _args LaserBeamInner_t inner;

  // TODO(ClementChambard)

  allocate_new_laser(3, &inner);

  _ins(714, LaserBeCall) _S(id) _S(arg) _args Laser *l = find_laser_by_id(id);
  if (dynamic_cast<LaserBeam *>(l))
    dynamic_cast<LaserBeam *>(l)->method_8(arg);

#ifndef __INCLUDE_PARTS__
  _insDefault

      return 0;
}
#endif
