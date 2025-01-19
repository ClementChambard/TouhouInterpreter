#include "./CopyTextures.hpp"
#include "./AnmManager.h"
#include "./CopyShader.h"
#include <GL/glew.h>

#include <FrameBuffer.h>

namespace anm::cptex {

namespace {
    struct CopyTexturesVbo {
        CopyTexturesVbo();
        ~CopyTexturesVbo();
        CopyTexturesVbo(CopyTexturesVbo&) = delete;
        CopyTexturesVbo(CopyTexturesVbo&&) = delete;
        CopyTexturesVbo& operator=(CopyTexturesVbo&) = delete;
        CopyTexturesVbo& operator=(CopyTexturesVbo&&) = delete;
        struct Vertex {
            ns::vec2 dstUV{};
            ns::vec2 srcUV{};
        };
        void setData(Vertex quad[4]);
        void draw();

     private:
        GLuint vboId;
        GLuint vaoId;
    };

    CopyTexturesVbo::CopyTexturesVbo() {
        glCreateVertexArrays(1, &vaoId);
        glCreateBuffers(1, &vboId);

        glNamedBufferData(vboId, 4 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexArrayAttrib(vaoId, 0);
        glEnableVertexArrayAttrib(vaoId, 1);

        glVertexArrayVertexBuffer(vaoId, 0, vboId, 0, sizeof(Vertex));

        glVertexArrayAttribBinding(vaoId, 0, 0);
        glVertexArrayAttribBinding(vaoId, 1, 0);

        glVertexArrayAttribFormat(vaoId, 0, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, dstUV));
        glVertexArrayAttribFormat(vaoId, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, srcUV));
    }

    CopyTexturesVbo::~CopyTexturesVbo() {
        glBindVertexArray(0);
        glDeleteBuffers(1, &vboId);
        glDeleteVertexArrays(1, &vaoId);
    }

    void CopyTexturesVbo::setData(Vertex quad[4]) {
        glNamedBufferSubData(vboId, 0, 4 * sizeof(Vertex), quad);
    }

    void CopyTexturesVbo::draw() {
        glBindVertexArray(vaoId);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    CopyShader* copyTexturesShader;
    CopyTexturesVbo* copyTexturesVbo;
}

void init() {
    copyTexturesShader = new CopyShader();
    copyTexturesVbo = new CopyTexturesVbo();
}

void cleanup() {
    delete copyTexturesShader;
    delete copyTexturesVbo;
}

void doCopy(ns::Texture* src, ns::Texture* dst, ns::vec4 const &src_uvs, ns::vec4 const &dst_uvs) {
    auto last_fb = ns::FrameBuffer::BOUND_FRAMEBUFFER;
    auto fb = dst->get_framebuffer();
    if (!fb) return;
    fb->bind();
    CopyTexturesVbo::Vertex vertices[4] = {
        {{dst_uvs.z, dst_uvs.y}, {src_uvs.z, src_uvs.w}},
        {{dst_uvs.z, dst_uvs.w}, {src_uvs.z, src_uvs.y}},
        {{dst_uvs.x, dst_uvs.y}, {src_uvs.x, src_uvs.w}},
        {{dst_uvs.x, dst_uvs.w}, {src_uvs.x, src_uvs.y}},
    };
    copyTexturesVbo->setData(vertices);
    copyTexturesShader->use();
    anm::use_texture(src);
    copyTexturesVbo->draw();
    ns::FrameBuffer::bind_framebuffer(last_fb);
}

} // namespace anm::cptex
