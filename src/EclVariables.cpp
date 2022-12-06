#include "Enemy.h"
#include "EnemyManager.h"
#include "GlobalData.h"
#include <math/math.h>
#include <math/Random.h>

//////// TODO: some variables are missing ///////////
//////// For the boss variables, do some check //////

#define VAR(x) context.currentContext->stack.data[context.currentContext->variable_stack_offset[x]]

int32_t Enemy::checkVarI(int32_t id)
{
    if (id == -10000) return random();
    if (id == -9988) return context.currentContext->time;
    if (id == -9986) return Globals::get()->timeout;
    if (id == -9985) return enemy.ecl_int_vars[0];
    if (id == -9984) return enemy.ecl_int_vars[1];
    if (id == -9983) return enemy.ecl_int_vars[2];
    if (id == -9982) return enemy.ecl_int_vars[3];
    if (id == -9961) return enemy.anmIds[0].val;
    if (id == -9960) return Globals::get()->rank;
    if (id == -9959) return Globals::get()->difficulty;
    if (id == -9957) return true;
    if (id == -9954) return enemy.life.current;
    if (id == -9953) return Globals::get()->difficulty == 0;
    if (id == -9952) return Globals::get()->difficulty == 1;
    if (id == -9951) return Globals::get()->difficulty == 2;
    if (id == -9950) return Globals::get()->difficulty == 3;
    if (id == -9949) return EnemyManager::GetInstance()->data.miss_count;
    if (id == -9948) return EnemyManager::GetInstance()->data.bomb_count;
    if (id == -9947) return EnemyManager::GetInstance()->data.can_still_capture_spell;
    if (id == -9946) return EnemyManager::GetInstance()->enemyCount;
    if (id == -9945) return Globals::get()->shottype;
    if (id == -9943) return EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->data.boss_ids[0])->enemy.ecl_int_vars[0];
    if (id == -9942) return EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->data.boss_ids[0])->enemy.ecl_int_vars[1];
    if (id == -9941) return EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->data.boss_ids[0])->enemy.ecl_int_vars[2];
    if (id == -9940) return EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->data.boss_ids[0])->enemy.ecl_int_vars[3];
    if (id == -9931) return EnemyManager::GetInstance()->data.last_enemy_id;
    if (id == -9930) return Globals::get()->power;
    if (id == -9929) return Globals::get()->DS1;
    if (id == -9928) return Globals::get()->DS2;
    if (id == -9927) return Globals::get()->DS3;
    if (id == -9926) return EnemyManager::GetInstance()->data.ecl_int_vars[0];
    if (id == -9925) return EnemyManager::GetInstance()->data.ecl_int_vars[1];
    if (id == -9924) return EnemyManager::GetInstance()->data.ecl_int_vars[2];
    if (id == -9923) return EnemyManager::GetInstance()->data.ecl_int_vars[3];
    if (id == -9914) return enemyId;
    if (id == -9913) return Globals::get()->photoCount;
    if (id == -9912) return Globals::get()->DS3;
    if (id == -9909) return parentEnemyId;
    if (id == -9908) return EnemyManager::GetInstance()->killableEnemyCount();
    if (id == -9907) return Globals::get()->spellID;
    if (id == -9906) return enemy.abs_pos.flags;
    if (id == -9905) return Globals::get()->chapter;
    if (id == -9904) return Globals::get()->miss_count_total;
    if (id == -9903) return Globals::get()->hyper;
    if (id == -9902) return Globals::get()->graze_recent;
    if (id == -9901) return Globals::get()->goasts;
    if (id == -9900) return Globals::get()->hyperdying;
    if (id == -9899) return Globals::get()->achievement_mode;
    if (id >= 0) return VAR(id/4).asInt;
    if (id < 0) { stackToRemove++; return context.currentContext->stack.data[context.currentContext->stack.stackOffset+id].asInt; }
    return id;
}

