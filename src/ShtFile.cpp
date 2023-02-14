#include "ShtFile.h"

#include <fstream>

ShtFile_t* ALREADY_LOADED_SHTFILE = nullptr;

ShtFile_t* open_sht(std::string filename)
{

    std::ifstream file(filename, std::ios::binary);
    if (file.fail()) return nullptr;
    file.seekg(0, std::ios::end);
    std::streamoff fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    fileSize -= file.tellg();
    unsigned char* buf = new unsigned char[fileSize];
    file.read(reinterpret_cast<char*>(buf), fileSize);
    file.close();

    ShtFileRaw_t* raw = (ShtFileRaw_t*)buf;
    ShtFile_t* sht = new ShtFile_t();
    return sht;
    // TODO
    sht->header = raw->header;
    for (int i = 0; i < 20; i++) sht->option_pos[i] = raw->option_pos[i];

    for (int i = 0; i < raw->header.sht_off_cnt; i++) {
        ShtShooterRaw_t* shooters = &((ShtShooterRaw_t*)&raw->sht_offsets[raw->header.sht_off_cnt])[raw->sht_offsets[i]];
        sht->shooters.push_back({});

        while (*(int32_t*)shooters != -1) {
            sht->shooters.back().emplace_back(shooters);
            shooters = &shooters[1];
        }
    }

    delete[] buf;
    return sht;
}

int ____empty____() { return 0; }

int (*SHT_ON_INIT[])(void) = {
    ____empty____
};

int (*SHT_ON_TICK[])(void) = {
    ____empty____
};

int (*SHT_FUNC_3[])(void) = {
    ____empty____
};

int (*SHT_ON_HIT[])(void) = {
    ____empty____
};

ShtShooter_t::ShtShooter_t(ShtShooterRaw_t* raw)
{
    fire_rate        = raw->fire_rate;
    start_delay      = raw->start_delay;
    damage           = raw->damage;
    offset           = raw->offset;
    hitbox           = raw->hitbox;
    angle            = raw->angle;
    speed            = raw->speed;
    __unknown_1c     = raw->__unknown_1c;
    option           = raw->option;
    __unknown_21     = raw->__unknown_21;
    anm              = raw->anm;
    anm_hit          = raw->anm_hit;
    sfx_hit          = raw->sfx_hit;
    long_fire_rate   = raw->long_fire_rate;
    long_start_delay = raw->long_start_delay;
    for (int i = 0; i < 16; i++) __unknown[i] = raw->__unknown[i];
    on_init       = SHT_ON_INIT[raw->on_init];
    on_tick       = SHT_ON_TICK[raw->on_init];
    __func_3      = SHT_FUNC_3[raw->__func_3];
    on_hit        = SHT_ON_HIT[raw->on_hit];
}
