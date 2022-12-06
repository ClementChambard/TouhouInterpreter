#include "BulletHandler.h"
#include<cstring>

void BM_reset(EnemyBulletShooter_t* bm)
{
    memset(bm, 0, sizeof(EnemyBulletShooter_t));
    bm->spd1 = 2.f;
    bm->cnt_count = 1;
    bm->cnt_layers = 1;
    bm->shot_sfx = 0x15;
    bm->__shot_transform_sfx = 0x26;
    bm->sfx_flag = 0x23;
}

EtEx_t& EtEx_t::operator=(EtEx_t &other)
{
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
    return other;
}
