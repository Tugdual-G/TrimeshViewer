#ifndef TRIMESH_RENDER_H_
#define TRIMESH_RENDER_H_

#include "include/glad/glad.h" // glad should be included before glfw3
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <vector>

class MeshRender {
  unsigned int shader_program, compute_program;
  unsigned int VAO, VBO;

public:
  unsigned int width{0}, height{0};
  double *vertexes_attr{NULL}; // contains vertices pos and normals (vvvnnn)
  unsigned int n_vertices{0};
  unsigned int *faces{NULL};
  unsigned int n_faces{0};

  void *userpointer;
  GLFWwindow *window;

  MeshRender(int w_width, int w_height, std::vector<double> &vertices,
             std::vector<unsigned int> &faces,
             std::vector<double> &vertex_normals)
      : width(w_width), height(w_height), n_vertices(vertices.size() / 3),
        faces(faces.data()), n_faces(faces.size() / 3) {
    // double max = *std::max_element(vertices.begin(), vertices.end());
    vertexes_attr = new double[vertices.size() * 2];
    for (unsigned int i = 0; i < n_vertices; ++i) {
      for (unsigned int j = 0; j < 3; ++j) {
        vertexes_attr[i * 6 + j] = vertices.at(i * 3 + j);
        vertexes_attr[i * 6 + j + 3] = vertex_normals.at(i * 3 + j);
      }
    }
  }

  void init_window();
  void init_render();
  int render_finalize();
  int render_loop(int (*data_update_function)(void *fargs), void *fargs);
  void (*keyboard_callback)(GLFWwindow *window, int key, int scancode,
                            int action, int mods) = NULL;
};

void set_image2D(unsigned int unit, unsigned int *imageID, unsigned int width,
                 unsigned int height, uint16_t *img_data);

#endif // TRIMESH_RENDER_H_
