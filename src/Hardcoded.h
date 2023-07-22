#ifndef HARDCODED_H_
#define HARDCODED_H_

#include "./GlobalData.h"
#include <json/json.h>

extern Json::Value HARDCODED_DATA;
extern Json::Value HARDCODED_DATA_BASE;
extern Json::Value STAGE_DATA_TABLE;
extern Json::Value BULLET_TYPE_TABLE;
extern Json::Value ITEM_ANM_SCRIPT_IDS;
extern Json::Value LASER_DATA;
extern Json::Value PLAYERS;
extern Json::Value GUI_ANMS;
extern int TOUHOU_VERSION;

void Hardcoded_Load();

#endif // HARDCODED_H_
