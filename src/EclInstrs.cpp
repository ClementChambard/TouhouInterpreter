#include "./AnmOpener/AnmManager.h"
#include "./BulletManager.h"
#include "./EclContext.h"
#include "./EclInstrMacros.h"
#include "./Enemy.h"
#include "./EnemyManager.h"
#include "./GlobalData.h"
#include "./Hardcoded.h"
#include "./Laser/LaserManager.h"
#include "./Player.h"
#include "./Gui.hpp"
#include "./StdOpener/Stage.hpp"
#include "./Spellcard.h"
#include "./Supervisor.h"
#include "./EclInstrs/EclFuncs.hpp"
#include "./AsciiPopupManager.hpp"
#include "./ScreenEffect.hpp"
#include <math/Random.h>
#include <NSEngine.hpp>

#define __INCLUDE_PARTS__

int Enemy::execInstr(EclRunContext_t* cont, const EclRawInstr_t* instr) {
    _insNop

#include "./EclInstrs/EclInstrs000.hpp"
#include "./EclInstrs/EclInstrs300.hpp"
#include "./EclInstrs/EclInstrs400.hpp"
#include "./EclInstrs/EclInstrs500.hpp"
#include "./EclInstrs/EclInstrs600.hpp"
#include "./EclInstrs/EclInstrs700.hpp"
#include "./EclInstrs/EclInstrs800.hpp"

        _insDefault

        return 0;
}
