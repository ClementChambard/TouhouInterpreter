#include "./Camera.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace anm {

void CameraSky_t::init(float beg, float end, float c0,
                       float c1, float c2, float c3) {
    begin_distance = beg;
    end_distance = end;
    color_components[0] = c0;
    color_components[1] = c1;
    color_components[2] = c2;
    color_components[3] = c3;
    color.b = color_components[0];
    color.g = color_components[1];
    color.r = color_components[2];
    color.a = color_components[3];
}

Camera_t* Camera_t::as_2d_matrix() {
  glm::vec2 pos = {viewport.Width * 0.5 + viewport.X,
                   viewport.Height * 0.5 + viewport.Y};
  float z = (viewport.Height / 2.f) / tan(fov_y / 2);
  view_matrix = glm::lookAtLH(glm::vec3{pos.x, pos.y, z}, {pos.x, pos.y, 0.f},
                              {0.f, -1.f, 0.f});
  projection_matrix = glm::perspectiveLH(
      fov_y, static_cast<float>(viewport.Width) / viewport.Height, 1.f,
      10000.f);
  return this;
}

Camera_t* Camera_t::as_3d_matrix() {
  glm::vec3 pos = position + __rocking_vector_1;
  glm::vec3 look_at = pos + facing_normalized;
  view_matrix = glm::lookAtLH(pos, look_at, up);
  projection_matrix = glm::perspectiveLH(
      fov_y, static_cast<float>(viewport.Width) / viewport.Height, 30.f,
      8000.f);
  right.x = up.z * facing_normalized.y - up.y * facing_normalized.z;
  right.y = up.x * facing_normalized.z - up.z * facing_normalized.x;
  right.z = up.y * facing_normalized.x - up.x * facing_normalized.y;
  right = glm::normalize(right);
  return this;
}

} // namespace anm
