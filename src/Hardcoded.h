#ifndef HARDCODED_H_
#define HARDCODED_H_

#include <json/json.h>
#include "GlobalData.h"

extern Json::Value HARDCODED_DATA;
extern Json::Value HARDCODED_DATA_BASE;
extern Json::Value STAGE_DATA_TABLE;
extern Json::Value BULLET_TYPE_TABLE;
extern Json::Value LASER_DATA;
extern int TOUHOU_VERSION;

void Hardcoded_Load();

#endif // HARDCODED_H_
