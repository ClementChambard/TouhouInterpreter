#ifndef COPYTEXTURES_INCLUDED_H
#define COPYTEXTURES_INCLUDED_H

#include <glm/glm.hpp>

namespace anm::cptex {

void init();
void cleanup();

// TODO: detach usage from opengl
void doCopy(int src_glid, int dst_glid, glm::vec4 const &src_uvs,
                   glm::vec4 const &dst_uvs);

}; // namespace anm::cptex

#endif // COPYTEXTURES_INCLUDED_H
