/* Basic script to debug and test implementation */
#include "colormap.hpp"
#include "trimesh_render.hpp"
#include <vector>

constexpr double phi{1.6180339887498};

auto main() -> int {
  // Define a list of points
  //

  std::vector<double> icosahedron_vertices = {
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

  // move and scale
  for (auto &v : icosahedron_vertices) {
    v *= 0.1;
  }
  for (int i = 0; i < icosahedron_vertices.size() / 3; ++i) {
    icosahedron_vertices.at(i * 3 + 2) -= 0.5;
  }

  std::vector<unsigned int> icosahedron_faces = {
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

  std::vector<double> cube_vertices = {
      // points coord
      -1, -1, -1, // 0
      1,  -1, -1, // 1
      1,  -1, 1,  // 2
      -1, -1, 1,  // 3
      -1, 1,  -1, //
      1,  1,  -1, //
      1,  1,  1,  //
      -1, 1,  1,  //
  };
  // move and scale the cube
  for (auto &v : cube_vertices) {
    v *= 0.25;
    v += 0.25;
  }

  std::vector<unsigned int> cube_faces = {
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

  std::vector<double> tet_vertices = {
      // points coord
      1,  1,  1,  // 0
      1,  -1, -1, // 1
      -1, 1,  -1, // 2
      -1, -1, 1,  // 3
  };
  // move and scale
  for (auto &v : tet_vertices) {
    v *= 0.15;
    v -= 0.3;
  }

  std::vector<unsigned int> tet_faces = {
      0, 1, 3, // 0
      1, 2, 3, // 1
      1, 2, 0, // 2
      2, 0, 3, // 3
  };

  // A test value to generate colors
  std::vector<double> cube_scalar_vertex_value(cube_vertices.size() / 3);
  for (int i = 0; i < cube_scalar_vertex_value.size(); ++i) {
    cube_scalar_vertex_value.at(i) = i;
  }

  // A test value to generate colors
  std::vector<double> ico_scalar_vertex_value(icosahedron_vertices.size() / 3);
  for (int i = 0; i < ico_scalar_vertex_value.size(); ++i) {
    ico_scalar_vertex_value.at(i) = i + 6;
  }
  std::vector<double> ico_colors = Colormap::get_interpolated_colors(
      ico_scalar_vertex_value, Colormap::MAGMA, 0, 20);

  std::vector<double> cube_colors =
      Colormap::get_nearest_colors(cube_scalar_vertex_value, Colormap::VIRIDIS);

  MeshRender render(500, 500, cube_vertices, cube_faces, cube_colors);
  render.add_object(icosahedron_vertices, icosahedron_faces, ico_colors);
  render.add_object(tet_vertices, tet_faces);
  render.render_loop(nullptr, nullptr);
  render.render_finalize();

  return 0;
}
