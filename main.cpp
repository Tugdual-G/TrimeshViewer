#include "mesh.hpp"
#include "opengl/colormap.hpp"
#include "opengl/trimesh_render.hpp"
#include "plyfile.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main() {

  PlyFile file0("deformHQ.ply");

  Mesh &mesh = file0.mesh;

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

  MeshRender render0(1000, 1000, mesh.vertices, mesh.faces);

  render0.add_vertex_colors(colors);
  render0.render_loop(NULL, NULL);
  render0.render_finalize();

  // MeshRender render(500, 500, mesh.vertices, mesh.faces,
  // mesh.vertex_normals); render.add_vertex_colors(colors);
  // render.render_loop(NULL, NULL);
  // render.render_finalize();
  return 0;
}
