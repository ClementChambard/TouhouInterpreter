#ifndef BLITSHADER_INCLUDED_H
#define BLITSHADER_INCLUDED_H

#include <ShaderProgram.h>

namespace anm {

class BlitShader : public ns::ShaderProgram {
    public:
        BlitShader() : ShaderProgram(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH) { bind_attributes(); get_all_uniform_location(); };

        void set_projection_matrix(const ns::mat4& mat);
        void set_view_matrix(const ns::mat4& mat);

    protected:
        void bind_attributes() override;
        void get_all_uniform_location() override;

    private:
        static const char* VERTEX_SHADER_PATH;
        static const char* FRAGMENT_SHADER_PATH;

        GLuint location_projectionMatrix = 0;
        GLuint location_viewMatrix = 0;
};

} // namespace anm

#endif
