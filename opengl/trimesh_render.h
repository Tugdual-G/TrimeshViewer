#ifndef TRIMESH_RENDER_H_
#define TRIMESH_RENDER_H_

#include "../plymesh.h"
#include "include/glad/glad.h" // glad should be included before glfw3
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>

class MeshRender {
  unsigned int shader_program, compute_program;
  unsigned int VAO, VBO;

public:
  unsigned int width{0}, height{0};
  double *vertices{NULL};
  int n_vertices{0};
  unsigned int *faces{NULL};
  int n_faces{0};
  PlyMesh *plymesh;

  void *userpointer;
  GLFWwindow *window;

  MeshRender(int w_width, int w_height, PlyMesh *mesh)
      : width(w_width), height(w_height), vertices(mesh->vertices.data()),
        n_vertices(mesh->n_vertices), faces(mesh->faces.data()),
        n_faces(mesh->n_faces), plymesh(mesh) {
    double max = *std::max_element(vertices, vertices + n_vertices * 3);
    std::cout << "v size :" << mesh->vertices.size() << std::endl;
    std::cout << "vc size :" << n_vertices * 3 << std::endl;
    for (double *v = vertices; v < vertices + n_vertices * 3; ++v) {
      *v /= max;
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
