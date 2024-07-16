#include "../Anm/AnmManager.h"
#include "../Bullet/BulletManager.h"
#include "./EclContext.h"
#include "./EclInstrMacros.h"
#include "./Enemy.h"
#include "./EclFuncs.hpp"
#include "./EnemyManager.h"
#include "../GlobalData.h"
#include "../Hardcoded.h"
#include "../Laser/LaserManager.h"
#include "../Player.h"
#include "../Gui.hpp"
#include "../Std/Stage.hpp"
#include "../Spellcard.h"
#include "../Supervisor.h"
#include "../AsciiPopupManager.hpp"
#include "../ScreenEffect.hpp"
#include <math/math.hpp>
#include <asserts.h>
#include <NSEngine.hpp>

#define __INCLUDE_PARTS__

int Enemy::execInstr(EclRunContext_t* cont, const EclRawInstr_t* instr) {
    _insNop

#include "./EclInstrs000.hpp"
#include "./EclInstrs300.hpp"
#include "./EclInstrs400.hpp"
#include "./EclInstrs500.hpp"
#include "./EclInstrs600.hpp"
#include "./EclInstrs700.hpp"
#include "./EclInstrs800.hpp"

    _insDefault

    return 0;
}
