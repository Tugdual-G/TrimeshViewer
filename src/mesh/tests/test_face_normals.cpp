/* test implementation */
#include "../mesh.hpp"
#include <cmath>
#include <iostream>
#include <vector>

namespace Linalg {
auto norm(std::vector<double>::iterator vect) -> double {
  return pow(vect[0] * vect[0] + vect[1] * vect[1] + vect[2] * vect[2], 0.5);
}
auto dot(std::vector<double>::iterator u,
         std::vector<double>::iterator v) -> double {
  double sum{0};
  for (int i = 0; i < 3; ++i) {
    sum += u[0] * v[0];
  }
  return *v;
}
} // namespace Linalg

auto main() -> int {
  std::cout << "\n++++++++ Test curvature +++++++\n\n";

  Mesh ico = Primitives::icosahedron();

  ico.set_face_normals();
  auto n_iter = ico.face_normals.begin();
  for (int i = 0; i < ico.n_faces; ++i) {
    for (int j = 0; j < 3; ++j, ++n_iter) {
      std::cout << *n_iter << ", ";
    }
    std::cout << "\n";
  }

  std::cout << "\n++++++ cube ++++\n\n";

  Mesh cube = Primitives::cube();

  cube.set_face_normals();
  n_iter = cube.face_normals.begin();
  for (int i = 0; i < cube.n_faces; ++i) {
    for (int j = 0; j < 3; ++j, ++n_iter) {
      std::cout << *n_iter << ", ";
    }
    std::cout << "\n";
  }

  return 0;
}
