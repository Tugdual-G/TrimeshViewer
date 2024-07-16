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
  std::cout << "\n\n ++++++++++ Test one-ring ++++++\n";
  std::cout << "\n\n ++++++++++ cube  ++++++\n";
  Mesh cube = Primitives::cube();
  cube.set_one_ring();
  cube.print_one_ring();

  std::cout << "\n\n ++++++++++ open cube  ++++++\n";
  cube.faces.resize(cube.faces.size() - 6);
  Mesh open_cube(cube.vertices, cube.faces);
  open_cube.set_one_ring();
  open_cube.print_one_ring();

  return 0;
}
