#ifndef __INCLUDE_PARTS__
#include "../AnmOpener/AnmManager.h"
#include "../EclContext.h"
#include "../EclInstrMacros.h"
#include "../Enemy.h"
#include "../EnemyManager.h"
#include "../Hardcoded.h"
#include "../Spellcard.h"
#include <math/Random.h>
#define PRINT false
inline int Enemy::execInstr(EclRunContext_t* cont, const EclRawInstr_t* instr) {
    _insNop
#endif
#define COPY_VARS_TO(e)                                                        \
    {                                                                          \
        for (int i = 0; i < 4; i++)                                            \
            e->enemy.ecl_int_vars[i] = enemy.ecl_int_vars[i];                  \
        for (int i = 0; i < 8; i++)                                            \
            e->enemy.ecl_float_vars[i] = enemy.ecl_float_vars[i];              \
        e->parentEnemyId = enemyId;                                            \
    }
#define COPY_VARS_TO_M(e)                                                      \
    {                                                                          \
        for (int i = 0; i < 4; i++)                                            \
            e->enemy.ecl_int_vars[i] = enemy.ecl_int_vars[i];                  \
        for (int i = 0; i < 8; i++)                                            \
            e->enemy.ecl_float_vars[i] = enemy.ecl_float_vars[i];              \
        e->parentEnemyId = enemyId;                                            \
        e->enemy.flags |= 0x80000;                                             \
    }
#define ANM_SLOT(a) ENEMY_MANAGER_PTR->loadedAnms[a]->getSlot()

        // ENEMY CREATION AND ANM SCRIPT
    _ins(300, enmCreate) _noprint _z(sub) _f(x) _f(y) _S(hp)
                                  _S(score) _S(item) _args
    auto e = EnemyManager::GetInstance()->SpawnEnemy(sub,
        x + enemy.final_pos.pos.x, y + enemy.final_pos.pos.y, hp, score, item);
    if (e)
        COPY_VARS_TO(e);
    _ins(301, enmCreateA) _noprint _z(sub) _f(x) _f(y) _S(hp)
                                   _S(score) _S(item) _args
    auto e = EnemyManager::GetInstance()->SpawnEnemy(sub,
        x, y, hp, score, item);
    if (e)
        COPY_VARS_TO(e);
    _ins(304, enmCreateM) _noprint _z(sub) _f(x) _f(y) _S(hp)
                                   _S(score) _S(item) _args
    auto e = EnemyManager::GetInstance()->SpawnEnemy(sub,
        x + enemy.final_pos.pos.x, y + enemy.final_pos.pos.y, hp, score, item);
    if (e)
        COPY_VARS_TO_M(e);
    _ins(305, enmCreateAM) _noprint _z(sub) _f(x) _f(y) _S(hp)
                                    _S(score) _S(item) _args
    auto e = EnemyManager::GetInstance()->SpawnEnemy(sub,
        x, y, hp, score, item);
    if (e)
        COPY_VARS_TO_M(e);
    _ins(309, enmCreateF) _noprint _z(sub) _f(x) _f(y) _S(hp)
                                   _S(score) _S(item) _args
    if (isBoss_) break;
    auto e = EnemyManager::GetInstance()->SpawnEnemy(sub,
        x + enemy.final_pos.pos.x, y + enemy.final_pos.pos.y, hp, score, item);
    if (e)
        COPY_VARS_TO(e);
    _ins(310, enmCreateAF) _noprint _z(sub) _f(x) _f(y) _S(hp)
                                    _S(score) _S(item) _args
    if (isBoss_) break;
    auto e = EnemyManager::GetInstance()->SpawnEnemy(sub,
        x, y, hp, score, item);
    if (e)
        COPY_VARS_TO(e);
    _ins(311, enmCreateMF) _noprint _z(sub) _f(x) _f(y) _S(hp)
                                    _S(score) _S(item) _args
    if (isBoss_) break;
    auto e = EnemyManager::GetInstance()->SpawnEnemy(sub,
        x + enemy.final_pos.pos.x, y + enemy.final_pos.pos.y, hp, score, item);
    if (e)
        COPY_VARS_TO_M(e);
    _ins(312, enmCreateAMF) _noprint _z(sub) _f(x) _f(y) _S(hp)
                                     _S(score) _S(item) _args
    if (isBoss_) break;
    auto e = EnemyManager::GetInstance()->SpawnEnemy(sub,
        x, y, hp, score, item);
    if (e)
        COPY_VARS_TO_M(e);

    _ins(302, anmSelect) _S(anmID) _args enemy.selectedAnmID = anmID;

    _ins(303, anmSetSpr) _S(slot) _S(scr) _args
        AnmManager::deleteVM(enemy.anmIds[slot].val);
    if (scr < 0) {
        enemy.anmIds[slot].val = 0;
    } else {
        int id = AnmManager::SpawnVM(ANM_SLOT(enemy.selectedAnmID), scr);
        if (AnmManager::getVM(id)) {
            // set invisible if intangible enemy,
            // set finalSpriteSize and anmslot0... if slot = 0
        } else {
            id = 0;
        }
        enemy.anmIds[slot].val = id;
    }

    _ins(306, anmSetMain) _S(slot) _S(scr) _args
        AnmManager::deleteVM(enemy.anmIds[slot].val);
    if (scr < 0) {
        enemy.anmIds[slot].val = 0;
    } else {
        int id = AnmManager::SpawnVM(ANM_SLOT(enemy.selectedAnmID), scr);
        if (AnmManager::getVM(id)) {
            // set invisible if intangible enemy,
            // set finalSpriteSize and anmslot0... if slot = 0
        } else {
            id = 0;
        }
        enemy.anmIds[slot].val = id;
    }
    if (slot == 0) {
        enemy.flags |= 0x100000;
        enemy.anm0anmID = enemy.selectedAnmID;
        enemy.anm0scr = scr;
        enemy.anmSetMain = scr;
        enemy.anmMainSubscr = 0;
    }

    _ins(307, anmPlay) _S(anm) _S(scr) _args
    auto vm = AnmManager::getVM(AnmManager::SpawnVM(ANM_SLOT(anm), scr));
    if (vm)
        vm->setEntityPos(enemy.abs_pos.pos.x,
                         enemy.abs_pos.pos.y, enemy.abs_pos.pos.z);

    _ins(308, anmPlayAbs) _S(anm) _S(scr) _args
    AnmManager::SpawnVM(ANM_SLOT(anm), scr);

    _ins(313, anmSelPlay) _S(slot) _args
    AnmManager::deleteVM(enemy.anmIds[slot].val);
    int id = AnmManager::SpawnVM(ANM_SLOT(enemy.selectedAnmID),
                                 enemy.anmSetMain + 5);
    if (AnmManager::getVM(id)) {
        // set invisible if intangible enemy,
        // set finalSpriteSize and anmslot0... if slot = 0
    } else {
        id = 0;
    }
    enemy.anmIds[slot].val = id;

    _ins(314, anmPlayHigh) _S(anm) _S(scr) _args
    auto vm = AnmManager::getVM(AnmManager::SpawnVM(ANM_SLOT(anm), scr));
    if (vm)
        vm->setEntityPos(enemy.abs_pos.pos.x,
                         enemy.abs_pos.pos.y, enemy.abs_pos.pos.z);

    _ins(315, anmPlayRotate) _S(anm) _S(scr) _f(rot) _args
    auto vm = AnmManager::getVM(AnmManager::SpawnVM(ANM_SLOT(anm), scr));
    if (vm)
        vm->setEntityPos(enemy.abs_pos.pos.x,
                         enemy.abs_pos.pos.y, enemy.abs_pos.pos.z);

    _ins(316, anm316) _S(slot) _S(scr) _args
        AnmManager::deleteVM(enemy.anmIds[slot].val);
    if (scr < 0) {
        enemy.anmIds[slot].val = 0;
    } else {
        int id = AnmManager::SpawnVM(ANM_SLOT(enemy.selectedAnmID),
                                     scr + enemy.anmSetMain + 5);
        if (AnmManager::getVM(id)) {
            // set invisible if intangible enemy,
            // set finalSpriteSize and anmslot0... if slot = 0
        } else {
            id = 0;
        }
        enemy.anmIds[slot].val = id;
    }

    _ins(317, anmSwitch) _S(slot) _S(switc) _args
    auto vm = AnmManager::getVM(enemy.anmIds[slot].val);
    if (vm)
        vm->interrupt(switc);

    _ins(318, anmReset)
        AnmManager::deleteVM(enemy.anmIds[0].val);
    enemy.anmIds[0].val = AnmManager::SpawnVM(0, enemy.anmSetMain);
    enemy.flags &= 0xffffffffffefffff;
    enemy.anm0scr = enemy.anmSetMain;
    enemy.anmMainSubscr = 0;
    enemy.anm0anmID = enemy.selectedAnmID;

    _ins(319, anmRotate) _S(slot) _f(angle) _args
    auto vm = AnmManager::getVM(enemy.anmIds[slot].val);
    if (vm)
        vm->rotation.z = angle;
    if (vm)
        vm->bitflags.rotated = 1;

    _ins(320, anmMove) _S(slot) _f(x) _f(y) _args
    enemy.anmPos[slot] = { x, y, 0 };

    _ins(321, enmMapleEnemy) _z(sub) _f(x) _f(y) _S(hp) _S(score)
                             _S(item) _args _notImpl;

    _ins(322, enm322) _S(a) _S(b) _args _notImpl;

    _ins(323, deathAnm) _S(anm) _S(scr) _args
    enemy.deathAnm = ANM_SLOT(anm);
    enemy.deathScr = scr;

    _ins(324, enmPos2) _notImpl;

    _ins(325, anmCol) _S(slot) _S(r) _S(g) _S(b) _args
    auto vm = AnmManager::getVM(enemy.anmIds[slot].val);
    if (vm)
        vm->color_1 = { (uint8_t)r, (uint8_t)g, (uint8_t)b, vm->color_1.a };

    _ins(326, anmColT) _S(slot) _S(t) _S(m) _S(r) _S(g) _S(b) _args
    auto vm = AnmManager::getVM(enemy.anmIds[slot].val);
    if (vm)
        vm->rgb1_i.start({ vm->color_1.r, vm->color_1.g, vm->color_1.b },
                         { (uint8_t)r, (uint8_t)g, (uint8_t)b }, t, m);

    _ins(327, anmAlpha) _S(slot) _S(a) _args
    auto vm = AnmManager::getVM(enemy.anmIds[slot].val);
    if (vm)
        vm->color_1.a = a;

    _ins(328, anmAlphaT) _S(slot) _S(t) _S(m) _S(a) _args
    auto vm = AnmManager::getVM(enemy.anmIds[slot].val);
    if (vm)
        vm->alpha1_i.start(vm->color_1.a, a, t, m);

    _ins(329, anmScale) _S(slot) _f(x) _f(y) _args
    auto vm = AnmManager::getVM(enemy.anmIds[slot].val);
    if (vm)
        vm->setScale(x, y);
    if (vm)
        vm->bitflags.scaled = true;

    _ins(330, anmScaleT) _S(slot) _S(t) _S(m) _f(x) _f(y) _args
    auto vm = AnmManager::getVM(enemy.anmIds[slot].val);
    if (vm)
        vm->scale_i.start(vm->scale, { x, y }, t, m);

    _ins(331, anmAlpha2) _S(slot) _S(a) _args
    auto vm = AnmManager::getVM(enemy.anmIds[slot].val);
    if (vm)
        vm->color_2.a = a;

    _ins(332, anmAlpha2T) _S(slot) _S(t) _S(m) _S(a) _args
    auto vm = AnmManager::getVM(enemy.anmIds[slot].val);
    if (vm)
        vm->alpha2_i.start(vm->color_2.a, a, t, m);
    if (vm)
        vm->bitflags.colmode = 1;

    _ins(333, anmPosT) _S(slot) _S(t) _S(m) _f(x) _f(y) _args
    auto vm = AnmManager::getVM(enemy.anmIds[slot].val);
    if (vm)
        vm->pos_i.start(vm->pos, { x, y, 0.f }, t, m);

    _ins(334, anm334) _S(a) _args _notImpl;
    // effect and anm 508 related, seiga lightning ?

    _ins(335, anmScale2) _S(slot) _f(x) _f(y) _args
    auto vm = AnmManager::getVM(enemy.anmIds[slot].val);
    if (vm)
        vm->bitflags.scaled = 1;
    if (vm)
        vm->scale_2 = { x, y };

    _ins(336, anmLayer) _S(slot) _S(layer) _args
    auto vm = AnmManager::getVM(enemy.anmIds[slot].val);
    if (vm)
        vm->setLayer(layer);

    _ins(337, anmBM_16_anmPlayPos)
    if (TOUHOU_VERSION < 16) { _S(anm) _S(scr) _f(x) _f(y) _f(z) _args
        AnmManager::getVM(AnmManager::SpawnVM(ANM_SLOT(anm), scr))->setPos2(x, y, z);
    } else { _S(slot) _S(bm) _args
        auto vm = AnmManager::getVM(enemy.anmIds[slot].val);
        if (vm)
            vm->bitflags.blendmode = bm;
    }

    _ins(338, anmPlayPos) _S(anm) _S(scr) _f(x) _f(y) _f(z) _args
    AnmManager::getVM(AnmManager::SpawnVM(ANM_SLOT(anm), scr))->setPos2(x, y, z);

    _ins(339, anm339) _S(a) _S(b) _S(c) _args
    auto vm = AnmManager::getVM(AnmManager::SpawnVM(a+7, b));
    if (vm) {
        for (int i = 0; i < c; i++)
            vm->update();
    }

    _ins(340, enmDelete) _S(id) _args
    Enemy* e = EnemyManager::GetInstance()->EnmFind(id);
    // should set the delete flag instead and be deleted by the enmMgr
    if (e)
        e->Die();

#ifndef __INCLUDE_PARTS__
    _insDefault return 0;
}
#endif
