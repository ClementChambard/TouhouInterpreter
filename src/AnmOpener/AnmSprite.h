#ifndef ANMSPRITE_H_
#define ANMSPRITE_H_

#include <cstdint>
#include <iostream>

struct AnmSprite {
    uint32_t texID = 0;  // Right now it uses TextureManager's texID
    float x = 0, y = 0, w = 0, h = 0;
    float u1 = 0, v1 = 0, u2 = 0, v2 = 0;

    uint32_t opengl_texid = 0;
    void genTexCoords(float w, float h);
};

#endif
