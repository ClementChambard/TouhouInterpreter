#ifndef IMGUIWRAPPER_H_
#define IMGUIWRAPPER_H_

#include <ostream>
#define IM_VEC2_CLASS_EXTRA \
    ImVec2(glm::vec2 const& v) : x(v.x), y(v.y) {} \
    operator glm::vec2() { return {x, y}; }

#define IM_VEC4_CLASS_EXTRA \
    ImVec4(glm::vec4 const& v) : x(v.x), y(v.y), z(v.z), w(v.w) {} \
    operator glm::vec4() { return {x, y, z, w}; }

#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>

inline ImVec2 operator+(ImVec2 const& v1, ImVec2 const& v2) {
    return ImVec2(v1.x + v2.x, v1.y + v2.y);
}

inline ImVec2 operator-(ImVec2 const& v1) {
    return ImVec2(-v1.x, -v1.y);
}

inline ImVec2 operator-(ImVec2 const& v1, ImVec2 const& v2) {
    return v1 + -v2;
}

inline ImVec2 operator*(ImVec2 const& v1, ImVec2 const& v2) {
    return ImVec2(v1.x * v2.x, v1.y * v2.y);
}

inline ImVec2 operator*(ImVec2 const& v1, float const f) {
    return ImVec2(v1.x * f, v1.y * f);
}

inline ImVec2 operator*(float const f, ImVec2 const& v1) {
    return ImVec2(v1.x * f, v1.y * f);
}

inline ImVec2 operator/(ImVec2 const& v1, ImVec2 const& v2) {
    return ImVec2(v1.x / v2.x, v1.y / v2.y);
}

inline ImVec2 operator/(ImVec2 const& v1, float const f) {
    return ImVec2(v1.x / f, v1.y / f);
}

inline std::ostream& operator<<(std::ostream& os, ImVec2 const& v) {
    return os << "(" << v.x << ", " << v.y << ")";
}

#endif // IMGUIWRAPPER_H_
