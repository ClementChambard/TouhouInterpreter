#include "./FogShader.h"

namespace anm {

const char* FogShader::VERTEX_SHADER_PATH = "assets/shaders/fogVertexShader";
const char* FogShader::FRAGMENT_SHADER_PATH = "assets/shaders/fogFragmentShader";

void FogShader::bind_attributes() {
    bind_attribute(0, "vertexPosition");
    bind_attribute(1, "vertexColor");
    bind_attribute(2, "vertexTexCoords");
}

void FogShader::get_all_uniform_location() {
    location_projectionMatrix = get_uniform_location("projectionMatrix");
    location_viewMatrix       = get_uniform_location("viewMatrix"      );
    location_cameraPosition   = get_uniform_location("cameraPosition"  );
    location_fogColor         = get_uniform_location("fogColor"        );
    location_fogMin           = get_uniform_location("fogMin"          );
    location_fogMax           = get_uniform_location("fogMax"          );
}

void FogShader::SetProjectionMatrix(const ns::mat4& mat) {
    load_mat4(location_projectionMatrix, mat);
}

void FogShader::SetViewMatrix(const ns::mat4& mat) {
    load_mat4(location_viewMatrix, mat);
}

void FogShader::SetCameraPosition(const ns::vec3& vec) {
    load_vec3(location_cameraPosition, vec);
}

void FogShader::SetFog(float min, float max, const ns::vec4& color) {
    load_float(location_fogMin, min);
    load_float(location_fogMax, max);
    load_vec4(location_fogColor, color);
}

} // namespace anm
