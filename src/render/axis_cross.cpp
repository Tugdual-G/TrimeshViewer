/* These methods are used to generate the mesh of the axis cross. */
#include "quatern_transform.hpp"
#include "trimesh_render.hpp"
#include <cmath>
#include <vector>

constexpr double pi{3.14159265359};
constexpr int N{8};
constexpr double r_e{0.08};
constexpr double r_i{0.04};
constexpr double base_length{0.8};
constexpr double sqrt2{1.4142};

void mesh_arrow(std::vector<double> &vertices, std::vector<unsigned int> &faces,
                int n_angles) {
  /* Returns a mesh representing an arrow of coordinates direction (1, 0, 0),
   * with base at (0,0,0). */

  int n_vertices = 3 * n_angles + 2;
  int n_faces = 6 * n_angles;

  vertices.resize(n_vertices * 3, r_i);

  faces.resize(n_faces * 3, 0);

  std::vector<double> theta_ext(n_angles);
  std::vector<double> theta_int(n_angles);

  for (int i = 0; i < n_angles; ++i) {
    theta_ext.at(i) = 2 * pi / n_angles * i + pi / n_angles;
    theta_int.at(i) = 2 * pi / n_angles * i;
  }

  vertices[1] = 0;
  vertices[2] = 0;
  // shaft base
  for (int i = 0; i < n_angles; ++i) {
    vertices[3 * (i + 1) + 1] = r_i * sin(theta_int.at(i));
    vertices[3 * (i + 1) + 2] = r_i * cos(theta_int.at(i));
  }
  // cone junction
  for (int i = 0; i < n_angles; ++i) {
    vertices[3 * (i + n_angles + 1)] = base_length;
    vertices[3 * (i + n_angles + 1) + 1] = r_i * sin(theta_int.at(i));
    vertices[3 * (i + n_angles + 1) + 2] = r_i * cos(theta_int.at(i));
  }
  // cone base
  for (int i = 0; i < n_angles; ++i) {
    vertices[3 * (i + 2 * n_angles + 1)] = base_length;
    vertices[3 * (i + 2 * n_angles + 1) + 1] = r_e * sin(theta_ext.at(i));
    vertices[3 * (i + 2 * n_angles + 1) + 2] = r_e * cos(theta_ext.at(i));
  }
  // tip
  vertices.at(3 * (3 * n_angles + 1)) = 1;
  vertices.at(3 * (3 * n_angles + 1) + 1) = 0;
  vertices.at(3 * (3 * n_angles + 1) + 2) = 0;

  // shaft base
  for (int i = 0; i < n_angles; ++i) {
    faces.at(3 * i) = (i + 1) % n_angles + 1;
    faces.at(3 * i + 1) = i + 1;
    faces.at(3 * i + 2) = 0;
  }

  // shaft
  for (int i = 0; i < n_angles; ++i) {
    faces[3 * (i + n_angles)] = i + 1;
    faces[3 * (i + n_angles) + 1] = (i + 1) % n_angles + 1;
    faces[3 * (i + n_angles) + 2] = n_angles + i + 1;
  }
  for (int i = 0; i < n_angles; ++i) {
    faces[3 * (i + 2 * n_angles)] = (i + 1) % n_angles + 1;
    faces[3 * (i + 2 * n_angles) + 1] = n_angles + (i + 1) % n_angles + 1;
    faces[3 * (i + 2 * n_angles) + 2] = n_angles + i + 1;
  }

  // Cone base
  for (int i = 0; i < n_angles; ++i) {
    faces[3 * (i + 3 * n_angles)] = n_angles + i % n_angles + 1;
    faces[3 * (i + 3 * n_angles) + 1] = n_angles + (i + 1) % n_angles + 1;
    faces[3 * (i + 3 * n_angles) + 2] = 2 * n_angles + i + 1;
  }
  for (int i = 0; i < n_angles; ++i) {
    faces[3 * (i + 4 * n_angles)] = n_angles + (i + 1) % n_angles + 1;
    faces[3 * (i + 4 * n_angles) + 1] = 2 * n_angles + (i + 1) % n_angles + 1;
    faces[3 * (i + 4 * n_angles) + 2] = 2 * n_angles + i + 1;
  }

  // Cone tip
  for (int i = 0; i < n_angles; ++i) {
    faces.at(3 * (i + 5 * n_angles)) = 2 * n_angles + i + 1;
    faces.at(3 * (i + 5 * n_angles) + 1) =
        2 * n_angles + (i + 1) % n_angles + 1;
    faces.at(3 * (i + 5 * n_angles) + 2) = 3 * n_angles + 1;
  }
}

