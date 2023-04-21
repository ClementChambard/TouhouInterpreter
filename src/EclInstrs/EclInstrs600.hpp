#ifndef __INCLUDE_PARTS__
#include <cstdint>
#include "../EclInstrMacros.h"
#include "../EclContext.h"
#include "../Enemy.h"
#include "../EnemyManager.h"
#include "../Spellcard.h"
#include "../Laser/LaserManager.h"
#include "../AnmOpener/AnmManager.h"
#include "../Player.h"
#include <math/Random.h>
#include "../BulletManager.h"
#define PRINT false
int Enemy::execInstr(EclRunContext_t* cont, EclRawInstr_t* instr)
{
    _insNop
#endif
#define BH(i) enemy.BSs[i]
#define EX(i,j) enemy.BSs[i].ex[j]


    _ins(600,         etNew) _S(id)                                                                               _args
        BM_reset(&enemy.BSs[id]);
        enemy.et_ex_id[id] = 0;
        enemy.bulletOffsets[id] = {};
        enemy.bulletOrigins[id] = {};

    _ins(601,          etOn) _S(id)                                                                               _args
        if (enemy.bulletOrigins[id].z <= 0.9f) enemy.BSs[id].__vec3_8 = enemy.final_pos.pos + enemy.bulletOffsets[id];
        else enemy.BSs[id].__vec3_8 = {enemy.bulletOrigins[id].x + enemy.bulletOffsets[id].x, enemy.bulletOrigins[id].y + enemy.bulletOffsets[id].y, 0.f};
        BulletManager::GetInstance()->set_protect_range(enemy.etProtRange);
        BulletManager::GetInstance()->Shoot(&enemy.BSs[id]);
        BulletManager::GetInstance()->set_protect_range(0.f);

    _ins(602,      etSprite) _S(id)  _S(t)   _S(c)                                                                _args
        enemy.BSs[id].type = t;
        enemy.BSs[id].__color = c;

    _ins(603,      etOffset) _S(id)  _f(x)   _f(y)                                                                _args
        enemy.bulletOffsets[id] = {x, y, 0};

    _ins(604,       etAngle) _S(id)  _f(a1)  _f(a2)                                                               _args
        enemy.BSs[id].ang_aim = a1;
        enemy.BSs[id].ang_bullet_dist = a2;

    _ins(605,       etSpeed) _S(id)  _f(s1)  _f(s2)                                                               _args
        enemy.BSs[id].spd1 = s1;
        enemy.BSs[id].spd2 = s2;

    _ins(606,       etCount) _S(id)  _S(c1)  _S(c2)                                                               _args
        enemy.BSs[id].cnt_count = c1;
        enemy.BSs[id].cnt_layers = c2;

    _ins(607,         etAim) _S(id)  _S(aim)                                                                      _args
        enemy.BSs[id].aim_type = aim;

    _ins(608,       etSound) _S(id)  _S(s1)  _S(s2)                                                               _args
        enemy.BSs[id].shot_sfx = s1;
        enemy.BSs[id].__shot_transform_sfx = s2;

    _ins(609,       etExSet) _S(id)  _S(i)   _S(as) _S(t)  _S(a)  _S(b)  _f(r)  _f(s)                             _args
        if (i > 19) { _warn("et_ex > 19"); break; }
        if (id > 17 || id < 0) id = 0;
        enemy.BSs[id].ex[i].slot = as;
        enemy.BSs[id].ex[i].type = t;
        enemy.BSs[id].ex[i].a = a;
        enemy.BSs[id].ex[i].b = b;
        enemy.BSs[id].ex[i].r = r;
        enemy.BSs[id].ex[i].s = s;
        enemy.et_ex_id[id] = i + 1;

    _ins(610,      etExSet2) _S(id)  _S(i)  _S(as) _S(t)  _S(a)  _S(b)  _S(c)  _S(d)  _f(r)  _f(s)  _f(m)  _f(n) _args
        if (i > 19) { _warn("et_ex > 19"); break; }
        if (id > 17 || id < 0) id = 0;
        enemy.BSs[id].ex[i].slot = as;
        enemy.BSs[id].ex[i].type = t;
        enemy.BSs[id].ex[i].a = a;
        enemy.BSs[id].ex[i].b = b;
        enemy.BSs[id].ex[i].c = c;
        enemy.BSs[id].ex[i].d = d;
        enemy.BSs[id].ex[i].r = r;
        enemy.BSs[id].ex[i].s = s;
        enemy.BSs[id].ex[i].m = m;
        enemy.BSs[id].ex[i].n = n;
        enemy.et_ex_id[id] = i + 1;

    _ins(611,          etEx) _S(id)  _S(as) _S(t)  _S(a)  _S(b)  _f(r)  _f(s)                                    _args
        if (id > 17 || id < 0) id = 0;
        int ex_id = enemy.et_ex_id[id];
        if (ex_id > 19) { _warn("et_ex > 19"); break; }
        enemy.BSs[id].ex[ex_id].slot = as;
        enemy.BSs[id].ex[ex_id].type = t;
        enemy.BSs[id].ex[ex_id].a = a;
        enemy.BSs[id].ex[ex_id].b = b;
        enemy.BSs[id].ex[ex_id].r = r;
        enemy.BSs[id].ex[ex_id].s = s;
        enemy.et_ex_id[id]++;

    _ins(612,         etEx2) _S(id)  _S(as) _S(t)  _S(a)  _S(b)  _S(c)  _S(d)  _f(r)  _f(s)  _f(m)  _f(n)        _args
        if (id > 17 || id < 0) id = 0;
        int ex_id = enemy.et_ex_id[id];
        if (ex_id > 19) { _warn("et_ex > 19"); break; }
        enemy.BSs[id].ex[ex_id].slot = as;
        enemy.BSs[id].ex[ex_id].type = t;
        enemy.BSs[id].ex[ex_id].a = a;
        enemy.BSs[id].ex[ex_id].b = b;
        enemy.BSs[id].ex[ex_id].c = c;
        enemy.BSs[id].ex[ex_id].d = d;
        enemy.BSs[id].ex[ex_id].r = r;
        enemy.BSs[id].ex[ex_id].s = s;
        enemy.BSs[id].ex[ex_id].m = m;
        enemy.BSs[id].ex[ex_id].n = n;
        enemy.et_ex_id[id]++;

    _ins(613,    etClearAll)                                                                                     _args // TODO: better
        BulletManager::GetInstance()->ClearScreen(0);
        LASER_MANAGER_PTR->cancel_all(true);

    _ins(614,        etCopy) _S(dst) _S(src)                                                                     _args
        enemy.BSs          [dst] = enemy.BSs          [src];
        enemy.bulletOffsets[dst] = enemy.bulletOffsets[src];
        enemy.bulletOrigins[dst] = enemy.bulletOrigins[src];

    _ins(615,      etCancel) _f(r)                                                                               _args // TODO: better
        BulletManager::GetInstance()->ClearScreen(1, r, enemy.final_pos.pos.x, enemy.final_pos.pos.y);
        LASER_MANAGER_PTR->cancel_in_radius(enemy.final_pos.pos, 1, 1, r);

    _ins(616,       etClear) _f(r)                                                                               _args // TODO: better
        BulletManager::GetInstance()->ClearScreen(0, r, enemy.final_pos.pos.x, enemy.final_pos.pos.y);
        LASER_MANAGER_PTR->cancel_in_radius(enemy.final_pos.pos, 0, 1, r);

    _ins(617,     etSpeedR3) _S(id)  _f(a1) _f(b1) _f(c1) _f(a2) _f(b2) _f(c2)                                   _args // TODO: check that Rank3, Rank5 and Rank2 work properly
        enemy.BSs[id].spd1 = Rank3(a1, b1, c1);
        enemy.BSs[id].spd2 = Rank3(a2, b2, c2);

    _ins(618,     etSpeedR5) _S(id)  _f(a1) _f(b1) _f(c1) _f(d1) _f(e1) _f(a2) _f(b2) _f(c2) _f(d2) _f(e2)       _args
        enemy.BSs[id].spd1 = Rank5(a1, b1, c1, d1, e1);
        enemy.BSs[id].spd2 = Rank5(a2, b2, c2, d2, e2);

    _ins(619,     etSpeedR2) _S(id)  _f(a1) _f(b1) _f(a2) _f(b2)                                                 _args
        enemy.BSs[id].spd1 = Rank2(a1, b1);
        enemy.BSs[id].spd2 = Rank2(a2, b2);

    _ins(620,     etCountR3) _S(id)  _S(a1) _S(b1) _S(c1) _S(a2) _S(b2) _S(c2)                                   _args
        enemy.BSs[id].cnt_count  = Rank3(a1, b1, c1);
        enemy.BSs[id].cnt_layers = Rank3(a2, b2, c2);

    _ins(621,     etCountR5) _S(id)  _S(a1) _S(b1) _S(c1) _S(d1) _S(e1) _S(a2) _S(b2) _S(c2) _S(d2) _S(e2)       _args
        enemy.BSs[id].cnt_count  = Rank5(a1, b1, c1, d1, e1);
        enemy.BSs[id].cnt_layers = Rank5(a2, b2, c2, d2, e2);

    _ins(622,     etCountR2) _S(id)  _S(a1) _S(b1) _S(a2) _S(b2)                                                 _args
        enemy.BSs[id].cnt_count  = Rank2(a1, b1);
        enemy.BSs[id].cnt_layers = Rank2(a2, b2);

    _ins(623, angleToPlayer) _rf(va) _f(x)  _f(y)                                                                _args
        va = math::point_direction(x, y, PLAYER_PTR->inner.pos.x, PLAYER_PTR->inner.pos.y);

    _ins(624,      etSpeedD) _S(id)  _f(a1) _f(b1) _f(c1) _f(d1) _f(a2) _f(b2) _f(c2) _f(d2)                     _args
        enemy.BSs[id].spd1 = Diff(a1, b1, c1, d1);
        enemy.BSs[id].spd2 = Diff(a2, b2, c2, d2);

    _ins(625,      etCountD) _S(id)  _S(a1) _S(b1) _S(c1) _S(d1) _S(a2) _S(b2) _S(c2) _S(d2)                     _args
        enemy.BSs[id].cnt_count = Diff(a1, b1, c1, d1);
        enemy.BSs[id].cnt_layers = Diff(a2, b2, c2, d2);

    _ins(626,   etOffsetRad) _S(id)  _f(a)  _f(r)                                                                _args
        enemy.bulletOffsets[id] = {r*cos(a), r*sin(a), 0};

    _ins(627,        etDist) _S(id)  _f(d)                                                                       _args
        enemy.BSs[id].distance = d;

    _ins(628,      etOrigin) _S(id)  _f(x)  _f(y)                                                                _args
        enemy.bulletOrigins[id] = {x, y, 1.f};
        if (x < -990.f) enemy.bulletOrigins[id].z = 0.f;

    _ins(629,           fog) _f(r)   _S(c)                                                                       _args      _notImpl

    _ins(630,       callStd) _S(s)                                                                               _args
        fileManager->stdf->interrupt(s);

    _ins(631,      lifeHide) _S(t)                                                                               _args      _notImpl

    _ins(632,       funcSet) _S(id)                                                                              _args      _notImpl
        // is_func_set_2 = 0;
        // func_from_ecl_func_set = ...

    _ins(633,    flagExtDmg) _S(state)                                                                           _args      _notImpl
        // func_from_ecl_flag_extra_damage = ...

    _ins(634, setHitboxFunc) _S(id)                                                                              _args      _notImpl
        // hitbox_func = ...

    _ins(635,     etCancel2) _f(r)                                                                               _args
        BulletManager::GetInstance()->ClearScreen(3, r, enemy.final_pos.pos.x, enemy.final_pos.pos.y);    // TODO change clearScreen
        LASER_MANAGER_PTR->cancel_in_radius(enemy.final_pos.pos, 1, 1, r);

    _ins(636,      etClear2) _f(r)                                                                               _args
        BulletManager::GetInstance()->ClearScreen(2, r, enemy.final_pos.pos.x, enemy.final_pos.pos.y);
        LASER_MANAGER_PTR->cancel_in_radius(enemy.final_pos.pos, 0, 1, r);

    _ins(637,      funcCall) _S(id)                                                                              _args      _notImpl
        // immediately call the function

    _ins(638,      scoreAdd) _S(amt)                                                                             _args      _notImpl
        // add score in globals and generate small popup

    _ins(639,      funcSet2) _S(id)                                                                              _args      _notImpl
        // is_func_set_2 = 1;
        // func_from_ecl_func_set = ...

    _ins(640,       etExSub) _S(id)  _S(i)  _z(s)                                                                _args
        EX(id, i).string = s.c_str();

    _ins(641,  etExSubtract) _S(id)                                                                              _args
        if (--enemy.et_ex_id[id] < 0) enemy.et_ex_id[id] = 0;



#ifndef __INCLUDE_PARTS__
    _insDefault

  return 0;
}
#endif
