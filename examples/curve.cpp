#include "../src/mesh/mesh.hpp"
#include "../src/render/trimesh_render.hpp"
#include <cmath>
#include <vector>

void parametric_curve1(double *coord, double *tangent, double t) {
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

void parametric_curve2(double *coord, double *tangent, double t) {
  // parametric curve designed for t in [0.0, 1.0].
  constexpr double Tau{2.0 * 3.1415926535898};
  constexpr double r{0.3};
  t *= Tau;
  coord[0] = r * cos(1.5 * t);
  coord[1] = r * sin(3.3 * t) + 0.5 * r;
  coord[2] = 1.5 * r * (sin(t) + cos(t));

  tangent[0] = -1.5 * r * sin(1.5 * t);
  tangent[1] = 3.0 * r * cos(3 * t);
  tangent[2] = 1.5 * r * (cos(t) - sin(t));
}
auto main() -> int {

  constexpr int N{300};

  std::vector<double> coords(N * 3);
  std::vector<double> tangents(N * 3);
  for (int i = 0; i < N; ++i) {
    parametric_curve1(&coords[i * 3], &tangents[i * 3],
                      (double)i * 1.0 / (N - 1));
  }

  Mesh torus = Primitives::torus(0.2, 0.06, 16);

  MeshRender render(500, 500, torus.vertices, torus.faces);
  render.add_curve(coords, tangents);

  for (int i = 0; i < N; ++i) {
    parametric_curve2(&coords[i * 3], &tangents[i * 3],
                      (double)i * 1.0 / (N - 1));
  }
  render.add_curve(coords, tangents);

  render.render_loop(nullptr, nullptr);
  render.render_finalize();
  return 0;
}
