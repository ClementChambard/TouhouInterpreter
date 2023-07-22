#include "./Hardcoded.h"
#include <fstream>
#include <iostream>

Json::Value HARDCODED_DATA;
Json::Value HARDCODED_DATA_BASE;
Json::Value STAGE_DATA_TABLE;
Json::Value PLAYERS;
Json::Value BULLET_TYPE_TABLE;
Json::Value LASER_DATA;
Json::Value ITEM_ANM_SCRIPT_IDS;
Json::Value GUI_ANMS;

int TOUHOU_VERSION = 0;

void prepare_vals() {
    STAGE_DATA_TABLE = HARDCODED_DATA["stage_data_table"].empty()
        ? HARDCODED_DATA_BASE["stage_data_table"]
        : HARDCODED_DATA["stage_data_table"];
    BULLET_TYPE_TABLE = HARDCODED_DATA["bullet_type_table"].empty()
        ? HARDCODED_DATA_BASE["bullet_type_table"]
        : HARDCODED_DATA["bullet_type_table"];
    TOUHOU_VERSION = HARDCODED_DATA["version"].empty()
        ? HARDCODED_DATA_BASE["version"].asInt()
        : HARDCODED_DATA["version"].asInt();
    LASER_DATA = HARDCODED_DATA["laser_data"].empty()
        ? HARDCODED_DATA_BASE["laser_data"]
        : HARDCODED_DATA["laser_data"];
    PLAYERS = HARDCODED_DATA["players"].empty()
        ? HARDCODED_DATA_BASE["players"]
        : HARDCODED_DATA["players"];
    ITEM_ANM_SCRIPT_IDS = HARDCODED_DATA["item_anm_script_ids"].empty()
        ? HARDCODED_DATA_BASE["item_anm_script_ids"]
        : HARDCODED_DATA["item_anm_script_ids"];
    GUI_ANMS = HARDCODED_DATA["gui"].empty()
        ? HARDCODED_DATA_BASE["gui"]
        : HARDCODED_DATA["gui"];
}

void Hardcoded_Load() {
    std::ifstream file("hardcoded.json");

    if (file.fail()) {
        std::cout << "resulting to superfile \n";
        file.open("../hardcoded.json");
        if (file.fail()) {
            std::cout << "hardcoded data not found\n";
            return;
        }
        Json::Reader reader;
        reader.parse(file, HARDCODED_DATA_BASE);
        HARDCODED_DATA = HARDCODED_DATA_BASE;
        file.close();
        prepare_vals();
        return;
    }

    Json::Reader reader;

    reader.parse(file, HARDCODED_DATA);
    file.close();
    file.open("../hardcoded.json");
    reader.parse(file, HARDCODED_DATA_BASE);
    file.close();
    prepare_vals();
}

