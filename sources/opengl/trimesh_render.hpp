#ifndef TRIMESH_RENDER_H_
#define TRIMESH_RENDER_H_

#include "glad/include/glad/glad.h" // glad should be included before glfw3
#include "quatern_transform.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

enum SHADER_PROGRAM_TYPE {
  FLAT_FACES,
  SMOOTH_FACES,
};

class MeshRender {
  SHADER_PROGRAM_TYPE program_type{FLAT_FACES};
  unsigned int shader_program, compute_program;
  unsigned int VAO, VBO, EBO;
  unsigned int q_loc, q_inv_loc, zoom_loc;

  void init_window();
  void init_render();
  void resize_VAO();
  void set_shader_program();

public:
  unsigned int width{0}, height{0};
  std::vector<double>
      vertices_attr; // contains vertices pos and normals (vvvnnn)
  unsigned int n_vertices{0};
  std::vector<unsigned int> faces;
  unsigned int n_faces{0};
  std::vector<unsigned int>
      vert_attr_sizes; // size in bytes of x, n, c
                       // (xxxnnnccc)->[3*sizeof(double), 3*sizeof()...]
  std::vector<unsigned int>
      vert_attr_numbers; // number of x, n, c components (xxxnnnccc)->[3,3,3]

  // defining the rotation transformation of the current view.
  Quaternion q{1, 0, 0, 0}, q_inv{1, 0, 0, 0};
  // defining the zoom level
  double zoom_level{1}; // zoom is separate from quaternion since we don't want
                        // to break through the object.

  void *userpointer = this; // for use in glfw callback
  GLFWwindow *window;

  MeshRender(int w_width, int w_height, std::vector<double> &vertices,
             std::vector<unsigned int> &faces,
             std::vector<double> &vertex_normals)
      : width(w_width), height(w_height), n_vertices(vertices.size() / 3),
        faces(faces), n_faces(faces.size() / 3) {
    program_type = SMOOTH_FACES;
    vert_attr_sizes.resize(3, 3 * sizeof(double));
    vert_attr_numbers.resize(3, 3);
    vertices_attr.resize(vertices.size() * 3, 0);
    for (unsigned int i = 0; i < n_vertices; ++i) {
      for (unsigned int j = 0; j < 3; ++j) {
        vertices_attr.at(i * 9 + j) = vertices.at(i * 3 + j);
        vertices_attr.at(i * 9 + j + 3) = vertex_normals.at(i * 3 + j);
      }
      vertices_attr.at(i * 9 + 7) = 0.7; // intial colors
      vertices_attr.at(i * 9 + 8) = 0.8;
    }
    init_window();
    init_render();
  }

  MeshRender(int w_width, int w_height, std::vector<double> &vertices,
             std::vector<unsigned int> &faces)
      : width(w_width), height(w_height), n_vertices(vertices.size() / 3),
        faces(faces), n_faces(faces.size() / 3) {

    program_type = FLAT_FACES;
    vertices_attr.resize(vertices.size() * 2);
    vert_attr_sizes.resize(2, 3 * sizeof(double));
    vert_attr_numbers.resize(2, 3);

    for (unsigned int i = 0; i < n_vertices; ++i) {
      for (unsigned int j = 0; j < 3; ++j) {
        vertices_attr.at(i * 6 + j) = vertices.at(i * 3 + j);
      }
      vertices_attr.at(i * 6 + 4) = 0.7; // intial colors
      vertices_attr.at(i * 6 + 5) = 0.8;
    }
    init_window();
    init_render();
  }

  int render_finalize();
  int render_loop(int (*data_update_function)(void *fargs), void *fargs);
  // void (*keyboard_callback)(GLFWwindow *window, int key, int scancode,
  //                           int action, int mods) = NULL;
  void add_vertex_normals(std::vector<double> &normals);
  void update_vertex_colors(std::vector<double> &colors);
  // void cursor_callback(GLFWwindow *window, double xpos, double ypos);
};

void set_image2D(unsigned int unit, unsigned int *imageID, unsigned int width,
                 unsigned int height, uint16_t *img_data);

#endif // TRIMESH_RENDER_H_
