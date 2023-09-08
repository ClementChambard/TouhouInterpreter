#ifndef FOGSHADER_INCLUDED_H
#define FOGSHADER_INCLUDED_H

#include <ShaderProgram.h>

class FogShader : public NSEngine::ShaderProgram {
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
        static const std::string VERTEX_SHADER_PATH;
        static const std::string FRAGMENT_SHADER_PATH;

        GLuint location_projectionMatrix;
        GLuint location_viewMatrix;
        GLuint location_cameraPosition;
        GLuint location_fogColor;
        GLuint location_fogMin;
        GLuint location_fogMax;

};

#endif
