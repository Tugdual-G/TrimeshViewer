#include "quatern_transform.hpp"
extern "C" {
#include "math.h"
}
#include <algorithm>
#include <iostream>
#include <vector>

auto Quaternion::c() -> Quaternion {
  Quaternion c(q[0], -q[1], -q[2], -q[3]);
  return c;
}

auto Quaternion::norm2() -> double {
  return pow(q[0], 2) + pow(q[1], 2) + pow(q[2], 2) + pow(q[3], 2);
}

auto Quaternion::norm() -> double { return pow(norm2(), 0.5); };

auto Quaternion::operator+(Quaternion v) -> Quaternion {
  Quaternion quatern;
  quatern[0] = this->q[0] + v[0];
  quatern[1] = this->q[1] + v[1];
  quatern[2] = this->q[2] + v[2];
  quatern[3] = this->q[3] + v[3];
  return quatern;
}

void Quaternion::operator+=(Quaternion v) {
  this->q[0] = this->q[0] + v[0];
  this->q[1] = this->q[1] + v[1];
  this->q[2] = this->q[2] + v[2];
  this->q[3] = this->q[3] + v[3];
}

auto Quaternion::operator-(Quaternion v) -> Quaternion {
  Quaternion quatern;
  quatern[0] = this->q[0] - v[0];
  quatern[1] = this->q[1] - v[1];
  quatern[2] = this->q[2] - v[2];
  quatern[3] = this->q[3] - v[3];
  return quatern;
}

void Quaternion::operator-=(Quaternion v) {
  this->q[0] = this->q[0] - v[0];
  this->q[1] = this->q[1] - v[1];
  this->q[2] = this->q[2] - v[2];
  this->q[3] = this->q[3] - v[3];
}

auto Quaternion::operator*(Quaternion v) -> Quaternion {
  // std::vector<double> q(4);
  Quaternion quatern(-v[1] * q[1] - v[2] * q[2] - v[3] * q[3] + v[0] * q[0],
                     v[1] * q[0] + v[2] * q[3] - v[3] * q[2] + v[0] * q[1],
                     -v[1] * q[3] + v[2] * q[0] + v[3] * q[1] + v[0] * q[2],
                     v[1] * q[2] - v[2] * q[1] + v[3] * q[0] + v[0] * q[3]);
  return quatern;
}

// Quaternion Quaternion::operator*(Quaternion &v) {
//   // std::vector<double> q(4);
//   Quaternion quatern(-v[1] * q[1] - v[2] * q[2] - v[3] * q[3] + v[0] * q[0],
//                      v[1] * q[0] + v[2] * q[3] - v[3] * q[2] + v[0] * q[1],
//                      -v[1] * q[3] + v[2] * q[0] + v[3] * q[1] + v[0] * q[2],
//                      v[1] * q[2] - v[2] * q[1] + v[3] * q[0] + v[0] * q[3]);

//   return quatern;
// }

auto Quaternion::operator*(double v) -> Quaternion {
  Quaternion quatern;
  quatern[0] = q[0] * v;
  quatern[1] = q[1] * v;
  quatern[2] = q[2] * v;
  quatern[3] = q[3] * v;
  return quatern;
}

// Quaternion Quaternion::operator*(double &v) {
//   Quaternion quatern;
//   quatern[0] = q[0] * v;
//   quatern[1] = q[1] * v;
//   quatern[2] = q[2] * v;
//   quatern[3] = q[3] * v;
//   return quatern;
// }

void Quaternion::operator*=(double v) {
  q[0] = q[0] * v;
  q[1] = q[1] * v;
  q[2] = q[2] * v;
  q[3] = q[3] * v;
}
// void Quaternion::operator*=(double &v) {
//   q[0] = q[0] * v;
//   q[1] = q[1] * v;
//   q[2] = q[2] * v;
//   q[3] = q[3] * v;
// }

void Quaternion::operator/=(double v) {
  q[0] = q[0] / v;
  q[1] = q[1] / v;
  q[2] = q[2] / v;
  q[3] = q[3] / v;
}
// void Quaternion::operator/=(double &v) {
//   q[0] = q[0] / v;
//   q[1] = q[1] / v;
//   q[2] = q[2] / v;
//   q[3] = q[3] / v;
// }

