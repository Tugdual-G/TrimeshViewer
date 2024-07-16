/* test implementation */
#include "../mesh.hpp"
#include <cmath>
#include <iostream>
#include <vector>

auto main() -> int {
  std::cout << "\n\n ++++++++++ Test vertex adjacent faces ++++++\n";

  std::cout << "\n\n ++++++++++ open cube  ++++++\n";
  Mesh cube = Primitives::cube();
  cube.faces.resize(cube.faces.size() - 6);
  Mesh open_cube(cube.vertices, cube.faces);
  open_cube.print_faces();
  open_cube.set_vertex_adjacent_faces();
  open_cube.print_vertex_adjacent_face();

  return 0;
}
