#ifndef COPYSHADER_INCLUDED_H
#define COPYSHADER_INCLUDED_H

#include <ShaderProgram.h>
#include <string>

class CopyShader : public NSEngine::ShaderProgram {
    public:
        CopyShader() : ShaderProgram(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH) { bindAttributes(); getAllUniformLocation(); };

        void SetTint(const glm::vec4& tint);

    protected:
        void bindAttributes() override;
        void getAllUniformLocation() override;

    private:
        static const std::string VERTEX_SHADER_PATH;
        static const std::string FRAGMENT_SHADER_PATH;

        GLuint location_tint = 0;
        glm::vec4 last_tint{};
};

#endif