void mesh_cube(std::vector<double> &vertices,
               std::vector<unsigned int> &faces) {
  /* Returns a cube centered at the origin. */
  vertices.resize(24, 0);
  faces.resize(36, 0);

  // thetrahedron
  std::vector<double> vertices_data = {
      // points coord
      -1, -1, -1, // 0
      1,  -1, -1, // 1
      1,  -1, 1,  // 2
      -1, -1, 1,  // 3
      -1, 1,  -1, // 4
      1,  1,  -1, // 5
      1,  1,  1,  // 6
      -1, 1,  1,  // 7
  };

  std::vector<int> faces_data = {
      0, 1, 2, // 0
      2, 3, 0, // 1
      1, 5, 6, // 2
      6, 2, 1, // 3
      2, 6, 7, // 4
      2, 7, 3, // 5
      0, 3, 4, // 6
      4, 3, 7, // 7
      5, 4, 7, // 8
      5, 7, 6, // 9
      1, 0, 4, // 10
      1, 4, 5, // 11
  };

  std::copy(vertices_data.begin(), vertices_data.end(), vertices.begin());
  std::copy(faces_data.begin(), faces_data.end(), faces.begin());
}

void colored_cross(std::vector<double> &vertices,
                   std::vector<unsigned int> &faces,
                   std::vector<double> &colors) {

  mesh_arrow(vertices, faces, N);

  int n_vertices = (int)vertices.size() / 3;
  int n_faces = (int)faces.size() / 3;
  vertices.resize(vertices.size() * 3);
  faces.resize(faces.size() * 3);

  // Creating the y and z arrows by rotating the x arrow
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

  std::vector<Quaternion> vert_quat(n_vertices);

  for (int i = 0; i < n_vertices; ++i) {
    vert_quat.at(i)[0] = 0;
    vert_quat.at(i)[1] = vertices[3 * i];
    vert_quat.at(i)[2] = vertices[3 * i + 1];
    vert_quat.at(i)[3] = vertices[3 * i + 2];
  }

  // y arrow
  for (int i = 0; i < n_vertices; ++i) {
    vert_tmp.multiply(qx_y, vert_quat.at(i));
    vert_tmp = vert_tmp * qx_y_inv;
    vertices.at((i + n_vertices) * 3) = vert_tmp[1];
    vertices.at((i + n_vertices) * 3 + 1) = vert_tmp[2];
    vertices.at((i + n_vertices) * 3 + 2) = vert_tmp[3];
  }

  // z arrow
  for (int i = 0; i < n_vertices; ++i) {
    vert_tmp.multiply(qx_z, vert_quat.at(i));
    vert_tmp = vert_tmp * qx_z_inv;
    vertices.at((i + 2 * n_vertices) * 3) = vert_tmp[1];
    vertices.at((i + 2 * n_vertices) * 3 + 1) = vert_tmp[2];
    vertices.at((i + 2 * n_vertices) * 3 + 2) = vert_tmp[3];
  }

  // Add offset to the faces indices
  for (int i = 0; i < n_faces * 3; ++i) {
    faces.at(i + n_faces * 3) = n_vertices + faces.at(i);
    faces.at(i + n_faces * 6) = 2 * n_vertices + faces.at(i);
  }

  colors.resize(vertices.size(), 0);

  for (int i = 0; i < n_vertices; ++i) {
    colors.at(i * 3) = 1;                        // red
    colors.at((i + n_vertices) * 3 + 1) = 1;     // blue
    colors.at((i + 2 * n_vertices) * 3 + 2) = 1; // green
  }
}

void MeshRender::set_axis_cross() {

  std::vector<double> axis_vertices;
  std::vector<unsigned int> axis_faces;

  // subsequent resize will not alter capacity
  axis_vertices.reserve(3 * (3 * N + 2) + 24);
  axis_faces.reserve(3 * (6 * N) + 36);

  std::vector<double> axis_colors;
  colored_cross(axis_vertices, axis_faces, axis_colors);
  int n_vertices = (int)axis_vertices.size() / 3;

  // Adding a cube at the origin
  std::vector<double> cube_vertices;
  std::vector<unsigned int> cube_faces;
  mesh_cube(cube_vertices, cube_faces);
  for (auto &v : cube_vertices) {
    v *= r_i;
  }

  // Add offset to the faces indices of the cube
  for (auto &f : cube_faces) {
    f += n_vertices;
  }

  axis_vertices.resize(axis_vertices.size() + cube_vertices.size());
  axis_faces.resize(axis_faces.size() + cube_faces.size());

  std::copy(cube_vertices.begin(), cube_vertices.end(),
            axis_vertices.end() - cube_vertices.size());

  std::copy(cube_faces.begin(), cube_faces.end(),
            axis_faces.end() - cube_faces.size());

  axis_colors.resize(axis_vertices.size());
  // Paint the origin cube white
  for (auto c = axis_colors.begin() + n_vertices * 3; c != axis_colors.end();
       ++c) {
    *c = 1;
  }

  // scaling
  for (auto &v : axis_vertices) {
    v *= 0.25;
  }

  add_object(axis_vertices, axis_faces, axis_colors,
             ShaderProgramType::AXIS_CROSS_FLAT);
}
