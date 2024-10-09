#include "ShtFile.h"
#include "./Ecl/EnemyManager.h"
#include "./Gui.hpp"
#include "Hardcoded.h"
#include "Player.h"
#include "Anm/AnmManager.h"
#include <math/math.hpp>

#include <fstream>

ShtFile_t* ALREADY_LOADED_SHTFILE = nullptr;

ShtFile_t* open_sht(cstr filename) {
    std::ifstream file(filename, std::ios::binary);
    if (file.fail())
        return nullptr;
    file.seekg(0, std::ios::end);
    std::streamoff fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    fileSize -= file.tellg();
    unsigned char* buf = new unsigned char[fileSize];
    file.read(reinterpret_cast<char*>(buf), fileSize);
    file.close();

    ShtFileRaw_t* raw = reinterpret_cast<ShtFileRaw_t*>(buf);
    ShtFile_t* sht = new ShtFile_t();

    sht->header = raw->header;
    uint8_t* rest = raw->next_bytes;
    if (TOUHOU_VERSION <= 13) {
        sht->header.max_dmg = 99999;
        sht->header.unknown_2 = 0;
        sht->header.unknown_3 = 0;
        sht->header.unknown_4 = 0;
        sht->header.unknown_5 = 0;
        sht->header.unknown_6 = 0;
        rest = (uint8_t*)&raw->header.max_dmg;
    }
    if (TOUHOU_VERSION <= 10) {
        sht->header.SA_power_divisor = 20;
        rest = (uint8_t*)&raw->header.SA_power_divisor;
    }

    int option_cnt = math::max(HARDCODED_DATA["option_count"].asInt(), (i32)sht->header.pwr_lvl_cnt);
    for (int i = 0; i < option_cnt; i++)
        for (int j = 0; j <= i; j++) {
            sht->option_pos.push_back({ *reinterpret_cast<float*>(rest + 0), *reinterpret_cast<float*>(rest + 4) });
            rest += 8;
        }
    for (int i = 0; i < option_cnt; i++)
        for (int j = 0; j <= i; j++) {
            sht->option_pos.push_back({ *reinterpret_cast<float*>(rest + 0), *reinterpret_cast<float*>(rest + 4) });
            rest += 8;
        }

    auto poffsets = reinterpret_cast<int32_t*>(rest);
    auto pshooters = poffsets + sht->header.sht_off_cnt;

    for (int i = 0; i < raw->header.sht_off_cnt; i++) {
        ShtShooterRaw_t* shooters = reinterpret_cast<ShtShooterRaw_t*>(reinterpret_cast<int64_t>(pshooters) + poffsets[i]);
        sht->shooters.push_back({});

        while (*reinterpret_cast<int32_t*>(shooters) != -1) {
            if (TOUHOU_VERSION <= 13) {
                sht->shooters.back().emplace_back(shooters, 0);
                sht->shooters.back().back().anm -= 11;
                shooters = reinterpret_cast<ShtShooterRaw_t*>(reinterpret_cast<uint64_t>(shooters) + offsetof(ShtShooterRaw_t, __unknown));
            } else if (TOUHOU_VERSION >= 18) {
                sht->shooters.back().emplace_back(reinterpret_cast<ShtShooterRaw18_t*>(shooters));
                shooters = reinterpret_cast<ShtShooterRaw_t*>(reinterpret_cast<uint64_t>(shooters) + offsetof(ShtShooterRaw_t, __unknown[18]));
            } else {
                sht->shooters.back().emplace_back(shooters);
                shooters = &shooters[1];
            }
        }
    }

    delete[] buf;
    return sht;
}

int ____empty____(PlayerBullet_t*) { return 0; }

int sht_on_init_1(PlayerBullet_t* bullet)
{
    bullet->targeted_entity = 0;
    return 0;
}

