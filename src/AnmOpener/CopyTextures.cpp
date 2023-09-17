#include "./CopyTextures.hpp"
#include "./AnmManager.h"

#include <TextureManager.h>
#include <FrameBuffer.h>

CopyShader* CopyTextures::shader;
CopyTextures::Vbo* CopyTextures::vbo;

void CopyTextures::init() {
  shader = new CopyShader();
  vbo = new Vbo();
}

void CopyTextures::cleanup() {
  delete shader;
  delete vbo;
}

void CopyTextures::doCopy(int src_glid, int dst_glid, glm::vec4 const &src_uvs,
                          glm::vec4 const &dst_uvs) {
    auto last_fb = NSEngine::FrameBuffer::BOUND_FRAMEBUFFER;
    auto fb = NSEngine::TextureManager::GetTextureByOpengl(dst_glid)
        ->getFramebuffer();
    if (!fb) return;
    fb->bind();
    vbo->bind();
    Vbo::Vertex vertices[4] = {
        {{dst_uvs.z, dst_uvs.y}, {src_uvs.z, src_uvs.w}},
        {{dst_uvs.z, dst_uvs.w}, {src_uvs.z, src_uvs.y}},
        {{dst_uvs.x, dst_uvs.y}, {src_uvs.x, src_uvs.w}},
        {{dst_uvs.x, dst_uvs.w}, {src_uvs.x, src_uvs.y}},
    };
    vbo->setData(vertices);
    shader->start();
    AnmManager::last_used_texture = -1;
    glBindTexture(GL_TEXTURE_2D, src_glid);
    vbo->draw();
    glBindTexture(GL_TEXTURE_2D, 0);
    shader->stop();
    vbo->unbind();
    NSEngine::FrameBuffer::bindFramebuffer(last_fb);
}

CopyTextures::Vbo::Vbo() {
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

CopyTextures::Vbo::~Vbo() {
    glDeleteBuffers(1, &vboId);
}

void CopyTextures::Vbo::setData(Vertex quad[4]) {
    glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(Vertex), quad);
}

void CopyTextures::Vbo::draw() {
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void CopyTextures::Vbo::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, dstUV)));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, srcUV)));
}

void CopyTextures::Vbo::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
