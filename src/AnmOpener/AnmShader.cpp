#include "./AnmShader.h"

const char* AnmShader::VERTEX_SHADER_PATH = "assets/shaders/anmVertexShader";
const char* AnmShader::FRAGMENT_SHADER_PATH = "assets/shaders/anmFragmentShader";

void AnmShader::bindAttributes() {
    bindAttribute(0, "vertexPosition");
    bindAttribute(1, "vertexColor");
    bindAttribute(2, "vertexTexCoords");
}

void AnmShader::getAllUniformLocation() {
    location_projectionMatrix = getUniformLocation("projectionMatrix");
    location_viewMatrix       = getUniformLocation("viewMatrix"      );
}

void AnmShader::SetProjectionMatrix(const glm::mat4& mat) {
    loadMat4(location_projectionMatrix, mat);
}

void AnmShader::SetViewMatrix(const glm::mat4& mat) {
    loadMat4(location_viewMatrix, mat);
}