int sht_on_init_2(PlayerBullet_t* bullet)
{
    bullet->hitbox.x = 0.0;
    // SoundManager::play_sound_at_position(0x14);
    if (bullet->damageSourceId == 0)
        return 1;
    PLAYER_PTR->inner.damage_sources[bullet->damageSourceId - 1].field_0x80 = 1;
    PLAYER_PTR->inner.damage_sources[bullet->damageSourceId - 1].hitbox.x = 0.f;
    bullet->flags &= 0xfffffffe;
    return 0;
}

int sht_on_init_3(PlayerBullet_t* bullet)
{
    if (bullet->damageSourceId)
        PLAYER_PTR->inner.damage_sources[bullet->damageSourceId - 1].flags &= 0xfffffffe;
    ns::vec3 pos = bullet->pos.pos;
    pos.x -= 90.f;
    static constexpr float angles[] = {
        -1.832596,
        -1.570796,
        -1.570796,
        -1.308997,
    };
    static constexpr float posYInc[] = {
        -16.0,
        0.0,
        16.0,
        0.0,
    };
    static constexpr int damageInc[] = {
        -0x28,
        0,
        0,
        -0x28,
    };
    for (int i = 0; i < 4; i++) {
        int dsID = create_damage_source(pos, angles[i], i + 13, damageInc[i] + bullet->damage, 50.f, 60.f);
        if (dsID == 0)
            return 1;
        PLAYER_PTR->inner.damage_sources[dsID - 1].field_0x7c = 10000000;
        PLAYER_PTR->inner.damage_sources[dsID - 1].ds_on_hit = 1;
        PLAYER_PTR->inner.damage_sources[dsID - 1].bullet_id = bullet->id;
        PLAYER_PTR->inner.damage_sources[dsID - 1].field_0x80 = 1;
        PLAYER_PTR->inner.damage_sources[dsID - 1].field_0x88 = i;
        PLAYER_PTR->inner.damage_sources[dsID - 1].field_0x8c = 1;
        pos += ns::vec3(60.0, posYInc[i], 0);
    }
    return 0;
}

int sht_on_init_5(PlayerBullet_t* bullet)
{
    if (bullet->damageSourceId)
        PLAYER_PTR->inner.damage_sources[bullet->damageSourceId - 1].flags &= 0xfffffffe;
    ns::vec3 pos = bullet->pos.pos;
    pos.x += 90.f;
    static constexpr float angles[] = {
        -1.308997,
        -1.570796,
        -1.570796,
        -1.832596,
    };
    static constexpr float posYInc[] = {
        -16.0,
        0.0,
        16.0,
        0.0,
    };
    static constexpr int damageInc[] = {
        -0x28,
        0,
        0,
        -0x28,
    };
    for (int i = 0; i < 4; i++) {
        int dsID = create_damage_source(pos, angles[i], i + 13, damageInc[i] + bullet->damage, 50.f, 60.f);
        if (dsID == 0)
            return 1;
        PLAYER_PTR->inner.damage_sources[dsID - 1].field_0x7c = 10000000;
        PLAYER_PTR->inner.damage_sources[dsID - 1].ds_on_hit = 1;
        PLAYER_PTR->inner.damage_sources[dsID - 1].bullet_id = bullet->id;
        PLAYER_PTR->inner.damage_sources[dsID - 1].field_0x80 = 1;
        PLAYER_PTR->inner.damage_sources[dsID - 1].field_0x88 = i;
        PLAYER_PTR->inner.damage_sources[dsID - 1].field_0x8c = 1;
        pos += ns::vec3(-60.0, posYInc[i], 0);
    }
    return 0;
}

