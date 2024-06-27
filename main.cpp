#include "mesh.hpp"
#include "opengl/colormap.hpp"
#include "opengl/trimesh_render.hpp"
#include "plyfile.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// double verticesdata[] = {
//     // points coord
//     -1, -1, -1, // Point 1
//     1,  -1, -1, // Point 2
//     1,  -1, 1,  // Point 3
//     -1, -1, 1,  // Point 4
//     -1, 1,  -1, //
//     1,  1,  -1, //
//     1,  1,  1,  //
//     -1, 1,  1,  //
// };

// int squaredata[] = {
//     0, 1, 2, // 0
//     2, 3, 0, // 1
//     1, 5, 6, // 2
//     6, 2, 1, // 3
//     2, 6, 7, // 4
//     2, 7, 3, // 5
//     0, 3, 4, // 6
//     4, 3, 7, // 7
//     5, 4, 7, // 8
//     5, 7, 6, // 9
//     1, 0, 4, // 10
//     1, 4, 5, // 11
// };

int main() {

  PlyFile file0("deformHQ.ply");
  file0.print();

  // std::cout << "\n";
  // std::cout << "\n";
  // for (unsigned int i = 0; i < file0.n_vertices; ++i) {
  //   for (unsigned int j = 0; j < 3; ++j) {
  //     std::cout << file0.vertices.at(i * 3 + j) << " ";
  //   }
  //   std::cout << "\n";
  // }

  // std::cout << "\n";
  // std::cout << "\n";
  // for (unsigned int i = 0; i < file0.n_faces; ++i) {
  //   std::cout << i << " : ";
  //   for (unsigned int j = 0; j < 3; ++j) {
  //     std::cout << file0.faces.at(i * 3 + j) << " ";
  //   }
  //   std::cout << "\n";
  // }

  // // std::cout << "okmain0\n";
  Mesh mesh = file0.mesh;
  // mesh.print_vertices();
  // std::cout << "\n";
  // mesh.print_faces();

  mesh.set_one_ring();

  std::vector<double> k;
  mesh.set_mean_curvature();

  mesh.scalar_mean_curvature(k);

  std::vector<double> colors;
  get_nearest_colors(k, colors, MAGMA);

  auto [minv, maxv] =
      std::minmax_element(mesh.vertices.begin(), mesh.vertices.end());

  double extent_vert = *maxv - *minv;
  extent_vert *= 1.2;
  for (auto &v : mesh.vertices) {
    v /= extent_vert;
  }

  MeshRender render0(500, 500, mesh.vertices, mesh.faces);

  render0.add_vertex_colors(colors);
  render0.render_loop(NULL, NULL);
  render0.render_finalize();

  // MeshRender render(500, 500, mesh.vertices, mesh.faces,
  // mesh.vertex_normals); render.add_vertex_colors(colors);
  // render.render_loop(NULL, NULL);
  // render.render_finalize();
  return 0;
}
