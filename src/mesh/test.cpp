/* test render implementation */
#include "colormap.hpp"
#include "trimesh_render.hpp"
#include <vector>

auto main() -> int {
  // Define a list of points

  std::vector<double> vertices = {
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

  std::vector<unsigned int> faces = {
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

  for (auto &v : vertices) {
    v *= 0.5;
  }

  Mesh mesh(vertices, faces);

  mesh.set_one_ring();

  mesh.set_one_ring();
  mesh.print_one_ring();

  std::vector<double> kn = mesh.get_mean_curvature(mesh.one_ring);
  std::vector<double> k = mesh.get_scalar_mean_curvature(kn);

  std::vector<double> colors =
      Colormap::get_nearest_colors(k, Colormap::VIRIDIS);

  MeshRender render0(500, 500, mesh.vertices, mesh.faces, colors);
  render0.render_loop(nullptr, nullptr);
  render0.render_finalize();

  return 0;
}