int sht_on_tick_1(PlayerBullet_t* bullet)
{
    if (bullet->active == 2)
        return 0;
    if (ENEMY_MANAGER_PTR) {
        if (bullet->targeted_entity == 0)
            bullet->targeted_entity = ENEMY_MANAGER_PTR->closest_enemy_id(ns::vec2(bullet->pos.pos));
        if (auto e = ENEMY_MANAGER_PTR->EnmFind(bullet->targeted_entity); e) {
            if (!(e->getData()->flags & 0xc000021U)) {
                if (0x3b < bullet->__field_c) {
                    bullet->pos.speed += 0.2;
                    return 0;
                }
                float ang_inc = math::point_direction(bullet->pos.pos.x, bullet->pos.pos.y, e->getData()->final_pos.pos.x, e->getData()->final_pos.pos.y) - bullet->pos.angle;
                float speed = bullet->pos.speed;
                math::angle_normalize(ang_inc);
                if (ns::abs(ang_inc) < 0.7853982) {
                    if (ns::abs(ang_inc) < 0.2617994) {
                        speed = 16.0;
                        if (bullet->pos.speed + 0.2 <= 16.0) {
                            speed = bullet->pos.speed + 0.2;
                        }
                    }
                } else {
                    speed = 4.0;
                    if (4.0 <= bullet->pos.speed - 0.2) {
                        speed = bullet->pos.speed - 0.2;
                    }
                }
                bullet->pos.angle = bullet->pos.angle + ang_inc * 0.08;
                math::angle_normalize(bullet->pos.angle);
                bullet->pos.speed = speed;
                return 0;
            }
        } else
            bullet->targeted_entity = 0;
    }

    if (bullet->pos.speed + 0.1 <= 16.0) {
        bullet->pos.speed += 0.1;
    } else
        bullet->pos.speed = 16.0f;

    return 0;
}

int sht_on_tick_2(PlayerBullet_t* bullet) {
    auto shooter = PLAYER_PTR->sht_file->shooters[bullet->shter >> 8][bullet->shter & 0xff];
    if (shooter.option > 0)
        bullet->pos.pos = ns::vec3(PLAYER_PTR->inner.options[shooter.option - 1].scaled_cur_pos.x, PLAYER_PTR->inner.options[shooter.option - 1].scaled_cur_pos.y, 0) / 128.f;
    else
        bullet->pos.pos = ns::vec3(PLAYER_PTR->inner.integer_pos.x, PLAYER_PTR->inner.integer_pos.y, 0) / 128.f;

    // float ang1, ang2;
    // if (!PLAYER_PTR->inner.focusing) {
    //     ang1 = shooter.angle;
    //     math::angle_normalize(ang1);
    // } else {
    //     ang1 = -ns::PI_1_2<f32>;
    // }
    //
    // ang2 = bullet->pos.angle - ang1;
    // math::angle_normalize(ang2);
    //
    // if (ns::abs(ang2) < 0.001) {
    //     ang1 -= bullet->pos.angle;
    //     math::angle_normalize(ang1);
    // }
    //
    // bullet->pos.angle = ang1;

    if (bullet->active == 2)
        return 0;

    //(**(code **)(*DAT_00526454 + 0x40)) (DAT_00526454,(int)(((PLAYER_PTR->inner).pos.x * 1000.0) / 192.0));
    if (bullet->hitbox.x < 512.0) {
        (bullet->hitbox).x = bullet->hitbox.x + 18.0;
        if (bullet->damageSourceId)
            PLAYER_PTR->inner.damage_sources[bullet->damageSourceId - 1].hitbox.x = bullet->hitbox.x + 18.0;
    }

    // FUN_0044db90(bullet.pos.pos,(bullet->pos).angle, bullet->hitbox.x * 0.5);

    if (auto vm = anm::get_vm(bullet->anmId); vm) {
        vm->bitflags.scaled = true;
        vm->bitflags.zoomed = true;
        vm->sprite_size.x = bullet->hitbox.x;
        vm->uv_scale.x = bullet->hitbox.x * 0.001953125;
    } else {
        bullet->anmId = 0;
    }

    if (bullet->damageSourceId != 0)
        PLAYER_PTR->inner.damage_sources[bullet->damageSourceId - 1].pos.pos = bullet->pos.pos + ns::vec3(math::lengthdir_vec(bullet->hitbox.x / 2.f, bullet->pos.angle), 0);

    if (bullet->hitting == 0) {
        if (bullet->active != 1) {
            bullet->hitting = 0;
            return 0;
        }
        if (bullet->blink == 1) {
            anm::interrupt_tree(bullet->anmId, 3);
            bullet->blink = 0;
        }
    }

    if (bullet->active == 1) {
        if (PLAYER_PTR->inner.shoot_key_short_timer > -1 && ((((GLOBALS.inner.HYPER_FLAGS >> 1) & 1) || !(shooter.option & 0xf) || shooter.option - 1 < PLAYER_PTR->inner.power) && ((((GLOBALS.inner.HYPER_FLAGS >> 1) & 1) && (GLOBALS.inner.HYPER_TYPE == 3 || GLOBALS.inner.HYPER_TYPE == 1)) || !PLAYER_PTR->inner.focusing)) && PLAYER_PTR->inner.state != 2 && PLAYER_PTR->inner.state != 4) {
            if (((((PLAYER_PTR->inner.unique_bullets[(shooter.option < 0x10) ? shooter.option & 0xf : shooter.option >> 4] == GLOBALS.inner.CURRENT_POWER / GLOBALS.inner.POWER_PER_LEVEL) && !PLAYER_PTR->inner.focusing) && !(PLAYER_PTR->flags & 0x40)) && (!GUI_PTR || !GUI_PTR->msg)) && ENEMY_MANAGER_PTR) {
                bullet->hitting = 0;
                return 0;
            }
        }

        if (bullet->damageSourceId)
            PLAYER_PTR->inner.damage_sources[bullet->damageSourceId - 1].flags &= 0xfffffffe;

        bullet->active = 2;

        anm::interrupt_tree(bullet->anmId, 1);

        PLAYER_PTR->inner.unique_bullets[(shooter.option < 0x10) ? shooter.option & 0xf : shooter.option >> 4] = 0;

        // FUN_004655d0(0x14);
    }
    bullet->hitting = 0;
    return 0;
}