float Enemy::checkVarF(float id)
{
    if (id == -9999.f) return Random::Float01();
    if (id == -9998.f) return Random::Angle();
    if (id == -9997.f) return enemy.final_pos.pos.x;
    if (id == -9996.f) return enemy.final_pos.pos.y;
    if (id == -9995.f) return enemy.abs_pos.pos.x;
    if (id == -9994.f) return enemy.abs_pos.pos.y;
    if (id == -9993.f) return enemy.rel_pos.pos.x;
    if (id == -9992.f) return enemy.rel_pos.pos.y;
    if (id == -9991.f) return Globals::get()->playerX;
    if (id == -9990.f) return Globals::get()->playerY;
    if (id == -9989.f) return math::point_direction(enemy.final_pos.pos.x, enemy.final_pos.pos.y, Globals::get()->playerX, Globals::get()->playerY);
    if (id == -9987.f) return Random::Floatm11();
    if (id == -9981.f) return enemy.ecl_float_vars[0];
    if (id == -9980.f) return enemy.ecl_float_vars[1];
    if (id == -9979.f) return enemy.ecl_float_vars[2];
    if (id == -9978.f) return enemy.ecl_float_vars[3];
    if (id == -9977.f) return enemy.final_pos.pos.x;
    if (id == -9976.f) return enemy.final_pos.pos.y;
    if (id == -9975.f) return enemy.abs_pos.pos.x;
    if (id == -9974.f) return enemy.abs_pos.pos.y;
    if (id == -9973.f) return enemy.rel_pos.pos.x;
    if (id == -9972.f) return enemy.rel_pos.pos.y;
    if (id == -9971.f) return enemy.abs_pos.angle;
    if (id == -9970.f) return enemy.rel_pos.angle;
    if (id == -9969.f) return enemy.abs_pos.speed;
    if (id == -9968.f) return enemy.rel_pos.speed;
    if (id == -9967.f) return enemy.abs_pos.circle_radius;
    if (id == -9966.f) return enemy.rel_pos.circle_radius;
    if (id == -9965.f) return Globals::get()->playerX;
    if (id == -9964.f) return Globals::get()->playerY;
    if (id == -9963.f) return EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->data.boss_ids[0])->enemy.final_pos.pos.x;
    if (id == -9962.f) return EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->data.boss_ids[0])->enemy.final_pos.pos.y;
    if (id == -9958.f) return enemy.final_pos.angle;
    if (id == -9956.f) return math::point_direction(enemy.abs_pos.pos.x, enemy.abs_pos.pos.y, Globals::get()->playerX, Globals::get()->playerY);
    if (id == -9955.f) return math::point_direction(enemy.rel_pos.pos.x, enemy.rel_pos.pos.y, Globals::get()->playerX, Globals::get()->playerY);
    if (id == -9944.f) return math::point_distance(enemy.final_pos.pos.x, enemy.final_pos.pos.y, Globals::get()->playerX, Globals::get()->playerY);
    if (id == -9939.f) return EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->data.boss_ids[0])->enemy.ecl_float_vars[0];
    if (id == -9938.f) return EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->data.boss_ids[0])->enemy.ecl_float_vars[1];
    if (id == -9937.f) return EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->data.boss_ids[0])->enemy.ecl_float_vars[2];
    if (id == -9936.f) return EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->data.boss_ids[0])->enemy.ecl_float_vars[3];
    if (id == -9935.f) return enemy.ecl_float_vars[4];
    if (id == -9934.f) return enemy.ecl_float_vars[5];
    if (id == -9933.f) return enemy.ecl_float_vars[6];
    if (id == -9932.f) return enemy.ecl_float_vars[7];
    if (id == -9922.f) return EnemyManager::GetInstance()->data.ecl_float_vars[0];
    if (id == -9921.f) return EnemyManager::GetInstance()->data.ecl_float_vars[1];
    if (id == -9920.f) return EnemyManager::GetInstance()->data.ecl_float_vars[2];
    if (id == -9919.f) return EnemyManager::GetInstance()->data.ecl_float_vars[3];
    if (id == -9918.f) return EnemyManager::GetInstance()->data.ecl_float_vars[4];
    if (id == -9917.f) return EnemyManager::GetInstance()->data.ecl_float_vars[5];
    if (id == -9916.f) return EnemyManager::GetInstance()->data.ecl_float_vars[6];
    if (id == -9915.f) return EnemyManager::GetInstance()->data.ecl_float_vars[7];
    if (id == -9911.f) return EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->data.boss_ids[0])->enemy.final_pos.angle;
    if (id == -9910.f) return EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->data.boss_ids[0])->enemy.final_pos.speed;
    if (id >= 0.f) return VAR((int)id/4).asFloat;
    if (id < 0.f) { stackToRemove++; return context.currentContext->stack.data[context.currentContext->stack.stackOffset+(int)id].asFloat; }
    return id;
}

