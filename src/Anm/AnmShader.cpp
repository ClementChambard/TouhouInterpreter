#include "./AnmShader.h"

namespace anm {

const char* BaseShader::VERTEX_SHADER_PATH = "assets/shaders/anmVertexShader";
const char* BaseShader::FRAGMENT_SHADER_PATH = "assets/shaders/anmFragmentShader";

void BaseShader::bind_attributes() {
    bind_attribute(0, "vertexPosition");
    bind_attribute(1, "vertexColor");
    bind_attribute(2, "vertexTexCoords");
}

void BaseShader::get_all_uniform_location() {
    location_projectionMatrix = get_uniform_location("projectionMatrix");
    location_viewMatrix       = get_uniform_location("viewMatrix"      );
}

void BaseShader::set_projection_matrix(const ns::mat4& mat) {
    load_mat4(location_projectionMatrix, mat);
}

void BaseShader::set_view_matrix(const ns::mat4& mat) {
    load_mat4(location_viewMatrix, mat);
}

} // namespace anm
