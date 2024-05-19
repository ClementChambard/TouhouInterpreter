#ifndef FOGSHADER_INCLUDED_H
#define FOGSHADER_INCLUDED_H

#include <ShaderProgram.h>

namespace anm {

class FogShader : public ns::ShaderProgram {
    public:
        FogShader() : ShaderProgram(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH) { bindAttributes(); getAllUniformLocation(); };

        void SetProjectionMatrix(const glm::mat4& mat);
        void SetViewMatrix(const glm::mat4& mat);
        void SetCameraPosition(const glm::vec3& vec);
        void SetFog(float min, float max, const glm::vec4& color);

    protected:
        void bindAttributes() override;
        void getAllUniformLocation() override;

    private:
        static const char* VERTEX_SHADER_PATH;
        static const char* FRAGMENT_SHADER_PATH;

        GLuint location_projectionMatrix = 0;
        GLuint location_viewMatrix = 0;
        GLuint location_cameraPosition = 0;
        GLuint location_fogColor = 0;
        GLuint location_fogMin = 0;
        GLuint location_fogMax = 0;

};

} // namespace anm

#endif
