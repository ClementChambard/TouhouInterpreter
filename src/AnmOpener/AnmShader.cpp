#include "./AnmShader.h"

namespace anm {

const char* BaseShader::VERTEX_SHADER_PATH = "assets/shaders/anmVertexShader";
const char* BaseShader::FRAGMENT_SHADER_PATH = "assets/shaders/anmFragmentShader";

void BaseShader::bindAttributes() {
    bindAttribute(0, "vertexPosition");
    bindAttribute(1, "vertexColor");
    bindAttribute(2, "vertexTexCoords");
}

void BaseShader::getAllUniformLocation() {
    location_projectionMatrix = getUniformLocation("projectionMatrix");
    location_viewMatrix       = getUniformLocation("viewMatrix"      );
}

void BaseShader::SetProjectionMatrix(const glm::mat4& mat) {
    loadMat4(location_projectionMatrix, mat);
}

void BaseShader::SetViewMatrix(const glm::mat4& mat) {
    loadMat4(location_viewMatrix, mat);
}

} // namespace anm
