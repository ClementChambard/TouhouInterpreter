#ifndef __INCLUDE_PARTS__
#include "../EclInstrMacros.h"
#include "../EclContext.h"
#include "../Enemy.h"
#include "../EnemyManager.h"
#include "../Spellcard.h"
#include "../AnmOpener/AnmManager.h"
#include <math/Random.h>
#define PRINT false
int Enemy::execInstr(EclRunContext_t* cont, EclRawInstr_t* instr)
{
    _insNop
#endif



    _ins(800,      enmCall) _S(id) _z(sub)               _args
        auto e = EnemyManager::GetInstance()->EnmFind(id);
        if (e) e->Init(sub);
        // clear async contexts
        // reset ecl
        // set sub reset

    _ins(801,       enmPos) _f(vx) _f(vy)  _S(id)        _args
        auto e = EnemyManager::GetInstance()->EnmFind(id);
        if (e)
        {
            vx = e->enemy.final_pos.pos.x;
            vy = e->enemy.final_pos.pos.y;
        }

    _ins(802, broadcastInt) _S(id)                       _args

        for (int i = 0; i < 4; i++) // check this again just in case
        {
            auto e = EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->data.boss_ids[i]);
            if (e) e->enemy.interrupts[id].time = 1;
        }



#ifndef __INCLUDE_PARTS__
    _insDefault

  return 0;
}
#endif
