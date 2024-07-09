#ifndef QUATERN_TRANSFORM_H_
#define QUATERN_TRANSFORM_H_
#include <cmath>
#include <vector>

// TODO templates

class Quaternion {

public:
  std::vector<double> q;

  auto operator[](unsigned int i) -> double & { return q[i]; }
  auto operator[](unsigned int i) const -> double { return q[i]; }
  auto at(unsigned int i) -> double & { return q.at(i); }
  [[nodiscard]] auto at(unsigned int i) const -> double { return q.at(i); }

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

  auto c() -> Quaternion;

  auto norm2() -> double;

  auto norm() -> double;

  auto operator+(Quaternion v) -> Quaternion;

  void operator+=(Quaternion v);

  auto operator-(Quaternion v) -> Quaternion;

  void operator-=(Quaternion v);

  auto operator*(Quaternion v) -> Quaternion;
  // Quaternion operator*(Quaternion &v);

  auto operator*(double v) -> Quaternion;
  // Quaternion operator*(double &v);

  void operator*=(double v);
  // void operator*=(double &v);

  void operator/=(double v);
  // void operator/=(double &v);
  auto operator/(double v) -> Quaternion;

  auto operator/(double &v) -> Quaternion;

  auto inv() -> Quaternion;

  auto operator/(Quaternion &v) -> Quaternion;

  void set(double x, double y, double z, double w);

  void multiply(Quaternion &u, Quaternion &v);

  void print();
};

#endif // QUATERN_TRANSFORM_H_
