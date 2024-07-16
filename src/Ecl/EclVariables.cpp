#include "./Enemy.h"
#include "./EnemyManager.h"
#include "../GlobalData.h"
#include "../GoastManager.h"
#include "../Player.h"
#include <math/math.hpp>

//////// TODO: some variables are missing ///////////
//////// For the boss variables, do some check //////

#define VAR(x) context.currentContext->stack.data[\
        context.currentContext->stack.baseOffset + x]

int32_t Enemy::checkVarI(int32_t id) {
    if (id == -10000)
        return random();
    if (id == -9999)
        return Random::Float01();
    if (id == -9998)
        return Random::Angle();
    if (id == -9997)
        return enemy.final_pos.pos.x;
    if (id == -9996)
        return enemy.final_pos.pos.y;
    if (id == -9995)
        return enemy.abs_pos.pos.x;
    if (id == -9994)
        return enemy.abs_pos.pos.y;
    if (id == -9993)
        return enemy.rel_pos.pos.x;
    if (id == -9992)
        return enemy.rel_pos.pos.y;
    if (id == -9991)
        return PLAYER_PTR->inner.pos.x;
    if (id == -9990)
        return PLAYER_PTR->inner.pos.y;
    if (id == -9989)
        return math::point_direction(enemy.final_pos.pos.x,
                                     enemy.final_pos.pos.y,
                                     PLAYER_PTR->inner.pos.x,
                                     PLAYER_PTR->inner.pos.y);
    if (id == -9988)
        return context.currentContext->time;
    if (id == -9987)
        return Random::Floatm11();
    if (id == -9986)
        return GLOBALS.timeout;
    if (id == -9985)
        return enemy.ecl_int_vars[0];
    if (id == -9984)
        return enemy.ecl_int_vars[1];
    if (id == -9983)
        return enemy.ecl_int_vars[2];
    if (id == -9982)
        return enemy.ecl_int_vars[3];
    if (id == -9981)
        return enemy.ecl_float_vars[0];
    if (id == -9980)
        return enemy.ecl_float_vars[1];
    if (id == -9979)
        return enemy.ecl_float_vars[2];
    if (id == -9978)
        return enemy.ecl_float_vars[3];
    if (id == -9977)
        return enemy.final_pos.pos.x;
    if (id == -9976)
        return enemy.final_pos.pos.y;
    if (id == -9975)
        return enemy.abs_pos.pos.x;
    if (id == -9974)
        return enemy.abs_pos.pos.y;
    if (id == -9973)
        return enemy.rel_pos.pos.x;
    if (id == -9972)
        return enemy.rel_pos.pos.y;
    if (id == -9971)
        return enemy.abs_pos.angle;
    if (id == -9970)
        return enemy.rel_pos.angle;
    if (id == -9969)
        return enemy.abs_pos.speed;
    if (id == -9968)
        return enemy.rel_pos.speed;
    if (id == -9967)
        return enemy.abs_pos.circle_radius;
    if (id == -9966)
        return enemy.rel_pos.circle_radius;
    if (id == -9965)
        return PLAYER_PTR->inner.pos.x;
    if (id == -9964)
        return PLAYER_PTR->inner.pos.y;
    if (id == -9963)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->
            boss_ids[0])->enemy.final_pos.pos.x;
    if (id == -9962)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->
            boss_ids[0])->enemy.final_pos.pos.y;
    if (id == -9961)
        return enemy.anmIds[0].val;
    if (id == -9960)
        return GLOBALS.inner.RANK;
    if (id == -9959)
        return GLOBALS.inner.DIFFICULTY;
    if (id == -9958)
        return enemy.final_pos.angle;
    if (id == -9957)
        return true;
    if (id == -9956)
        return math::point_direction(enemy.abs_pos.pos.x,
                                     enemy.abs_pos.pos.y,
                                     PLAYER_PTR->inner.pos.x,
                                     PLAYER_PTR->inner.pos.y);
    if (id == -9955)
        return math::point_direction(enemy.rel_pos.pos.x,
                                     enemy.rel_pos.pos.y,
                                     PLAYER_PTR->inner.pos.x,
                                     PLAYER_PTR->inner.pos.y);
    if (id == -9954)
        return enemy.life.current;
    if (id == -9953)
        return GLOBALS.inner.DIFFICULTY == 0;
    if (id == -9952)
        return GLOBALS.inner.DIFFICULTY == 1;
    if (id == -9951)
        return GLOBALS.inner.DIFFICULTY == 2;
    if (id == -9950)
        return GLOBALS.inner.DIFFICULTY == 3;
    if (id == -9949)
        return ENEMY_MANAGER_PTR->miss_count;
    if (id == -9948)
        return ENEMY_MANAGER_PTR->bomb_count;
    if (id == -9947)
        return ENEMY_MANAGER_PTR->can_still_capture_spell;
    if (id == -9946)
        return ENEMY_MANAGER_PTR->enemyCount;
    if (id == -9945)
        return GLOBALS.inner.SHOTTYPE;
    if (id == -9944)
        return math::point_distance(enemy.final_pos.pos.x,
                                    enemy.final_pos.pos.y,
                                    PLAYER_PTR->inner.pos.x,
                                    PLAYER_PTR->inner.pos.y);
    if (id == -9943)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_int_vars[0];
    if (id == -9942)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_int_vars[1];
    if (id == -9941)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_int_vars[2];
    if (id == -9940)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_int_vars[3];
    if (id == -9939)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_float_vars[0];
    if (id == -9938)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_float_vars[1];
    if (id == -9937)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_float_vars[2];
    if (id == -9936)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_float_vars[3];
    if (id == -9935)
        return enemy.ecl_float_vars[4];
    if (id == -9934)
        return enemy.ecl_float_vars[5];
    if (id == -9933)
        return enemy.ecl_float_vars[6];
    if (id == -9932)
        return enemy.ecl_float_vars[7];
    if (id == -9931)
        return ENEMY_MANAGER_PTR->last_enemy_id;
    if (id == -9930)
        return GLOBALS.inner.CURRENT_POWER;
    if (id == -9929)
        return GLOBALS.DS1;
    if (id == -9928)
        return GLOBALS.DS2;
    if (id == -9927)
        return GLOBALS.DS3;
    if (id == -9926)
        return ENEMY_MANAGER_PTR->ecl_int_vars[0];
    if (id == -9925)
        return ENEMY_MANAGER_PTR->ecl_int_vars[1];
    if (id == -9924)
        return ENEMY_MANAGER_PTR->ecl_int_vars[2];
    if (id == -9923)
        return ENEMY_MANAGER_PTR->ecl_int_vars[3];
    if (id == -9922)
        return ENEMY_MANAGER_PTR->ecl_float_vars[0];
    if (id == -9921)
        return ENEMY_MANAGER_PTR->ecl_float_vars[1];
    if (id == -9920)
        return ENEMY_MANAGER_PTR->ecl_float_vars[2];
    if (id == -9919)
        return ENEMY_MANAGER_PTR->ecl_float_vars[3];
    if (id == -9918)
        return ENEMY_MANAGER_PTR->ecl_float_vars[4];
    if (id == -9917)
        return ENEMY_MANAGER_PTR->ecl_float_vars[5];
    if (id == -9916)
        return ENEMY_MANAGER_PTR->ecl_float_vars[6];
    if (id == -9915)
        return ENEMY_MANAGER_PTR->ecl_float_vars[7];
    if (id == -9914)
        return enemyId;
    if (id == -9913)
        return GLOBALS.photoCount;
    if (id == -9912)
        return GLOBALS.DS3;
    if (id == -9911)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.final_pos.angle;
    if (id == -9910)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.final_pos.speed;
    if (id == -9909)
        return parentEnemyId;
    if (id == -9908)
        return ENEMY_MANAGER_PTR->killableEnemyCount();
    if (id == -9907)
        return GLOBALS.inner.SPELL_ID;
    if (id == -9906)
        return enemy.abs_pos.flags;
    if (id == -9905)
        return GLOBALS.inner.CURRENT_CHAPTER;
    if (id == -9904)
        return GLOBALS.inner.MISS_COUNT_GLOBAL;
    if (id == -9903)
        return GLOBALS.hyper;
    if (id == -9902)
        return GLOBALS.graze_recent;
    if (id == -9901)
        return GOAST_MANAGER_PTR ? GOAST_MANAGER_PTR->token_count : 0;
    if (id == -9900)
        return GLOBALS.hyperdying;
    if (id == -9899)
        return GLOBALS.achievement_mode;
    if (id >= 0)
        return VAR(id / 4).asInt;
    stackToRemove++;
    return context.currentContext->stack.data[
        context.currentContext->stack.stackOffset + id].asInt;
}

