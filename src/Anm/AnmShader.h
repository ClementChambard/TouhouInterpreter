#ifndef ANMSHADER_INCLUDED_H
#define ANMSHADER_INCLUDED_H

#include <ShaderProgram.h>

// TODO: Use ns::DefaultShader instead

namespace anm {

class BaseShader : public ns::ShaderProgram {
    public:
        BaseShader() : ShaderProgram(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH) { bind_attributes(); get_all_uniform_location(); };

        void set_projection_matrix(const ns::mat4& mat);
        void set_view_matrix(const ns::mat4& mat);

    protected:
        void bind_attributes() override;
        void get_all_uniform_location() override;

    private:
        static const char* VERTEX_SHADER_PATH;
        static const char* FRAGMENT_SHADER_PATH;

        u32 location_projectionMatrix = 0;
        u32 location_viewMatrix = 0;

};

} // namespace anm

#endif