int sht_on_tick_3(PlayerBullet_t* bullet)
{
    if (bullet->active) {
        bullet->pos.speed += 0.3f;
    }
    return 0;
}

int sht_on_tick_4(PlayerBullet_t* bullet)
{
    if (bullet->active) {
        bullet->pos.speed *= 0.8f;
    }
    return 0;
}

int sht_on_tick_6(PlayerBullet_t* bullet)
{
    bullet->damage += 2;
    return 0;
}

int (*SHT_ON_INIT[])(PlayerBullet_t*) = {
    nullptr,
    sht_on_init_1,
    sht_on_init_2, // Something with damage sources & sound (at 44c5e0)
    sht_on_init_3,
    sht_on_init_2,
    sht_on_init_5,
    nullptr,
    nullptr,
}; // TODO: variable array depending on the game

int (*SHT_ON_TICK[])(PlayerBullet_t*) = {
    nullptr,
    sht_on_tick_1, // amulets
    sht_on_tick_2, // lasers
    sht_on_tick_3, // missiles
    sht_on_tick_4,
    sht_on_tick_2, // lasers
    sht_on_tick_6,
    nullptr,
};

int (*SHT_FUNC_3[])(PlayerBullet_t*) = {
    ____empty____,
    ____empty____,
    ____empty____,
    ____empty____,
    ____empty____,
    ____empty____,
    ____empty____,
    ____empty____
};

int pl_b_default_on_hit(PlayerBullet_t& b)
{
    b.pos.pos.z = 0.1;
    b.active = 2;
    b.pos.speed *= 0.125;
    auto vm = anm::get_vm(b.anmId);
    if (vm) {
        anm::interrupt_tree(b.anmId, 1);
        vm->entity_pos = b.pos.pos;
    }
    if (b.damageSourceId)
        PLAYER_PTR->inner.damage_sources[b.damageSourceId - 1].flags &= 0xfffffffe;
    return b.damage;
}

