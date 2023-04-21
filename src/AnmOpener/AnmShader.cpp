#include "AnmShader.h"

const std::string AnmShader::VERTEX_SHADER_PATH = "assets/shaders/anmVertexShader";
const std::string AnmShader::FRAGMENT_SHADER_PATH = "assets/shaders/anmFragmentShader";

void AnmShader::bindAttributes()
{
    bindAttribute(0, "vertexPosition");
    bindAttribute(1, "vertexColor");
    bindAttribute(2, "vertexTexCoords");
}

void AnmShader::getAllUniformLocation()
{
    location_projectionMatrix = getUniformLocation("projectionMatrix");
    location_viewMatrix       = getUniformLocation("viewMatrix"      );
    location_cameraPosition   = getUniformLocation("cameraPosition"  );
    location_fogColor         = getUniformLocation("fogColor"        );
    location_fogMin           = getUniformLocation("fogMin"          );
    location_fogMax           = getUniformLocation("fogMax"          );
}

void AnmShader::SetProjectionMatrix(const glm::mat4& mat)
{
    loadMat4(location_projectionMatrix, mat);
}
void AnmShader::SetViewMatrix(const glm::mat4& mat)
{
    loadMat4(location_viewMatrix, mat);
}
void AnmShader::SetCameraPosition(const glm::vec3& vec)
{
    loadVec3(location_cameraPosition, vec);
}
void AnmShader::SetFog(float min, float max, const glm::vec4& color)
{
    loadFloat(location_fogMin, min);
    loadFloat(location_fogMax, max);
    loadVec4(location_fogColor, color);
}
