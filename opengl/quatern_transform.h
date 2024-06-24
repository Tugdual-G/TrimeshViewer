#ifndef QUATERN_TRANSFORM_H_
#define QUATERN_TRANSFORM_H_
#include "math.h"
#include <iostream>
#include <vector>

class Quaternion {

public:
  std::vector<double> q;
  double *x;
  double *y;
  double *z;
  double *w;

  Quaternion() { q.resize(4); }

  Quaternion(double x, double y, double z, double w) {
    q.resize(4);
    q[0] = x;
    q[1] = y;
    q[2] = z;
    q[3] = w;
    this->x = &q[0];
    this->y = &q[1];
    this->z = &q[2];
    this->w = &q[3];
  }

  Quaternion(double &x, double &y, double &z, double &w) {
    q.resize(4);
    q[0] = x;
    q[1] = y;
    q[2] = z;
    q[3] = w;
    this->x = &q[0];
    this->y = &q[1];
    this->z = &q[2];
    this->w = &q[3];
  }

  Quaternion(std::vector<double> &q) : q(q) {
    if (q.size() != 4) {
      q.resize(4);
    }
    this->x = &q[0];
    this->y = &q[1];
    this->z = &q[2];
    this->w = &q[3];
  }

  Quaternion c() {
    Quaternion c(q[0], -q[1], -q[2], -q[3]);
    return c;
  }

  double norm2() {
    return pow(q[0], 2) + pow(q[1], 2) + pow(q[2], 2) + pow(q[3], 2);
  }

  double norm() { return pow(norm2(), 0.5); };

  Quaternion operator+(Quaternion v) {
    Quaternion quatern;
    quatern.q[0] = this->q[0] + v.q[0];
    quatern.q[1] = this->q[1] + v.q[1];
    quatern.q[2] = this->q[2] + v.q[2];
    quatern.q[3] = this->q[3] + v.q[3];
    return quatern;
  }

  void operator+=(Quaternion v) {
    this->q[0] = this->q[0] + v.q[0];
    this->q[1] = this->q[1] + v.q[1];
    this->q[2] = this->q[2] + v.q[2];
    this->q[3] = this->q[3] + v.q[3];
  }

  Quaternion operator-(Quaternion v) {
    Quaternion quatern;
    quatern.q[0] = this->q[0] - v.q[0];
    quatern.q[1] = this->q[1] - v.q[1];
    quatern.q[2] = this->q[2] - v.q[2];
    quatern.q[3] = this->q[3] - v.q[3];
    return quatern;
  }

  void operator-=(Quaternion v) {
    this->q[0] = this->q[0] - v.q[0];
    this->q[1] = this->q[1] - v.q[1];
    this->q[2] = this->q[2] - v.q[2];
    this->q[3] = this->q[3] - v.q[3];
  }

  Quaternion operator*(Quaternion v) {
    // std::vector<double> q(4);
    Quaternion quatern(-v.q[1] * this->q[1] - v.q[2] * this->q[2] -
                           v.q[3] * this->q[3] + v.q[0] * this->q[0],
                       v.q[1] * this->q[0] + v.q[2] * this->q[3] -
                           v.q[3] * this->q[2] + v.q[0] * this->q[1],
                       -v.q[1] * this->q[3] + v.q[2] * this->q[0] +
                           v.q[3] * this->q[1] + v.q[0] * this->q[2],
                       v.q[1] * this->q[2] - v.q[2] * this->q[1] +
                           v.q[3] * this->q[0] + v.q[0] * this->q[3]);

    return quatern;
  }

  Quaternion operator*(double v) {
    Quaternion quatern;
    quatern.q[0] = this->q[0] * v;
    quatern.q[1] = this->q[1] * v;
    quatern.q[2] = this->q[2] * v;
    quatern.q[3] = this->q[3] * v;
    return quatern;
  }

  void operator*=(double v) {
    this->q[0] = this->q[0] * v;
    this->q[1] = this->q[1] * v;
    this->q[2] = this->q[2] * v;
    this->q[3] = this->q[3] * v;
  }

  void operator/=(double v) {
    this->q[0] = this->q[0] / v;
    this->q[1] = this->q[1] / v;
    this->q[2] = this->q[2] / v;
    this->q[3] = this->q[3] / v;
  }

  Quaternion operator/(double v) {
    Quaternion quatern;
    quatern.q[0] = this->q[0] / v;
    quatern.q[1] = this->q[1] / v;
    quatern.q[2] = this->q[2] / v;
    quatern.q[3] = this->q[3] / v;
    return quatern;
  }

  Quaternion inv() { return c() / norm2(); }

  Quaternion operator/(Quaternion &v) { return (*this * v.inv()); }

  void print_quaternion() {
    std::cout << q[0] << " ";
    std::cout << q[1] << " ";
    std::cout << q[2] << " ";
    std::cout << q[3] << std::endl;
  }
};

#endif // QUATERN_TRANSFORM_H_
