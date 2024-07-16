#include "./BlitShader.h"

namespace anm {

const char* BlitShader::VERTEX_SHADER_PATH = "assets/shaders/blitVertexShader";
const char* BlitShader::FRAGMENT_SHADER_PATH = "assets/shaders/blitFragmentShader";

void BlitShader::bind_attributes() {
    bind_attribute(0, "vertexPosition");
    bind_attribute(1, "vertexColor");
    bind_attribute(2, "vertexTexCoords");
}

void BlitShader::get_all_uniform_location() {
    location_projectionMatrix = get_uniform_location("projectionMatrix");
    location_viewMatrix = get_uniform_location("viewMatrix");
}

void BlitShader::set_projection_matrix(const ns::mat4& mat) {
    load_mat4(location_projectionMatrix, mat);
}
void BlitShader::set_view_matrix(const ns::mat4& mat) {
    load_mat4(location_viewMatrix, mat);
}

} // namespace anm
