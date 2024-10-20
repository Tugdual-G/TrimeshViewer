#include "trimesh_render.hpp"
#include "compile_shader.hpp"
#include "glad/include/glad/glad.h" // glad should be included before glfw3
#include "linalg.hpp"
#include "quatern_transform.hpp"
#include "vector_instance.hpp"
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <map>
#include <numeric>
#include <string>

auto glCheckError_(const char *file, int line) -> GLenum;
#define glCheckError() glCheckError_(__FILE__, __LINE__)

constexpr double MOUSE_SENSITIVITY{0.005};
constexpr double SCROLL_SENSITIVITY{0.05};

void framebuffer_size_callback(__attribute__((unused)) GLFWwindow *window,
                               int width, int height) {
  glViewport(0, 0, width, height);
}

void MeshRender::init_window() {

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4); // anti-aliasing

  window = glfwCreateWindow(width, height, "MeshRender", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Error, failed to create GLFW window\n";
    glfwTerminate();
    exit(1);
  }
  glfwMakeContextCurrent(window);
  glfwSetWindowUserPointer(window, userpointer);

  glfwSetKeyCallback(window, keyboard_callback);
  glfwSetCursorPosCallback(window, cursor_callback);
  glfwSetScrollCallback(window, scroll_callback);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
    std::cout << "Failed to initialize GLAD\n";
    exit(1);
  }

  glEnable(GL_MULTISAMPLE); // anti-aliasing
  glDepthRange(1, 0);       // Makes opengl right-handed
}

void MeshRender::Object::set_shader_program() {

  shader_program = create_program(OBJECT_SHADER_MAP.at(object_type));

  glUseProgram(shader_program);

  q_loc = glGetUniformLocation(shader_program, "q");
  q_inv_loc = glGetUniformLocation(shader_program, "q_inv");
  zoom_loc = glGetUniformLocation(shader_program, "zoom_level");
  viewport_size_loc = glGetUniformLocation(
      shader_program, "viewport_size"); // for the aspect ratio
}

void MeshRender::init_storage() {

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glCheckError();
}

void MeshRender::resize_VBO() {
  // Resize the VAO and update vertex attributes data
  long int stride = (long int)sizeof(float) * vertices_stride();

  int n_vertice_attr = (int)VERT_ATTR_LENGTHS.size();
  long int offset{0 * sizeof(float)};

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER,
               (long)vertices_attr.size() * (long)sizeof(float),
               vertices_attr.data(), GL_STATIC_DRAW);

  for (int i = 0; i < n_vertice_attr; ++i) {
    // TODO do not hardcode 3 !
    glVertexAttribPointer(i, (int)VERT_ATTR_LENGTHS[i], GL_FLOAT, GL_FALSE,
                          (int)stride, (void *)(offset));
    glEnableVertexAttribArray(i);
    offset += VERT_ATTR_LENGTHS[i] * (long int)sizeof(float);
  }
}

void MeshRender::resize_EBO() {
  // Square EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               (long)sizeof(unsigned int) * faces.size(), faces.data(),
               GL_STATIC_DRAW);
  glCheckError();
}

