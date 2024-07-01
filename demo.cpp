#include "sources/mesh.hpp"
#include "sources/opengl/colormap.hpp"
#include "sources/opengl/trimesh_render.hpp"
#include "sources/plyfile.hpp"
#include <iostream>
#include <vector>

int main() {

  PlyFile file("meshes/deformHQ.ply");

  Mesh mesh(file.vertices, file.faces);

  std::vector<PropertyName> normal_property_names = {
      PropertyName::nx, PropertyName::ny, PropertyName::nz};
  std::vector<double> normals;
  file.get_subelement_data<double>("vertices", normal_property_names, normals);

  mesh.set_one_ring();

  std::vector<double> k;
  mesh.set_mean_curvature();

  mesh.scalar_mean_curvature(k);

  std::vector<double> colors;
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

  MeshRender render(500, 500, mesh.vertices, mesh.faces, normals);
  // MeshRender render(500, 500, mesh.vertices, mesh.faces);
  render.update_vertex_colors(colors);
  render.render_loop(NULL, NULL);
  render.render_finalize();
  return 0;
}
