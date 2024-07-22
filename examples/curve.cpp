#include "../src/mesh/mesh.hpp"
#include "../src/render/trimesh_render.hpp"
#include <cmath>
#include <vector>

void parametric_curve(double *coord, double *tangent, double t) {
  // parametric curve designed for t in [0.0, 1.0].
  constexpr double Tau{2.0 * 3.1415926535898};
  constexpr double r{0.3};
  t *= Tau;
  coord[0] = r * cos(t);
  coord[1] = r * sin(2.0 * t);
  coord[2] = r * (sin(t) + cos(t));

  tangent[0] = -r * sin(t);
  tangent[1] = 2.0 * r * cos(2 * t);
  tangent[2] = r * (cos(t) - sin(t));
}

auto main() -> int {

  constexpr int N{40};

  std::vector<double> coords(N * 3);
  std::vector<double> tangents(N * 3);
  for (int i = 0; i < N; ++i) {
    parametric_curve(&coords[i * 3], &tangents[i * 3], (double)i * 1.0 / N);
  }

  Mesh torus = Primitives::torus(0.1, 0.01, 16);
  MeshRender render(500, 500, torus.vertices, torus.faces);
  render.add_curve(coords, tangents);

  render.render_loop(nullptr, nullptr);
  render.render_finalize();
  return 0;
}
