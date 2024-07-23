#include "../src/mesh/mesh.hpp"
#include "../src/render/colormap.hpp"
#include "../src/render/trimesh_render.hpp"
#include <cmath>
#include <random>
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

void magnetic_streamlines(std::vector<double> &coords,
                          std::vector<double> &directions,
                          std::vector<unsigned int> &indices, double R, int N,
                          int n_ring_sections);

auto main() -> int {

  Mesh torus = Primitives::torus(0.3, 0.04, 32);
  torus.set_one_ring();
  std::vector<double> kn = torus.get_mean_curvature(torus.one_ring);
  std::vector<double> k = torus.get_scalar_mean_curvature(kn);
  std::vector<double> colors =
      Colormap::get_nearest_colors(k, Colormap::VIRIDIS);

  std::vector<double> streamlines_coords;
  std::vector<double> streamlines_tangents;
  std::vector<unsigned int> streamlines_indices;
  magnetic_streamlines(streamlines_coords, streamlines_tangents,
                       streamlines_indices, 0.3, 2000, 36);

  double length{0};
  std::vector<double> vortex_scalval(streamlines_coords.size() / 3, 0);
  // gives color to the vectors and changes their length
  for (int i = 0; i < (int)vortex_scalval.size(); ++i) {
    length = Linalg::norm(&streamlines_tangents[i * 3]);
    Linalg::normalize(&streamlines_tangents[i * 3]);
    length = pow(length / 290.0, 0.6);
    vortex_scalval.at(i) = length;
  }
  std::vector<double> vect_colors =
      Colormap::get_nearest_colors(vortex_scalval, Colormap::MAGMA);

  MeshRender render(500, 500, torus.vertices, torus.faces, colors);

  render.add_curves(streamlines_coords, streamlines_tangents,
                    streamlines_indices, MeshRender::ObjectType::QUAD_CURVE);

  render.render_loop(nullptr, nullptr);
  render.render_finalize();
  return 0;
}

void magnetic_field(const std::vector<double> &coord,
                    std::vector<double> &vector, double R,
                    int n_ring_sections) {
  // Integrates the Biot&Savart Law over a ring.

  constexpr double pi{3.1415926535898};

  // Great radius unit vector
  std::vector<double> e_R(3, 0);
  std::vector<double> e_phi(3, 0);

  // Great radius vector angle
  double phi{0};
  double dl = 2.0 * pi * R / n_ring_sections;

  std::vector<double> dist(3);
  double magnitude{0};

  vector[0] = 0;
  vector[1] = 0;
  vector[2] = 0;

  for (int torus_section_idx = 0; torus_section_idx < n_ring_sections;
       ++torus_section_idx) {
    phi = torus_section_idx * 2 * pi / n_ring_sections;

    e_R[0] = cos(phi);
    e_R[1] = sin(phi);
    e_phi[0] = -e_R[1];
    e_phi[1] = e_R[0];

    dist = Linalg::vect_sub(coord, Linalg::vect_scal_mult(e_R, R));
    magnitude = (pow(dist[0], 2) + pow(dist[1], 2) + pow(dist[2], 2));
    magnitude = magnitude > 1e-9 ? magnitude : 1e9;

    vector = Linalg::vect_add(
        Linalg::vect_scal_mult(
            Linalg::cross(e_phi.data(), Linalg::normalize(dist).data()),
            dl / magnitude),
        vector);
  }
};

void magnetic_streamlines(std::vector<double> &coords,
                          std::vector<double> &directions,
                          std::vector<unsigned int> &indices, double R, int N,
                          int n_ring_sections) {

  constexpr double pi{3.1415926535898};

  std::uniform_real_distribution<double> unifx(-1.5, 1.5);
  std::uniform_real_distribution<double> unify(-0.02, 0.02);
  std::uniform_real_distribution<double> unifz(-1.5, 0.5);
  std::default_random_engine re;

  double dt = 0.001;
  double T{0.1};

  int n_it = T / dt;

  coords.resize(n_it * N * 3);
  directions.resize(coords.size());
  indices.resize(4 * (coords.size() / 3 - 3));

  int idx{0};
  for (int i = 0; i < N; ++i) {
    for (int k = 0; k < n_it - 3; ++k) {
      idx = i * n_it + k;
      indices.at(idx * 4) = idx;
      indices.at(idx * 4 + 1) = idx + 1;
      indices.at(idx * 4 + 2) = idx + 2;
      indices.at(idx * 4 + 3) = idx + 3;
    }
  }

  std::vector<double> coord(3);
  std::vector<double> vector(3);
  std::vector<double> coord_tmp(3);
  std::vector<double> vector_tmp(3);
  dt /= 2.0;

  for (int i = 0; i < N; ++i) {
    coord[0] = unifx(re);
    coord[1] = unify(re);
    coord[2] = unifx(re);
    for (int it = 0; it < n_it; ++it) {

      vector[0] = 0;
      vector[1] = 0;
      vector[2] = 0;
      idx = i * n_it + it;

      coords.at(idx * 3) = coord[0];
      coords.at(idx * 3 + 1) = coord[1];
      coords.at(idx * 3 + 2) = coord[2];

      magnetic_field(coord, vector, R, n_ring_sections);
      directions.at(idx * 3) = vector[0];
      directions.at(idx * 3 + 1) = vector[1];
      directions.at(idx * 3 + 2) = vector[2];

      coord_tmp[0] = coord[0] + dt * vector[0];
      coord_tmp[1] = coord[1] + dt * vector[1];
      coord_tmp[2] = coord[2] + dt * vector[2];
      vector_tmp = vector;
      magnetic_field(coord_tmp, vector, R, n_ring_sections);
      vector = Linalg::vect_add(vector_tmp, vector);

      coord[0] = coord[0] + dt * vector[0];
      coord[1] = coord[1] + dt * vector[1];
      coord[2] = coord[2] + dt * vector[2];
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
