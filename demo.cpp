#include "sources/mesh.hpp"
#include "sources/opengl/colormap.hpp"
#include "sources/opengl/trimesh_render.hpp"
#include "sources/plyfile.hpp"
#include <iostream>
#include <vector>

int main() {

  PlyFile file("meshes/deform.ply");

  Mesh mesh(file.vertices, file.faces);

  // std::vector<PropertyName> normal_property_names = {
  //     PropertyName::nx, PropertyName::ny, PropertyName::nz};
  // std::vector<double> normals;
  // file.get_subelement_data<double>("vertices", normal_property_names,
  // normals);

  mesh.set_one_ring();

  std::vector<double> k;
  mesh.set_mean_curvature();

  mesh.scalar_mean_curvature(k);

  std::vector<double> colors;
  get_interpolated_colors(k, colors, INFERNO);

  auto [minv, maxv] =
      std::minmax_element(mesh.vertices.begin(), mesh.vertices.end());

  // double max{*maxv}, min{*minv};
  double extent_vert = *maxv - *minv;
  extent_vert *= 1.2;
  for (auto &v : mesh.vertices) {
    v /= extent_vert;
    v -= 0.3;
  }

  PlyFile file2("meshes/spinningtop.ply");
  auto [minv2, maxv2] =
      std::minmax_element(file2.vertices.begin(), file2.vertices.end());
  extent_vert = *maxv2 - *minv2;
  extent_vert *= 1.3;
  for (auto &v : file2.vertices) {
    v /= extent_vert;
    v += 0.3;
  }

  // MeshRender render(500, 500, mesh.vertices, mesh.faces, normals);
  MeshRender render(500, 500, mesh.vertices, mesh.faces, colors);
  // render.update_vertex_colors(colors, 1);
  render.add_object(file2.vertices, file2.faces, ShaderProgramType::FLAT_FACES);
  render.render_loop(NULL, NULL);
  render.render_finalize();
  return 0;
}