auto MeshRender::render_loop(int (*data_update_function)(void *fargs),
                             void *fargs) -> int {

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  int flag = 1;
  glClearColor(0.0, 0.0, 0.0, 0.0);

  glEnable(GL_DEPTH_TEST);
  // Accept fragment if closer to the camera
  glDepthFunc(GL_LESS);

  if (data_update_function == nullptr) {
    while ((glfwWindowShouldClose(window) == 0)) {
      glfwGetWindowSize(window, &width, &height);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      for (auto &obj : objects) {
        draw(obj);
      }
      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  } else {
    while ((glfwWindowShouldClose(window) == 0) && (flag != 0)) {

      glfwGetWindowSize(window, &width, &height);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      for (auto &obj : objects) {
        draw(obj);
      }
      glfwSwapBuffers(window);
      glfwPollEvents();

      flag = data_update_function(fargs);
    }
  }

  glCheckError();
  return 0;
}

void MeshRender::init_frame() const {
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glClearColor(0.0, 0.0, 0.0, 0.0);

  glEnable(GL_DEPTH_TEST);
  // Accept fragment if closer to the camera
  glDepthFunc(GL_LESS);
}

void MeshRender::draw_frame() {

  glfwGetWindowSize(window, &width, &height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (auto &obj : objects) {
    draw(obj);
  }
  glfwSwapBuffers(window);
  glfwPollEvents();
}

auto MeshRender::render_finalize() -> int {
  // Cleanup
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glfwTerminate();
  return 0;
}

void MeshRender::draw(Object &obj) {
  glUseProgram(obj.shader_program);
  // Mouse rotation
  glUniform4f(obj.q_loc, (float)q[0], (float)q[1], (float)q[2], (float)q[3]);
  glUniform4f(obj.q_inv_loc, (float)q_inv[0], (float)q_inv[1], (float)q_inv[2],
              (float)q_inv[3]);
  // Zoom
  glUniform1f(obj.zoom_loc, (float)zoom_level);

  glUniform2f(obj.viewport_size_loc, (float)width, (float)height);

  switch (obj.object_type) {
  case ObjectType::AXIS_CROSS:
  case ObjectType::MESH:
    glDrawElementsBaseVertex(
        GL_TRIANGLES, obj.faces_indices_length, GL_UNSIGNED_INT,
        (void *)(obj.faces_indices_offset * sizeof(unsigned int)),
        obj.attr_offset / obj.total_number_attr);
    break;

  case ObjectType::QUAD_CURVE:
  case ObjectType::TUBE_CURVE:
  case ObjectType::SMOOTH_TUBE_CURVE:
    glDrawElementsBaseVertex(
        GL_LINES_ADJACENCY, obj.faces_indices_length, GL_UNSIGNED_INT,
        (void *)(obj.faces_indices_offset * sizeof(unsigned int)),
        obj.attr_offset / obj.total_number_attr);
    break;

  case ObjectType::VECTOR:
    glDrawElementsInstancedBaseVertex(
        GL_TRIANGLES, obj.faces_indices_length, GL_UNSIGNED_INT,
        (void *)(obj.faces_indices_offset * sizeof(unsigned int)),
        obj.n_instances, obj.attr_offset / obj.total_number_attr);

    break;

  default:
    break;
  }
};

void MeshRender::add_indices(const std::vector<unsigned int> &new_indices) {
  faces.resize(faces.size() + new_indices.size());
  std::copy(new_indices.begin(), new_indices.end(),
            faces.end() - (long int)new_indices.size());
  resize_EBO();
}

void MeshRender::update_indices(const std::vector<unsigned int> &new_indices,
                                Object &obj) {
  // updates faces
  std::vector<unsigned int> faces_tmp(faces);

  faces.resize(faces.size() +
               ((long int)new_indices.size() - obj.faces_indices_length));

  std::copy(new_indices.begin(), new_indices.end(),
            faces.begin() + (long)obj.faces_indices_offset);

  std::copy(faces_tmp.begin() +
                (long)(obj.faces_indices_offset + obj.faces_indices_length),
            faces_tmp.end(),
            faces.begin() +
                (long)(obj.faces_indices_offset + new_indices.size()));

  obj.faces_indices_length = (long)new_indices.size();
  resize_EBO();
}

void MeshRender::fill_vertice_attr(const std::vector<double> &new_vertices,
                                   const std::vector<double> &new_colors,
                                   long int vertices_offset) {
  // vertices_offset is the global offset in the vector

  long int stride = vertices_stride();
  long int color_offset = vertices_attr_offset(VertexAttr::COLOR);

  if (new_colors.size() == new_vertices.size()) {
    for (long int i = 0; i < (long)new_vertices.size() / 3; ++i) {
      for (long int j = 0; j < 3; ++j) {
        vertices_attr.at(vertices_offset + i * stride + j) =
            (float)new_vertices.at(i * 3 + j);
        vertices_attr.at(vertices_offset + i * stride + color_offset + j) =
            (float)new_colors.at(i * 3 + j);
      }
    }
  } else if (new_colors.size() == 3) {
    for (long int i = 0; i < (long)new_vertices.size() / 3; ++i) {
      for (long int j = 0; j < 3; ++j) {
        vertices_attr.at(vertices_offset + i * stride + j) =
            (float)new_vertices.at(i * 3 + j);
        vertices_attr.at(vertices_offset + i * stride + color_offset + j) =
            (float)new_colors.at(j);
      }
    }
  } else {
    throw std::invalid_argument(
        "New vertices size and colors size don't match in " +
        std::string(__func__) + "\n");
  }
}

void MeshRender::add_vertices(const std::vector<double> &new_vertices,
                              const std::vector<double> &colors) {

  long int stride = vertices_stride();
  long int n_curent_vertices = n_vertices();

  vertices_attr.resize(vertices_attr.size() +
                       (new_vertices.size() / 3) * stride);

  fill_vertice_attr(new_vertices, colors, n_curent_vertices * stride);

  resize_VBO();
}

void MeshRender::update_vertices(const std::vector<double> &new_vertices,
                                 const std::vector<double> &colors,
                                 Object &obj) {

  // update vertices, can change the number of vertices.
  std::vector<float> attr_tmp(vertices_attr);
  long int new_attr_length = vertices_stride() * (long)new_vertices.size() / 3;

  vertices_attr.resize(vertices_attr.size() +
                       (new_attr_length - obj.attr_length));

  fill_vertice_attr(new_vertices, colors, obj.attr_offset);

  std::copy(attr_tmp.begin() + (long)(obj.attr_offset + obj.attr_length),
            attr_tmp.end(),
            vertices_attr.begin() + (long)(obj.attr_offset + new_attr_length));

  obj.attr_length = new_attr_length;

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, (long)(vertices_attr.size() * sizeof(float)),
               vertices_attr.data(), GL_STATIC_DRAW);
}

MeshRender::Object::Object(ObjectType type, long int attr_offset,
                           long int attr_length, long int total_number_attr,
                           long int indices_offset, long int indices_length,
                           long int vertices_per_primitive)
    : object_type(type), attr_offset(attr_offset), attr_length(attr_length),
      total_number_attr(total_number_attr),
      faces_indices_offset(indices_offset),
      faces_indices_length(indices_length),
      vertices_per_primitive(vertices_per_primitive) {
  set_shader_program();
}

void MeshRender::update_object(const std::vector<double> &ivertices, int id) {
  /* Update the vertices positions of an object. */
  Object &obj = objects.at(id);

  for (unsigned int i = 0; i < obj.n_vertices(); ++i) {
    for (unsigned int j = 0; j < 3; ++j) {
      vertices_attr.at(obj.attr_offset + i * obj.total_number_attr + j) =
          (float)ivertices.at(i * 3 + j);
    }
  }

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferSubData(GL_ARRAY_BUFFER, (long)sizeof(float) * obj.attr_offset,
                  (long)sizeof(float) * obj.attr_length,
                  vertices_attr.data() + obj.attr_offset);
}

void MeshRender::update_object(const std::vector<double> &ivertices,
                               const std::vector<unsigned int> &ifaces,
                               int id) {
  /* TODO rename the ubdate_object as ubdate_trimesh */
  /* Update the vertices and faces of an object. */
  Object &obj = objects.at(id);

  // updates faces
  update_indices(ifaces, obj);

  update_vertices(ivertices, DEFAULT_COLOR, obj);

  glCheckError();
}

void MeshRender::update_object(const std::vector<double> &ivertices,
                               const std::vector<unsigned int> &ifaces,
                               const std::vector<double> &icolors, int id) {

  if (ivertices.size() != icolors.size()) {
    throw std::invalid_argument(
        "Vertices size and colors size don't match in " +
        std::string(__func__) + "\n");
  }
  /* Update the vertices and faces of an object. */
  Object &obj = objects.at(id);

  // updates faces
  update_indices(ifaces, obj);

  update_vertices(ivertices, icolors, obj);

  glCheckError();
}

auto MeshRender::add_object(const std::vector<double> &ivertices,
                            const std::vector<unsigned int> &ifaces,
                            const std::vector<double> &colors,
                            ObjectType object_type) -> int {

  auto attr_offset = (long int)vertices_attr.size();
  auto attr_length = (long int)ivertices.size() * 2;
  auto faces_indices_offset = (long int)faces.size();
  auto faces_indices_length = (long int)ifaces.size();

  Object new_obj(object_type, attr_offset, attr_length, vertices_stride(),
                 faces_indices_offset, faces_indices_length,
                 OBJECT_VERTICES_PER_PRIMITIVE_MAP.at(object_type));

  add_indices(ifaces);
  add_vertices(ivertices, colors);

  objects.push_back(new_obj);
  return (int)objects.size() - 1;
}

auto MeshRender::add_mesh(const std::vector<double> &ivertices,
                          const std::vector<unsigned int> &ifaces,
                          const std::vector<double> &colors) -> int {

  return add_object(ivertices, ifaces, colors, ObjectType::MESH);
}

auto MeshRender::add_mesh(const std::vector<double> &ivertices,
                          const std::vector<unsigned int> &ifaces) -> int {
  return add_object(ivertices, ifaces, DEFAULT_COLOR, ObjectType::MESH);
}

void MeshRender::fill_vectors_instance_attr(
    const std::vector<double> &coords, const std::vector<double> &directions,
    const std::vector<double> &colors, std::vector<float> &instances_attr) {

  instances_attr.resize(coords.size() * 3);
  if (colors.size() == coords.size()) {
    for (int i = 0; i < (int)coords.size(); i += 3) {
      instances_attr.at(i * 3) = (float)coords.at(i);
      instances_attr.at(i * 3 + 1) = (float)coords.at(i + 1);
      instances_attr.at(i * 3 + 2) = (float)coords.at(i + 2);

      instances_attr.at(i * 3 + 3) = (float)directions.at(i);
      instances_attr.at(i * 3 + 4) = (float)directions.at(i + 1);
      instances_attr.at(i * 3 + 5) = (float)directions.at(i + 2);

      instances_attr.at(i * 3 + 6) = (float)colors.at(i);
      instances_attr.at(i * 3 + 7) = (float)colors.at(i + 1);
      instances_attr.at(i * 3 + 8) = (float)colors.at(i + 2);
    }
  } else if (colors.size() == 3) {

    for (int i = 0; i < (int)coords.size(); i += 3) {
      instances_attr.at(i * 3) = (float)coords.at(i);
      instances_attr.at(i * 3 + 1) = (float)coords.at(i + 1);
      instances_attr.at(i * 3 + 2) = (float)coords.at(i + 2);

      instances_attr.at(i * 3 + 3) = (float)directions.at(i);
      instances_attr.at(i * 3 + 4) = (float)directions.at(i + 1);
      instances_attr.at(i * 3 + 5) = (float)directions.at(i + 2);

      instances_attr.at(i * 3 + 6) = (float)colors.at(0);
      instances_attr.at(i * 3 + 7) = (float)colors.at(1);
      instances_attr.at(i * 3 + 8) = (float)colors.at(2);
    }
  }
}

auto MeshRender::add_vectors(const std::vector<double> &coords,
                             const std::vector<double> &directions,
                             const std::vector<double> &colors) -> int {

  // Draws a set of colored vectors or a single colored vector
  int obj_id = add_object(VectorInstance::vector_instance_vertices,
                          VectorInstance::vector_instance_faces, DEFAULT_COLOR,
                          ObjectType::VECTOR);

  Object &obj = objects.at(obj_id);
  obj.n_instances = (int)coords.size() / 3;

  std::vector<float> instances_attr;

  fill_vectors_instance_attr(coords, directions, colors, instances_attr);

  unsigned int vector_VBO{0};
  glGenBuffers(1, &vector_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, vector_VBO);
  glBufferData(GL_ARRAY_BUFFER,
               (long)sizeof(float) * (long)instances_attr.size(),
               instances_attr.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(3);

  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(4);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glVertexAttribDivisor(2, 1);
  glVertexAttribDivisor(3, 1);
  glVertexAttribDivisor(4, 1);

  return obj_id;
}

auto MeshRender::add_vectors(const std::vector<double> &coords,
                             const std::vector<double> &directions) -> int {
  // Draws a set of vectors or a single vector
  return add_vectors(coords, directions, DEFAULT_COLOR);
}

static auto get_gost_point(const double *point0,
                           const double *point1) -> std::vector<double> {
  // Create a point just before point0, almost aligned with point 0 and 1.
  // this function exist for open curves since we need adjacency.

  auto tangent = Linalg::vect_sub(point0, point1);
  tangent = Linalg::normalize(tangent);
  auto normal = Linalg::normal(tangent.data());
  normal = Linalg::vect_scal_mult(normal.data(), 0.01);
  tangent = Linalg::vect_add(tangent.data(), normal.data());
  tangent = Linalg::vect_scal_mult(tangent.data(), 0.1);
  return Linalg::vect_add(point0, tangent.data());
}

auto curvetype_to_objecttype(CurveType type) {
  switch (type) {
  case CurveType::QUAD_CURVE:
    return ObjectType::QUAD_CURVE;
    break;
  case CurveType::TUBE_CURVE:
    return ObjectType::TUBE_CURVE;
    break;

  case CurveType::SMOOTH_TUBE_CURVE:
    return ObjectType::SMOOTH_TUBE_CURVE;
    break;
  default:
    return ObjectType::NONE;
  }
}

auto MeshRender::add_curve(const std::vector<double> &coords,
                           const std::vector<double> &colors, CurveType type,
                           double width) -> int {
  // Adds a curve object, generate gost points at the extremities.

  ObjectType obtype = curvetype_to_objecttype(type);

  std::vector<double> coords_clean(coords.size() + 6);
  std::copy(coords.begin(), coords.end(), coords_clean.begin() + 3);

  auto gost_point = get_gost_point(coords.data(), &coords[3]);
  std::copy(gost_point.begin(), gost_point.end(), coords_clean.begin());

  gost_point =
      get_gost_point(&coords[coords.size() - 3], &coords[coords.size() - 6]);
  std::copy(gost_point.begin(), gost_point.end(), coords_clean.end() - 3);

  std::vector<unsigned int> curve_indices(4 * (coords_clean.size() / 3 - 3));
  for (unsigned int i = 0; i < curve_indices.size() / 4; ++i) {
    curve_indices.at(i * 4) = i;
    curve_indices.at(i * 4 + 1) = i + 1;
    curve_indices.at(i * 4 + 2) = i + 2;
    curve_indices.at(i * 4 + 3) = i + 3;
  }

  int obj_id{0};
  std::vector<double> per_point_color(coords_clean.size());
  if (colors.size() == 3) {
    for (int i = 0; i < (int)coords_clean.size(); i += 3) {
      per_point_color.at(i) = colors[0];
      per_point_color.at(i + 1) = colors[1];
      per_point_color.at(i + 2) = colors[2];
    }
    obj_id = add_object(coords_clean, curve_indices, per_point_color, obtype);
  } else if (colors.size() == coords.size()) {
    std::copy(colors.begin(), colors.end(), per_point_color.begin() + 3);
    obj_id = add_object(coords_clean, curve_indices, per_point_color, obtype);
  } else {
    throw std::invalid_argument("Coords size and colors size don't match in " +
                                std::string(__func__) + "\n");
  }

  Object &obj = objects.at(obj_id);
  obj.vertices_per_primitive = 4; // for line adjacency
  glUniform1f(glGetUniformLocation(obj.shader_program, "r"), (float)width);

  return obj_id;
}

auto MeshRender::add_curves(const std::vector<double> &coords,
                            const std::vector<double> &colors,
                            const std::vector<unsigned int> &curves_indices,
                            CurveType type, double width) -> int {
  // Draws multiples curves. Indices are for example:
  // [0, 1, 2, 3, |  1, 2, 3, 4, | 2, 3, 4, 5] In the order they are feeded to
  // the GPU

  ObjectType obtype = curvetype_to_objecttype(type);

  int obj_id{0};
  if (colors.size() == 3) {
    std::vector<double> per_point_color(coords.size());
    for (int i = 0; i < (int)coords.size(); i += 3) {
      per_point_color.at(i) = colors[0];
      per_point_color.at(i + 1) = colors[1];
      per_point_color.at(i + 1) = colors[2];
    }
    obj_id = add_object(coords, curves_indices, per_point_color, obtype);
  } else if (colors.size() == coords.size()) {
    obj_id = add_object(coords, curves_indices, colors, obtype);
  } else {
    throw std::invalid_argument("Coords size and colors size don't match in " +
                                std::string(__func__) + "\n");
  }

  Object &obj = objects.at(obj_id);
  glUniform1f(glGetUniformLocation(obj.shader_program, "r"), (float)width);

  return obj_id;
}

void MeshRender::update_vertex_colors(std::vector<double> &colors,
                                      unsigned int object_idx) {

  size_t n_vertice_attr = vertices_stride();

  Object &obj = objects.at(object_idx);

  if ((int)colors.size() / 3 != obj.n_vertices()) {
    throw std::invalid_argument(
        "Vertices size and colors size don't match in " +
        std::string(__func__) + "\n");
  }

  for (long int i = 0; i < obj.n_vertices(); ++i) {
    for (long int j = 0; j < 3; ++j) {
      // copies the precedent attributes
      vertices_attr.at(obj.attr_offset + i * n_vertice_attr + n_vertice_attr -
                       3 + j) = (float)colors.at(i * 3 + j);
    }
  }

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // TODO do not reload all data
  glBufferData(GL_ARRAY_BUFFER,
               (long)vertices_attr.size() * (long)sizeof(float),
               vertices_attr.data(), GL_STATIC_DRAW);
}

void set_image2D(unsigned int unit, unsigned int *imageID, unsigned int width,
                 unsigned int height, uint16_t *img_data) {

  glGenTextures(1, imageID);
  glBindTexture(GL_TEXTURE_2D, *imageID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R16UI, width, height, 0, GL_RED_INTEGER,
               GL_UNSIGNED_SHORT, img_data);
  glBindImageTexture(unit, *imageID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R16UI);
  glCheckError();
}

auto MeshRender::vertices_stride() -> long int {
  return std::reduce(VERT_ATTR_LENGTHS.begin(), VERT_ATTR_LENGTHS.end());
}

auto MeshRender::vertices_attr_offset(VertexAttr attr) -> long int {
  switch (attr) {
  case VertexAttr::VERTEX:
    return 0;
    break;
  case VertexAttr::NORMAL:
    return VERT_ATTR_LENGTHS.at(0);
    break;
  case VertexAttr::COLOR:
    return VERT_ATTR_LENGTHS.at(0); // + VERT_ATTR_LENGTHS.at(1);
    break;
  default:
    return -1;
  }
};

auto MeshRender::vertices_attr_size(VertexAttr attr) -> long int {
  switch (attr) {
  case VertexAttr::VERTEX:
    return VERT_ATTR_LENGTHS.at(0);
    break;
  case VertexAttr::NORMAL:
    return VERT_ATTR_LENGTHS.at(1);
    break;
  case VertexAttr::COLOR:
    return VERT_ATTR_LENGTHS.at(1); // + VERT_ATTR_LENGTHS.at(1);
    break;
  default:
    return -1;
  }
};

auto MeshRender::n_vertices() -> long int {
  return (long)vertices_attr.size() / vertices_stride();
}

void cursor_callback(GLFWwindow *window, double xpos, double ypos) {
  static double x_old{0};
  static double y_old{0};
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    auto *render = (MeshRender *)glfwGetWindowUserPointer(window);
    double dx = xpos - x_old;
    double dy = ypos - y_old;

    double norm_dm = pow(dy * dy + dx * dx, 0.5);

    // quaternionic transform
    double sin_dm = sin(norm_dm * MOUSE_SENSITIVITY) / norm_dm;
    // std::cout << " sin_dm :" << sin_dm << std::endl;
    Quaternion q_new(cos(norm_dm * MOUSE_SENSITIVITY), -dy * sin_dm,
                     -dx * sin_dm, 0);

    render->q = q_new * render->q;
    render->q_inv = render->q_inv * q_new.inv();
  }

  x_old = xpos;
  y_old = ypos;
}

void scroll_callback(GLFWwindow *window, __attribute__((unused)) double xoffset,
                     double yoffset) {
  auto *rdr = (MeshRender *)glfwGetWindowUserPointer(window);
  rdr->zoom_level *= 1.0 + yoffset * SCROLL_SENSITIVITY;
}

void keyboard_callback(__attribute__((unused)) GLFWwindow *window, int key,
                       __attribute__((unused)) int scancode, int action,
                       __attribute__((unused)) int mods) {

  auto *rdr = (MeshRender *)glfwGetWindowUserPointer(window);
  if (action == GLFW_PRESS) {
    switch (key) {
    case GLFW_KEY_I:
      rdr->q *= 1.1;
      rdr->q_inv *= 1.1;
      break;
    case GLFW_KEY_O:
      rdr->q *= 0.9;
      rdr->q_inv *= 0.9;
      break;
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, 1);
      break;
    default:
      break;
    }
  }
}

