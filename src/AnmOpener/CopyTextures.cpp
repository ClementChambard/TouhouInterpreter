#include "./CopyTextures.hpp"
#include "./AnmManager.h"
#include "./CopyShader.h"

#include <TextureManager.h>
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

  CopyTexturesVbo::CopyTexturesVbo() {
      glGenBuffers(1, &vboId);
      glBindBuffer(GL_ARRAY_BUFFER, vboId);
      glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  CopyTexturesVbo::~CopyTexturesVbo() {
      glDeleteBuffers(1, &vboId);
  }

  void CopyTexturesVbo::setData(Vertex quad[4]) {
      glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(Vertex), quad);
  }

  void CopyTexturesVbo::draw() {
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }

  void CopyTexturesVbo::bind() {
      glBindBuffer(GL_ARRAY_BUFFER, vboId);
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                            reinterpret_cast<void*>(offsetof(Vertex, dstUV)));
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                            reinterpret_cast<void*>(offsetof(Vertex, srcUV)));
  }

  void CopyTexturesVbo::unbind() {
      glBindBuffer(GL_ARRAY_BUFFER, 0);
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

void doCopy(int src_glid, int dst_glid, glm::vec4 const &src_uvs,
                          glm::vec4 const &dst_uvs) {
    auto last_fb = ns::FrameBuffer::BOUND_FRAMEBUFFER;
    auto fb = ns::TextureManager::GetTextureByOpengl(dst_glid)
        ->getFramebuffer();
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
    copyTexturesShader->start();
    anm::reset_texture();
    glBindTexture(GL_TEXTURE_2D, src_glid);
    copyTexturesVbo->draw();
    glBindTexture(GL_TEXTURE_2D, 0);
    copyTexturesShader->stop();
    copyTexturesVbo->unbind();
    ns::FrameBuffer::bindFramebuffer(last_fb);
}

} // namespace anm::cptex
