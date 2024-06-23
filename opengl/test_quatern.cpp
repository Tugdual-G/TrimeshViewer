#include "quatern_transform.h"
#include <iostream>
#include <vector>

int main() {
  Quaternion q1(0, 1, 2, 3);
  Quaternion q2(1, 2, 3, 4);
  std::vector<double> v(4);
  v[0] = 4;
  v[1] = 3;
  v[2] = 2;
  v[3] = 1;
  Quaternion q3(v);

  std::cout << "  q1 =  ";
  q1.print_quaternion();
  std::cout << "  q2 =  ";
  q2.print_quaternion();
  std::cout << "  q3 =  ";
  q3.print_quaternion();

  std::cout << std::endl;
  Quaternion q1x2, q1m3, q1p2, invq3;

  q1p2 = q1 + q2;
  q1m3 = q3 - q2;
  q1x2 = q1 * q2;
  invq3 = q3 * q3.inv();

  std::cout << "  q1p2 = q1 + q2 =  ";
  q1p2.print_quaternion();
  std::cout << std::endl;

  std::cout << "  q1m3 = q3 - q2 =  ";
  q1m3.print_quaternion();
  std::cout << std::endl;

  std::cout << "  q1x2 = q1 * q2 =  ";
  q1x2.print_quaternion();
  std::cout << std::endl;

  std::cout << "  q3_inv*q3 =  ";
  invq3.print_quaternion();
  std::cout << std::endl;

  std::cout << "  norm(q1) =  " << q1.norm();
  std::cout << std::endl;

  std::cout << "  norm2(q1) =  " << q1.norm2();
  std::cout << std::endl;

  std::cout << "  norm(q1)*norm(q1) =  " << q1.norm() * q1.norm();
  std::cout << std::endl;

  Quaternion gg{0, 1, 2, 3};
  std::cout << "  gg =  " << std::endl;
  gg.print_quaternion();
  std::cout << std::endl;
  return 0;
}
