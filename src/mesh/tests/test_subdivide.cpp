/* test implementation */
#include "../mesh.hpp"
#include <iostream>
#include <vector>

auto main() -> int {
  std::cout << "\n++++++++ Test subdivide +++++++\n\n";
  Mesh tet = Primitives::tetrahedron();
  tet.set_edges();
  for (int i = 0; i < (int)tet.edges.size() / 2; ++i) {
    std::cout << tet.edges.at(i * 2) << " , " << tet.edges.at(i * 2 + 1)
              << "\n";
  }
  std::cout << "\n+++++++++++++++\n\n";
  tet.subdivide();
  for (int i = 0; i < (int)tet.edges.size() / 2; ++i) {
    std::cout << tet.edges.at(i * 2) << " , " << tet.edges.at(i * 2 + 1)
              << "\n";
  }
  tet.print_faces();

  return 0;
}
