#include "mesh.hpp"
#include <cmath>
#include <iostream>
#include <vector>

constexpr double phi{1.6180339887498}; // golden ratio
constexpr double pi{3.1415926535898};

auto Primitives::icosahedron() -> Mesh {
  /* Returns a regular isocahedron centered at the origin,
   * with egdes of unit length.
   * Circumsphere radius ~= 0.95105651629515 ;
   * */
  Mesh mesh;

  mesh.vertices = {
      -1,   phi,  0,    //
      1,    phi,  0,    //
      -1,   -phi, 0,    //
      1,    -phi, 0,    //
      0,    -1,   phi,  //
      0,    1,    phi,  //
      0,    -1,   -phi, //
      0,    1,    -phi, //
      phi,  0,    -1,   //
      phi,  0,    1,    //
      -phi, 0,    -1,   //
      -phi, 0,    1     //
  };

  // scale
  for (auto &v : mesh.vertices) {
    v *= 0.5;
  }

  mesh.faces = {
      0,  11, 5,  //
      0,  5,  1,  //
      0,  1,  7,  //
      0,  7,  10, //
      0,  10, 11, //
      11, 10, 2,  //
      5,  11, 4,  //
      1,  5,  9,  //
      7,  1,  8,  //
      10, 7,  6,  //
      3,  9,  4,  //
      3,  4,  2,  //
      3,  2,  6,  //
      3,  6,  8,  //
      3,  8,  9,  //
      9,  8,  1,  //
      4,  9,  5,  //
      2,  4,  11, //
      6,  2,  10, //
      8,  6,  7   //
  };

  mesh.n_vertices = (int)mesh.vertices.size() / 3;
  mesh.n_faces = (int)mesh.faces.size() / 3;
  return mesh;
}

auto Primitives::tetrahedron() -> Mesh {
  /* Returns a regular tetrahedron centered at the origin,
   * with egdes of unit length. */
  Mesh mesh;

  mesh.vertices = {
      // points coord
      1,  1,  1,  // 0
      1,  -1, -1, // 1
      -1, 1,  -1, // 2
      -1, -1, 1,  // 3
  };
  // scale
  for (auto &v : mesh.vertices) {
    v /= 2.82842712474619;
  }

  mesh.faces = {
      0, 1, 3, // 0
      1, 2, 3, // 1
      1, 2, 0, // 2
      2, 0, 3, // 3
  };

  mesh.n_vertices = (int)mesh.vertices.size() / 3;
  mesh.n_faces = (int)mesh.faces.size() / 3;
  return mesh;
}

