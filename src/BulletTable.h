#ifndef BULLET_TABLE_H
#define BULLET_TABLE_H

#include <cstdint>

struct TableBulletColor_t {
    int32_t main_sprite_id;
    int32_t spawn_sprite_id;
    int32_t cancel_sprite_id;
    int32_t cancel_script;
};

struct TableBulletSprite_t {
    int32_t script;
    TableBulletColor_t colors[16];
    float default_radius;
    int32_t default_layer;
    int32_t __field_10c;
    int32_t __field_110;
    int32_t __field_114;
};

// added 5 just in case
extern TableBulletSprite_t BULLET_TYPE_DEFINITIONS[49];

#endif
