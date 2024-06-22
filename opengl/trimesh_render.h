#ifndef TRIMESH_RENDER_H_
#define TRIMESH_RENDER_H_

#include "include/glad/glad.h" // glad should be included before glfw3
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <vector>

class MeshRender {
  unsigned int shader_program, compute_program;
  unsigned int VAO, VBO, EBO;

public:
  unsigned int width{0}, height{0};
  std::vector<double>
      vertexes_attr; // contains vertices pos and normals (vvvnnn)
  unsigned int n_vertices{0};
  std::vector<unsigned int> faces;
  unsigned int n_faces{0};
  unsigned int size_vertice_attr; // xxxnnnccc
  unsigned int n_vertice_attr;

  void *userpointer;
  GLFWwindow *window;

  MeshRender(int w_width, int w_height, std::vector<double> &vertices,
             std::vector<unsigned int> &faces,
             std::vector<double> &vertex_normals)
      : width(w_width), height(w_height), n_vertices(vertices.size() / 3),
        faces(faces), n_faces(faces.size() / 3) {
    size_vertice_attr = 6 * sizeof(double);
    n_vertice_attr = 6;
    vertexes_attr.resize(vertices.size() * 2);
    for (unsigned int i = 0; i < n_vertices; ++i) {
      for (unsigned int j = 0; j < 3; ++j) {
        vertexes_attr.at(i * 6 + j) = vertices.at(i * 3 + j);
        vertexes_attr.at(i * 6 + j + 3) = vertex_normals.at(i * 3 + j);
      }
    }
  }

  void init_window();
  void init_render();
  int render_finalize();
  int render_loop(int (*data_update_function)(void *fargs), void *fargs);
  void (*keyboard_callback)(GLFWwindow *window, int key, int scancode,
                            int action, int mods) = NULL;
  void add_vertex_normals(std::vector<double> &normals);
  void add_vertex_colors(std::vector<double> &normals);
};

void set_image2D(unsigned int unit, unsigned int *imageID, unsigned int width,
                 unsigned int height, uint16_t *img_data);

#endif // TRIMESH_RENDER_H_