auto Primitives::cube() -> Mesh {
  /* Returns a cube centered at the origin,
   * with egdes of unit length. */

  Mesh mesh;

  mesh.vertices = {
      -1, -1, -1, // 0
      1,  -1, -1, // 1
      1,  -1, 1,  // 2
      -1, -1, 1,  // 3
      -1, 1,  -1, // 4
      1,  1,  -1, // 5
      1,  1,  1,  // 6
      -1, 1,  1,  // 7
  };
  // scale
  for (auto &v : mesh.vertices) {
    v *= 0.5;
  }

  mesh.faces = {
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

  mesh.n_vertices = (int)mesh.vertices.size() / 3;
  mesh.n_faces = (int)mesh.faces.size() / 3;
  return mesh;
}

template <class T>
auto vect_add(std::vector<T> u, std::vector<T> v) -> std::vector<T> {
  if (u.size() != v.size()) {
    throw "\n add_vect error \n";
  }
  std::vector<T> w(u.size());
  for (long unsigned int i = 0; i < u.size(); ++i) {
    w[i] = u[i] + v[i];
  }
  return w;
}

template <class T>
auto vect_scal_mult(std::vector<T> u, T alpha) -> std::vector<T> {
  std::vector<T> w(u.size());
  for (long unsigned int i = 0; i < u.size(); ++i) {
    w[i] = u[i] * alpha;
  }
  return w;
}

auto Primitives::torus(double R, double r, int n) -> Mesh {
  Mesh mesh;

  int N = (int)n * R / r; // number of discrete sections
  N = (N >> 1) << 1;      // N should be pair;

  // Great radius unit vector
  std::vector<double> e_r(3);

  // Great radius vector angle
  double phi{0};

  // Small radius vector angle
  double theta{0};

  mesh.vertices.resize(n * N * 3);
  std::vector<double> vertice;
  for (int i = 0; i < N; ++i) {
    phi = i * 2 * pi / N;
    e_r.at(0) = std::cos(phi);
    e_r.at(1) = std::sin(phi);
    for (int j = 0; j < n; ++j) {
      theta = j * 2 * pi / n;
      theta += (i % 2) * pi / n;

      vertice = vect_add(vect_scal_mult(e_r, R),
                         vect_scal_mult(e_r, r * std::cos(theta)));

      vertice.at(2) = r * std::sin(theta);

      std::copy(vertice.begin(), vertice.end(),
                mesh.vertices.begin() + (i * n + j) * 3);
    }
  }

  int offset{0};
  int face{0};
  mesh.faces.resize(n * N * 2 * 3);
  for (int i = 0; i < N; ++i) {
    offset = i % 2;
    for (int j = 0; j < n; ++j) {
      face = (i * n + j) * 2;
      mesh.faces.at(face * 3) = i * n + j;
      mesh.faces.at(face * 3 + 1) = ((i + 1) % N) * n + (j + offset) % n;
      mesh.faces.at(face * 3 + 2) = i * n + ((j + 1) % n);
      ++face;
      mesh.faces.at(face * 3) = i * n + ((j + 1) % n);
      mesh.faces.at(face * 3 + 1) = ((i + 1) % N) * n + (j + offset) % n;
      mesh.faces.at(face * 3 + 2) = ((i + 1) % N) * n + ((j + 1 + offset) % n);
    }
  }
  mesh.n_faces = (int)mesh.faces.size() / 3;
  mesh.n_vertices = (int)mesh.vertices.size() / 3;
  return mesh;
}

auto arrow(double shaft_radius, double cone_radius, double base_length,
           int n_angles) -> Mesh {
  /* Returns a mesh representing an arrow of coordinates direction (1, 0, 0),
   * with base at (0,0,0). */

  // constexpr double cone_radius{0.08};
  //  constexpr double shaft_radius{0.04};
  // constexpr double base_length{0.8};

  int n_vertices = 3 * n_angles + 2;
  int n_faces = 6 * n_angles;

  std::vector<double> vertices(n_vertices * 3, 0);

  std::vector<unsigned int> faces(n_faces * 3, 0);

  std::vector<double> theta_ext(n_angles);
  std::vector<double> theta_int(n_angles);

  for (int i = 0; i < n_angles; ++i) {
    theta_ext.at(i) = 2 * pi / n_angles * i + pi / n_angles;
    theta_int.at(i) = 2 * pi / n_angles * i;
  }

  // shaft base
  for (int i = 0; i < n_angles; ++i) {
    vertices[3 * (i + 1) + 1] = shaft_radius * sin(theta_int.at(i));
    vertices[3 * (i + 1) + 2] = shaft_radius * cos(theta_int.at(i));
  }
  // cone junction
  for (int i = 0; i < n_angles; ++i) {
    vertices[3 * (i + n_angles + 1)] = base_length;
    vertices[3 * (i + n_angles + 1) + 1] = shaft_radius * sin(theta_int.at(i));
    vertices[3 * (i + n_angles + 1) + 2] = shaft_radius * cos(theta_int.at(i));
  }
  // cone base
  for (int i = 0; i < n_angles; ++i) {
    vertices[3 * (i + 2 * n_angles + 1)] = base_length;
    vertices[3 * (i + 2 * n_angles + 1) + 1] =
        cone_radius * sin(theta_ext.at(i));
    vertices[3 * (i + 2 * n_angles + 1) + 2] =
        cone_radius * cos(theta_ext.at(i));
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
  return Mesh{vertices, faces};
}

static void normalize(double *w) {
  double inv_norm =
      1.0 / pow(pow(w[0], 2.0) + pow(w[1], 2.0) + pow(w[2], 2.0), 0.5);
  w[0] *= inv_norm;
  w[1] *= inv_norm;
  w[2] *= inv_norm;
}

static auto norm(double *w) -> double {
  return pow(pow(w[0], 2.0) + pow(w[1], 2.0) + pow(w[2], 2.0), 0.5);
}

static auto cross(const std::vector<double> &u,
                  const std::vector<double> &v) -> std::vector<double> {
  std::vector<double> w(3);
  w[0] = u[1] * v[2] - u[2] * v[1];
  w[1] = u[2] * v[0] - u[0] * v[2];
  w[2] = u[0] * v[1] - u[1] * v[0];
  return w;
}

static void mat_mul(const double *A, double *u) {
  double v[3];
  for (int i = 0; i < 3; ++i) {
    v[i] = 0;
    for (int j = 0; j < 3; ++j) {
      v[i] += u[j] * A[i * 3 + j];
    }
  }
  for (int i = 0; i < 3; ++i) {
    u[i] = v[i];
  }
}

auto Primitives::vector(std::vector<double> &base_coord,
                        std::vector<double> &direction) -> Mesh {
  if (base_coord.size() != 3) {
    throw;
  }
  if (direction.size() != 3) {
    throw;
  }
  double length = norm(direction.data());
  auto vect = arrow(0.04, 0.08, 0.8, 32);

  std::vector<double> e_x = {1, 0.0};
  std::vector<double> e_i(direction);
  normalize(e_i.data());
  auto e_k = cross(e_x, e_i);
  normalize(e_k.data());
  auto e_j = cross(e_k, e_i);
  // normalize(e_j);
  std::vector<double> transform(9, 0);
  for (int i = 0; i < 3; ++i) {
    transform[i * 3] = e_i[i];
    transform[i * 3 + 1] = e_j[i];
    transform[i * 3 + 2] = e_k[i];
  }

  for (int i = 0; i < vect.n_vertices; ++i) {
    mat_mul(transform.data(), &vect.vertices[3 * i]);
    vect.vertices[3 * i] = vect.vertices[3 * i] * length + base_coord[0];
    vect.vertices[3 * i + 1] =
        vect.vertices[3 * i + 1] * length + base_coord[1];
    vect.vertices[3 * i + 2] =
        vect.vertices[3 * i + 2] * length + base_coord[2];
  }

  return vect;
}

auto Primitives::vectors(std::vector<double> &base_coord,
                         std::vector<double> &direction) -> Mesh {
  if (base_coord.size() < 3) {
    throw;
  }
  if (direction.size() < 3) {
    throw;
  }
  Mesh vect = arrow(0.04, 0.08, 0.8, 12);
  std::cout << "vect n_faces : " << vect.n_faces << "\n";
  std::cout << "vect n_vertices : " << vect.n_vertices << "\n";
  int n_vect = (int)base_coord.size() / 3;
  std::vector<double> e_x = {1, 0.0};
  std::vector<double> e_i(3);
  std::vector<double> e_j(3);
  std::vector<double> e_k(3);
  std::vector<double> transform(9, 0);
  double length{0};

  std::vector<double> vertices(vect.vertices.size() * n_vect);
  std::vector<unsigned int> faces(vect.faces.size() * n_vect);
  int vertices_offset{0};
  int faces_offset{0};

  for (int i = 0; i < n_vect * 3; i += 3) {

    std::copy(&direction[i], &direction[i + 3], e_i.begin());

    length = norm(e_i.data());
    normalize(e_i.data());

    e_k = cross(e_x, e_i);
    normalize(e_k.data());
    e_j = cross(e_k, e_i);
    // normalize(e_j);

    for (int j = 0; j < 3; ++j) {
      transform[j * 3] = e_i[j];
      transform[j * 3 + 1] = e_j[j];
      transform[j * 3 + 2] = e_k[j];
    }

    std::copy(vect.vertices.begin(), vect.vertices.end(),
              &vertices[vertices_offset]);

    for (int j = vertices_offset; j < vect.n_vertices * 3 + vertices_offset;
         j += 3) {
      mat_mul(transform.data(), &vertices[j]);
      vertices[j] = vertices[j] * length + base_coord[i];
      vertices[j + 1] = vertices[j + 1] * length + base_coord[i + 1];
      vertices[j + 2] = vertices[j + 2] * length + base_coord[i + 2];
    }

    std::copy(vect.faces.begin(), vect.faces.end(), &faces[faces_offset]);
    faces_offset += vect.faces.size();
    vertices_offset += vect.vertices.size();
    for (auto &f : vect.faces) {
      f += vect.n_vertices;
    }
  }

  return Mesh{vertices, faces};
}
