#include "./CopyShader.h"

namespace anm {

const char* CopyShader::VERTEX_SHADER_PATH = "assets/shaders/copyVertexShader";
const char* CopyShader::FRAGMENT_SHADER_PATH = "assets/shaders/copyFragmentShader";

void CopyShader::bind_attributes() {
    bind_attribute(0, "dstUvs");
    bind_attribute(1, "srcUvs");
}

void CopyShader::get_all_uniform_location() {
    location_tint = get_uniform_location("tint");
    use();
    load_vec4(location_tint, {1.f, 1.f, 1.f, 1.f});
    last_tint = {1.f, 1.f, 1.f, 1.f};
}

void CopyShader::set_tint(const ns::vec4& tint) {
    if (last_tint == tint) return;
    load_vec4(location_tint, tint);
    last_tint = tint;
}

} // namespace anm
