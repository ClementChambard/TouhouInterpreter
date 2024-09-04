#ifndef COPYSHADER_INCLUDED_H
#define COPYSHADER_INCLUDED_H

#include <ShaderProgram.h>

namespace anm {

class CopyShader : public ns::ShaderProgram {
    public:
        CopyShader() : ShaderProgram(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH) { bind_attributes(); get_all_uniform_location(); };

        void set_tint(const ns::vec4& tint);

    protected:
        void bind_attributes() override;
        void get_all_uniform_location() override;

    private:
        static const char* VERTEX_SHADER_PATH;
        static const char* FRAGMENT_SHADER_PATH;

        u32 location_tint = 0;
        ns::vec4 last_tint{};
};

} // namespace anm

#endif
