#include "axiscross.hpp"
#include "quatern_transform.hpp"
#include <iostream>
#include <math.h>
#include <numeric>
#include <vector>

#define pi 3.14159265359
#define N 8
#define r_e 0.08
#define r_i 0.04
#define base_length 0.8
#define sqrt2 1.4142

int main() {

  std::vector<unsigned int> faces_true = {
      0,  1,  8,  //
      1,  2,  9,  //
      2,  3,  10, //
      3,  4,  11, //
      4,  5,  12, //
      5,  6,  13, //
      6,  7,  14, //
      7,  0,  15, //
      1,  9,  8,  //
      2,  10, 9,  //
      3,  11, 10, //
      4,  12, 11, //
      5,  13, 12, //
      6,  14, 13, //
      7,  15, 14, //
      0,  8,  15, //
      8,  9,  16, //
      9,  10, 17, //
      10, 11, 18, //
      11, 12, 19, //
      12, 13, 20, //
      13, 14, 21, //
      14, 15, 22, //
      15, 8,  23, //
      9,  17, 16, //
      10, 18, 17, //
      11, 19, 18, //
      12, 20, 19, //
      13, 21, 20, //
      14, 22, 21, //
      15, 23, 22, //
      8,  16, 23, //
      16, 17, 24, //
      17, 18, 24, //
      18, 19, 24, //
      19, 20, 24, //
      20, 21, 24, //
      21, 22, 24, //
      22, 23, 24, //
      23, 16, 24, //
  };

  std::vector<double> vertices_xaxis((3 * N + 1) * 3, 0);
  std::vector<Quaternion> vert_quat;
  vert_quat.resize(3 * N + 1);
  std::vector<unsigned int> faces((5 * N) * 3, 0);
  std::vector<double> theta_ext(N);
  std::vector<double> theta_int(N);

  for (unsigned int i = 0; i < N; ++i) {
    theta_ext.at(i) = 3 * pi / N * i;
    theta_int.at(i) = 2 * pi / N * i;
  }

  // shaft base
  for (unsigned int i = 0; i < N; ++i) {
    vertices_xaxis[3 * i + 1] = r_i * sin(theta_int.at(i));
    vertices_xaxis[3 * i + 2] = r_i * cos(theta_int.at(i));
  }
  // cone junction
  for (unsigned int i = 0; i < N; ++i) {
    vertices_xaxis[3 * (i + N)] = base_length;
    vertices_xaxis[3 * (i + N) + 1] = r_i * sin(theta_int.at(i));
    vertices_xaxis[3 * (i + N) + 2] = r_i * cos(theta_int.at(i));
  }
  // cone base
  for (unsigned int i = 0; i < N; ++i) {
    vertices_xaxis[3 * (i + 2 * N)] = base_length;
    vertices_xaxis[3 * (i + 2 * N) + 1] = r_e * sin(theta_int.at(i));
    vertices_xaxis[3 * (i + 2 * N) + 2] = r_e * cos(theta_int.at(i));
  }
  // tip
  vertices_xaxis.at(3 * N) = 1;

  // shaft
  for (unsigned int i = 0; i < N; ++i) {
    faces[3 * i] = i;
    faces[3 * i + 1] = (i + 1) % N;
    faces[3 * i + 2] = N + i;
  }
  for (unsigned int i = 0; i < N; ++i) {
    faces[3 * (i + N)] = (i + 1) % N;
    faces[3 * (i + N) + 1] = N + (i + 1) % N;
    faces[3 * (i + N) + 2] = N + i;
  }

  // Cone base
  for (unsigned int i = 0; i < N; ++i) {
    faces[3 * (i + 2 * N)] = N + i % N;
    faces[3 * (i + 2 * N) + 1] = N + (i + 1) % N;
    faces[3 * (i + 2 * N) + 2] = 2 * N + i;
  }
  for (unsigned int i = 0; i < N; ++i) {
    faces[3 * (i + 3 * N)] = N + (i + 1) % N;
    faces[3 * (i + 3 * N) + 1] = 2 * N + (i + 1) % N;
    faces[3 * (i + 3 * N) + 2] = 2 * N + i;
  }

  // Cone tip
  for (unsigned int i = 0; i < N; ++i) {
    faces.at(3 * (i + 4 * N)) = 2 * N + i;
    faces.at(3 * (i + 4 * N) + 1) = 2 * N + (i + 1) % N;
    faces.at(3 * (i + 4 * N) + 2) = 3 * N;
  }

  // for (unsigned int i = 0; i < 15 * N; ++i) {
  //   faces.at(i) -= faces_true.at(i);
  // }
  // for (unsigned int i = 0; i < 5 * N; ++i) {
  //   if (i % N == 0) {
  //     std::cout << "______ \n";
  //   }
  //   for (unsigned int j = 0; j < 3; ++j) {
  //     std::cout << faces.at(i * 3 + j) << " ";
  //   }
  //   std::cout << "\n";
  // }

  // transform

  std::vector<double> vertices_yaxis((3 * N + 1) * 3, 0);
  std::vector<double> vertices_zaxis((3 * N + 1) * 3, 0);
  Quaternion qx_y(sqrt2 / 2.0, 0, 0, sqrt2 / 2);
  Quaternion qx_z(sqrt2 / 2.0, 0, sqrt2 / 2, 0);
  Quaternion vert_tmp;

  for (unsigned int i = 0; i < 3 * N + 1; ++i) {
    vert_quat.at(i)[0] = 0;
    vert_quat.at(i)[1] = vertices_xaxis[3 * i];
    vert_quat.at(i)[2] = vertices_xaxis[3 * i + 1];
    vert_quat.at(i)[3] = vertices_xaxis[3 * i + 2];
  }

  for (unsigned int i = 0; i < 3 * N + 1; ++i) {
    vert_tmp.multiply(qx_y, vert_quat.at(i));
    vertices_yaxis.at(i * 3) = vert_tmp[1];
    vertices_yaxis.at(i * 3 + 1) = vert_tmp[2];
    vertices_yaxis.at(i * 3 + 2) = vert_tmp[3];
  }
  for (unsigned int i = 0; i < 3 * N + 1; ++i) {
    vert_tmp.multiply(qx_z, vert_quat.at(i));
    vertices_zaxis.at(i * 3) = vert_tmp[1];
    vertices_zaxis.at(i * 3 + 1) = vert_tmp[2];
    vertices_zaxis.at(i * 3 + 2) = vert_tmp[3];
  }

  return 0;
}
