#ifndef COPYTEXTURES_INCLUDED_H
#define COPYTEXTURES_INCLUDED_H

#include <math/types/types.hpp>
#include <Texture.hpp>

namespace anm::cptex {

void init();
void cleanup();

void doCopy(ns::Texture* src, ns::Texture* dst, ns::vec4 const &src_uvs,
                   ns::vec4 const &dst_uvs);

}; // namespace anm::cptex

#endif // COPYTEXTURES_INCLUDED_H
