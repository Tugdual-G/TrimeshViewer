#include "../src/mesh/mesh.hpp"
#include "../src/ply/plyfile.hpp"
#include "../src/render/colormap.hpp"
#include "../src/render/trimesh_render.hpp"
#include <vector>

auto main() -> int {

  PlyFile file("../meshes/deform.ply");

  Mesh mesh(file.vertices, file.faces);

  // std::vector<PropertyName> normal_property_names = {
  //     PropertyName::nx, PropertyName::ny, PropertyName::nz};
  // std::vector<double> normals;
  // file.get_subelement_data<double>("vertices", normal_property_names,
  // normals);

  mesh.set_one_ring();
  std::vector<double> kn = mesh.get_mean_curvature(mesh.one_ring);
  std::vector<double> k = mesh.get_scalar_mean_curvature(kn);

  std::vector<double> colors =
      Colormap::get_nearest_colors(k, Colormap::VIRIDIS);

  auto [minv, maxv] =
      std::minmax_element(mesh.vertices.begin(), mesh.vertices.end());

  double extent_vert = *maxv - *minv;
  extent_vert *= 1.2;
  for (auto &v : mesh.vertices) {
    v /= extent_vert;
    v -= 0.3;
  }

  PlyFile file2("../meshes/spinningtop.ply");
  auto [minv2, maxv2] =
      std::minmax_element(file2.vertices.begin(), file2.vertices.end());
  extent_vert = *maxv2 - *minv2;
  extent_vert *= 1.3;
  for (auto &v : file2.vertices) {
    v /= extent_vert;
    v += 0.3;
  }

  PlyFile file3("../meshes/deformHQ.ply");
  auto [minv3, maxv3] =
      std::minmax_element(file3.vertices.begin(), file3.vertices.end());
  extent_vert = *maxv3 - *minv3;
  extent_vert *= 1.7;

  for (size_t i = 0; i < file3.n_vertices; ++i) {
    file3.vertices.at(i * 3) /= extent_vert;
    file3.vertices.at(i * 3) -= 0.5;
    file3.vertices.at(i * 3 + 1) /= extent_vert;
    file3.vertices.at(i * 3 + 1) += 0.4;
    file3.vertices.at(i * 3 + 2) /= extent_vert;
  }

  MeshRender render(500, 500, mesh.vertices, mesh.faces, colors);
  render.add_object(file2.vertices, file2.faces);
  render.add_object(file3.vertices, file3.faces);
  render.render_loop(nullptr, nullptr);
  render.render_finalize();
  return 0;
}
