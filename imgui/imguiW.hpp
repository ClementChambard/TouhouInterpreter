#ifndef IMGUIWRAPPER_H_
#define IMGUIWRAPPER_H_

#define IM_VEC2_CLASS_EXTRA \
    ImVec2(glm::vec2 const& v) : x(v.x), y(v.y) {} \
    operator glm::vec2() { return {x, y}; }

#define IM_VEC4_CLASS_EXTRA \
    ImVec4(glm::vec4 const& v) : x(v.x), y(v.y), z(v.z), w(v.w) {} \
    operator glm::vec4() { return {x, y, z, w}; }

#include <glm/glm.hpp>
#include <imgui.h>

#endif // IMGUIWRAPPER_H_
