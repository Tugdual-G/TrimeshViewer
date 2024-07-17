/* This file loads three meshes and display them.*/
#include "../src/mesh/mesh.hpp"
#include "../src/ply/plyfile.hpp"
#include "../src/render/colormap.hpp"
#include "../src/render/trimesh_render.hpp"
#include <cmath>
#include <vector>

namespace Geometry {
auto norm(std::vector<double>::iterator vect) -> double {
  return pow(vect[0] * vect[0] + vect[1] * vect[1] + vect[2] * vect[2], 0.5);
}
} // namespace Geometry

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

  ///////////////////////////////////////////
  //          Torus
  ///////////////////////////////////////////
  Mesh torus = Primitives::torus(0.2, 0.08, 16);
  for (int i = 0; i < torus.n_vertices; ++i) {
    torus.vertices.at(i * 3) -= 0.4;
    torus.vertices.at(i * 3 + 1) -= 0.4;
  }

  ///////////////////////////////////////////
  //  Building a sphere from an icosahedron
  ///////////////////////////////////////////
  Mesh sphere = Primitives::icosahedron();
  double radius = Geometry::norm(sphere.vertices.begin());
  double norm{0};
  sphere.subdivide();
  sphere.subdivide();
  for (int i = 0; i < sphere.n_vertices; ++i) {
    norm = Geometry::norm(sphere.vertices.begin() + i * 3);
    for (int j = 0; j < 3; ++j) {
      sphere.vertices.at(i * 3 + j) =
          radius * sphere.vertices.at(i * 3 + j) / norm;
    }
  }
  for (auto &v : sphere.vertices) {
    v *= 0.2;
  }
  for (int i = 0; i < sphere.n_vertices; ++i) {
    sphere.vertices.at(i * 3 + 1) -= 0.5;
    sphere.vertices.at(i * 3 + 2) += 0.2;
  }
  std::vector<double> sphere_scalar_vertex_value(sphere.n_vertices);
  for (int i = 0; i < sphere.n_vertices; ++i) {
    sphere_scalar_vertex_value.at(i) = sphere.vertices.at(i * 3);
  }
  std::vector<double> sphere_colors = Colormap::get_interpolated_colors(
      sphere_scalar_vertex_value, Colormap::MAGMA, -0.5, 0.5);

  /////////////////////////////////////////
  //          Icosahedron
  ////////////////////////////////////////
  Mesh ico = Primitives::icosahedron();
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
  render.add_object(sphere.vertices, sphere.faces, sphere_colors);
  render.add_object(torus.vertices, torus.faces);
  render.render_loop(nullptr, nullptr);
  render.render_finalize();
  return 0;
}
