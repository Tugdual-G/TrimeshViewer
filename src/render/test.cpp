/* Basic script to debug and test implementation */
#include "colormap.hpp"
#include "trimesh_render.hpp"
#include <cmath>
#include <vector>

constexpr double phi{1.6180339887498};

struct Fargs {
  /* For the animaion callback. */
  std::vector<double> *vertices{nullptr};
  std::vector<double> *colors{nullptr};
  MeshRender *render{nullptr};
  int obj_id{0};
};

auto callback(void *fargs) -> int {
  static double t{0};
  /* Animation callback. */
  auto *args = (Fargs *)fargs;

  static std::vector<double> init_colors = *args->colors;

  for (auto &v : *args->vertices) {
    v -= 0.001;
  }

  args->render->update_object(*args->vertices, args->obj_id);
  double r = (1.0 + std::sin(t)) / 2.0;
  double g = (1.0 + std::cos(1.12 * t)) / 2.0;
  double b = (1.0 + std::cos(1.51 * t)) / 2.0;
  for (int i = 0; i < (int)(*args->vertices).size() / 3; ++i) {
    (*args->colors).at(i * 3) = init_colors.at(i * 3) * r;
    (*args->colors).at(i * 3 + 1) = init_colors.at(i * 3 + 1) * g;
    (*args->colors).at(i * 3 + 2) = init_colors.at(i * 3 + 2) * b;
  }
  args->render->update_vertex_colors(*args->colors, args->obj_id);
  t += 0.03;
  return 1;
}

auto norm(const double *v) -> double {
  return pow(v[0] * v[0] + v[1] * v[1] + v[2] * v[2], 0.5);
}
void parametric_curve(double *coord, double *tangent, double t) {
  // parametric curve designed for t in [0.0, 1.0].
  constexpr double Tau{2.0 * 3.1415926535898};
  constexpr double r{0.3};
  t *= Tau;
  coord[0] = r * cos(1.5 * t);
  coord[1] = r * sin(3.3 * t) + 0.5 * r;
  coord[2] = 1.5 * r * (sin(t) + cos(t));

  tangent[0] = -1.5 * r * sin(1.5 * t);
  tangent[1] = 3.3 * r * cos(3.3 * t);
  tangent[2] = 1.5 * r * (cos(t) - sin(t));
}

