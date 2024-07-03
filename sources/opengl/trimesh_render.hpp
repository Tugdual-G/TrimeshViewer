#ifndef TRIMESH_RENDER_H_
#define TRIMESH_RENDER_H_

#include "glad/include/glad/glad.h" // glad should be included before glfw3
#include "quatern_transform.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

enum class ShaderProgramType {
  FLAT_FACES,
  SMOOTH_FACES,
  AXIS_CROSS_FLAT,
  AXIS_CROSS_SMOOTH,
};

class MeshRender {
  class Object {
  public:
    unsigned int attr_offset{0}, // in number of elements nor in bytes
        attr_length{0};

    unsigned int n_vertices{0}, n_faces{0};

    unsigned int faces_indices_offset{0}, // in number of elements nor in bytes
        faces_indices_length{0};

    unsigned int shader_program{0};
    unsigned int q_loc{0}, q_inv_loc{0}, zoom_loc{0};
    Quaternion &q, &q_inv;
    double &zoom_level;
    ShaderProgramType program_type{ShaderProgramType::FLAT_FACES};
    unsigned int total_number_attr;

    void set_shader_program();

    Object(Quaternion &q, Quaternion &q_inv, double &zoom_level,
           unsigned int total_length_attr)
        : q(q), q_inv(q_inv), zoom_level(zoom_level),
          total_number_attr(total_length_attr){};

    void draw() {
      glUseProgram(shader_program);
      // Mouse rotation
      glUniform4f(q_loc, (float)q[0], (float)q[1], (float)q[2], (float)q[3]);
      glUniform4f(q_inv_loc, (float)q_inv[0], (float)q_inv[1], (float)q_inv[2],
                  (float)q_inv[3]);
      // Zoom
      glUniform1f(zoom_loc, zoom_level);

      // glDrawElements(GL_TRIANGLES, faces_indices_length, GL_UNSIGNED_INT,
      //                (void *)(faces_indices_offset * sizeof(unsigned int)));
      glDrawElementsBaseVertex(
          GL_TRIANGLES, faces_indices_length, GL_UNSIGNED_INT,
          (void *)(faces_indices_offset * sizeof(unsigned int)),
          attr_offset / total_number_attr);
    };
  };

  // unsigned int shader_program, compute_program;
  unsigned int VAO, VBO, EBO;
  // unsigned int q_loc, q_inv_loc, zoom_loc;

  std::vector<Object> objects{};

  void init_window();
  void init_render();
  void resize_VAO();
  // void set_shader_program();

public:
  unsigned int width{0}, height{0};

  std::vector<double> vertices_attr{
      0}; // contains vertices pos and normals (vvvnnn)
  unsigned int n_total_vertices{0};

  std::vector<unsigned int> faces{0};
  unsigned int n_total_faces{0}; // total number of faces

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

  // MeshRender(int w_width, int w_height, std::vector<double> &vertices,
  //            std::vector<unsigned int> &faces,
  //            std::vector<double> &vertex_normals)
  //     : width(w_width), height(w_height), n_vertices(vertices.size() / 3),
  //       faces(faces), n_faces(faces.size() / 3) {
  //   program_type = SMOOTH_FACES;
  //   vert_attr_sizes.resize(3, 3 * sizeof(double));
  //   vert_attr_numbers.resize(3, 3);
  //   vertices_attr.resize(vertices.size() * 3, 0);
  //   for (unsigned int i = 0; i < n_vertices; ++i) {
  //     for (unsigned int j = 0; j < 3; ++j) {
  //       vertices_attr.at(i * 9 + j) = vertices.at(i * 3 + j);
  //       vertices_attr.at(i * 9 + j + 3) = vertex_normals.at(i * 3 + j);
  //     }
  //     vertices_attr.at(i * 9 + 7) = 0.7; // intial colors
  //     vertices_attr.at(i * 9 + 8) = 0.8;
  //   }
  //   init_window();
  //   init_render();
  // }

  int add_object(std::vector<double> &ivertices,
                 std::vector<unsigned int> ifaces,
                 ShaderProgramType shader_type);

  int add_object(std::vector<double> &ivertices,
                 std::vector<unsigned int> ifaces, std::vector<double> colors,
                 ShaderProgramType shader_type);

  void set_axis_cross();
  MeshRender(int w_width, int w_height, std::vector<double> &ivertices,
             std::vector<unsigned int> &ifaces)
      : width(w_width), height(w_height) {

    vert_attr_sizes.resize(2, 3 * sizeof(double));
    vert_attr_numbers.resize(2, 3);
    init_window();
    vertices_attr.resize(0);
    faces.resize(0);
    set_axis_cross();
    add_object(ivertices, ifaces, ShaderProgramType::FLAT_FACES);
    init_render();
  }

  MeshRender(int w_width, int w_height, std::vector<double> &ivertices,
             std::vector<unsigned int> &ifaces, std::vector<double> &icolors)
      : width(w_width), height(w_height) {

    vert_attr_sizes.resize(2, 3 * sizeof(double));
    vert_attr_numbers.resize(2, 3);
    init_window();
    vertices_attr.resize(0);
    faces.resize(0);
    set_axis_cross();
    add_object(ivertices, ifaces, icolors, ShaderProgramType::FLAT_FACES);
    init_render();
  }

  int render_finalize();
  int render_loop(int (*data_update_function)(void *fargs), void *fargs);
  // void (*keyboard_callback)(GLFWwindow *window, int key, int scancode,
  //                           int action, int mods) = NULL;
  void add_vertex_normals(std::vector<double> &normals);
  void update_vertex_colors(std::vector<double> &colors,
                            unsigned int object_idx);
  // void cursor_callback(GLFWwindow *window, double xpos, double ypos);
};

void set_image2D(unsigned int unit, unsigned int *imageID, unsigned int width,
                 unsigned int height, uint16_t *img_data);

#endif // TRIMESH_RENDER_H_
