#ifndef LINALG_H_
#define LINALG_H_
#include <cmath>
#include <vector>

namespace Linalg {

auto dot(const double *u, const double *v) -> double {
  return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
};

auto dot(std::vector<double> u, std::vector<double> v) -> double {
  return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
};

auto norm(const double *vect) -> double { return pow(dot(vect, vect), 0.5); }

auto cross(const double *u, const double *v) -> std::vector<double> {
  std::vector<double> w(3);
  w[0] = u[1] * v[2] - u[2] * v[1];
  w[1] = u[2] * v[0] - u[0] * v[2];
  w[2] = u[0] * v[1] - u[1] * v[0];
  return w;
}

auto normalize(std::vector<double> u) -> std::vector<double> {
  std::vector<double> v(3);
  double inv_norm = 1.0 / norm(u.data());
  v[0] = u[0] * inv_norm;
  v[1] = u[1] * inv_norm;
  v[2] = u[2] * inv_norm;
  return v;
}

auto normalize(const double *u) -> std::vector<double> {
  std::vector<double> v(3);
  double inv_norm = 1.0 / norm(u);
  v[0] = u[0] * inv_norm;
  v[1] = u[1] * inv_norm;
  v[2] = u[2] * inv_norm;
  return v;
}

auto vect_scal_mult(const double *u, double alpha) -> std::vector<double> {
  std::vector<double> w(3);
  w[0] = u[0] * alpha;
  w[1] = u[1] * alpha;
  w[2] = u[2] * alpha;
  return w;
}

auto vect_add(const double *u, const double *v) -> std::vector<double> {
  std::vector<double> w(3);
  w[0] = u[0] + v[0];
  w[1] = u[1] + v[1];
  w[2] = u[2] + v[2];
  return w;
}

auto vect_sub(const double *u, const double *v) -> std::vector<double> {
  std::vector<double> w(3);
  w[0] = u[0] - v[0];
  w[1] = u[1] - v[1];
  w[2] = u[2] - v[2];
  return w;
}

auto normal(const double *v) -> std::vector<double> {
  // returns a vector normal to v.
  std::vector<double> n(3);
  std::vector<double> v_tmp = normalize(v);

  if (std::abs(v_tmp[0]) > 1e-6 || std::abs(v_tmp[1]) > 1e-6) {
    n[0] = -v_tmp[1];
    n[1] = v_tmp[0];
    n[2] = 0;
    return n;
  }
  n[0] = 1;
  n[1] = 0;
  n[2] = 0;
  return n;
}

} // namespace Linalg

#endif // LINALG_H_
