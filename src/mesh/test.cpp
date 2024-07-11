/* test render implementation */
#include "mesh.hpp"
#include <iostream>
#include <vector>

auto test_edges() -> int {
  Mesh cube = Primitives::cube();
  cube.set_edges();
  for (int i = 0; i < cube.edges.size() / 2; ++i) {
    std::cout << cube.edges.at(i * 2) << " , " << cube.edges.at(i * 2 + 1)
              << "\n";
  }

  std::cout << "\n+++++++++++++++\n\n";
  Mesh tet = Primitives::tetrahedron();
  tet.set_edges();
  for (int i = 0; i < tet.edges.size() / 2; ++i) {
    std::cout << tet.edges.at(i * 2) << " , " << tet.edges.at(i * 2 + 1)
              << "\n";
  }

  return 1;
}

auto main() -> int {
  // Define a list of points
  test_edges();

  return 0;
}
