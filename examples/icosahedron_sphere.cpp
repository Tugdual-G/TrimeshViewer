/* Animation of the transformation of an isocahedron into a sphere.*/
#include "../src/mesh/mesh.hpp"
#include "../src/render/colormap.hpp"
#include "../src/render/trimesh_render.hpp"
#include <cmath>
#include <iostream>
#include <unistd.h>
#include <vector>

namespace Linalg {
auto norm(std::vector<double>::iterator vect) -> double {
  return pow(vect[0] * vect[0] + vect[1] * vect[1] + vect[2] * vect[2], 0.5);
}
} // namespace Linalg

struct Fargs {
  /* Arguments for the animaion callback. */
  Mesh *mesh{nullptr};
  MeshRender *render{nullptr};
  double radius{0};
};

auto callback(void *fargs) -> int {
  /* Animation callback. */
  static unsigned int i{0};
  auto *args = (Fargs *)fargs;
  Mesh *mesh = args->mesh;
  double norm{0};

  ++i;
  if (i % 10 == 0 && i < 80) {
    mesh->subdivide();
    for (int i = 0; i < mesh->n_vertices; ++i) {
      norm = Linalg::norm(mesh->vertices.begin() + i * 3);
      for (int j = 0; j < 3; ++j) {
        mesh->vertices.at(i * 3 + j) =
            args->radius * mesh->vertices.at(i * 3 + j) / norm;
      }
    }
    // Generates colors
    std::vector<double> ico_scalar_vertex_value(mesh->n_vertices);
    for (int i = 0; i < mesh->n_vertices; ++i) {
      ico_scalar_vertex_value.at(i) = mesh->vertices.at(i * 3);
    }
    std::vector<double> ico_colors = Colormap::get_interpolated_colors(
        ico_scalar_vertex_value, Colormap::INFERNO, -0.5, 0.5);

    args->render->update_object(mesh->vertices, mesh->faces, ico_colors, 1);
  }

  usleep(1e4);
  return 1;
}

auto main() -> int {

  Mesh ico = Primitives::icosahedron();
  // Scale the vertices.
  for (auto &v : ico.vertices) {
    v *= 0.4;
  }

  double radius = Linalg::norm(ico.vertices.begin());
  // Generates colors
  std::vector<double> ico_scalar_vertex_value(ico.n_vertices);
  for (int i = 0; i < ico.n_vertices; ++i) {
    ico_scalar_vertex_value.at(i) = ico.vertices.at(i * 3);
  }
  std::vector<double> ico_colors = Colormap::get_interpolated_colors(
      ico_scalar_vertex_value, Colormap::INFERNO, -0.5, 0.5);
  // Generates colors

  MeshRender render(500, 500, ico.vertices, ico.faces, ico_colors);
  Fargs fargs{&ico, &render, radius};
  callback(&fargs);
  render.render_loop(callback, &fargs);
  render.render_finalize();
  std::cout << "number of vertices : " << ico.n_vertices << "\n";
  std::cout << "number of faces : " << ico.n_faces << "\n";
  return 0;
}
