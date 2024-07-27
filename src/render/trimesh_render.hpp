#ifndef TRIMESH_RENDER_H_
#define TRIMESH_RENDER_H_

#include "compile_shader.hpp"
#include "glad/include/glad/glad.h" // glad should be included before glfw3
#include "quatern_transform.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <map>
#include <vector>

enum class CurveType : int {
  QUAD_CURVE,
  TUBE_CURVE,
  SMOOTH_TUBE_CURVE,
};

enum class ObjectType : int {
  NONE,
  MESH,
  VECTOR,
  QUAD_CURVE,
  TUBE_CURVE,
  SMOOTH_TUBE_CURVE,
  AXIS_CROSS,
};

class MeshRender {
public:
  auto add_mesh(const std::vector<double> &ivertices,
                const std::vector<unsigned int> &ifaces) -> int;

  auto add_mesh(const std::vector<double> &ivertices,
                const std::vector<unsigned int> &ifaces,
                const std::vector<double> &colors) -> int;

  void update_vertex_colors(std::vector<double> &colors,
                            unsigned int object_idx);

  void update_object(const std::vector<double> &ivertices, int id);

  void update_object(const std::vector<double> &ivertices,
                     const std::vector<unsigned int> &ifaces, int id);

  void update_object(const std::vector<double> &ivertices,
                     const std::vector<unsigned int> &ifaces,
                     const std::vector<double> &icolors, int id);

  // Draws a set of vector or a single vectors
  auto add_vectors(const std::vector<double> &coords,
                   const std::vector<double> &directions) -> int;

  auto add_vectors(const std::vector<double> &coords,
                   const std::vector<double> &directions,
                   const std::vector<double> &colors) -> int;

  auto add_curve(const std::vector<double> &coords,
                 const std::vector<double> &colors, CurveType type,
                 double width) -> int;

  auto add_curves(const std::vector<double> &coords,
                  const std::vector<double> &colors,
                  const std::vector<unsigned int> &curves_indices,
                  CurveType type, double width) -> int;

  void set_axis_cross();

  MeshRender(int w_width, int w_height) : width(w_width), height(w_height) {
    init_window();
    init_render();
    vertices_attr.resize(0);
    faces.resize(0);
  }

  auto render_finalize() -> int;
  auto render_loop(int (*data_update_function)(void *fargs),
                   void *fargs) -> int;

  // void add_vertex_normals(std::vector<double> &normals);

private:
  class Object {
    // Represent a mesh to be rendered and its positions in the openGL buffers.
    void set_shader_program();

  public:
    ObjectType object_type;

    // first element position in the Vertex Buffer Object/vertices_attr
    long int attr_offset{-1}; // in elements, (not in bytes)
    // number of elements in the Vertex Buffer Object
    long int attr_length{-1};

    // total number of attributes per vertex
    long int total_number_attr{-1};

    auto n_vertices() const -> long int {
      return attr_length / total_number_attr;
    }

    // Offset in the EBO/faces
    // A face can also be a pentagon, a line strip ...
    long int faces_indices_offset{-1}; // elements (not bytes)
    long int faces_indices_length{-1};
    long int vertices_per_primitive{-1};

    auto n_faces() const -> long int {
      return faces_indices_length / vertices_per_primitive;
    }

    // each object has its own shader program for flexibility
    int shader_program{0};
    // Transformation quaternions uniforms
    int q_loc{0}, q_inv_loc{0};
    int zoom_loc{0};          // zoom uniform
    int viewport_size_loc{0}; // uniform to keep the aspect ratio
    int n_instances{0};

    Object(ObjectType type, long int attr_offset, long int attr_length,
           long int total_number_attr, long int indices_offset,
           long int indices_length, long int vertices_per_primitive);
  };

  // viewport size
  int width{0}, height{0};

  // contains vertices coordinates , colors, normals... for all meshes
  std::vector<float> vertices_attr;
  // list of vertices for each triangular face for all meshes
  std::vector<unsigned int> faces;

  // Number of elements per vertices attrib group (position, normal, color)
  const std::vector<long int> vert_attr_group_length{3, 3};
  auto vertices_stride() -> long int;

  // defining the rotation transformation of the current view.
  Quaternion q{1, 0, 0, 0}, q_inv{1, 0, 0, 0};
  // defining the zoom level
  double zoom_level{1}; // zoom is separate from quaternion since we don't
                        // want to break through the object.

  void *userpointer{this}; // for use in glfw callback
  GLFWwindow *window{};

  // ID of the global mesh storage
  unsigned int VAO{0}, VBO{0}, EBO{0};

  // list of object to be rendered
  std::vector<Object> objects;

  void init_window();
  void init_render();
  void resize_VBO();
  void resize_EBO();
  void draw(Object &obj);

  friend void cursor_callback(GLFWwindow *window, double xpos, double ypos);
  friend void scroll_callback(GLFWwindow *window,
                              __attribute__((unused)) double xoffset,
                              double yoffset);
  friend void keyboard_callback(__attribute__((unused)) GLFWwindow *window,
                                int key, __attribute__((unused)) int scancode,
                                int action, __attribute__((unused)) int mods);

  void add_indices(const std::vector<unsigned int> &new_indices);
  void update_indices(const std::vector<unsigned int> &new_indices,
                      Object &obj);
  void add_vertices(const std::vector<double> &new_vertices);
  void add_vertices(const std::vector<double> &new_vertices,
                    const std::vector<double> &colors);

  void update_vertices(const std::vector<double> &new_vertices, Object &obj);
  void update_vertices(const std::vector<double> &new_vertices,
                       const std::vector<double> &colors, Object &obj);

  auto add_object(const std::vector<double> &ivertices,
                  const std::vector<unsigned int> &ifaces,
                  ObjectType object_type) -> int;

  auto add_object(const std::vector<double> &ivertices,
                  const std::vector<unsigned int> &ifaces,
                  const std::vector<double> &colors,
                  ObjectType object_type) -> int;
};

// convert between different types
const std::map<ObjectType, ShaderProgramType> OBJECT_SHADER_MAP{
    {ObjectType::MESH, ShaderProgramType::FLAT_FACES},
    {ObjectType::VECTOR, ShaderProgramType::VECTOR_INSTANCE},
    {ObjectType::QUAD_CURVE, ShaderProgramType::QUAD_CURVE},
    {ObjectType::TUBE_CURVE, ShaderProgramType::TUBE_CURVE},
    {ObjectType::SMOOTH_TUBE_CURVE, ShaderProgramType::SMOOTH_TUBE_CURVE},
    {ObjectType::AXIS_CROSS, ShaderProgramType::AXIS_CROSS}};

const std::map<ObjectType, int> OBJECT_VERTICES_PER_PRIMITIVE_MAP{
    {ObjectType::NONE, -1},      {ObjectType::MESH, 3},
    {ObjectType::VECTOR, 3},     {ObjectType::QUAD_CURVE, 4},
    {ObjectType::TUBE_CURVE, 4}, {ObjectType::SMOOTH_TUBE_CURVE, 4},
    {ObjectType::AXIS_CROSS, 3}};

void keyboard_callback(GLFWwindow *window, int key, int scancode, int action,
                       int mods);
void cursor_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
#endif // TRIMESH_RENDER_H_
