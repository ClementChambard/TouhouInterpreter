#include "./EclFuncs.hpp"
#include "../Player.h"
#include "../Bullet/BulletManager.h"
#include "../Anm/AnmManager.h"
#include <math/math.hpp>

int (*ECL_SET_FUNC[20])(EnemyData*) = {
    nullptr,
    [](EnemyData* enm) -> int {
        Bullet* b;
        BULLET_MANAGER_PTR->iter_current =
            BULLET_MANAGER_PTR->tick_list_head.next;
        if (!BULLET_MANAGER_PTR->iter_current) {
            BULLET_MANAGER_PTR->iter_next = nullptr;
            b = nullptr;
        } else {
            BULLET_MANAGER_PTR->iter_next =
                BULLET_MANAGER_PTR->iter_current->next;
            b = BULLET_MANAGER_PTR->iter_current->value;
        }
        float ang = ns::frandangle();
        while (b != NULL) {
            if ((enm->ecl_int_vars[1] * 100 <= b->__ex_react_a) &&
                (b->__ex_react_a < enm->ecl_int_vars[1] * 100 + 100)) {
                b->ex_index = 8;
                ns::vec2 dir =
                    ns::lengthdir_vec(enm->ecl_float_vars[0], ang);
                b->et_ex[8].slot = 0;
                b->et_ex[8].a = enm->ecl_int_vars[0];
                b->et_ex[8].b = 9;
                b->et_ex[8].r = enm->ecl_float_vars[1] + dir.x;
                b->et_ex[8].s = enm->ecl_float_vars[2] + dir.y;
                b->et_ex[8].type = 0x20000;
                ang += enm->ecl_float_vars[3];
                ns::angle_normalize(ang);
            }
            BULLET_MANAGER_PTR->iter_current =
                BULLET_MANAGER_PTR->iter_next;
            if (!BULLET_MANAGER_PTR->iter_current) {
                BULLET_MANAGER_PTR->iter_next = nullptr;
                b = nullptr;
            } else {
                BULLET_MANAGER_PTR->iter_next =
                    BULLET_MANAGER_PTR->iter_current->next;
                b = BULLET_MANAGER_PTR->iter_current->value;
            }
        }
        return 0;
    },
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr
};

int (*ECL_EXTDMG_FUNC[10])(EnemyData*, int) = {
    nullptr,
    [](EnemyData* enm, int dmg) -> int {
        if (enm->ecl_int_vars[3] > 0) {
            dmg += enm->ecl_int_vars[3];
            enm->ecl_int_vars[3] = 0;
        }
        return dmg;
    },
    [](EnemyData* enm, int dmg) -> int {
        uint a = 0;
        auto vm1 = anm::get_vm(enm->anmIds[1]);
        if (!vm1) {
            enm->anmIds[1] = 0;
        } else {
            ns::vec2 scale = vm1->scale * ns::vec2(192, 32);
            ns::vec3 pos = enm->final_pos.pos;
            ns::vec3 dmgpos = {0, 0, 0};
            pos.y += 24.0;
            dmg += applyDamage(pos, &scale, 0.f, &a, dmgpos, 0,
                        enm->enemy->enemyId, vm1->rotation.z);
            pos.y += 32.0;
            dmg += applyDamage(pos, nullptr, 48.f, &a, dmgpos, 0,
                        enm->enemy->enemyId, 0.f);
        }
        return dmg;
    },
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr
};

int (*ECL_HITBOX_FUNC[10])(EnemyData*) = {
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr
};
