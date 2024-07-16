#include "./Camera.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
  ns::vec2 pos = ns::vec2(viewport.Width  * 0.5 + viewport.X,
                          viewport.Height * 0.5 + viewport.Y);
  float z = (viewport.Height / 2.f) / ns::tan(fov_y / 2);

  view_matrix = ns::mat4::lookat_lh({pos, z}, {pos, 0.f}, ns::vec3::down());
  projection_matrix = ns::mat4::perspective_lh(
      fov_y, static_cast<float>(viewport.Width) / viewport.Height, 1.f,
      10000.f);

  return this;
}

Camera_t* Camera_t::as_3d_matrix() {
  ns::vec3 pos = position + __rocking_vector_1;
  ns::vec3 look_at = pos + facing_normalized;

  view_matrix = ns::mat4::lookat_lh(pos, look_at, up);

  projection_matrix = ns::mat4::perspective_lh(
      fov_y, static_cast<float>(viewport.Width) / viewport.Height, 30.f,
      8000.f);
  
  right.x = up.z * facing_normalized.y - up.y * facing_normalized.z;
  right.y = up.x * facing_normalized.z - up.z * facing_normalized.x;
  right.z = up.y * facing_normalized.x - up.x * facing_normalized.y;
  right.normalize();
  return this;
}

} // namespace anm
