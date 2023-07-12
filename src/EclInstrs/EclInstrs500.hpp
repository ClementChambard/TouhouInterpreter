#ifndef __INCLUDE_PARTS__
#include "../AnmOpener/AnmManager.h"
#include "../BulletManager.h"
#include "../EclContext.h"
#include "../EclInstrMacros.h"
#include "../Enemy.h"
#include "../EnemyManager.h"
#include "../GlobalData.h"
#include "../Laser/LaserManager.h"
#include "../Gui.hpp"
#include "../Spellcard.h"
#include <math/Random.h>
#define PRINT false
inline int Enemy::execInstr(EclRunContext_t* cont, const EclRawInstr_t* instr) {
    _insNop
#endif

    // Enemy Properties management
    _ins(500, setHurtbox) _f(w) _f(h) _args
    enemy.hurtbox_size = { w, h };
    // sets some vars and call a func, see at th17.exe::423af2

    _ins(501, setHitbox) _f(w) _f(h) _args
    enemy.hitbox_size = { w, h };

    _ins(502, flagSet) _S(n) _args
    enemy.flags |= n;
    // do some wierd thing : investigate

    _ins(503, flagClear) _S(n) _args
    enemy.flags &= (~n);
    // same as above

    _ins(504, moveLimit) _f(x) _f(y) _f(w) _f(h) _args
    enemy.moveLimitPos = { x, y };
    enemy.moveLimitSize = { w, h };
    enemy.flags |= 0x20000;

    _ins(505, moveLimitReset) _args
    enemy.flags &= 0xfffffffffffdffff;

    _ins(506, dropClear) _args
    for (int i = 0; i < 17; i++)
        enemy.drops.extra_counts[i] = 0;
    // check function th17.exe::FUN_41da60

    _ins(507, dropExtra) _S(t) _S(a) _args
    enemy.drops.extra_counts[t - 1] = a;

    _ins(508, dropArea) _f(w) _f(h) _args
    enemy.drops.area = { w, h };

    _ins(509, dropItems) _args
    if (GLOBALS.inner.SPELL_ID < 0)
        enemy.drops.eject_all_drops(enemy.final_pos.pos);
    // check if in spell practice first

    _ins(510, dropMain) _S(t) _args
    enemy.drops.main_type = t;

    _ins(511, lifeSet) _S(hp) _args
    enemy.life.current = enemy.life.max = hp;
    enemy.life.curx7 = hp * 7;

    _ins(512, setBoss) _S(a) _args
    ENEMY_MANAGER_PTR->flags &= 0xfffffffe;
    if (a < 0) {
      if (enemy.flags & 0x800000) {
        ENEMY_MANAGER_PTR->boss_ids[enemy.ownBossId] = 0;
      }
      enemy.flags &= 0xff7fffff;
    } else {
      enemy.flags |= 0x800000;
      ENEMY_MANAGER_PTR->boss_ids[a] = enemyId;
      enemy.ownBossId = a;
    }

    _ins(513, timerReset) _args
    enemy.timeInEcl = 0;

    _ins(514, setInterrupt) _S(slot) _S(hp) _S(t) _z(sub) _args
    enemy.interrupts[slot].life = hp;
    enemy.interrupts[slot].time = t;
    enemy.interrupts[slot].subNext = enemy.interrupts[slot].subTimeout = sub;

    _ins(515, setInvuln) _S(t) _args
    enemy.invFrame = t;

    _ins(516, playSound) _S(id) _args _notImpl;

    _ins(517, setScreenShake) _S(t) _S(s) _S(e) _args _notImpl;

    _ins(518, dialogueRead) _S(i) _args _notImpl;

    _ins(519, dialogueWait) _args _notImpl;

    _ins(520, bossWait) _args
    if (isBoss_) {
        cont->time--;
        _ret;
    }

    _ins(521, setTimeout) _S(slot) _z(sub) _args
    enemy.interrupts[slot].subTimeout = sub;

    _ins(522, spellEx) _S(i) _S(t) _S(ty) _z(name) _args
    Spellcard::GetInstance()
        ->Init(i, t, ty, name);
    enemy.life.isSpell = 1;
    enemy.life.curx7 = enemy.life.current * 7;

    _ins(523, spellEnd) _args
    Spellcard::GetInstance()->Stop();
    enemy.life.isSpell = 0;

    _ins(524, setChapter) _S(i) _args _notImpl;

    _ins(525, enmKillAll) _args
    EnemyManager::GetInstance()->EnmKillAll(this);

    _ins(526, etProtectRange) _f(r) _args
    enemy.etProtRange = r * r;

    _ins(527, lifeMarker) _S(slot) _f(hp) _S(col) _args
    GUI_PTR->boss_bars[0].markers[slot*2] = hp/enemy.life.max;
    GUI_PTR->boss_bars[0].markers[slot*2+1] = col;

    _ins(528, spellUnused) _S(i) _S(t) _S(ty) _z(name) _args _notImpl;

    _ins(529, rankF3) _rf(var) _f(a) _f(b) _f(c) _args
    // TODO(ClementChambard): real
    var = a;

    _ins(530, rankF5) _rf(var) _f(a) _f(b) _f(c) _f(d) _f(e) _args
    // TODO(ClementChambard): real
    var = a;

    _ins(531, rankF2) _rf(var) _f(a) _f(b) _args
    // TODO(ClementChambard): real
    var = a;

    _ins(532, rankI3) _rS(var) _S(a) _S(b) _S(c) _args
    // TODO(ClementChambard): real
    var = a;

    _ins(533, rankI5) _rS(var) _S(a) _S(b) _S(c) _S(d) _S(e) _args
    // TODO(ClementChambard): real
    var = a;

    _ins(534, rankI2) _rS(var) _S(a) _S(b) _args
    // TODO(ClementChambard): real
    var = a;

    _ins(535, diffI) _rS(v) _S(E) _S(N) _S(H) _S(L) _args
    v = Diff(E, N, H, L);

    _ins(536, diffF) _rf(v) _f(E) _f(N) _f(H) _f(L) _args
    v = Diff(E, N, H, L);

    _ins(537, spell) _S(i) _S(t) _S(ty) _z(name) _args
    Spellcard::GetInstance()->Init(i + GLOBALS.inner.DIFFICULTY,
            t, ty, name);
    enemy.life.isSpell = 1;
    enemy.life.curx7 = enemy.life.current * 7;

    _ins(538, spell2) _S(i) _S(t) _S(ty) _z(name) _args
    i -= 1;
    Spellcard::GetInstance()->Init(i + GLOBALS.inner.DIFFICULTY,
            t, ty, name);
    enemy.life.isSpell = 1;
    enemy.life.curx7 = enemy.life.current * 7;

    _ins(539, spell3) _S(i) _S(t) _S(ty) _z(name) _args
    i -= 2;
    Spellcard::GetInstance()->Init(i + GLOBALS.inner.DIFFICULTY,
            t, ty, name);
    enemy.life.isSpell = 1;
    enemy.life.curx7 = enemy.life.current * 7;

    _ins(540, stars) _S(n) _args
        GUI_PTR->stars_nb = n;

    _ins(541, noHbDur) _S(t) _args
    enemy.noHbFrame = t;

    _ins(542, spellTimeout) _args
    Spellcard::GetInstance()->SetTimeout();
    enemy.life.isSpell = 1;

    _ins(543, unknown543) _args _alert
    // call th17.exe::FUN_41dff0

    _ins(544, unknown544) _S(a) _args _alert
    enemy.flags = (enemy.flags ^ (enemy.flags ^ (a << 0x1b))) & 0x8000000;

    _ins(545, laserCancel) _args
    LASER_MANAGER_PTR->cancel_all(0);

    _ins(546, bombShield) _S(s) _S(scr) _args
    enemy.bombShieldOnAnmMain = scr;
    enemy.bombShieldOffAnmMain = enemy.anm0scr;
    enemy.flags &= 0xffffffffdffffffe;
    enemy.flags = (enemy.flags ^ (enemy.flags ^ (s << 0x1c))) & 0x10000000;

    _ins(547, gameSpeed) _f(s) _args _alert;

    _ins(548, diffWait) _S(e) _S(n) _S(h) _S(l) _args
    context.currentContext->time -= Diff(e, n, h, l);

    _ins(549, unknown549) _S(a) _args _alert
    enemy.flags = 0x80000000 | (enemy.flags & 0xffffffff7fffffff);

    _ins(550, unknown550) _S(a) _args _alert
    enemy.ecl550 = a;

    _ins(551, unknown551) _S(a) _args _notImpl;

    _ins(552, zIndex) _S(l) _args
    enemy.anmLayers = l;

    _ins(553, hitSound) _S(s) _args
    enemy.hitSnd = s;

    _ins(554, logo) _args
    AnmManager::getVM(AnmManager::SpawnVM(6, 0));

    _ins(555, enmAlive) _rS(var) _S(id) _args
    var = EnemyManager::GetInstance()->EnmFind(id) != nullptr;

    _ins(556, setDeath) _z(sub) _args
    enemy.setDeath = sub;

    _ins(557, fogTime) _S(t) _S(m) _S(c) _f(s) _f(e) _args _notImpl;

    _ins(558, flagMirror) _S(state) _args
    enemy.abs_pos.flags = state;

    _ins(559, enmLimit) _S(n) _args
    EnemyManager::GetInstance()->enemy_limit = n;

    _ins(560, unknown560) _f(r) _f(s) _args
    BulletManager::GetInstance()->set_bounce_rect(r, s);

    _ins(561, die) _args
    Die();

    _ins(562, dropItemsSp) _args
    enemy.drops.eject_all_drops(enemy.final_pos.pos);
    // spawn item even in spell practice

    _ins(563, hbRect) _S(state) _args
    enemy.flags &= ~(1 << 12);
    enemy.flags |= state << 12;

    _ins(564, hitboxRotate) _f(a) _args _notImpl;

    _ins(565, bombInv) _f(mul) _args
    enemy.bombDmgMul = mul;

    _ins(566, unknown566) _args _notImpl;

    _ins(567, unknown567) _S(a) _args _notImpl;

    _ins(568, spellMode) _S(state) _args _notImpl;

    _ins(569, unknown569) _S(a) _args _notImpl;

    _ins(570, unknown570) _args _notImpl;

    _ins(571, unknown571) _args _notImpl;

    _ins(572, lifeNow) _S(hp) _args
    enemy.life.current = hp;
    enemy.life.curx7 = hp * 7;

#ifndef __INCLUDE_PARTS__
    _insDefault

        return 0;
}
#endif