auto glCheckError_(const char *file, int line) -> GLenum {
  GLenum errorCode = 0;
  while ((errorCode = glGetError()) != GL_NO_ERROR) {
    switch (errorCode) {
    case GL_INVALID_ENUM: {
      std::cout << " ERROR : INVALID_ENUM , file : " << file
                << ", line : " << line;
      break;
    }
    case GL_INVALID_VALUE: {
      std::cout << " ERROR : INVALID_VALUE , file : " << file
                << ", line : " << line;
      break;
    }
    case GL_INVALID_OPERATION: {
      std::cout << " ERROR : INVALID_OPERATION , file : " << file
                << ", line : " << line;
      break;
    }
    case GL_STACK_OVERFLOW: {
      std::cout << " ERROR : STACK_OVERFLOW , file : " << file
                << ", line : " << line;
      break;
    }
    case GL_STACK_UNDERFLOW: {
      std::cout << " ERROR : STACK_UNDERFLOW , file : " << file
                << ", line : " << line;
      break;
    }
    case GL_OUT_OF_MEMORY: {
      std::cout << " ERROR : OUT_OF_MEMORY , file : " << file
                << ", line : " << line;
      break;
    }
    case GL_INVALID_FRAMEBUFFER_OPERATION: {
      std::cout << " ERROR : INVALID_FRAMEBUFFER_OPERATION , file : " << file
                << ", line : " << line;
      break;
    }
    }
  }
  return errorCode;
}
