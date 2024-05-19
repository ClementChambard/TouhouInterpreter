#include "./CopyShader.h"

namespace anm {

const char* CopyShader::VERTEX_SHADER_PATH = "assets/shaders/copyVertexShader";
const char* CopyShader::FRAGMENT_SHADER_PATH = "assets/shaders/copyFragmentShader";

void CopyShader::bindAttributes() {
    bindAttribute(0, "dstUvs");
    bindAttribute(1, "srcUvs");
}

void CopyShader::getAllUniformLocation() {
    location_tint = getUniformLocation("tint");
    start();
    loadVec4(location_tint, {1.f, 1.f, 1.f, 1.f});
    last_tint = {1.f, 1.f, 1.f, 1.f};
    stop();
}

void CopyShader::SetTint(const glm::vec4& tint) {
    if (last_tint == tint) return;
    loadVec4(location_tint, tint);
    last_tint = tint;
}

} // namespace anm
