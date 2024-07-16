#ifndef FOGSHADER_INCLUDED_H
#define FOGSHADER_INCLUDED_H

#include <ShaderProgram.h>

namespace anm {

class FogShader : public ns::ShaderProgram {
    public:
        FogShader() : ShaderProgram(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH) { bind_attributes(); get_all_uniform_location(); };

        void SetProjectionMatrix(const ns::mat4& mat);
        void SetViewMatrix(const ns::mat4& mat);
        void SetCameraPosition(const ns::vec3& vec);
        void SetFog(float min, float max, const ns::vec4& color);

    protected:
        void bind_attributes() override;
        void get_all_uniform_location() override;

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
