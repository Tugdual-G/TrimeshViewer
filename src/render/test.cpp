/* Basic script to debug and test implementation */
#include "colormap.hpp"
#include "trimesh_render.hpp"
#include <vector>

auto main() -> int {
  // Define a list of points

  // pentagonal pyramid
  std::vector<double> pent_vertices = {
      // points coord
      0,    -1, 0, //
      1,    0,  0, //
      0.5,  1,  0, //
      -0.5, 1,  0, //
      -1,   0,  0, //
      0,    0,  1, //
      0,    0,  0, //
  };
  // move and scale the pent
  for (auto &v : pent_vertices) {
    v *= 0.2;
    v -= 0.1;
  }

  std::vector<unsigned int> pent_faces = {
      0, 1, 5, // 0
      1, 2, 5, // 1
      2, 3, 5, // 2
      3, 4, 5, // 3
      4, 0, 5, // 4
      1, 0, 6, // 5
      2, 1, 6, // 6
      3, 2, 6, // 7
      4, 3, 6, // 8
      0, 4, 6, // 9
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

  std::vector<double> thet_vertices = {
      // points coord
      0, 0, 0, // 0
      1, 0, 0, // 1
      0, 1, 0, // 2
      0, 0, 1, // 3
  };
  // move and scale the cube
  for (auto &v : thet_vertices) {
    v *= 0.25;
    v -= 0.5;
  }

  std::vector<unsigned int> thet_faces = {
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
  std::vector<double> pent_scalar_vertex_value(pent_vertices.size() / 3);
  for (int i = 0; i < pent_scalar_vertex_value.size(); ++i) {
    pent_scalar_vertex_value.at(i) = i + 4;
  }

  std::vector<double> cube_colors =
      Colormap::get_nearest_colors(cube_scalar_vertex_value, Colormap::VIRIDIS);

  std::vector<double> pent_colors = Colormap::get_interpolated_colors(
      pent_scalar_vertex_value, Colormap::MAGMA, 0, 10);

  MeshRender render(500, 500, cube_vertices, cube_faces, cube_colors);
  render.add_object(thet_vertices, thet_faces);
  render.add_object(pent_vertices, pent_faces, pent_colors);
  render.render_loop(nullptr, nullptr);
  render.render_finalize();

  return 0;
}
