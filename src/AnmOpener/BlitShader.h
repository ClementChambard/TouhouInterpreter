#ifndef BLITSHADER_INCLUDED_H
#define BLITSHADER_INCLUDED_H

#include <ShaderProgram.h>

namespace anm {

class BlitShader : public ns::ShaderProgram {
    public:
        BlitShader() : ShaderProgram(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH) { bindAttributes(); getAllUniformLocation(); };

        void SetProjectionMatrix(const glm::mat4& mat);
        void SetViewMatrix(const glm::mat4& mat);

    protected:
        void bindAttributes() override;
        void getAllUniformLocation() override;

    private:
        static const char* VERTEX_SHADER_PATH;
        static const char* FRAGMENT_SHADER_PATH;

        GLuint location_projectionMatrix = 0;
        GLuint location_viewMatrix = 0;
};

} // namespace anm

#endif