auto main() -> int {
  // Define a list of points
  //

  std::vector<double> icosahedron_vertices = {
      -1,   phi,  0,    //
      1,    phi,  0,    //
      -1,   -phi, 0,    //
      1,    -phi, 0,    //
      0,    -1,   phi,  //
      0,    1,    phi,  //
      0,    -1,   -phi, //
      0,    1,    -phi, //
      phi,  0,    -1,   //
      phi,  0,    1,    //
      -phi, 0,    -1,   //
      -phi, 0,    1     //
  };

  // move and scale
  for (auto &v : icosahedron_vertices) {
    v *= 0.1;
  }
  for (int i = 0; i < (int)icosahedron_vertices.size() / 3; ++i) {
    icosahedron_vertices.at(i * 3 + 2) -= 0.5;
  }

  std::vector<unsigned int> icosahedron_faces = {
      0,  11, 5,  //
      0,  5,  1,  //
      0,  1,  7,  //
      0,  7,  10, //
      0,  10, 11, //
      11, 10, 2,  //
      5,  11, 4,  //
      1,  5,  9,  //
      7,  1,  8,  //
      10, 7,  6,  //
      3,  9,  4,  //
      3,  4,  2,  //
      3,  2,  6,  //
      3,  6,  8,  //
      3,  8,  9,  //
      9,  8,  1,  //
      4,  9,  5,  //
      2,  4,  11, //
      6,  2,  10, //
      8,  6,  7   //
  };

  std::vector<double> cube_vertices = {
      // points coord
      -1, -1, -1, // 0
      1,  -1, -1, // 1
      1,  -1, 1,  // 2
      -1, -1, 1,  // 3
      -1, 1,  -1, //
      1,  1,  -1, //
      1,  1,  1,  //
      -1, 1,  1,  //
  };
  // move and scale the cube
  for (auto &v : cube_vertices) {
    v *= 0.25;
    v += 0.25;
  }

  std::vector<unsigned int> cube_faces = {
      0, 1, 2, // 0
      2, 3, 0, // 1
      1, 5, 6, // 2
      6, 2, 1, // 3
      2, 6, 7, // 4
      2, 7, 3, // 5
      0, 3, 4, // 6
      4, 3, 7, // 7
      5, 4, 7, // 8
      5, 7, 6, // 9
      1, 0, 4, // 10
      1, 4, 5, // 11
  };

  std::vector<double> tet_vertices = {
      // points coord
      1,  1,  1,  // 0
      1,  -1, -1, // 1
      -1, 1,  -1, // 2
      -1, -1, 1,  // 3
  };
  // move and scale
  for (auto &v : tet_vertices) {
    v *= 0.15;
    v -= 0.3;
  }

  std::vector<unsigned int> tet_faces = {
      0, 1, 3, // 0
      1, 2, 3, // 1
      1, 2, 0, // 2
      2, 0, 3, // 3
  };

  // A test value to generate colors
  std::vector<double> cube_scalar_vertex_value(cube_vertices.size() / 3);
  for (int i = 0; i < (int)cube_scalar_vertex_value.size(); ++i) {
    cube_scalar_vertex_value.at(i) = i;
  }

  // A test value to generate colors
  std::vector<double> ico_scalar_vertex_value(icosahedron_vertices.size() / 3);
  for (int i = 0; i < (int)ico_scalar_vertex_value.size(); ++i) {
    ico_scalar_vertex_value.at(i) = i + 6;
  }
  std::vector<double> ico_colors = Colormap::get_interpolated_colors(
      ico_scalar_vertex_value, Colormap::MAGMA, 0, 20);

  std::vector<double> cube_colors =
      Colormap::get_nearest_colors(cube_scalar_vertex_value, Colormap::VIRIDIS);

  constexpr int N{200};
  std::vector<double> tube_coords(N * 3);
  std::vector<double> tube_tangents(N * 3);

  std::vector<double> velocity_magnitude(tube_coords.size() / 3);
  for (int i = 0; i < N; ++i) {
    parametric_curve(&tube_coords[i * 3], &tube_tangents[i * 3],
                     (double)i * 1.5 / (N - 1));

    velocity_magnitude.at(i) = norm(&tube_tangents[i * 3]);
  }

  std::vector<double> tube_colors{0.7, 0.4, 0.9};

  MeshRender render(500, 500, cube_vertices, cube_faces, cube_colors);

  render.add_object(icosahedron_vertices, icosahedron_faces, ico_colors);

  int curve_idx = render.add_curve(tube_coords, tube_colors,
                                   CurveType::SMOOTH_TUBE_CURVE, 0.01);

  tube_colors =
      Colormap::get_nearest_colors(velocity_magnitude, Colormap::PLASMA);
  tube_colors.resize(tube_colors.size() + 6);
  render.update_vertex_colors(tube_colors, curve_idx);

  ///////////////////////////////////////////////////
  // Replace an object by another
  /////////////////////////////////////////////////
  int ico_id =
      render.add_object(icosahedron_vertices, icosahedron_faces, ico_colors);
  icosahedron_faces = cube_faces;
  icosahedron_vertices = cube_vertices;
  cube_colors =
      Colormap::get_nearest_colors(cube_scalar_vertex_value, Colormap::MAGMA);
  render.update_object(icosahedron_vertices, icosahedron_faces, cube_colors,
                       ico_id);

  // Arguments for the animation callback
  Fargs ico_args = {.vertices = &icosahedron_vertices,
                    .colors = &cube_colors,
                    .render = &render,
                    .obj_id = ico_id};

  render.add_object(tet_vertices, tet_faces);
  render.render_loop(callback, &ico_args);
  render.render_finalize();

  return 0;
}