int FUN_00449c80(ns::vec3 const& pos, int param_3, uint32_t param_4, float param_5, float param_6)
{
    uint uVar2;
    int iVar4;

    iVar4 = 0;
    int32_t iVar5 = PLAYER_PTR->inner.last_created_damage_source_index;
    while (true) {
        iVar5++;
        if (iVar5 >= 0x100)
            iVar5 = 0;
        uVar2 = (PLAYER_PTR->inner).damage_sources[iVar5].flags;
        if ((uVar2 & 1) == 0)
            break;
        iVar4 += 1;
        if (0xff < iVar4) {
            (PLAYER_PTR->inner).last_created_damage_source_index = iVar5;
            return iVar5 + 1;
        }
    }
    (PLAYER_PTR->inner).damage_sources[iVar5].pos.pos = pos;
    (PLAYER_PTR->inner).damage_sources[iVar5].flags = (uVar2 & 0xfffffffb) | 3;
    (PLAYER_PTR->inner).damage_sources[iVar5].field_0x4 = param_5;
    (PLAYER_PTR->inner).damage_sources[iVar5].field_0x8 = param_6;
    (PLAYER_PTR->inner).damage_sources[iVar5].time_to_live = param_3;
    (PLAYER_PTR->inner).damage_sources[iVar5].damage = param_4;
    (PLAYER_PTR->inner).damage_sources[iVar5].field_0x78 = 0;
    (PLAYER_PTR->inner).damage_sources[iVar5].field_0x7c = 9999999;
    (PLAYER_PTR->inner).damage_sources[iVar5].field_0x80 = 1;
    (PLAYER_PTR->inner).damage_sources[iVar5].ds_on_hit = 0;
    (PLAYER_PTR->inner).damage_sources[iVar5].field_0x84 = 0;
    (PLAYER_PTR->inner).damage_sources[iVar5].field_0x88 = 0;
    (PLAYER_PTR->inner).damage_sources[iVar5].field_0x8c = 0;
    (PLAYER_PTR->inner).last_created_damage_source_index = iVar5;
    return iVar5 + 1;
}

