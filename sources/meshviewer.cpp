#include "mesh.hpp"
#include "opengl/colormap.hpp"
#include "opengl/trimesh_render.hpp"
#include "plyfile.hpp"
// #include <fstream>
#include <iostream>
// #include <sstream>
// #include <string>
#include <vector>

int main(__attribute__((unused)) int argc, char *argv[]) {

  PlyFile file(argv[1]);

  std::vector<PropertyName> normal_prop_names = {
      PropertyName::red, PropertyName::green, PropertyName::blue};

  std::vector<double> colors;
  file.get_subelement_data<double>("faces", normal_prop_names, colors);

  Mesh mesh(file.vertices, file.faces);

  mesh.set_one_ring();

  std::vector<double> k;
  mesh.set_mean_curvature();

  mesh.scalar_mean_curvature(k);

  // std::vector<double> colors;
  get_interpolated_colors(k, colors, INFERNO);

  auto [minv, maxv] =
      std::minmax_element(mesh.vertices.begin(), mesh.vertices.end());

  double max{*maxv}, min{*minv};
  std::cout << "max " << max << ", min " << min << "\n";
  double extent_vert = *maxv - *minv;
  extent_vert *= 1.2;
  for (auto &v : mesh.vertices) {
    v /= extent_vert;
  }

  MeshRender render(500, 500, mesh.vertices, mesh.faces);

  render.add_vertex_colors(colors);
  render.render_loop(NULL, NULL);
  render.render_finalize();
  return 0;
}
