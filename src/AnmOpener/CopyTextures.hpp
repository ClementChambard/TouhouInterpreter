#ifndef COPYTEXTURES_INCLUDED_H
#define COPYTEXTURES_INCLUDED_H

#include "CopyShader.h"

struct CopyTextures {
    static void init();
    static void cleanup();
    static void doCopy(int src_glid, int dst_glid, glm::vec4 const &src_uvs,
                       glm::vec4 const &dst_uvs);

 private:
    struct Vbo {
        Vbo();
        ~Vbo();
        Vbo(Vbo&) = delete;
        Vbo(Vbo&&) = delete;
        Vbo& operator=(Vbo&) = delete;
        Vbo& operator=(Vbo&&) = delete;
        struct Vertex {
            glm::vec2 dstUV{};
            glm::vec2 srcUV{};
        };
        void setData(Vertex quad[4]);
        void bind();
        void unbind();
        void draw();

     private:
        GLuint vboId;
    };

    static CopyShader* shader;
    static Vbo* vbo;
};

#endif // COPYTEXTURES_INCLUDED_H
