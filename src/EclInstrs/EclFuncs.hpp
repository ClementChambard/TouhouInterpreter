#ifndef ECLFUNCS_INCLUDED_H
#define ECLFUNCS_INCLUDED_H

#include "../Enemy.h"

extern int (*ECL_SET_FUNC[20])(EnemyData*);
extern int (*ECL_EXTDMG_FUNC[10])(EnemyData*, int);
extern int (*ECL_HITBOX_FUNC[10])(EnemyData*);

#endif // !ECLFUNCS_INCLUDED_H