int (*SHT_ON_HIT[])(PlayerBullet_t*, ns::vec3 const&, float, float, float) = {
    nullptr,
    [](PlayerBullet_t* param_1, ns::vec3 const&, float, float, float) {
        float ang = (param_1->pos).angle + Random::Floatm11() * 0.3490658;
        math::angle_normalize(ang);
        anm::VM* vm;
        anm::getLoaded(8)->createEffectPos(0x95, ang, param_1->pos.pos, -1, &vm);
        vm->color_1.r = (rand() & 0x7f) + 0x7f;
        vm->color_1.g = (rand() & 0x3f) + 0x40;
        vm->color_1.b = (rand() & 0x3f) + 0x40;
        vm->color_1.a = (rand() & 0x3f) + 0x60;
        if (GLOBALS.inner.HYPER_FLAGS & 2)
            PLAYER_PTR->field_0x190ec += 0x32;
        return pl_b_default_on_hit(*param_1);
    },
    [](PlayerBullet_t* param_1, ns::vec3 const& param_2, float param_3, float, float param_5) {
        param_1->hitting = 1;
        if (param_1->blink == 0) {
            anm::interrupt_tree(param_1->anmId, 2);
            param_1->blink = 1;
        }
        if (param_3 == 0) {
            if (!param_1->damageSourceId)
                return 0;
            float d = PLAYER_PTR->inner.damage_sources[param_1->damageSourceId - 1].hitbox.y * 0.5 + param_5;
            float yy = (param_2.x - param_1->pos.pos.x) * ns::sin(-param_1->pos.angle) + (param_2.y - param_1->pos.pos.y) * ns::cos(-param_1->pos.angle);
            float xx = (param_2.x - param_1->pos.pos.x) * ns::cos(-param_1->pos.angle) - (param_2.y - param_1->pos.pos.y) * ns::sin(-param_1->pos.angle);
            if (((ns::abs(yy) <= d) && (-d <= xx)) && ((0.0 <= xx || (yy * yy + xx * xx <= d * d))))
                xx -= ns::sqrt(1.0 - (yy / d) * (yy / d)) * d;
            xx += 8.0;
            if (xx < 0.0)
                xx = 0.0;
            (param_1->hitbox).x = xx;
            PLAYER_PTR->inner.damage_sources[param_1->damageSourceId - 1].hitbox.x = xx;
        } else {
            if (!param_1->damageSourceId)
                return 0;
            float xx;
            // local_38 = PLAYER_PTR->inner.damage_sources[param_1->damageSourceId - 1].hitbox.x;
            // pfVar4 = PLAYER_PTR->inner.damage_sources[param_1->damageSourceId - 1].hitbox.y;
            // iVar6 = FUN_00403f60((float *)&local_48,&local_18.x,(float *)&param_1->pos,param_2->x, param_2->y,*pfVar4 + *param_3,param_3[1] + *pfVar4,param_4);
            // if (iVar6 != 0) {
            //     float a = math::point_direction(param_1->pos.pos, local_48);
            //     if (a - param_1->pos.angle > 3.141593) a -= param_1->pos.angle + 6.283185;
            //     else if (3.141593 < param_1->pos.angle - a) a -= param_1->pos.angle - 6.283185;
            //     else a -= param_1->pos.angle;
            //     if (1.570796 <= abs(a)) xx = 8.0;
            //     else xx = math::point_distance(param_1->pos.pos, local_48) + 8.0;
            // }
            /*else*/ xx = math::point_distance(param_1->pos.pos, param_2) - 24.f;
            if (xx < 0.0)
                xx = 0.0;
            param_1->hitbox.x = xx;
            PLAYER_PTR->inner.damage_sources[param_1->damageSourceId - 1].hitbox.x = xx + 16.0;
        }
        if (!(param_1->__field_c.current & 1)) {
            anm::VM* vm;
            PLAYER_PTR->playerAnm->createEffectPos(8, 0, ns::vec3 { math::lengthdir_vec(param_1->hitbox.x, param_1->pos.angle), 0.f } + param_1->pos.pos, -1, &vm);
            vm->rotation.z = param_1->pos.angle;
            vm->pos_i.start({ 0, 0, 0 }, { math::lengthdir_vec(64.f, param_1->pos.angle), 0.f }, 20, 4);
            // put in effectmanager
        }
        if (abs(param_1->__field_c.current) % 4 == 0) {
            if (GLOBALS.inner.HYPER_FLAGS & 2)
                PLAYER_PTR->field_0x190ec += 100;
            if (param_1->damageSourceId != 0)
                return (PLAYER_PTR->inner).damage_sources[param_1->damageSourceId - 1].damage;
        }
        return 0;
    },
    [](PlayerBullet_t* param_1, ns::vec3 const&, float, float, float) {
        auto& shooter = PLAYER_PTR->sht_file->shooters[param_1->shter >> 8][param_1->shter & 0xff];
        int iVar2 = FUN_00449c80(param_1->pos.pos, 10, shooter.damage, 24.0, 1.0);
        if (iVar2 != 0) {
            PLAYER_PTR->inner.damage_sources[iVar2 - 1].field_0x80 = 4;
            PLAYER_PTR->inner.damage_sources[iVar2 - 1].pos.speed = 0.3;
            PLAYER_PTR->inner.damage_sources[iVar2 - 1].pos.angle = -ns::PI_1_2<f32>;
        }
        anm::interrupt_tree(param_1->anmId, 1);
        param_1->active = 2;
        param_1->pos.speed = 0.3;
        if (param_1->damageSourceId) {
            PLAYER_PTR->inner.damage_sources[param_1->damageSourceId - 1].flags &= 0xfffffffe;
            PLAYER_PTR->inner.damage_sources[param_1->damageSourceId - 1].pos = param_1->pos;
        }
        param_1->damageSourceId = 0;
        // SoundManager::play_sound_at_position(0x41);
        if (GLOBALS.inner.HYPER_FLAGS & 2)
            PLAYER_PTR->field_0x190ec += 200;
        return param_1->damage;
    },
    [](PlayerBullet_t*, ns::vec3 const&, float, float, float) { if (GLOBALS.inner.HYPER_FLAGS & 2) PLAYER_PTR->field_0x190ec += 0x46; return -1; },
    [](PlayerBullet_t*, ns::vec3 const&, float, float, float) { return -1; },
    [](PlayerBullet_t* param_1, ns::vec3 const&, float, float, float) {
        auto& shooter = PLAYER_PTR->sht_file->shooters[param_1->shter >> 8][param_1->shter & 0xff];
        int iVar2 = FUN_00449c80(param_1->pos.pos, 10, shooter.damage, 24.0, 5.0);
        if (iVar2 != 0) {
            PLAYER_PTR->inner.damage_sources[iVar2 - 1].field_0x80 = 3;
            PLAYER_PTR->inner.damage_sources[iVar2 - 1].pos.speed = 0.3;
            PLAYER_PTR->inner.damage_sources[iVar2 - 1].pos.angle = -ns::PI_1_2<f32>;
        }
        anm::interrupt_tree(param_1->anmId, 1);
        param_1->active = 2;
        param_1->pos.speed = 0.3;
        if (param_1->damageSourceId) {
            PLAYER_PTR->inner.damage_sources[param_1->damageSourceId - 1].flags &= 0xfffffffe;
            PLAYER_PTR->inner.damage_sources[param_1->damageSourceId - 1].pos = param_1->pos;
        }
        param_1->damageSourceId = 0;
        // SoundManager::play_sound_at_position(0x41);
        if (GLOBALS.inner.HYPER_FLAGS & 2)
            PLAYER_PTR->field_0x190ec += 200;
        return param_1->damage;
    },
    nullptr,
    nullptr
};

