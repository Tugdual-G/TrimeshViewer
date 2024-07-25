#include "../src/mesh/mesh.hpp"
#include "../src/render/colormap.hpp"
#include "../src/render/trimesh_render.hpp"
#include <algorithm>
#include <cmath>
#include <vector>

void parametric_curve(double *coord, double *tangent, double t) {
  // parametric curve designed for t in [0.0, 1.0].
  constexpr double Tau{2.0 * 3.1415926535898};
  constexpr double r{0.3};
  t *= Tau;
  coord[0] = r * cos(1.5 * t);
  coord[1] = r * sin(3.3 * t) + 0.5 * r;
  coord[2] = 1.5 * r * (sin(t) + cos(t));

  tangent[0] = -1.5 * r * sin(1.5 * t);
  tangent[1] = 3.3 * r * cos(3.3 * t);
  tangent[2] = 1.5 * r * (cos(t) - sin(t));
}

auto norm(const double *v) -> double {
  return pow(v[0] * v[0] + v[1] * v[1] + v[2] * v[2], 0.5);
}

auto main() -> int {

  constexpr int N{300};

  std::vector<double> coords(N * 3);
  std::vector<double> tangents(N * 3);
  Mesh torus = Primitives::torus(0.2, 0.06, 16);

  MeshRender render(500, 500);
  render.add_object(torus.vertices, torus.faces);

  std::vector<double> velocity_magnitude(coords.size() / 3);
  for (int i = 0; i < N; ++i) {
    parametric_curve(&coords[i * 3], &tangents[i * 3],
                     (double)i * 1.0 / (N - 1));
    velocity_magnitude.at(i) = norm(&tangents[i * 3]);
  }

  const auto [min, max] =
      std::minmax_element(velocity_magnitude.begin(), velocity_magnitude.end());
  double range = *max - *min;

  std::vector<double> colors =
      Colormap::get_nearest_colors(velocity_magnitude, Colormap::PLASMA,
                                   *min - range * 0.4, *max + range * 0.2);

  render.add_curve(coords, colors, CurveType::SMOOTH_TUBE_CURVE, 0.01);

  render.render_loop(nullptr, nullptr);
  render.render_finalize();
  return 0;
}
