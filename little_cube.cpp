#include "mesh.hpp"
#include "opengl/colormap.hpp"
#include "opengl/trimesh_render.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main() {
  // Define a list of points
  double verticesdata[] = {
      // points coord
      -1, -1, -1, // Point 1
      1,  -1, -1, // Point 2
      1,  -1, 1,  // Point 3
      -1, -1, 1,  // Point 4
      -1, 1,  -1, //
      1,  1,  -1, //
      1,  1,  1,  //
      -1, 1,  1,  //
  };

  int squaredata[] = {
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

  for (double *v = verticesdata; v != verticesdata + 24; ++v) {
    *v /= 5.0;
  }

  std::vector<double> vertices(verticesdata, verticesdata + 24);
  std::vector<unsigned int> faces(squaredata, squaredata + 36);

  Mesh mesh(vertices, faces);

  mesh.set_one_ring();

  std::vector<double> k;
  mesh.set_mean_curvature();
  mesh.scalar_mean_curvature(k);

  std::vector<double> colors(mesh.vertices.size());
  get_nearest_colors(k, colors, VIRIDIS);

  MeshRender render0(500, 500, mesh.vertices, mesh.faces);

  render0.add_vertex_colors(colors);
  render0.render_loop(NULL, NULL);
  render0.render_finalize();

  return 0;
}