float Enemy::checkVarF(float id) {
    if (id == -10000.f)
        return random();
    if (id == -9999.f)
        return Random::Float01();
    if (id == -9998.f)
        return Random::Angle();
    if (id == -9997.f)
        return enemy.final_pos.pos.x;
    if (id == -9996.f)
        return enemy.final_pos.pos.y;
    if (id == -9995.f)
        return enemy.abs_pos.pos.x;
    if (id == -9994.f)
        return enemy.abs_pos.pos.y;
    if (id == -9993.f)
        return enemy.rel_pos.pos.x;
    if (id == -9992.f)
        return enemy.rel_pos.pos.y;
    if (id == -9991.f)
        return PLAYER_PTR->inner.pos.x;
    if (id == -9990.f)
        return PLAYER_PTR->inner.pos.y;
    if (id == -9989.f)
        return math::point_direction(enemy.final_pos.pos.x,
                                     enemy.final_pos.pos.y,
                                     PLAYER_PTR->inner.pos.x,
                                     PLAYER_PTR->inner.pos.y);
    if (id == -9988.f)
        return context.currentContext->time;
    if (id == -9987.f)
        return Random::Floatm11();
    if (id == -9986.f)
        return GLOBALS.timeout;
    if (id == -9985.f)
        return enemy.ecl_int_vars[0];
    if (id == -9984.f)
        return enemy.ecl_int_vars[1];
    if (id == -9983.f)
        return enemy.ecl_int_vars[2];
    if (id == -9982.f)
        return enemy.ecl_int_vars[3];
    if (id == -9981.f)
        return enemy.ecl_float_vars[0];
    if (id == -9980.f)
        return enemy.ecl_float_vars[1];
    if (id == -9979.f)
        return enemy.ecl_float_vars[2];
    if (id == -9978.f)
        return enemy.ecl_float_vars[3];
    if (id == -9977.f)
        return enemy.final_pos.pos.x;
    if (id == -9976.f)
        return enemy.final_pos.pos.y;
    if (id == -9975.f)
        return enemy.abs_pos.pos.x;
    if (id == -9974.f)
        return enemy.abs_pos.pos.y;
    if (id == -9973.f)
        return enemy.rel_pos.pos.x;
    if (id == -9972.f)
        return enemy.rel_pos.pos.y;
    if (id == -9971.f)
        return enemy.abs_pos.angle;
    if (id == -9970.f)
        return enemy.rel_pos.angle;
    if (id == -9969.f)
        return enemy.abs_pos.speed;
    if (id == -9968.f)
        return enemy.rel_pos.speed;
    if (id == -9967.f)
        return enemy.abs_pos.circle_radius;
    if (id == -9966.f)
        return enemy.rel_pos.circle_radius;
    if (id == -9965.f)
        return PLAYER_PTR->inner.pos.x;
    if (id == -9964.f)
        return PLAYER_PTR->inner.pos.y;
    if (id == -9963.f)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.final_pos.pos.x;
    if (id == -9962.f)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.final_pos.pos.y;
    if (id == -9961.f)
        return enemy.anmIds[0].val;
    if (id == -9960.f)
        return GLOBALS.inner.RANK;
    if (id == -9959.f)
        return GLOBALS.inner.DIFFICULTY;
    if (id == -9958.f)
        return enemy.final_pos.angle;
    if (id == -9957.f)
        return true;
    if (id == -9956.f)
        return math::point_direction(enemy.abs_pos.pos.x,
                                     enemy.abs_pos.pos.y,
                                     PLAYER_PTR->inner.pos.x,
                                     PLAYER_PTR->inner.pos.y);
    if (id == -9955.f)
        return math::point_direction(enemy.rel_pos.pos.x,
                                     enemy.rel_pos.pos.y,
                                     PLAYER_PTR->inner.pos.x,
                                     PLAYER_PTR->inner.pos.y);
    if (id == -9954.f)
        return enemy.life.current;
    if (id == -9953.f)
        return GLOBALS.inner.DIFFICULTY == 0;
    if (id == -9952.f)
        return GLOBALS.inner.DIFFICULTY == 1;
    if (id == -9951.f)
        return GLOBALS.inner.DIFFICULTY == 2;
    if (id == -9950.f)
        return GLOBALS.inner.DIFFICULTY == 3;
    if (id == -9949.f)
        return ENEMY_MANAGER_PTR->miss_count;
    if (id == -9948.f)
        return ENEMY_MANAGER_PTR->bomb_count;
    if (id == -9947.f)
        return ENEMY_MANAGER_PTR->can_still_capture_spell;
    if (id == -9946.f)
        return ENEMY_MANAGER_PTR->enemyCount;
    if (id == -9945.f)
        return GLOBALS.inner.SHOTTYPE;
    if (id == -9944.f)
        return math::point_distance(enemy.final_pos.pos.x,
                                    enemy.final_pos.pos.y,
                                    PLAYER_PTR->inner.pos.x,
                                    PLAYER_PTR->inner.pos.y);
    if (id == -9943.f)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_int_vars[0];
    if (id == -9942.f)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_int_vars[1];
    if (id == -9941.f)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_int_vars[2];
    if (id == -9940.f)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_int_vars[3];
    if (id == -9939.f)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_float_vars[0];
    if (id == -9938.f)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_float_vars[1];
    if (id == -9937.f)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_float_vars[2];
    if (id == -9936.f)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_float_vars[3];
    if (id == -9935.f)
        return enemy.ecl_float_vars[4];
    if (id == -9934.f)
        return enemy.ecl_float_vars[5];
    if (id == -9933.f)
        return enemy.ecl_float_vars[6];
    if (id == -9932.f)
        return enemy.ecl_float_vars[7];
    if (id == -9931.f)
        return ENEMY_MANAGER_PTR->last_enemy_id;
    if (id == -9930.f)
        return GLOBALS.inner.CURRENT_POWER;
    if (id == -9929.f)
        return GLOBALS.DS1;
    if (id == -9928.f)
        return GLOBALS.DS2;
    if (id == -9927.f)
        return GLOBALS.DS3;
    if (id == -9926.f)
        return ENEMY_MANAGER_PTR->ecl_int_vars[0];
    if (id == -9925.f)
        return ENEMY_MANAGER_PTR->ecl_int_vars[1];
    if (id == -9924.f)
        return ENEMY_MANAGER_PTR->ecl_int_vars[2];
    if (id == -9923.f)
        return ENEMY_MANAGER_PTR->ecl_int_vars[3];
    if (id == -9922.f)
        return ENEMY_MANAGER_PTR->ecl_float_vars[0];
    if (id == -9921.f)
        return ENEMY_MANAGER_PTR->ecl_float_vars[1];
    if (id == -9920.f)
        return ENEMY_MANAGER_PTR->ecl_float_vars[2];
    if (id == -9919.f)
        return ENEMY_MANAGER_PTR->ecl_float_vars[3];
    if (id == -9918.f)
        return ENEMY_MANAGER_PTR->ecl_float_vars[4];
    if (id == -9917.f)
        return ENEMY_MANAGER_PTR->ecl_float_vars[5];
    if (id == -9916.f)
        return ENEMY_MANAGER_PTR->ecl_float_vars[6];
    if (id == -9915.f)
        return ENEMY_MANAGER_PTR->ecl_float_vars[7];
    if (id == -9914.f)
        return enemyId;
    if (id == -9913.f)
        return GLOBALS.photoCount;
    if (id == -9912.f)
        return GLOBALS.DS3;
    if (id == -9911.f)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.final_pos.angle;
    if (id == -9910.f)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.final_pos.speed;
    if (id == -9909.f)
        return parentEnemyId;
    if (id == -9908.f)
        return ENEMY_MANAGER_PTR->killableEnemyCount();
    if (id == -9907.f)
        return GLOBALS.inner.SPELL_ID;
    if (id == -9906.f)
        return enemy.abs_pos.flags;
    if (id == -9905.f)
        return GLOBALS.inner.CURRENT_CHAPTER;
    if (id == -9904.f)
        return GLOBALS.inner.MISS_COUNT_GLOBAL;
    if (id == -9903.f)
        return GLOBALS.hyper;
    if (id == -9902.f)
        return GLOBALS.graze_recent;
    if (id == -9901.f)
        return GOAST_MANAGER_PTR ? GOAST_MANAGER_PTR->token_count : 0;
    if (id == -9900.f)
        return GLOBALS.hyperdying;
    if (id == -9899.f)
        return GLOBALS.achievement_mode;
    if (id >= 0.f)
        return VAR((int)id / 4).asFloat;
    stackToRemove++;
    return context.currentContext->stack.data[context.currentContext
        ->stack.stackOffset + static_cast<int>(id)].asFloat;
}

