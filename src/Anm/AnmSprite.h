#ifndef ANMSPRITE_H_
#define ANMSPRITE_H_

#include <defines.h>
#include <Texture.hpp>

namespace anm {

struct Sprite {
    ns::Texture* texture;
    f32 x = 0, y = 0, w = 0, h = 0;
    f32 u1 = 0, v1 = 0, u2 = 0, v2 = 0;

    void genTexCoords(f32 w, f32 h);
};

} // namespace anm

#endif
