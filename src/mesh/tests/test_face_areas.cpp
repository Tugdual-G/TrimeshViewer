/* test implementation */
#include "../mesh.hpp"
#include <cmath>
#include <iostream>
#include <vector>

auto main() -> int {
  std::cout << "\n++++++++ Test face_areas +++++++\n\n";

  Mesh cube = Primitives::cube();
  cube.set_vertex_normals();
  cube.set_one_ring();

  std::vector<double> face_area = cube.get_face_areas();
  for (auto &a : face_area) {
    std::cout << a << " , ";
  }

  return 0;
}