int32_t& Enemy::checkVarRI(int32_t id) {
    if (id == -9985)
        return enemy.ecl_int_vars[0];
    if (id == -9984)
        return enemy.ecl_int_vars[1];
    if (id == -9983)
        return enemy.ecl_int_vars[2];
    if (id == -9982)
        return enemy.ecl_int_vars[3];
    if (id == -9949)
        return ENEMY_MANAGER_PTR->miss_count;
    if (id == -9948)
        return ENEMY_MANAGER_PTR->bomb_count;
    if (id == -9947)
        return ENEMY_MANAGER_PTR->can_still_capture_spell;
    if (id == -9943)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_int_vars[0];
    if (id == -9942)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_int_vars[1];
    if (id == -9941)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_int_vars[2];
    if (id == -9940)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_int_vars[3];
    if (id == -9926)
        return ENEMY_MANAGER_PTR->ecl_int_vars[0];
    if (id == -9925)
        return ENEMY_MANAGER_PTR->ecl_int_vars[1];
    if (id == -9924)
        return ENEMY_MANAGER_PTR->ecl_int_vars[2];
    if (id == -9923)
        return ENEMY_MANAGER_PTR->ecl_int_vars[3];
    if (id >= 0)
        return VAR(id / 4).asInt; // Error: user will modify int variant but not float variant.
    return context.currentContext->int_vars[7];
}

