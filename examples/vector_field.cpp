#include "../src/mesh/mesh.hpp"
#include "../src/render/colormap.hpp"
#include "../src/render/trimesh_render.hpp"
#include <cmath>
#include <iostream>
#include <vector>

namespace Linalg {
auto norm(const double *vect) -> double;

auto cross(const double *u, const double *v) -> std::vector<double>;

void normalize(double *w);

template <class T>
auto vect_scal_mult(std::vector<T> u, T alpha) -> std::vector<T>;

template <class T>
auto vect_add(std::vector<T> u, std::vector<T> v) -> std::vector<T>;
} // namespace Linalg

void vortex_ring(std::vector<double> &coords, std::vector<double> &direction,
                 double R, double r, int n);

auto main() -> int {

  Mesh torus = Primitives::torus(0.4, 0.1, 16);
  torus.set_one_ring();
  std::vector<double> kn = torus.get_mean_curvature(torus.one_ring);
  std::vector<double> k = torus.get_scalar_mean_curvature(kn);
  std::vector<double> colors =
      Colormap::get_nearest_colors(k, Colormap::VIRIDIS);

  std::vector<double> vortex_coords;
  std::vector<double> vortex_directions;
  vortex_ring(vortex_coords, vortex_directions, 0.4, 0.3, 24);
  for (auto &d : vortex_directions) {
    d *= 0.05;
  }

  double length{0};
  std::vector<double> vortex_scalval(vortex_coords.size() / 3, 0);
  // gives color to the vectors and changes their length
  for (int i = 0; i < (int)vortex_scalval.size(); ++i) {
    length = Linalg::norm(&vortex_coords[i * 3]);
    vortex_scalval.at(i) = -length;
    length = 1 / (2 * std::sqrt(length));
    vortex_directions.at(i * 3) *= length;
    vortex_directions.at(i * 3 + 1) *= length;
    vortex_directions.at(i * 3 + 2) *= length;
  }
  std::vector<double> vect_colors =
      Colormap::get_nearest_colors(vortex_scalval, Colormap::MAGMA, -1.0, 0.0);

  MeshRender render(500, 500);
  render.add_mesh(torus.vertices, torus.faces, colors);

  render.add_vectors(vortex_coords, vortex_directions, vect_colors);

  render.render_loop(nullptr, nullptr);
  render.render_finalize();
  return 0;
}

void vortex_ring(std::vector<double> &coords, std::vector<double> &direction,
                 double R, double r, int n) {

  constexpr double pi{3.1415926535898};
  int N = (int)n * R / r; // number of discrete sections
  N = (N >> 1) << 1;      // N should be pair;

  // Great radius unit vector
  std::vector<double> e_R(3);
  std::vector<double> e_phi(3);
  std::vector<double> e_r(3);

  // Great radius vector angle
  double phi{0};

  // Small radius vector angle
  double theta{0};

  coords.resize(n * N * 3);
  direction.resize(n * N * 3);
  std::vector<double> coord;
  std::vector<double> vector;
  for (int i = 0; i < N; ++i) {
    phi = i * 2 * pi / N;
    e_R.at(0) = std::cos(phi);
    e_R.at(1) = std::sin(phi);
    e_phi.at(0) = -e_R.at(1);
    e_phi.at(1) = e_R.at(0);
    for (int j = 0; j < n; ++j) {
      theta = j * 2 * pi / n;
      theta += (i % 2) * pi / n;
      e_r = Linalg::vect_scal_mult(e_R, r * std::cos(theta));
      e_r.at(2) = r * std::sin(theta);

      coord = Linalg::vect_add(Linalg::vect_scal_mult(e_R, R), e_r);

      std::copy(coord.begin(), coord.end(), coords.begin() + (i * n + j) * 3);

      Linalg::normalize(e_r.data());
      vector = Linalg::cross(e_r.data(), e_phi.data());
      std::copy(vector.begin(), vector.end(),
                direction.begin() + (i * n + j) * 3);
    }
  }
}

namespace Linalg {
auto norm(const double *vect) -> double {
  return pow(vect[0] * vect[0] + vect[1] * vect[1] + vect[2] * vect[2], 0.5);
}

auto cross(const double *u, const double *v) -> std::vector<double> {
  std::vector<double> w(3);
  w[0] = u[1] * v[2] - u[2] * v[1];
  w[1] = u[2] * v[0] - u[0] * v[2];
  w[2] = u[0] * v[1] - u[1] * v[0];
  return w;
}

void normalize(double *w) {
  double inv_norm =
      1.0 / pow(pow(w[0], 2.0) + pow(w[1], 2.0) + pow(w[2], 2.0), 0.5);
  w[0] *= inv_norm;
  w[1] *= inv_norm;
  w[2] *= inv_norm;
}

template <class T>
auto vect_scal_mult(std::vector<T> u, T alpha) -> std::vector<T> {
  std::vector<T> w(u.size());
  for (long unsigned int i = 0; i < u.size(); ++i) {
    w[i] = u[i] * alpha;
  }
  return w;
}
template <class T>
auto vect_add(std::vector<T> u, std::vector<T> v) -> std::vector<T> {
  if (u.size() != v.size()) {
    throw "\n add_vect error \n";
  }
  std::vector<T> w(u.size());
  for (long unsigned int i = 0; i < u.size(); ++i) {
    w[i] = u[i] + v[i];
  }
  return w;
}

} // namespace Linalg
