/* test implementation */
#include "../mesh.hpp"
#include <iostream>
#include <vector>

auto main() -> int {
  std::cout << "\n++++++++ Test edges +++++++\n\n";

  Mesh cube = Primitives::cube();
  cube.set_edges();
  for (int i = 0; i < (int)cube.edges.size() / 2; ++i) {
    std::cout << cube.edges.at(i * 2) << " , " << cube.edges.at(i * 2 + 1)
              << "\n";
  }

  std::cout << "\n+++++++++++++++\n\n";
  Mesh tet = Primitives::tetrahedron();
  tet.set_edges();
  for (int i = 0; i < (int)tet.edges.size() / 2; ++i) {
    std::cout << tet.edges.at(i * 2) << " , " << tet.edges.at(i * 2 + 1)
              << "\n";
  }

  return 0;
}
