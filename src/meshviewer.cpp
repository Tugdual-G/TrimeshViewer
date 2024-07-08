#include "mesh/mesh.hpp"
#include "ply/plyfile.hpp"
#include "render/colormap.hpp"
#include "render/trimesh_render.hpp"
#include <iostream>
#include <vector>

int main(__attribute__((unused)) int argc, char *argv[]) {

  PlyFile file(argv[1]);

  Mesh mesh(file.vertices, file.faces);

  mesh.set_one_ring();

  // Takes one_ring as an argument to make explicit that the
  // method depends on the one-ring.
  std::vector<double> kn = mesh.get_mean_curvature(mesh.one_ring);
  std::vector<double> k = mesh.get_scalar_mean_curvature(kn);

  auto [minvk, maxvk] = std::minmax_element(k.begin(), k.end());
  double mink = *minvk, maxk = *maxvk;

  std::vector<double> colors =
      get_interpolated_colors(k, INFERNO, mink - 0.1, maxk);

  auto [minv, maxv] =
      std::minmax_element(mesh.vertices.begin(), mesh.vertices.end());

  double max{*maxv}, min{*minv};
  std::cout << "max " << max << ", min " << min << "\n";
  double extent_vert = *maxv - *minv;
  extent_vert *= 1.2;
  for (auto &v : mesh.vertices) {
    v /= extent_vert;
  }

  MeshRender render(500, 500, mesh.vertices, mesh.faces, colors);
  render.render_loop(NULL, NULL);
  render.render_finalize();
  return 0;
}
