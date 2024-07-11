/* This file loads three meshes and display them.*/
#include "../src/mesh/mesh.hpp"
#include "../src/ply/plyfile.hpp"
#include "../src/render/colormap.hpp"
#include "../src/render/trimesh_render.hpp"
#include <vector>

auto main() -> int {

  PlyFile file("../meshes/deform.ply");

  Mesh mesh(file.vertices, file.faces);

  // Use the mean curvature to color the mesh
  mesh.set_one_ring();
  std::vector<double> kn = mesh.get_mean_curvature(mesh.one_ring);
  std::vector<double> k = mesh.get_scalar_mean_curvature(kn);
  std::vector<double> colors =
      Colormap::get_nearest_colors(k, Colormap::VIRIDIS);

  // scales the mesh before display
  auto [minv, maxv] =
      std::minmax_element(mesh.vertices.begin(), mesh.vertices.end());
  double extent_vert = *maxv - *minv;
  extent_vert *= 1.3;
  for (auto &v : mesh.vertices) {
    v /= extent_vert;
    v += 0.1; // translates to low left corner
  }

  Mesh tet = Primitives::tetrahedron();
  // move and scale
  for (auto &v : tet.vertices) {
    v *= 0.25;
    v -= 0.3;
  }

  Mesh ico = Primitives::isocahedron();
  for (auto &v : ico.vertices) {
    v *= 0.25;
  }
  for (int i = 0; i < ico.n_vertices; ++i) {
    ico.vertices.at(i * 3 + 2) -= 0.5;
  }
  // Generates colors
  std::vector<double> ico_scalar_vertex_value(ico.n_vertices);
  for (int i = 0; i < ico.n_vertices; ++i) {
    ico_scalar_vertex_value.at(i) = i + 7;
  }
  std::vector<double> ico_colors = Colormap::get_interpolated_colors(
      ico_scalar_vertex_value, Colormap::MAGMA, 0, 20);

  MeshRender render(500, 500, mesh.vertices, mesh.faces, colors);
  render.add_object(ico.vertices, ico.faces, ico_colors);
  render.add_object(tet.vertices, tet.faces);
  render.render_loop(nullptr, nullptr);
  render.render_finalize();
  return 0;
}
