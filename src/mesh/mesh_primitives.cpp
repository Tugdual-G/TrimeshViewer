#include "mesh.hpp"
#include <cmath>
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
