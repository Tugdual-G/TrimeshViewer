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
  for (int i = 0; i < 3; ++i, ++u, ++v) {
    sum += *u * *v;
  }
  return sum;
}
} // namespace Linalg

auto main() -> int {
  std::cout << "\n++++++++ Test curvature +++++++\n\n";

  Mesh sphere = Primitives::icosahedron();
  double norm{0};
  for (int k = 0; k < 4; ++k) {
    sphere.subdivide();
    for (int i = 0; i < sphere.n_vertices; ++i) {
      norm = Linalg::norm(sphere.vertices.begin() + i * 3);
      for (int j = 0; j < 3; ++j) {
        sphere.vertices.at(i * 3 + j) = sphere.vertices.at(i * 3 + j) / norm;
      }
    }
  }

  std::cout << "\n ++++++++++ normal projection ++++++\n";
  sphere.set_vertex_normals();
  sphere.set_one_ring();
  auto curvature = sphere.get_mean_curvature(sphere.one_ring);
  for (auto k_iter = curvature.begin(), n_iter = sphere.vertex_normals.begin();
       k_iter < curvature.begin() + 30; k_iter += 3, n_iter += 3) {
    std::cout << Linalg::dot(k_iter, n_iter) << " , ";
  }
  std::cout << "\n\n ++++++++++ norm ++++++\n";
  for (auto k_iter = curvature.begin(); k_iter < curvature.begin() + 30;
       k_iter += 3) {
    std::cout << Linalg::norm(k_iter) << " , ";
  }

  std::cout << "\n\n ++++++++++ Cube ++++++\n";
  Mesh cube = Primitives::cube();
  // cube.subdivide();
  cube.set_vertex_normals();
  cube.set_one_ring();
  curvature = cube.get_mean_curvature(cube.one_ring);
  std::cout << "\n\n ++++++++++ norm ++++++\n";

  for (auto k_iter = curvature.begin(); k_iter < curvature.end(); k_iter += 3) {
    std::cout << Linalg::norm(k_iter) << " , ";
  }
  std::cout << "\n";

  return 0;
}
