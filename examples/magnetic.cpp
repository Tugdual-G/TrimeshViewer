#include "../src/mesh/mesh.hpp"
#include "../src/render/colormap.hpp"
#include "../src/render/trimesh_render.hpp"
#include <cmath>
#include <iostream>
#include <vector>

namespace Linalg {
auto norm(const double *vect) -> double;

auto cross(const double *u, const double *v) -> std::vector<double>;

auto normalize(std::vector<double> u) -> std::vector<double>;

void normalize(double *u);

template <class T>
auto vect_scal_mult(std::vector<T> u, T alpha) -> std::vector<T>;

template <class T>
auto vect_add(std::vector<T> u, std::vector<T> v) -> std::vector<T>;

template <class T>
auto vect_sub(std::vector<T> u, std::vector<T> v) -> std::vector<T>;
} // namespace Linalg

void magnetic_field(std::vector<double> &coords,
                    std::vector<double> &directions, double R, int N,
                    int n_ring_sections);

auto main() -> int {

  Mesh torus = Primitives::torus(0.3, 0.05, 16);
  torus.set_one_ring();
  std::vector<double> kn = torus.get_mean_curvature(torus.one_ring);
  std::vector<double> k = torus.get_scalar_mean_curvature(kn);
  std::vector<double> colors =
      Colormap::get_nearest_colors(k, Colormap::VIRIDIS);

  std::vector<double> vortex_coords;
  std::vector<double> vortex_directions;
  magnetic_field(vortex_coords, vortex_directions, 0.3, 40, 36);
  for (auto &d : vortex_directions) {
    d *= 0.1;
  }

  double length{0};
  std::vector<double> vortex_scalval(vortex_coords.size() / 3, 0);
  // gives color to the vectors and changes their length
  for (int i = 0; i < (int)vortex_scalval.size(); ++i) {
    length = Linalg::norm(&vortex_directions[i * 3]);
    Linalg::normalize(&vortex_directions[i * 3]);
    length = pow(length / 80.0, 0.7);
    length = length > 0.1 ? 0 : length;
    vortex_scalval.at(i) = length;
    vortex_directions.at(i * 3) *= length;
    vortex_directions.at(i * 3 + 1) *= length;
    vortex_directions.at(i * 3 + 2) *= length;
  }
  std::vector<double> vect_colors =
      Colormap::get_nearest_colors(vortex_scalval, Colormap::MAGMA);

  MeshRender render(500, 500, torus.vertices, torus.faces, colors);

  render.add_vectors(vortex_coords, vortex_directions, vect_colors);

  render.render_loop(nullptr, nullptr);
  render.render_finalize();
  return 0;
}

void magnetic_field(std::vector<double> &coords,
                    std::vector<double> &directions, double R, int N,
                    int n_ring_sections) {

  constexpr double pi{3.1415926535898};

  // Great radius unit vector
  std::vector<double> e_R(3);
  std::vector<double> e_phi(3);
  std::vector<double> e_r(3);

  // Great radius vector angle
  double phi{0};
  double dl = 2.0 * pi * R / n_ring_sections;
  double dx = 2.0 / N;

  coords.resize(N * N * N * 3);
  directions.resize(coords.size());

  std::vector<double> coord(3);
  std::vector<double> vector(3);
  std::vector<double> dist(3);
  double magnitude{0};

  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      for (int k = 0; k < N; ++k) {
        coord[0] = (i + 0.5) * dx - 1.0;
        coord[1] = (j + 0.5) * dx - 1.0;
        coord[2] = (k + 0.5) * dx - 1.0;

        directions.at((i * (N * N) + j * N + k) * 3) = 0;
        directions.at((i * (N * N) + j * N + k) * 3 + 1) = 0;
        directions.at((i * (N * N) + j * N + k) * 3 + 2) = 0;

        for (int torus_section_idx = 0; torus_section_idx < n_ring_sections;
             ++torus_section_idx) {
          phi = torus_section_idx * 2 * pi / n_ring_sections;

          e_R.at(0) = std::cos(phi);
          e_R.at(1) = std::sin(phi);
          e_phi.at(0) = -e_R.at(1);
          e_phi.at(1) = e_R.at(0);

          dist = Linalg::vect_sub(coord, Linalg::vect_scal_mult(e_R, R));
          vector = Linalg::cross(e_phi.data(), Linalg::normalize(dist).data());
          magnitude = (pow(dist[0], 2) + pow(dist[1], 2) + pow(dist[2], 2));
          magnitude = magnitude > 1e-9 ? magnitude : 1e9;

          vector = Linalg::vect_scal_mult(vector, dl / magnitude);

          directions.at((i * (N * N) + j * N + k) * 3) += vector[0];
          directions.at((i * (N * N) + j * N + k) * 3 + 1) += vector[1];
          directions.at((i * (N * N) + j * N + k) * 3 + 2) += vector[2];
        }
        coords.at((i * (N * N) + j * N + k) * 3) = coord[0];
        coords.at((i * (N * N) + j * N + k) * 3 + 1) = coord[1];
        coords.at((i * (N * N) + j * N + k) * 3 + 2) = coord[2];
      }
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

auto normalize(std::vector<double> u) -> std::vector<double> {
  std::vector<double> v(3);
  double inv_norm =
      1.0 / pow(pow(u[0], 2.0) + pow(u[1], 2.0) + pow(u[2], 2.0), 0.5);
  v[0] = u[0] * inv_norm;
  v[1] = u[1] * inv_norm;
  v[2] = u[2] * inv_norm;
  return v;
}

void normalize(double *u) {
  double inv_norm =
      1.0 / pow(pow(u[0], 2.0) + pow(u[1], 2.0) + pow(u[2], 2.0), 0.5);
  u[0] *= inv_norm;
  u[1] *= inv_norm;
  u[2] *= inv_norm;
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

template <class T>
auto vect_sub(std::vector<T> u, std::vector<T> v) -> std::vector<T> {
  if (u.size() != v.size()) {
    throw "\n add_vect error \n";
  }
  std::vector<T> w(u.size());
  for (long unsigned int i = 0; i < u.size(); ++i) {
    w[i] = u[i] - v[i];
  }
  return w;
}

} // namespace Linalg
