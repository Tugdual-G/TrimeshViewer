#include "../src/mesh/mesh.hpp"
#include "../src/render/colormap.hpp"
#include "../src/render/linalg.hpp"
#include "../src/render/trimesh_render.hpp"
#include <cmath>
#include <random>
#include <vector>

void streamlines(std::vector<double> &coords, std::vector<double> &directions,
                 std::vector<unsigned int> &indices, int N,
                 void(f)(double *X, double *T, double t), double t0, double t1,
                 int n, int *n_it);

void biot_savart(const std::vector<double> &coord, std::vector<double> &vector,
                 void(f)(double *X, double *T, double t), double t0, double t1,
                 int n);

void torus_curve(double *X, double *T, double t);

void parametric_curve(double *X, double *T, double t);

auto main() -> int {
  void (*curve_path)(double *, double *, double) = parametric_curve;

  constexpr int N{200};

  std::vector<double> vortex_line_coords(N * 3);
  std::vector<double> vortex_line_tangents(N * 3);
  std::vector<double> vortex_line_colors{0.1, 0.8, 0.8};
  for (int i = 0; i < N; ++i) {
    curve_path(&vortex_line_coords[i * 3], &vortex_line_tangents[i * 3],
               (double)i * 1.0 / (N - 1));
  }

  std::vector<double> streamlines_coords;
  std::vector<double> streamlines_tangents;
  std::vector<unsigned int> streamlines_indices;

  int n_it{0};
  constexpr int N_streamlines{500};
  streamlines(streamlines_coords, streamlines_tangents, streamlines_indices,
              N_streamlines, curve_path, 0.0, 1.0, 64, &n_it);

  std::vector<double> velocity_magnitude(streamlines_coords.size() / 3, 0);
  // for (int i = 0; i < (int)velocity_magnitude.size(); ++i) {
  //   velocity_magnitude.at(i) =
  //       pow(Linalg::norm(&streamlines_tangents[i * 3]), 0.5);
  // }

  for (int i = 0; i < N_streamlines; ++i) {
    for (int j = 0; j < n_it; ++j) {
      velocity_magnitude.at(i * n_it + j) =
          j * (25 + Linalg::norm(&streamlines_tangents[(i * n_it + j) * 3]));
    }
  }
  std::vector<double> streamlines_colors =
      Colormap::get_nearest_colors(velocity_magnitude, Colormap::MAGMA);

  MeshRender render(500, 500);
  render.add_curve(vortex_line_coords, vortex_line_colors,
                   CurveType::SMOOTH_TUBE_CURVE, 0.02);

  render.add_curves(streamlines_coords, streamlines_colors, streamlines_indices,
                    CurveType::TUBE_CURVE, 0.005);

  render.render_loop(nullptr, nullptr);
  render.render_finalize();
  return 0;
}

void streamlines(std::vector<double> &coords, std::vector<double> &directions,
                 std::vector<unsigned int> &indices, int N,
                 void(f)(double *X, double *T, double t), double t0, double t1,
                 int n, int *n_it) {
  // Computes random streamlines in a plane for a torus of wire.

  std::uniform_real_distribution<double> unifx(-0.8, 0.8);
  std::uniform_real_distribution<double> unify(-0.01, 0.01);
  std::uniform_real_distribution<double> unifz(-0.8, 0.8);
  std::default_random_engine re;

  double dt = 0.001;
  double T{0.1};

  *n_it = T / dt;

  coords.resize(*n_it * N * 3);
  directions.resize(coords.size());
  indices.resize(4 * (coords.size() / 3 - 3));

  int idx{0};
  for (int i = 0; i < N; ++i) {
    for (int k = 0; k < *n_it - 3; ++k) {
      idx = i * *n_it + k;
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
    for (int it = 0; it < *n_it; ++it) {

      vector[0] = 0;
      vector[1] = 0;
      vector[2] = 0;
      idx = i * *n_it + it;

      coords.at(idx * 3) = coord[0];
      coords.at(idx * 3 + 1) = coord[1];
      coords.at(idx * 3 + 2) = coord[2];

      biot_savart(coord, vector, f, t0, t1, n);
      directions.at(idx * 3) = vector[0];
      directions.at(idx * 3 + 1) = vector[1];
      directions.at(idx * 3 + 2) = vector[2];

      coord_tmp[0] = coord[0] + dt * vector[0];
      coord_tmp[1] = coord[1] + dt * vector[1];
      coord_tmp[2] = coord[2] + dt * vector[2];
      vector_tmp = vector;
      biot_savart(coord_tmp, vector, f, t0, t1, n);
      vector = Linalg::vect_add(vector_tmp.data(), vector.data());

      coord[0] = coord[0] + dt * vector[0];
      coord[1] = coord[1] + dt * vector[1];
      coord[2] = coord[2] + dt * vector[2];
    }
  }
}

void biot_savart(const std::vector<double> &coord, std::vector<double> &vector,
                 void(f)(double *X, double *T, double t), double t0, double t1,
                 int n) {
  // Integrates the Biot&Savart Law over a curve.

  double dl{0};
  double dt = (t1 - t0) / n;

  std::vector<double> dist(3);
  double magnitude2{0};

  vector[0] = 0;
  vector[1] = 0;
  vector[2] = 0;

  std::vector<double> X(3);
  std::vector<double> T(3);

  for (int i = 0; i <= n; ++i) {
    f(X.data(), T.data(), dt * i + t0);

    dl = Linalg::norm(T.data()) * dt;

    dist = Linalg::vect_sub(coord.data(), X.data());
    magnitude2 = Linalg::dot(dist.data(), dist.data());

    // to avoid extremes values
    magnitude2 = magnitude2 > 1e-9 ? magnitude2 : 1e9;

    vector = Linalg::vect_add(
        Linalg::vect_scal_mult(
            Linalg::cross(T.data(), Linalg::normalize(dist).data()),
            dl / magnitude2),
        vector);
  }
};

void torus_curve(double *X, double *T, double t) {
  constexpr double Tau{2.0 * 3.1415926535898};
  constexpr double r{0.4};
  t *= Tau;
  X[0] = r * std::cos(t);
  X[1] = r * std::sin(t);
  X[2] = 0;
  // normalized tangent
  T[0] = -std::sin(t);
  T[1] = std::cos(t);
  T[0] = 0;
}

void parametric_curve(double *X, double *T, double t) {
  // parametric curve designed for t in [0.0, 1.0].
  constexpr double Tau{2.0 * 3.1415926535898};
  constexpr double r{0.3};
  t *= Tau;
  X[0] = r * std::cos(2.0 * t);
  X[1] = r * std::sin(0.5 * t) * std::sin(3.3 * t) + 0.5 * r;
  X[2] = 1.5 * r * (std::sin(t) + std::cos(t));

  // normalized tangent
  T[0] = -1.5 * std::sin(1.5 * t);
  T[1] = 0.5 * std::cos(0.5 * t) * std::sin(3.3 * t) +
         3.3 * std::sin(0.5 * t) * std::cos(3.3 * t);

  T[2] = (std::cos(t) - std::sin(t));
  double norm = std::pow(T[0] * T[0] + T[1] * T[1] + T[2] * T[2], 0.5);
  T[0] /= norm;
  T[1] /= norm;
  T[2] /= norm;
}
