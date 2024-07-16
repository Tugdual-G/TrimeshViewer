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
  std::cout << "\n++++++++ Test vertex normals +++++++\n\n";

  std::cout << "\n++++++ cube ++++\n\n";

  Mesh cube = Primitives::cube();

  cube.set_face_normals();
  cube.set_vertex_normals();

  auto n_iter = cube.vertex_normals.begin();

  std::cout << "\n vect \n";
  for (int i = 0; i < cube.n_vertices; ++i) {
    std::cout << "vertice coord : ";
    for (int j = 0; j < 3; ++j) {
      std::cout << cube.vertices.at(3 * i + j) << ", ";
    }

    std::cout << "normal coord : ";
    for (int j = 0; j < 3; ++j, ++n_iter) {
      std::cout << *n_iter << ", ";
    }
    std::cout << "\n";
  }
  std::cout << "\n vect norm \n";

  n_iter = cube.vertex_normals.begin();
  for (int i = 0; i < cube.n_vertices; ++i, n_iter += 3) {
    std::cout << Linalg::norm(n_iter);
    std::cout << "\n";
  }

  return 0;
}
