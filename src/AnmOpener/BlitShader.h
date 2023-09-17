#ifndef BLITSHADER_INCLUDED_H
#define BLITSHADER_INCLUDED_H

#include <ShaderProgram.h>
#include <string>

class BlitShader : public NSEngine::ShaderProgram {
    public:
        BlitShader() : ShaderProgram(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH) { bindAttributes(); getAllUniformLocation(); };

        void SetProjectionMatrix(const glm::mat4& mat);
        void SetViewMatrix(const glm::mat4& mat);

    protected:
        void bindAttributes() override;
        void getAllUniformLocation() override;

    private:
        static const std::string VERTEX_SHADER_PATH;
        static const std::string FRAGMENT_SHADER_PATH;

        GLuint location_projectionMatrix;
        GLuint location_viewMatrix;
};

#endif
