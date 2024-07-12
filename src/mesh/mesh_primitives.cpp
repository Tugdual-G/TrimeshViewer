#include "mesh.hpp"
#include <vector>

constexpr double phi{1.6180339887498}; // golden ratio

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
