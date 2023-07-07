#include "./BulletHandler.h"
#include <cstring>

void BM_reset(EnemyBulletShooter_t* bm) {
    bm->type = 0;
    bm->__color = 0;
    bm->__vec3_8 = {};
    bm->ang_aim = 0.f;
    bm->ang_bullet_dist = 0.f;
    bm->spd1 = 2.f;
    bm->spd2 = 0.f;
    bm->distance = 0.f;
    bm->__laser_new_arg_4 = 0.f;
    bm->__field_344 = 0.f;
    bm->__field_348 = 0.f;
    bm->__field_34c = 0.f;
    bm->__laser_timing_start = 0;
    bm->__laser_timing_trans1 = 0;
    bm->__laser_timing_duration = 0;
    bm->__laser_timing_trans2 = 0;
    bm->__laser_timing_end = 0;
    bm->cnt_count = 1;
    bm->cnt_layers = 1;
    bm->aim_type = 0;
    bm->sfx_flag = 0x23;
    bm->shot_sfx = 0x15;
    bm->__shot_transform_sfx = 0x26;
    bm->__start_transform = 0;
    bm->__field_37c = 0;

    for (int i = 0; i < 20; i++) {
        bm->ex[i].r = 0.f;
        bm->ex[i].s = 0.f;
        bm->ex[i].m = 0.f;
        bm->ex[i].n = 0.f;
        bm->ex[i].a = 0;
        bm->ex[i].b = 0;
        bm->ex[i].c = 0;
        bm->ex[i].d = 0;
        bm->ex[i].type = 0;
        bm->ex[i].slot = 0;
        bm->ex[i].string = 0;
    }
}

EtEx_t& EtEx_t::operator=(EtEx_t const& other) {
    r = other.r;
    s = other.s;
    m = other.m;
    n = other.n;
    a = other.a;
    b = other.b;
    c = other.c;
    d = other.d;
    type = other.type;
    slot = other.slot;
    string = other.string;
    return *this;
}