auto Quaternion::operator/(double v) -> Quaternion {
  Quaternion quatern;
  quatern[0] = q[0] / v;
  quatern[1] = q[1] / v;
  quatern[2] = q[2] / v;
  quatern[3] = q[3] / v;
  return quatern;
}

auto Quaternion::operator/(double &v) -> Quaternion {
  Quaternion quatern;
  quatern[0] = q[0] / v;
  quatern[1] = q[1] / v;
  quatern[2] = q[2] / v;
  quatern[3] = q[3] / v;
  return quatern;
}
auto Quaternion::inv() -> Quaternion { return c() / norm2(); }

auto Quaternion::operator/(Quaternion &v) -> Quaternion { return (*this * v.inv()); }

void Quaternion::set(double x, double y, double z, double w) {
  q[0] = x;
  q[1] = y;
  q[2] = z;
  q[3] = w;
};

void Quaternion::multiply(Quaternion &u, Quaternion &v) {
  // A more efficient way of multiplying without storing temp value;
  this->set(-v[1] * u[1] - v[2] * u[2] - v[3] * u[3] + v[0] * u[0],
            v[1] * u[0] + v[2] * u[3] - v[3] * u[2] + v[0] * u[1],
            -v[1] * u[3] + v[2] * u[0] + v[3] * u[1] + v[0] * u[2],
            v[1] * u[2] - v[2] * u[1] + v[3] * u[0] + v[0] * u[3]);
}

void Quaternion::print() {
  std::cout << q[0] << " ";
  std::cout << q[1] << " ";
  std::cout << q[2] << " ";
  std::cout << q[3] << '\n';
}

////////////////////////////////////////////////////////////////
//
//    A little bit faster than vector<Quaternion> q  (~2x faster for
//    multiplications)
//
////////////////////////////////////////////////////////////////
//
//
// void mul_quatern_vect(double *u, double *v, double *uv) {
//   double uv_tmp[4];
//   uv_tmp[0] = -v[1] * u[1] - v[2] * u[2] - v[3] * u[3] + v[0] * u[0];
//   uv_tmp[1] = v[1] * u[0] + v[2] * u[3] - v[3] * u[2] + v[0] * u[1];
//   uv_tmp[2] = -v[1] * u[3] + v[2] * u[0] + v[3] * u[1] + v[0] * u[2];
//   uv_tmp[3] = v[1] * u[2] - v[2] * u[1] + v[3] * u[0] + v[0] * u[3];
//   uv[0] = uv_tmp[0];// In case *u = *uv
//   uv[1] = uv_tmp[1];
//   uv[2] = uv_tmp[2];
//   uv[3] = uv_tmp[3];
// }

// class QuaternionVector {
// public:
//   unsigned int n;
//   std::vector<double> qv;
//   QuaternionVector(std::vector<double> iqv) : n(iqv.size() / 4), qv(iqv) {}

//   QuaternionVector(unsigned int n) : n(n), qv(n * 4) {}

//   double &operator[](unsigned int i) { return qv[i]; }
//   double operator[](unsigned int i) const { return qv[i]; }

//   QuaternionVector operator*(Quaternion &v) {
//     QuaternionVector uv(this->n);
//     for (double *u = qv.data(), *uvp = uv.qv.data(); u < qv.data() + n * 4;
//          u += 4, uvp += 4) {
//       mul_quatern_vect(u, v.q.data(), uvp);
//     }
//     return uv;
//   }

//   void multiply(QuaternionVector &u, Quaternion &v) {
//     for (unsigned int i = 0; i < n; ++i) {
//       mul_quatern_vect(&u[i * 4], v.q.data(), &qv[i * 4]);
//     }
//   }

//   void print(unsigned int k) {
//     for (unsigned int i = 0; i < 4; ++i) {
//       std::cout << qv[4 * k + i] << " ";
//     }
//     std::cout << std::endl;
//   }
// };