int32_t& Enemy::checkVarRI(int32_t id)
{
    if (id == -9985) return enemy.ecl_int_vars[0];
    if (id == -9984) return enemy.ecl_int_vars[1];
    if (id == -9983) return enemy.ecl_int_vars[2];
    if (id == -9982) return enemy.ecl_int_vars[3];
    if (id == -9949) return EnemyManager::GetInstance()->data.miss_count;
    if (id == -9948) return EnemyManager::GetInstance()->data.bomb_count;
    if (id == -9947) return EnemyManager::GetInstance()->data.can_still_capture_spell;
    if (id == -9943) return EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->data.boss_ids[0])->enemy.ecl_int_vars[0];
    if (id == -9942) return EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->data.boss_ids[0])->enemy.ecl_int_vars[1];
    if (id == -9941) return EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->data.boss_ids[0])->enemy.ecl_int_vars[2];
    if (id == -9940) return EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->data.boss_ids[0])->enemy.ecl_int_vars[3];
    if (id == -9926) return EnemyManager::GetInstance()->data.ecl_int_vars[0];
    if (id == -9925) return EnemyManager::GetInstance()->data.ecl_int_vars[1];
    if (id == -9924) return EnemyManager::GetInstance()->data.ecl_int_vars[2];
    if (id == -9923) return EnemyManager::GetInstance()->data.ecl_int_vars[3];
    if (id >= 0) return VAR(id/4).asInt;
    return context.currentContext->int_vars[7];
}

float& Enemy::checkVarRF(float id)
{
    if (id == -9981.f) return enemy.ecl_float_vars[0];
    if (id == -9980.f) return enemy.ecl_float_vars[1];
    if (id == -9979.f) return enemy.ecl_float_vars[2];
    if (id == -9978.f) return enemy.ecl_float_vars[3];
    if (id == -9939.f) return EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->data.boss_ids[0])->enemy.ecl_float_vars[0];
    if (id == -9938.f) return EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->data.boss_ids[0])->enemy.ecl_float_vars[1];
    if (id == -9937.f) return EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->data.boss_ids[0])->enemy.ecl_float_vars[2];
    if (id == -9936.f) return EnemyManager::GetInstance()->EnmFind(EnemyManager::GetInstance()->data.boss_ids[0])->enemy.ecl_float_vars[3];
    if (id == -9935.f) return enemy.ecl_float_vars[4];
    if (id == -9934.f) return enemy.ecl_float_vars[5];
    if (id == -9933.f) return enemy.ecl_float_vars[6];
    if (id == -9932.f) return enemy.ecl_float_vars[7];
    if (id == -9922.f) return EnemyManager::GetInstance()->data.ecl_float_vars[0];
    if (id == -9921.f) return EnemyManager::GetInstance()->data.ecl_float_vars[1];
    if (id == -9920.f) return EnemyManager::GetInstance()->data.ecl_float_vars[2];
    if (id == -9919.f) return EnemyManager::GetInstance()->data.ecl_float_vars[3];
    if (id == -9918.f) return EnemyManager::GetInstance()->data.ecl_float_vars[4];
    if (id == -9917.f) return EnemyManager::GetInstance()->data.ecl_float_vars[5];
    if (id == -9916.f) return EnemyManager::GetInstance()->data.ecl_float_vars[6];
    if (id == -9915.f) return EnemyManager::GetInstance()->data.ecl_float_vars[7];
    if (id >= 0) return VAR((int)id/4).asFloat;
    return context.currentContext->float_vars[7];
}
