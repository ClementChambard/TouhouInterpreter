#include "FogShader.h"

const char* FogShader::VERTEX_SHADER_PATH = "assets/shaders/fogVertexShader";
const char* FogShader::FRAGMENT_SHADER_PATH = "assets/shaders/fogFragmentShader";

void FogShader::bindAttributes() {
    bindAttribute(0, "vertexPosition");
    bindAttribute(1, "vertexColor");
    bindAttribute(2, "vertexTexCoords");
}

void FogShader::getAllUniformLocation() {
    location_projectionMatrix = getUniformLocation("projectionMatrix");
    location_viewMatrix       = getUniformLocation("viewMatrix"      );
    location_cameraPosition   = getUniformLocation("cameraPosition"  );
    location_fogColor         = getUniformLocation("fogColor"        );
    location_fogMin           = getUniformLocation("fogMin"          );
    location_fogMax           = getUniformLocation("fogMax"          );
}

void FogShader::SetProjectionMatrix(const glm::mat4& mat) {
    loadMat4(location_projectionMatrix, mat);
}

void FogShader::SetViewMatrix(const glm::mat4& mat) {
    loadMat4(location_viewMatrix, mat);
}

void FogShader::SetCameraPosition(const glm::vec3& vec) {
    loadVec3(location_cameraPosition, vec);
}

void FogShader::SetFog(float min, float max, const glm::vec4& color) {
    loadFloat(location_fogMin, min);
    loadFloat(location_fogMax, max);
    loadVec4(location_fogColor, color);
}
