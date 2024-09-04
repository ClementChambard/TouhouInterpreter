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
      void bind();
      void unbind();
      void draw();
  
   private:
      GLuint vboId;
      GLuint vaoId;
  };

  CopyTexturesVbo::CopyTexturesVbo() {
      glGenVertexArrays(1, &vaoId);
      glBindVertexArray(vaoId);
      glGenBuffers(1, &vboId);
      glBindBuffer(GL_ARRAY_BUFFER, vboId);

      glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);

      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                            reinterpret_cast<void*>(offsetof(Vertex, dstUV)));
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                            reinterpret_cast<void*>(offsetof(Vertex, srcUV)));
  }

  CopyTexturesVbo::~CopyTexturesVbo() {
      glBindVertexArray(0);
      glDeleteBuffers(1, &vboId);
      glDeleteVertexArrays(1, &vaoId);
  }

  void CopyTexturesVbo::setData(Vertex quad[4]) {
      glBindBuffer(GL_ARRAY_BUFFER, vboId);
      glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(Vertex), quad);
  }

  void CopyTexturesVbo::draw() {
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }

  void CopyTexturesVbo::bind() {
      glBindVertexArray(vaoId);
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

void doCopy(ns::Texture* src, ns::Texture* dst, ns::vec4 const &src_uvs,
                          ns::vec4 const &dst_uvs) {
    auto last_fb = ns::FrameBuffer::BOUND_FRAMEBUFFER;
    auto fb = dst->get_framebuffer();
    if (!fb) return;
    fb->bind();
    copyTexturesVbo->bind();
    CopyTexturesVbo::Vertex vertices[4] = {
        {{dst_uvs.z, dst_uvs.y}, {src_uvs.z, src_uvs.w}},
        {{dst_uvs.z, dst_uvs.w}, {src_uvs.z, src_uvs.y}},
        {{dst_uvs.x, dst_uvs.y}, {src_uvs.x, src_uvs.w}},
        {{dst_uvs.x, dst_uvs.w}, {src_uvs.x, src_uvs.y}},
    };
    copyTexturesVbo->setData(vertices);
    copyTexturesShader->use();
    anm::reset_texture();
    glBindTexture(GL_TEXTURE_2D, src->get_opengl_id());
    copyTexturesVbo->draw();
    ns::FrameBuffer::bind_framebuffer(last_fb);
}

} // namespace anm::cptex
