#ifndef TRIMESH_RENDER_H_
#define TRIMESH_RENDER_H_

#include "glad/include/glad/glad.h" // glad should be included before glfw3
#include "quatern_transform.hpp"
#include <GLFW/glfw3.h>
#include <vector>

class MeshRender {

  enum class ShaderProgramType {
    FLAT_FACES,
    SMOOTH_FACES,
    AXIS_CROSS_FLAT, // Used to render the axis-cross
    AXIS_CROSS_SMOOTH,
  };

  class Object {
    // Represent a mesh to be rendered and its positions in the openGL buffers.
  public:
    // first element position in the Vertex Buffer Object/vertices_attr
    size_t attr_offset{0}; // in elements, (not in bytes)
    // number of elements in the Vertex Buffer Object
    size_t attr_length{0};
    // total number of attributes per vertex
    size_t total_number_attr;

    size_t n_vertices{0};
    size_t n_faces{0};

    // Offset in the EBO/faces
    size_t faces_indices_offset{0}; // elements (not bytes)
    size_t faces_indices_length{0};

    // each object has its own shader program for flexibility
    int shader_program{0};
    // Transformation quaternions uniforms
    int q_loc{0}, q_inv_loc{0};
    int zoom_loc{0};          // zoom uniform
    int viewport_size_loc{0}; // uniform to keep the aspect ratio
    ShaderProgramType program_type{ShaderProgramType::FLAT_FACES};

    void set_shader_program();

    Object(unsigned int total_length_attr)
        : total_number_attr(total_length_attr) {};
  };

  // viewport size
  int width{0}, height{0};

  // contains vertices coordinates , colors, normals... for all meshes
  // meshes
  std::vector<float> vertices_attr{0};
  // list of vertices for each triangular face for all meshes
  std::vector<unsigned int> faces{0};

  size_t n_total_vertices{0};
  size_t n_total_faces{0};

  // Number of elements per vertices attrib group (position, normal, color)
  std::vector<unsigned int> vert_attr_group_length;

  // defining the rotation transformation of the current view.
  Quaternion q{1, 0, 0, 0}, q_inv{1, 0, 0, 0};
  // defining the zoom level
  double zoom_level{1}; // zoom is separate from quaternion since we don't
                        // want to break through the object.

  void *userpointer{this}; // for use in glfw callback
  GLFWwindow *window{};

  unsigned int VAO{}, VBO{}, EBO{};

  // list of object to be rendered
  std::vector<Object> objects;

  void init_window();
  void init_render();
  void resize_VAO();
  void resize_EBO();
  void draw(Object &obj);
  void set_axis_cross();

  friend void cursor_callback(GLFWwindow *window, double xpos, double ypos);
  friend void scroll_callback(GLFWwindow *window,
                              __attribute__((unused)) double xoffset,
                              double yoffset);
  friend void keyboard_callback(__attribute__((unused)) GLFWwindow *window,
                                int key, __attribute__((unused)) int scancode,
                                int action, __attribute__((unused)) int mods);

  auto add_object(std::vector<double> &ivertices,
                  std::vector<unsigned int> &ifaces,
                  ShaderProgramType shader_type) -> int;

  auto add_object(std::vector<double> &ivertices,
                  std::vector<unsigned int> &ifaces, std::vector<double> colors,
                  ShaderProgramType shader_type) -> int;

public:
  auto add_object(std::vector<double> &ivertices,
                  std::vector<unsigned int> &ifaces) -> int;

  auto add_object(std::vector<double> &ivertices,
                  std::vector<unsigned int> &ifaces,
                  std::vector<double> &colors) -> int;

  void update_object(std::vector<double> &ivertices, int id);

  MeshRender(int w_width, int w_height, std::vector<double> &ivertices,
             std::vector<unsigned int> &ifaces)
      : width(w_width), height(w_height) {

    vert_attr_group_length.resize(2, 3);
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

    vert_attr_group_length.resize(2, 3);
    init_window();
    init_render();
    vertices_attr.resize(0);
    faces.resize(0);
    set_axis_cross();
    add_object(ivertices, ifaces, icolors, ShaderProgramType::FLAT_FACES);
  }

  auto render_finalize() -> int;
  auto render_loop(int (*data_update_function)(void *fargs),
                   void *fargs) -> int;
  void add_vertex_normals(std::vector<double> &normals);
  void update_vertex_colors(std::vector<double> &colors,
                            unsigned int object_idx);
};

void keyboard_callback(GLFWwindow *window, int key, int scancode, int action,
                       int mods);
void cursor_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
#endif // TRIMESH_RENDER_H_
