#ifndef SHTFILE_H_
#define SHTFILE_H_

#include <cstdint>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>

struct ShtFileHeader_t {
    int16_t unknown_head;
    int16_t sht_off_cnt;
    float hitbox_u;
    float grazebox_u;
    float itembox_u;
    float move_nf_str;
    float move_f_str;
    float move_nf_dia;
    float move_f_dia;
    int16_t pwr_lvl_cnt;
    int16_t max_dmg_u;
    int32_t SA_power_divisor;
    int32_t max_dmg;
    int32_t unknown_2;
    int32_t unknown_3;
    int32_t unknown_4;
    int32_t unknown_5;
    int32_t unknown_6;
    int32_t real_sht_off_cnt;
};

struct ShtShooterRaw_t {
    uint8_t fire_rate;
    uint8_t start_delay;
    int16_t damage;
    glm::vec2 offset;
    glm::vec2 hitbox;
    float angle;
    float speed;
    int32_t __unknown_1c;
    uint8_t option;
    uint8_t __unknown_21;
    uint8_t anm;
    uint8_t anm_hit;
    int16_t sfx_hit;
    uint8_t long_fire_rate;
    uint8_t long_start_delay;
    int32_t on_init;
    int32_t on_tick;
    int32_t __func_3;
    int32_t on_hit;
    int16_t __unknown[16];
};

struct ShtFileRaw_t {
    ShtFileHeader_t header;
    glm::vec2 option_pos[20];
    int32_t sht_offsets[];
};

struct ShtShooter_t {
    ShtShooter_t(ShtShooterRaw_t* raw);
    uint8_t fire_rate;
    uint8_t start_delay;
    int16_t damage;
    glm::vec2 offset;
    glm::vec2 hitbox;
    float angle;
    float speed;
    int32_t __unknown_1c;
    uint8_t option;
    uint8_t __unknown_21;
    uint8_t anm;
    uint8_t anm_hit;
    int16_t sfx_hit;
    uint8_t long_fire_rate;
    uint8_t long_start_delay;
    int (*on_init)(void);
    int (*on_tick)(void);
    int (*__func_3)(void);
    int (*on_hit)(void);
    int16_t __unknown[16];
};

struct ShtFile_t {
    ShtFileHeader_t header;
    glm::vec2 option_pos[20];
    std::vector<std::vector<ShtShooter_t>> shooters;
};

extern ShtFile_t* open_sht(std::string filename);

extern ShtFile_t* ALREADY_LOADED_SHTFILE;

#endif // SHTFILE_H_
