// #include "readply.h"
#include "opengl/trimesh_render.h"
#include "plymesh.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main() {
  // Define a list of points
  // double verticesdata[] = {
  //// points coord
  //-0.5, -1.0, 0.0, // Point 1
  // 1.0,  -1.0, 0.0, // Point 2
  // 1.0,  1.0,  0.0, // Point 3
  //-1.0, 1.0,  0.0, // Point 4
  //};
  //
  // int squaredata[] = {
  // 0, 1, 2, 2, 3, 0,
  //};
  //
  // std::vector<double> vertices(verticesdata, verticesdata + 12);
  // std::vector<unsigned int> faces(squaredata, squaredata + 6);
  //
  // PlyMesh mesh(vertices, faces);
  // mesh.from_file("deform.ply");

  PlyMesh mesh("deform.ply");
  MeshRender render(500, 500, &mesh);

  mesh.print();
  // mesh.print_faces();
  // mesh.print_vertices();

  render.init_window();
  render.init_render();
  render.render_loop(NULL, NULL);
  render.render_finalize();
  return 0;
}
