#ifndef ANMSPRITE_H_
#define ANMSPRITE_H_

#include <defines.h>

namespace anm {

struct Sprite {
    u32 texID = 0;  // Right now it uses TextureManager's texID
    f32 x = 0, y = 0, w = 0, h = 0;
    f32 u1 = 0, v1 = 0, u2 = 0, v2 = 0;

    u32 opengl_texid = 0;
    void genTexCoords(f32 w, f32 h);
};

} // namespace anm

#endif
