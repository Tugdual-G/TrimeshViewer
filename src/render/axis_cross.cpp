#include "quatern_transform.hpp"
#include "trimesh_render.hpp"
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

void MeshRender::set_axis_cross() {

  unsigned int n_vertices = 3 * N + 2 + 5;
  unsigned int n_faces = 6 * N + 4; // +4 for the hollow thetrahedron
  std::vector<double> axis_vertices(n_vertices * 9, r_i);
  std::vector<Quaternion> vert_quat;
  vert_quat.resize(n_vertices);
  std::vector<unsigned int> axis_faces(n_faces * 9, 0);
  std::vector<double> theta_ext(N);
  std::vector<double> theta_int(N);

  for (unsigned int i = 0; i < N; ++i) {
    theta_ext.at(i) = 2 * pi / N * i + pi / N;
    theta_int.at(i) = 2 * pi / N * i;
  }

  axis_vertices[1] = 0;
  axis_vertices[2] = 0;
  // shaft base
  for (unsigned int i = 0; i < N; ++i) {
    axis_vertices[3 * (i + 1) + 1] = r_i * sin(theta_int.at(i));
    axis_vertices[3 * (i + 1) + 2] = r_i * cos(theta_int.at(i));
  }
  // cone junction
  for (unsigned int i = 0; i < N; ++i) {
    axis_vertices[3 * (i + N + 1)] = base_length;
    axis_vertices[3 * (i + N + 1) + 1] = r_i * sin(theta_int.at(i));
    axis_vertices[3 * (i + N + 1) + 2] = r_i * cos(theta_int.at(i));
  }
  // cone base
  for (unsigned int i = 0; i < N; ++i) {
    axis_vertices[3 * (i + 2 * N + 1)] = base_length;
    axis_vertices[3 * (i + 2 * N + 1) + 1] = r_e * sin(theta_ext.at(i));
    axis_vertices[3 * (i + 2 * N + 1) + 2] = r_e * cos(theta_ext.at(i));
  }
  // tip
  axis_vertices.at(3 * (3 * N + 1)) = 1;
  axis_vertices.at(3 * (3 * N + 1) + 1) = 0;
  axis_vertices.at(3 * (3 * N + 1) + 2) = 0;

  // thetrahedron
  axis_vertices.at(3 * (3 * N + 2)) = r_i;
  axis_vertices.at(3 * (3 * N + 2) + 1) = -r_i;
  axis_vertices.at(3 * (3 * N + 2) + 2) = -r_i;

  axis_vertices.at(3 * (3 * N + 3)) = r_i;
  axis_vertices.at(3 * (3 * N + 3) + 1) = r_i;
  axis_vertices.at(3 * (3 * N + 3) + 2) = -r_i;

  axis_vertices.at(3 * (3 * N + 4)) = r_i;
  axis_vertices.at(3 * (3 * N + 4) + 1) = -r_i;
  axis_vertices.at(3 * (3 * N + 4) + 2) = r_i;

  axis_vertices.at(3 * (3 * N + 5)) = -r_i;
  axis_vertices.at(3 * (3 * N + 5) + 1) = -r_i;
  axis_vertices.at(3 * (3 * N + 5) + 2) = -r_i;

  axis_vertices.at(3 * (3 * N + 6)) = r_i;
  axis_vertices.at(3 * (3 * N + 6) + 1) = r_i;
  axis_vertices.at(3 * (3 * N + 6) + 2) = r_i;

  // shaft base
  for (unsigned int i = 0; i < N; ++i) {
    axis_faces.at(3 * i) = (i + 1) % N + 1;
    axis_faces.at(3 * i + 1) = i + 1;
    axis_faces.at(3 * i + 2) = 0;
  }

  // shaft
  for (unsigned int i = 0; i < N; ++i) {
    axis_faces[3 * (i + N)] = i + 1;
    axis_faces[3 * (i + N) + 1] = (i + 1) % N + 1;
    axis_faces[3 * (i + N) + 2] = N + i + 1;
  }
  for (unsigned int i = 0; i < N; ++i) {
    axis_faces[3 * (i + 2 * N)] = (i + 1) % N + 1;
    axis_faces[3 * (i + 2 * N) + 1] = N + (i + 1) % N + 1;
    axis_faces[3 * (i + 2 * N) + 2] = N + i + 1;
  }

  // Cone base
  for (unsigned int i = 0; i < N; ++i) {
    axis_faces[3 * (i + 3 * N)] = N + i % N + 1;
    axis_faces[3 * (i + 3 * N) + 1] = N + (i + 1) % N + 1;
    axis_faces[3 * (i + 3 * N) + 2] = 2 * N + i + 1;
  }
  for (unsigned int i = 0; i < N; ++i) {
    axis_faces[3 * (i + 4 * N)] = N + (i + 1) % N + 1;
    axis_faces[3 * (i + 4 * N) + 1] = 2 * N + (i + 1) % N + 1;
    axis_faces[3 * (i + 4 * N) + 2] = 2 * N + i + 1;
  }

  // Cone tip
  for (unsigned int i = 0; i < N; ++i) {
    axis_faces.at(3 * (i + 5 * N)) = 2 * N + i + 1;
    axis_faces.at(3 * (i + 5 * N) + 1) = 2 * N + (i + 1) % N + 1;
    axis_faces.at(3 * (i + 5 * N) + 2) = 3 * N + 1;
  }

  // thetrahedron
  axis_faces.at(3 * (6 * N)) = 3 * N + 2;
  axis_faces.at(3 * (6 * N) + 1) = 3 * N + 3;
  axis_faces.at(3 * (6 * N) + 2) = 3 * N + 4;

  axis_faces.at(3 * (6 * N + 1)) = 3 * N + 2;
  axis_faces.at(3 * (6 * N + 1) + 1) = 3 * N + 5;
  axis_faces.at(3 * (6 * N + 1) + 2) = 3 * N + 3;

  axis_faces.at(3 * (6 * N + 2)) = 3 * N + 2;
  axis_faces.at(3 * (6 * N + 2) + 1) = 3 * N + 4;
  axis_faces.at(3 * (6 * N + 2) + 2) = 3 * N + 5;

  axis_faces.at(3 * (6 * N + 3)) = 3 * N + 3;
  axis_faces.at(3 * (6 * N + 3) + 1) = 3 * N + 6;
  axis_faces.at(3 * (6 * N + 3) + 2) = 3 * N + 4;

  // Creating the y and z arrows by rotation
  Quaternion qx_y(sqrt2 / 2.0, 0, 0, -sqrt2 / 2);
  Quaternion qx_y_inv = qx_y.inv();
  Quaternion qx_z(sqrt2 / 2.0, 0, sqrt2 / 2, 0);
  Quaternion qx_z_inv = qx_z.inv();
  Quaternion vert_tmp;

  Quaternion qx_thet_y(sqrt2 / 2.0, 0, -sqrt2 / 2, 0);
  qx_y = qx_thet_y * qx_y;
  qx_y_inv = qx_y_inv * qx_thet_y.inv();

  Quaternion qx_thet_z(sqrt2 / 2.0, 0, 0, sqrt2 / 2);
  qx_z = qx_thet_z * qx_z;
  qx_z_inv = qx_z_inv * qx_thet_z.inv();

  for (unsigned int i = 0; i < n_vertices; ++i) {
    vert_quat.at(i)[0] = 0;
    vert_quat.at(i)[1] = axis_vertices[3 * i];
    vert_quat.at(i)[2] = axis_vertices[3 * i + 1];
    vert_quat.at(i)[3] = axis_vertices[3 * i + 2];
  }

  // y arrow
  for (unsigned int i = 0; i < n_vertices; ++i) {
    vert_tmp.multiply(qx_y, vert_quat.at(i));
    vert_tmp = vert_tmp * qx_y_inv;
    axis_vertices.at((i + n_vertices) * 3) = vert_tmp[1];
    axis_vertices.at((i + n_vertices) * 3 + 1) = vert_tmp[2];
    axis_vertices.at((i + n_vertices) * 3 + 2) = vert_tmp[3];
  }

  // z arrow
  for (unsigned int i = 0; i < n_vertices; ++i) {
    vert_tmp.multiply(qx_z, vert_quat.at(i));
    vert_tmp = vert_tmp * qx_z_inv;
    axis_vertices.at((i + 2 * n_vertices) * 3) = vert_tmp[1];
    axis_vertices.at((i + 2 * n_vertices) * 3 + 1) = vert_tmp[2];
    axis_vertices.at((i + 2 * n_vertices) * 3 + 2) = vert_tmp[3];
  }

  std::vector<double> axis_color(n_vertices * 9, 0);

  for (unsigned int i = 0; i < n_vertices - 5; ++i) {
    axis_color.at(i * 3) = 1;                        // red
    axis_color.at((i + n_vertices) * 3 + 1) = 1;     // blue
    axis_color.at((i + 2 * n_vertices) * 3 + 2) = 1; // green
  }
  // // lighter color scheme
  // for (unsigned int i = 0; i < n_vertices - 5; ++i) {
  //   axis_color.at(i * 3) = 1;                                  // red
  //   axis_color.at(i * 3 + 1) = 153.0 / 255;                    // red
  //   axis_color.at(i * 3 + 2) = 204.0 / 255;                    // red
  //   axis_color.at((i + n_vertices) * 3 + 0) = 153.0 / 255;     // blue
  //   axis_color.at((i + n_vertices) * 3 + 1) = 1;               // blue
  //   axis_color.at((i + n_vertices) * 3 + 2) = 204.0 / 255;     // blue
  //   axis_color.at((i + 2 * n_vertices) * 3 + 0) = 153.0 / 255; // green
  //   axis_color.at((i + 2 * n_vertices) * 3 + 1) = 204.0 / 255; // green
  //   axis_color.at((i + 2 * n_vertices) * 3 + 2) = 1;           // green
  // }

  for (unsigned int i = n_vertices - 5; i < n_vertices; ++i) {
    for (unsigned int j = 0; j < 3; ++j) {
      axis_color.at(i * 3 + j) = 1;
      axis_color.at((i + n_vertices) * 3 + j) = 1;
      axis_color.at((i + 2 * n_vertices) * 3 + j) = 1;
    }
  }

  for (unsigned int i = 0; i < n_faces * 3; ++i) {
    axis_faces.at(i + n_faces * 3) = n_vertices + axis_faces.at(i);
    axis_faces.at(i + n_faces * 6) = 2 * n_vertices + axis_faces.at(i);
  }

  // scaling and translation
  for (unsigned int i = 0; i < n_vertices * 9; ++i) {
    axis_vertices.at(i) *= 0.25;
  }

  add_object(axis_vertices, axis_faces, axis_color,
             ShaderProgramType::AXIS_CROSS_FLAT);
  // for (unsigned int i = 0; i < 3; ++i) {
  //   for (unsigned int j = 0; j < 3; ++j) {
  //     std::cout << axis_vertices.at(3 * (3 * N + 1 + i * n_vertices) + j)
  //               << ", ";
  //   }
  //   std::cout << "\n";
  // }
}