ShtShooter_t::ShtShooter_t(ShtShooterRaw_t* raw, int nFlags)
    : offset(raw->offset)
    , hitbox(raw->hitbox) {
    fire_rate = raw->fire_rate;
    start_delay = raw->start_delay;
    damage = raw->damage;
    angle = raw->angle;
    speed = raw->speed;
    __unknown_1c = raw->__unknown_1c;
    option = raw->option;
    __unknown_21__2_is_unique_bullet = raw->__unknown_21;
    anm = raw->anm;
    anm_hit = raw->anm_hit;
    sfx_hit = raw->sfx_hit;
    long_fire_rate = raw->long_fire_rate;
    long_start_delay = raw->long_start_delay;
    for (int i = 0; i < nFlags; i++)
        __unknown[i] = raw->__unknown[i];
    for (int i = nFlags; i < 16; i++)
        __unknown[i] = 0;
    on_init = SHT_ON_INIT[raw->on_init];
    on_tick = SHT_ON_TICK[raw->on_tick];
    __func_3 = SHT_FUNC_3[raw->__func_3];
    on_hit = SHT_ON_HIT[raw->on_hit];
}

ShtShooter_t::ShtShooter_t(ShtShooterRaw18_t* raw)
    : offset(raw->offset)
    , hitbox(raw->hitbox) {
    fire_rate = raw->fire_rate;
    start_delay = raw->start_delay;
    damage = raw->damage;
    angle = raw->angle;
    speed = raw->speed;
    __unknown_1c = raw->__unknown_1c;
    option = raw->option;
    __unknown_21__2_is_unique_bullet = raw->__unknown_21;
    anm = raw->anm - 5;
    anm_hit = raw->anm_hit;
    sfx_hit = raw->sfx_hit;
    long_fire_rate = raw->long_fire_rate;
    long_start_delay = raw->long_start_delay;
    for (int i = 0; i < 16; i++)
        __unknown[i] = raw->__unknown[i];
    on_init = SHT_ON_INIT[raw->on_init];
    on_tick = SHT_ON_TICK[raw->on_tick];
    __func_3 = SHT_FUNC_3[raw->__func_3];
    on_hit = SHT_ON_HIT[raw->on_hit];
}
