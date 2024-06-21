// #include "readply.h"
#include "opengl/trimesh_render.h"
#include "plymesh.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main() {
  // // Define a list of points
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
  //     0, 1, 2, //
  //     2, 3, 0, //
  //     1, 5, 6, //
  //     6, 2, 1, //
  //     2, 6, 7, //
  //     2, 7, 3, //
  //     0, 3, 4, //
  //     4, 3, 7, //
  //     5, 4, 7, //
  //     5, 7, 6, //
  //     1, 0, 4, //
  //     1, 4, 5, //
  // };

  // for (double *v = verticesdata; v != verticesdata + 24; ++v) {
  //   *v /= 4.0;
  // }

  // std::vector<double> vertices(verticesdata, verticesdata + 24);
  // std::vector<unsigned int> faces(squaredata, squaredata + 36);

  // PlyMesh mesh(vertices, faces);

  PlyMesh mesh("deform.ply");
  mesh.print();
  double max = *std::max_element(mesh.vertices.begin(), mesh.vertices.end());
  max *= 1.5;
  for (auto &v : mesh.vertices) {
    v /= max;
  }
  MeshRender render(500, 500, mesh.vertices, mesh.faces, mesh.vertex_normals);

  // mesh.print_vertex_adjacent_face();
  // mesh.print_vertex_normals();
  // mesh.print_faces();
  // mesh.print_normals();

  render.init_window();
  render.init_render();
  render.render_loop(NULL, NULL);
  render.render_finalize();
  return 0;
}
