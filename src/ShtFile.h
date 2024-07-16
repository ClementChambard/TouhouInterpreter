#ifndef SHTFILE_H_
#define SHTFILE_H_

#include <vector>
#include <math/types/types.hpp>

struct ShtFileHeader_t {
    i16 unknown_head;
    i16 sht_off_cnt;
    f32 hitbox_u;
    f32 grazebox_u;
    f32 itembox_u;
    f32 move_nf_str;
    f32 move_f_str;
    f32 move_nf_dia;
    f32 move_f_dia;
    i16 pwr_lvl_cnt;
    i16 max_dmg_u;
    i32 SA_power_divisor;
    i32 max_dmg;
    i32 unknown_2;
    i32 unknown_3;
    i32 unknown_4;
    i32 unknown_5;
    i32 unknown_6;
};

struct ShtShooterRaw_t {
    u8 fire_rate;
    u8 start_delay;
    i16 damage;
    ns::vec2 offset;
    ns::vec2 hitbox;
    f32 angle;
    f32 speed;
    i32 __unknown_1c;
    u8 option;
    u8 __unknown_21;
    u8 anm;
    u8 anm_hit;
    i16 sfx_hit;
    u8 long_fire_rate;
    u8 long_start_delay;
    i32 on_init;
    i32 on_tick;
    i32 __func_3;
    i32 on_hit;
    i16 __unknown[16];
};

struct ShtShooterRaw18_t {
    u8 fire_rate;
    u8 start_delay;
    i16 damage;
    ns::vec2 offset;
    ns::vec2 hitbox;
    f32 angle;
    f32 speed;
    i32 __unknown_1c;
    u8 option;
    u8 __unknown_21;
    u8 anm;
    u8 anm_hit;
    i16 sfx_hit;
    i16 aaa;
    i16 bbb;
    u8 long_fire_rate;
    u8 long_start_delay;
    i32 on_init;
    i32 on_tick;
    i32 __func_3;
    i32 on_hit;
    i16 __unknown[16];
};

struct ShtFileRaw_t {
    ShtFileHeader_t header;
    u8 next_bytes[];
};

struct PlayerBullet_t;
struct ShtShooter_t {
    explicit ShtShooter_t(ShtShooterRaw_t* raw, i32 nb = 16);
    explicit ShtShooter_t(ShtShooterRaw18_t* raw);
    u8 fire_rate;
    u8 start_delay;
    i16 damage;
    ns::vec2 offset;
    ns::vec2 hitbox;
    f32 angle;
    f32 speed;
    i32 __unknown_1c;
    u8 option;
    u8 __unknown_21__2_is_unique_bullet;
    u8 anm;
    u8 anm_hit;
    i16 sfx_hit;
    u8 long_fire_rate;
    u8 long_start_delay;
    i32 (*on_init)(PlayerBullet_t*);
    i32 (*on_tick)(PlayerBullet_t*);
    i32 (*__func_3)(PlayerBullet_t*);
    i32 (*on_hit)(PlayerBullet_t*, ns::vec3 const&, f32, f32, f32);
    i16 __unknown[16];
};

extern i32 pl_b_default_on_hit(PlayerBullet_t& b);

struct ShtFile_t {
    ShtFileHeader_t header;
    std::vector<ns::vec2> option_pos;
    std::vector<std::vector<ShtShooter_t>> shooters;
};

extern ShtFile_t* open_sht(cstr filename);

extern ShtFile_t* ALREADY_LOADED_SHTFILE;

#endif // SHTFILE_H_
