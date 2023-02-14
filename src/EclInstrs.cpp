#include "EclContext.h"
#include "EclInstrMacros.h"
#include "Enemy.h"
#include "EnemyManager.h"
#include "Spellcard.h"
#include "BulletManager.h"
#include "GlobalData.h"
#include "Laser/LaserManager.h"
#include "AnmOpener/AnmManager.h"
#include "Hardcoded.h"
#include <math/Random.h>

#define PRINT false
#define __INCLUDE_PARTS__

void Enemy::execInstr(EclRunContext_t* cont, EclRawInstr_t* instr)
{
    _insNop

    #include "EclInstrs/EclInstrs000.cpart"
    #include "EclInstrs/EclInstrs300.cpart"
    #include "EclInstrs/EclInstrs400.cpart"
    #include "EclInstrs/EclInstrs500.cpart"
    #include "EclInstrs/EclInstrs600.cpart"
    #include "EclInstrs/EclInstrs700.cpart"
    #include "EclInstrs/EclInstrs800.cpart"

    _insDefault
}
