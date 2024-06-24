#ifndef QUATERN_TRANSFORM_H_
#define QUATERN_TRANSFORM_H_
#include "math.h"
#include <algorithm>
#include <iostream>
#include <vector>

// TODO templates

class Quaternion {

public:
  std::vector<double> q;

  double &operator[](unsigned int i) { return q[i]; }
  double operator[](unsigned int i) const { return q[i]; }
  double &at(unsigned int i) { return q.at(i); }
  double at(unsigned int i) const { return q.at(i); }

  Quaternion() : q(4, 0) {}

  Quaternion(double x, double y, double z, double w) : q(4) {
    q[0] = x;
    q[1] = y;
    q[2] = z;
    q[3] = w;
  }

  Quaternion(double &x, double &y, double &z, double &w) : q(4) {
    q.resize(4);
    q[0] = x;
    q[1] = y;
    q[2] = z;
    q[3] = w;
  }

  Quaternion(std::vector<double> &q) : q(q) {}

  Quaternion c();

  double norm2();

  double norm();

  Quaternion operator+(Quaternion v);

  void operator+=(Quaternion v);

  Quaternion operator-(Quaternion v);

  void operator-=(Quaternion v);

  Quaternion operator*(Quaternion v);
  // Quaternion operator*(Quaternion &v);

  Quaternion operator*(double v);
  // Quaternion operator*(double &v);

  void operator*=(double v);
  // void operator*=(double &v);

  void operator/=(double v);
  // void operator/=(double &v);
  Quaternion operator/(double v);

  Quaternion operator/(double &v);

  Quaternion inv();

  Quaternion operator/(Quaternion &v);

  void set(double x, double y, double z, double w);

  void multiply(Quaternion &u, Quaternion &v);

  void print();
};

#endif // QUATERN_TRANSFORM_H_
