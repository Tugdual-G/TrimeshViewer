#include "mesh.h"
#include "opengl/colormap.h"
#include "opengl/trimesh_render.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main() {

  Mesh mesh("deform.ply");

  mesh.set_one_ring();

  std::vector<double> k;
  mesh.set_mean_curvature();
  mesh.scalar_mean_curvature(k);
  auto [k_min, k_max] = std::minmax_element(k.begin(), k.end());
  double min = *k_min;
  double max = *k_max;

  // std::cout << min << max << " \n";
  for (auto &k_i : k) {
    k_i = (k_i - min) / (max - min);
  }

  std::vector<double> colors;
  get_nearest_colors(k, colors, VIRIDIS);

  // for (unsigned int i = 0; i < colors.size() / 3; ++i) {
  //   colors[i * 3] = MAGMA[(unsigned int)(254 * k[i]) * 3];
  //   colors[i * 3 + 1] = MAGMA[(unsigned int)(254 * k[i]) * 3 + 1];
  //   colors[i * 3 + 2] = MAGMA[(unsigned int)(254 * k[i]) * 3 + 2];
  // }

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