float& Enemy::checkVarRF(float id) {
    if (id == -9981.f)
        return enemy.ecl_float_vars[0];
    if (id == -9980.f)
        return enemy.ecl_float_vars[1];
    if (id == -9979.f)
        return enemy.ecl_float_vars[2];
    if (id == -9978.f)
        return enemy.ecl_float_vars[3];
    if (id == -9939.f)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_float_vars[0];
    if (id == -9938.f)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_float_vars[1];
    if (id == -9937.f)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_float_vars[2];
    if (id == -9936.f)
        return ENEMY_MANAGER_PTR->EnmFind(
            ENEMY_MANAGER_PTR->boss_ids[0])
            ->enemy.ecl_float_vars[3];
    if (id == -9935.f)
        return enemy.ecl_float_vars[4];
    if (id == -9934.f)
        return enemy.ecl_float_vars[5];
    if (id == -9933.f)
        return enemy.ecl_float_vars[6];
    if (id == -9932.f)
        return enemy.ecl_float_vars[7];
    if (id == -9922.f)
        return ENEMY_MANAGER_PTR->ecl_float_vars[0];
    if (id == -9921.f)
        return ENEMY_MANAGER_PTR->ecl_float_vars[1];
    if (id == -9920.f)
        return ENEMY_MANAGER_PTR->ecl_float_vars[2];
    if (id == -9919.f)
        return ENEMY_MANAGER_PTR->ecl_float_vars[3];
    if (id == -9918.f)
        return ENEMY_MANAGER_PTR->ecl_float_vars[4];
    if (id == -9917.f)
        return ENEMY_MANAGER_PTR->ecl_float_vars[5];
    if (id == -9916.f)
        return ENEMY_MANAGER_PTR->ecl_float_vars[6];
    if (id == -9915.f)
        return ENEMY_MANAGER_PTR->ecl_float_vars[7];
    if (id >= 0)
        return VAR((int)id / 4).asFloat; // Error: user will modify float variant but not int variant.
    return context.currentContext->float_vars[7];
}
