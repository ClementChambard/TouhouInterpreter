#ifndef RENDERVERTEX_INCLUDED_H
#define RENDERVERTEX_INCLUDED_H

#include <math/types/types.hpp>
#include <color.h>

namespace anm {

struct RenderVertex_t {
    ns::vec4 transformed_pos {};
    ns::Color diffuse_color {};
    ns::vec2 texture_uv {};
};

extern RenderVertex_t SPRITE_TEMP_BUFFER[4];

} // namespace anm

#endif
