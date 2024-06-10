#ifndef __INCLUDE_PARTS__
#include "../EclInstrMacros.h"
#include "../Player.h"
#include "../EclContext.h"
#include "../Enemy.h"
#include "../EnemyManager.h"
#include <math/Random.h>
inline int Enemy::execInstr(EclRunContext_t* cont, const EclRawInstr_t* instr) {
    _insNop
#endif


    _ins(400, movePos) _f(x) _f(y) _args
    enemy.abs_pos.pos.x = x > -999999.f ? x : enemy.abs_pos.pos.x;
    enemy.abs_pos.pos.y = y > -999999.f ? y : enemy.abs_pos.pos.y;
    enemy.final_pos.pos = enemy.abs_pos.pos + enemy.rel_pos.pos;
    enemy.calc_final_pos();

    _ins(401, movePosTime) _S(t) _S(m) _f(x) _f(y) _args
    if (t < 1) {
        enemy.abs_pos_i.end_time = 0;
    } else {
        if (y <= -999999.0) y = enemy.abs_pos.pos.y;
        if (x <= -999999.0) x = enemy.abs_pos.pos.x;
        enemy.abs_pos_i.start(enemy.abs_pos.pos, {x, y, 0}, t, m);
        enemy.abs_pos.flags &= 0xfffffff0;
    }

    _ins(402, movePosRel) _f(x) _f(y) _args
    enemy.rel_pos.pos.x = x > -999999.f ? x : enemy.rel_pos.pos.x;
    enemy.rel_pos.pos.y = y > -999999.f ? y : enemy.rel_pos.pos.y;
    enemy.final_pos.pos = enemy.abs_pos.pos + enemy.rel_pos.pos;
    enemy.calc_final_pos();

    _ins(403, movePosRelTime) _S(t) _S(m) _f(x) _f(y) _args
    if (t < 1) {
        enemy.rel_pos_i.end_time = 0;
    } else {
        if (y <= -999999.0) y = enemy.rel_pos.pos.y;
        if (x <= -999999.0) x = enemy.rel_pos.pos.x;
        enemy.rel_pos_i.start(enemy.rel_pos.pos, {x, y, 0}, t, m);
        enemy.rel_pos.flags &= 0xfffffff0;
    }

    _ins(404, moveVel) _f(r) _f(s) _args
    if (-999999.0 < r) {
        if (enemy.flags & 0x80000) {
            r = PI - r;
            math::angle_normalize(r);
        }
        enemy.abs_pos.angle = r;
    }
    if (s > -999999.0) enemy.abs_pos.speed = s;
    enemy.abs_pos.flags &= 0xfffffff0;

    _ins(405, moveVelTime) _S(t) _S(m) _f(r) _f(s) _args
    if (t < 1) {
        enemy.abs_angle_i.end_time = 0;
        enemy.abs_speed_i.end_time = 0;
    } else {
        if (m == 7) {
            if (r <= -999999.0) r = 0;
            else if (enemy.flags & 0x80000) r = -r;
            if (s <= -999999.0) s = 0;
        } else {
            if (r <= -999999.0) r = enemy.abs_pos.angle;
            else if (enemy.flags & 0x80000) r = PI - r;
            if (s <= -999999.0) s = enemy.abs_pos.speed;
        }
        enemy.abs_angle_i.start(enemy.abs_pos.angle, r, t, m);
// do some wierd thing with angle, same each time play with angle interpolation
        enemy.abs_speed_i.start(enemy.abs_pos.speed, s, t, m);
        enemy.abs_pos.flags &= 0xfffffff0;
    }

    _ins(406, moveVelRel) _f(r) _f(s) _args
    if (-999999.0 < r) {
        if (enemy.flags & 0x80000) {
            r = PI - r;
            math::angle_normalize(r);
        }
        enemy.rel_pos.angle = r;
    }
    if (s > -999999.0) enemy.rel_pos.speed = s;
    enemy.rel_pos.flags &= 0xfffffff0;

    _ins(407, moveVelRelTime) _S(t) _S(m) _f(r) _f(s) _args
    if (t < 1) {
        enemy.rel_angle_i.end_time = 0;
        enemy.rel_speed_i.end_time = 0;
    } else {
        if (m == 7) {
            if (r <= -999999.0) r = 0;
            else if (enemy.flags & 0x80000) r = -r;
            if (s <= -999999.0) s = 0;
        } else {
            if (r <= -999999.0) r = enemy.rel_pos.angle;
            else if (enemy.flags & 0x80000) r = PI - r;
            if (s <= -999999.0) s = enemy.rel_pos.speed;
        }
        enemy.rel_angle_i.start(enemy.rel_pos.angle, r, t, m);
        enemy.rel_speed_i.start(enemy.rel_pos.speed, s, t, m);
        enemy.rel_pos.flags &= 0xfffffff0;
    }

    _ins(408, moveCirc) _f(t) _f(s) _f(r) _f(ri) _args
    if ((enemy.abs_pos.flags & 0xf) != 2)
        enemy.abs_pos.velocity = enemy.abs_pos.pos;
    if (-999999.0 < t) enemy.abs_pos.angle = t;
    if (-999999.0 < s) enemy.abs_pos.speed = s;
    if (-999999.0 < r) enemy.abs_pos.circle_radius = r;
    if (-999999.0 < ri) enemy.abs_pos.circle_radial_speed = ri;
    enemy.abs_pos.flags = (enemy.abs_pos.flags & 0xfffffff0) | 2;
    enemy.abs_pos.update_position();
    enemy.calc_final_pos();

    _ins(409, moveCircTime) _S(t) _S(m) _f(s) _f(r) _f(ri) _args
    if (s <= -999999.0) s = enemy.abs_pos.speed;
    if (ri <= -999999.0) ri = enemy.abs_pos.circle_radial_speed;
    if (r <= -999999.0) r = enemy.abs_pos.circle_radius;
    if (t < 1) {
        enemy.abs_speed_i.end_time = 0;
        enemy.abs_radial_dist_i.end_time = 0;
    } else {
        enemy.abs_speed_i.start(enemy.abs_pos.speed, s, t, m);
        enemy.abs_radial_dist_i.start({enemy.abs_pos.circle_radius,
            enemy.abs_pos.circle_radial_speed}, {r, ri}, t, m);
        enemy.abs_pos.flags = (enemy.abs_pos.flags & 0xfffffff0) | 2;
        enemy.abs_pos.update_position();
        enemy.calc_final_pos();
        }

    _ins(410, moveCircRel) _f(t) _f(s) _f(r) _f(ri) _args
    if ((enemy.rel_pos.flags & 0xf) != 2)
        enemy.rel_pos.velocity = enemy.rel_pos.pos;
    if (-999999.0 < t) enemy.rel_pos.angle = t;
    if (-999999.0 < s) enemy.rel_pos.speed = s;
    if (-999999.0 < r) enemy.rel_pos.circle_radius = r;
    if (-999999.0 < ri) enemy.rel_pos.circle_radial_speed = ri;
    enemy.rel_pos.flags = (enemy.rel_pos.flags & 0xfffffff0) | 2;
    enemy.rel_pos.update_position();
    enemy.calc_final_pos();

    _ins(411, moveCircRelTime) _S(t) _S(m) _f(s) _f(r) _f(ri) _args
    if (s <= -999999.0) s = enemy.rel_pos.speed;
    if (ri <= -999999.0) ri = enemy.rel_pos.circle_radial_speed;
    if (r <= -999999.0) r = enemy.rel_pos.circle_radius;
    if (t < 1) {
        enemy.rel_speed_i.end_time = 0;
        enemy.rel_radial_dist_i.end_time = 0;
    } else {
        enemy.rel_speed_i.start(enemy.rel_pos.speed, s, t, m);
        enemy.rel_radial_dist_i.start({enemy.rel_pos.circle_radius,
            enemy.rel_pos.circle_radial_speed}, {r, ri}, t, m);
        enemy.rel_pos.flags = (enemy.rel_pos.flags & 0xfffffff0) | 2;
        enemy.rel_pos.update_position();
        enemy.calc_final_pos();
    }

    _ins(412, moveRand) _S(t) _S(m) _f(s) _args
    float a;
    if (enemy.final_pos.pos.x < enemy.moveLimitPos.x -
        enemy.moveLimitSize.x / 4.0) {
        a = Random::Angle() / 3.f;
    } else if (enemy.final_pos.pos.x > enemy.moveLimitPos.x +
               enemy.moveLimitSize.x / 4.0) {
        a = Random::Angle() / 3.f + PI;
        math::angle_normalize(a);
    } else if (PLAYER_PTR->inner.pos.x <= enemy.final_pos.pos.x) {
        a = Random::Angle() * 0.25f;
        if (rand() % 3 != 0) a += PI;
    } else {
        a = Random::Angle() * 0.25f;
        if (rand() % 3 == 0) a += PI;
    }
    if (enemy.final_pos.pos.y < enemy.moveLimitPos.y -
        enemy.moveLimitSize.y / 4.0) a = fabs(a);
    if (enemy.final_pos.pos.y > enemy.moveLimitPos.y +
        enemy.moveLimitSize.y / 4.0) a = -fabs(a);
    if (0.05 > fabs(a + PI1_2))
        a = (a <= -PI1_2) ? -1.6207963 : -1.5207964;
    if (0.05 < fabs(a - PI1_2))
        a = (a >= PI1_2) ? 1.6207963 : 1.5207964;
    enemy.abs_angle_i.start(a, a, t, m);
    enemy.abs_speed_i.start(s, 0, t, m);
    enemy.abs_pos.flags &= 0xfffffff0;

    _ins(413, moveRandRel) _S(t) _S(m) _f(s) _args
    float a;
    if (enemy.final_pos.pos.x < enemy.moveLimitPos.x -
        enemy.moveLimitSize.x / 4.0) {
        a = Random::Angle() / 3.0;
    } else if (enemy.final_pos.pos.x > enemy.moveLimitPos.x +
               enemy.moveLimitSize.x / 4.0) {
        a = Random::Angle() / 3.0 + PI;
        math::angle_normalize(a);
    } else if (PLAYER_PTR->inner.pos.x <= enemy.final_pos.pos.x) {
        a = Random::Angle() * 0.25;
        if (rand() % 3 != 0) a += PI;
    } else {
        a = Random::Angle() * 0.25;
        if (rand() % 3 == 0) a += PI;
    }
    if (enemy.final_pos.pos.y < enemy.moveLimitPos.y -
        enemy.moveLimitSize.y / 4.0) a = fabs(a);
    if (enemy.final_pos.pos.y > enemy.moveLimitPos.y +
        enemy.moveLimitSize.y / 4.0) a = -fabs(a);
    if (0.05 > fabs(a + PI1_2))
        a = (a <= -PI1_2) ? -1.6207963 : -1.5207964;
    if (0.05 < fabs(a - PI1_2))
        a = (a >= PI1_2) ? 1.6207963 : 1.5207964;
    enemy.rel_angle_i.start(a, a, t, m);
    enemy.rel_speed_i.start(s, 0, t, m);
    enemy.rel_pos.flags &= 0xfffffff0;

    _ins(414, moveBoss) _args
    auto e = ENEMY_MANAGER_PTR
        ->EnmFind(ENEMY_MANAGER_PTR->boss_ids[0]);
    if (e) enemy.abs_pos.pos = e->enemy.final_pos.pos;

    _ins(415, moveBossRel) _args
    auto e = ENEMY_MANAGER_PTR
        ->EnmFind(ENEMY_MANAGER_PTR->boss_ids[0]);
    if (e) enemy.rel_pos.pos = e->enemy.final_pos.pos;

    _ins(416, movePos3d) _f(x) _f(y) _f(z) _args
    enemy.abs_pos.pos += glm::vec3{x, y, z};
    enemy.calc_final_pos();

    _ins(417, movePos3dRel) _f(x) _f(y) _f(z) _args
    enemy.rel_pos.pos += glm::vec3{x, y, z};
    enemy.calc_final_pos();

    _ins(418, moveAdd) _f(x) _f(y) _args
    if (x > -999999.0) enemy.abs_pos.velocity.x = x;
    if (y > -999999.0) enemy.abs_pos.velocity.y = y;
    enemy.abs_pos.update_position();
    enemy.calc_final_pos();

    _ins(419, moveAddRel) _f(x) _f(y) _args
    if (x > -999999.0) enemy.rel_pos.velocity.x = x;
    if (y > -999999.0) enemy.rel_pos.velocity.y = y;
    enemy.rel_pos.update_position();
    enemy.calc_final_pos();

    _ins(420, moveEll) _f(t) _f(s) _f(r) _f(ri) _f(d) _f(q) _args
    if ((enemy.abs_pos.flags & 0xf) != 2)
        enemy.abs_pos.velocity = enemy.abs_pos.pos;
    if (-999999.0 < t) enemy.abs_pos.angle = t;
    if (-999999.0 < s) enemy.abs_pos.speed = s;
    if (-999999.0 < r) enemy.abs_pos.circle_radius = r;
    if (-999999.0 < ri) enemy.abs_pos.circle_radial_speed = ri;
    if (-999999.0 < d) enemy.abs_pos.some_angle1 = d;
    if (-999999.0 < q) enemy.abs_pos.some_value = q;
    enemy.abs_pos.flags = (enemy.abs_pos.flags & 0xfffffff0) | 3;
    enemy.abs_pos.update_position();
    enemy.calc_final_pos();

    _ins(421, moveEllTime) _S(t) _S(m) _f(s) _f(r) _f(ri) _f(d) _f(q) _args
    if (s <= -999999.0) s = enemy.abs_pos.speed;
    if (r <= -999999.0) r = enemy.abs_pos.circle_radius;
    if (ri <= -999999.0) ri = enemy.abs_pos.circle_radial_speed;
    if (q <= -999999.0) q = enemy.abs_pos.some_value;
    if (d <= -999999.0) d = enemy.abs_pos.some_angle1;
    if (t < 1) {
        enemy.abs_speed_i.end_time = 0;
        enemy.abs_radial_dist_i.end_time = 0;
        enemy.abs_ellipse_i.end_time = 0;
    } else {
        enemy.abs_speed_i.start(enemy.abs_pos.speed, s, t, m);
        enemy.abs_radial_dist_i.start({enemy.abs_pos.circle_radius,
            enemy.abs_pos.circle_radial_speed}, {r, ri}, t, m);
        enemy.abs_ellipse_i.start({enemy.abs_pos.some_angle1,
            enemy.abs_pos.some_value}, {q, d}, t, m);
        enemy.abs_pos.velocity = enemy.abs_pos.pos;
        enemy.rel_pos.flags = (enemy.rel_pos.flags & 0xfffffff0) | 3;
        enemy.abs_pos.update_position();
    }

    _ins(422, moveEllRel) _f(t) _f(s) _f(r) _f(ri) _f(d) _f(q) _args
    if ((enemy.rel_pos.flags & 0xf) != 2)
        enemy.rel_pos.velocity = enemy.rel_pos.pos;
    if (-999999.0 < t) enemy.rel_pos.angle = t;
    if (-999999.0 < s) enemy.rel_pos.speed = s;
    if (-999999.0 < r) enemy.rel_pos.circle_radius = r;
    if (-999999.0 < ri) enemy.rel_pos.circle_radial_speed = ri;
    if (-999999.0 < d) enemy.rel_pos.some_angle1 = d;
    if (-999999.0 < q) enemy.rel_pos.some_value = q;
    enemy.rel_pos.flags = (enemy.rel_pos.flags & 0xfffffff0) | 3;
    enemy.rel_pos.update_position();
    enemy.calc_final_pos();

    _ins(423, moveEllRelTime) _S(t) _S(m) _f(s) _f(r) _f(ri) _f(d) _f(q) _args
    if (s <= -999999.0) s = enemy.rel_pos.speed;
    if (r <= -999999.0) r = enemy.rel_pos.circle_radius;
    if (ri <= -999999.0) ri = enemy.rel_pos.circle_radial_speed;
    if (q <= -999999.0) q = enemy.rel_pos.some_value;
    if (d <= -999999.0) d = enemy.rel_pos.some_angle1;
    if (t < 1) {
        enemy.rel_speed_i.end_time = 0;
        enemy.rel_radial_dist_i.end_time = 0;
        enemy.rel_ellipse_i.end_time = 0;
    } else {
        enemy.rel_speed_i.start(enemy.rel_pos.speed, s, t, m);
        enemy.rel_radial_dist_i.start({enemy.rel_pos.circle_radius,
            enemy.rel_pos.circle_radial_speed}, {r, ri}, t, m);
        enemy.rel_ellipse_i.start({enemy.rel_pos.some_angle1,
            enemy.rel_pos.some_value}, {q, d}, t, m);
        enemy.rel_pos.velocity = enemy.rel_pos.pos;
        enemy.rel_pos.flags = (enemy.rel_pos.flags & 0xfffffff0) | 3;
        enemy.rel_pos.update_position();
    }

    _ins(424, moveMirror) _S(state) _args
    enemy.flags = enemy.flags ^ (((state << 13) ^ enemy.flags) & 0x80000);

    _ins(425, moveBezier) _S(t) _f(x) _f(y) _f(x2) _f(y2) _f(x3) _f(y3) _args
    if (y2 <= -999999.0) y2 = enemy.abs_pos.pos.y;
    if (x2 <= -999999.0) x2 = enemy.abs_pos.pos.x;
    enemy.abs_pos_i.start_bezier(enemy.abs_pos.pos, {x2, y2, 0.f},
                                 {x, y, 0.f}, {x3, y3, 0.f}, t);
    enemy.abs_pos.flags &= 0xfffffff0;

    _ins(426, moveBezierRel) _S(t) _f(x) _f(y) _f(x2) _f(y2) _f(x3) _f(y3) _args
    if (y2 <= -999999.0) y2 = enemy.rel_pos.pos.y;
    if (x2 <= -999999.0) x2 = enemy.rel_pos.pos.x;
    enemy.rel_pos_i.start_bezier(enemy.rel_pos.pos, {x2, y2, 0.f},
                                 {x, y, 0.f}, {x3, y3, 0.f}, t);
    enemy.rel_pos.flags &= 0xfffffff0;

    _ins(427, moveReset) _args
    enemy.abs_pos.pos = enemy.rel_pos.pos + enemy.abs_pos.pos;
    enemy.rel_pos.pos = {0, 0, 0};
    enemy.rel_pos.speed = 0.f;
    enemy.abs_pos.speed = 0.f;
    enemy.rel_pos.velocity = {0, 0, 0};
    enemy.abs_pos.velocity = {0, 0, 0};
    enemy.rel_pos.flags &= 0xfffffff0;
    enemy.abs_pos.flags &= 0xfffffff0;
    enemy.abs_pos_i.end_time = 0;
    enemy.rel_pos_i.end_time = 0;
    enemy.abs_angle_i.end_time = 0;
    enemy.rel_angle_i.end_time = 0;
    enemy.abs_speed_i.end_time = 0;
    enemy.rel_speed_i.end_time = 0;
    enemy.abs_radial_dist_i.end_time = 0;
    enemy.rel_radial_dist_i.end_time = 0;
    enemy.abs_ellipse_i.end_time = 0;
    enemy.rel_ellipse_i.end_time = 0;

    _ins(428, moveVelNM) _f(r) _f(s) _args
    if (-999999.0 < r) enemy.abs_pos.angle = r;
    if (s > -999999.0) enemy.abs_pos.speed = s;
    enemy.abs_pos.flags &= 0xfffffff0;

    _ins(429, moveVelTimeNM) _S(t) _S(m) _f(r) _f(s) _args
    if (t < 1) {
        enemy.rel_angle_i.end_time = 0;
        enemy.rel_speed_i.end_time = 0;
    } else {
        if (m == 7) {
            if (r <= -999999.0) r = 0;
            if (s <= -999999.0) s = 0;
        } else {
            if (r <= -999999.0) r = enemy.rel_pos.angle;
            if (s <= -999999.0) s = enemy.rel_pos.speed;
        }
        enemy.rel_angle_i.start(enemy.rel_pos.angle, r, t, m);
        enemy.rel_speed_i.start(enemy.rel_pos.speed, s, t, m);
        enemy.rel_pos.flags &= 0xfffffff0;
    }

    _ins(430, moveVelRelNM) _f(r) _f(s) _args
    if (-999999.0 < r) enemy.rel_pos.angle = r;
    if (s > -999999.0) enemy.rel_pos.speed = s;
    enemy.rel_pos.flags &= 0xfffffff0;

    _ins(431, moveVelRelTimeNM) _S(t) _S(m) _f(r) _f(s) _args
    if (t < 1) {
        enemy.rel_angle_i.end_time = 0;
        enemy.rel_speed_i.end_time = 0;
    } else {
        if (m == 7) {
            if (r <= -999999.0) r = 0;
            if (s <= -999999.0) s = 0;
        } else {
            if (r <= -999999.0) r = enemy.rel_pos.angle;
            if (s <= -999999.0) s = enemy.rel_pos.speed;
        }
        enemy.rel_angle_i.start(enemy.rel_pos.angle, r, t, m);
        enemy.rel_speed_i.start(enemy.rel_pos.speed, s, t, m);
        enemy.rel_pos.flags &= 0xfffffff0;
    }

    _ins(432, moveEnm) _S(id) _args
    auto e = ENEMY_MANAGER_PTR->EnmFind(id);
    if (e) enemy.abs_pos.pos = e->enemy.final_pos.pos;

    _ins(433, moveEnmRel) _S(id) _args
    auto e = ENEMY_MANAGER_PTR->EnmFind(id);
    if (e) enemy.rel_pos.pos = e->enemy.final_pos.pos;

    _ins(434, moveCurve) _S(t) _S(mx) _S(my) _f(x) _f(y) _args
    if (t < 1) {
        enemy.abs_pos_i.end_time = 0;
    } else {
        if (x <= -999999.0) x = enemy.abs_pos.pos.x;
        if (y <= -999999.0) y = enemy.abs_pos.pos.y;
        enemy.abs_pos_i.start_curve(enemy.abs_pos.pos, {x, y, 0.f},
                                    t, mx, my, 0);
        enemy.abs_pos.flags &= 0xfffffff0;
    }

    _ins(435, moveCurveRel) _S(t) _S(mx) _S(my) _f(x) _f(y) _args
    if (t < 1) {
        enemy.rel_pos_i.end_time = 0;
    } else {
        if (x <= -999999.0) x = enemy.rel_pos.pos.x;
        if (y <= -999999.0) y = enemy.rel_pos.pos.y;
        enemy.rel_pos_i.start_curve(enemy.rel_pos.pos, {x, y, 0.f},
                                    t, mx, my, 0);
        enemy.rel_pos.flags &= 0xfffffff0;
    }

    _ins(436, moveAddTime) _S(t) _S(m) _f(x) _f(y) _args
    if (t < 1) {
        enemy.abs_pos_i.end_time = 0;
    } else {
        if (enemy.flags & 0x80000) {
            x = enemy.abs_pos.pos.x - x;
        } else {
            x += enemy.abs_pos.pos.x;
        }
        y += enemy.abs_pos.pos.y;
        if (y <= -999999.0) y = enemy.abs_pos.pos.y;
        if (x <= -999999.0) x = enemy.abs_pos.pos.x;
        enemy.abs_pos_i.start(enemy.abs_pos.pos, {x, y, 0}, t, m);
        enemy.abs_pos.flags &= 0xfffffff0;
    }

    _ins(437, moveAddRelTime) _S(t) _S(m) _f(x) _f(y) _args
    if (t < 1) {
        enemy.rel_pos_i.end_time = 0;
    } else {
        if (enemy.flags & 0x80000) {
            x = enemy.rel_pos.pos.x - x;
        } else {
            x += enemy.rel_pos.pos.x;
        }
        y += enemy.rel_pos.pos.y;
        if (y <= -999999.0) y = enemy.rel_pos.pos.y;
        if (x <= -999999.0) x = enemy.rel_pos.pos.x;
        enemy.rel_pos_i.start(enemy.rel_pos.pos, {x, y, 0}, t, m);
        enemy.rel_pos.flags &= 0xfffffff0;
    }

    _ins(438, moveCurveAdd) _S(t) _S(mx) _S(my) _f(x) _f(y) _args
    if (t < 1) {
        enemy.abs_pos_i.end_time = 0;
    } else {
        if (enemy.flags & 0x80000) {
            x = enemy.abs_pos.pos.x - x;
        } else {
            x += enemy.abs_pos.pos.x;
        }
        y += enemy.abs_pos.pos.y;
        if (x <= -999999.0) x = enemy.abs_pos.pos.x;
        if (y <= -999999.0) y = enemy.abs_pos.pos.y;
        enemy.abs_pos_i.start_curve(enemy.abs_pos.pos, {x, y, 0.f},
                                    t, mx, my, 0);
        enemy.abs_pos.flags &= 0xfffffff0;
    }

    _ins(439, moveCurveAddRel) _S(t) _S(mx) _S(my) _f(x) _f(y) _args
    if (t < 1) {
        enemy.rel_pos_i.end_time = 0;
    } else {
        if (enemy.flags & 0x80000) {
            x = enemy.rel_pos.pos.x - x;
        } else {
            x += enemy.rel_pos.pos.x;
        }
        y += enemy.rel_pos.pos.y;
        if (x <= -999999.0) x = enemy.rel_pos.pos.x;
        if (y <= -999999.0) y = enemy.rel_pos.pos.y;
        enemy.rel_pos_i.start_curve(enemy.rel_pos.pos, {x, y, 0.f},
                                    t, mx, my, 0);
        enemy.rel_pos.flags &= 0xfffffff0;
    }

    _ins(440, moveAngle) _f(a) _args
    if (enemy.flags & 0x80000) {
        a = PI - a;
        math::angle_normalize(a);
    }
    enemy.abs_pos.angle = a;
    enemy.abs_pos.flags &= 0xfffffff0;

    _ins(441, moveAngleTime) _S(t) _S(m) _f(a) _args
    if (0 < t) {
        if (m == 7) {
            if (a <= -999999.0) a = 0.f;
            else if (enemy.flags & 0x80000) a = -a;
        } else if (a <= -999999.0) {
            a = enemy.abs_pos.angle;
        } else if (enemy.flags & 0x80000) {
            a = PI - a;
        }
        enemy.abs_angle_i.start(enemy.abs_pos.angle, a, t, m);
        enemy.abs_pos.flags &= 0xfffffff0;
    } else {
        enemy.abs_angle_i.end_time = 0;
    }

    _ins(442, moveAngleRel) _f(a) _args
    if (enemy.flags & 0x80000) {
        a = PI - a;
        math::angle_normalize(a);
    }
    enemy.rel_pos.angle = a;
    enemy.rel_pos.flags &= 0xfffffff0;

    _ins(443, moveAngleRelTime) _S(t) _S(m) _f(a) _args
    if (0 < t) {
        if (m == 7) {
            if (a <= -999999.0) a = 0.f;
            else if (enemy.flags & 0x80000) a = -a;
        } else if (a <= -999999.0) {
            a = enemy.rel_pos.angle;
        } else if (enemy.flags & 0x80000) {
            a = PI - a;
        }
        enemy.rel_angle_i.start(enemy.rel_pos.angle, a, t, m);
        enemy.rel_pos.flags &= 0xfffffff0;
    } else {
        enemy.rel_angle_i.end_time = 0;
    }

    _ins(444, moveSpeed) _f(s) _args
    if (s > -999999.0) enemy.abs_pos.speed = s;
    enemy.abs_pos.flags &= 0xfffffff0;

    _ins(445, moveSpeedTime) _S(t) _S(m) _f(s) _args
    if (0 < t) {
        if (m == 7) {
            if (-999999.0 > s) s = 0;
        } else if (-999999.0 > s) {
            s = enemy.abs_pos.speed;
        }
        enemy.abs_speed_i.start(enemy.abs_pos.speed, s, t, m);
    } else {
        enemy.abs_speed_i.end_time = 0;
    }

    _ins(446, moveSpeedRel) _f(s) _args
    if (s > -999999.0) enemy.rel_pos.speed = s;
    enemy.rel_pos.flags &= 0xfffffff0;

    _ins(447, moveSpeedRelTime) _S(t) _S(m) _f(s) _args
    if (0 < t) {
        if (m == 7) {
            if (-999999.0 > s) s = 0;
        } else if (-999999.0 > s) {
            s = enemy.rel_pos.speed;
        }
        enemy.rel_speed_i.start(enemy.rel_pos.speed, s, t, m);
    } else {
        enemy.rel_speed_i.end_time = 0;
    }

#ifndef __INCLUDE_PARTS__
    _insDefault

  return 0;
}
#endif
