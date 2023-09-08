#include "BlitShader.h"

const std::string BlitShader::VERTEX_SHADER_PATH = "assets/shaders/blitVertexShader";
const std::string BlitShader::FRAGMENT_SHADER_PATH = "assets/shaders/blitFragmentShader";

void BlitShader::bindAttributes() {
    bindAttribute(0, "vertexPosition");
    bindAttribute(1, "vertexColor");
    bindAttribute(2, "vertexTexCoords");
}

void BlitShader::getAllUniformLocation() {
    location_projectionMatrix = getUniformLocation("projectionMatrix");
    location_viewMatrix = getUniformLocation("viewMatrix");
}

void BlitShader::SetProjectionMatrix(const glm::mat4& mat) {
    loadMat4(location_projectionMatrix, mat);
}
void BlitShader::SetViewMatrix(const glm::mat4& mat) {
    loadMat4(location_viewMatrix, mat);
}
