#include "trimesh_render.hpp"
#include "compile_shader.hpp"
#include "glad/include/glad/glad.h" // glad should be included before glfw3
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

void framebuffer_size_callback(__attribute__((unused)) GLFWwindow *window,
                               int width, int height) {
  glViewport(0, 0, width, height);
}

void MeshRender::init_render() {

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glCheckError();
}

void MeshRender::resize_EBO() {
  // Square EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               (long)sizeof(unsigned int) * n_total_indices, faces.data(),
               GL_STATIC_DRAW);

  glCheckError();
}

auto MeshRender::render_loop(int (*data_update_function)(void *fargs),
                             void *fargs) -> int {
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  int flag = 1;
  glClearColor(0.0, 0.0, 0.0, 0.0);

  glEnable(GL_DEPTH_TEST);
  // Accept fragment if closer to the camera
  glDepthFunc(GL_LESS);

  if (data_update_function == nullptr) {
    while ((glfwWindowShouldClose(window) == 0)) {
      glfwGetWindowSize(window, &width, &height);
      glBindVertexArray(VAO);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      for (auto &obj : objects) {
        draw(obj);
      }
      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  } else {
    while ((glfwWindowShouldClose(window) == 0) && (flag != 0)) {
      glfwGetWindowSize(window, &width, &height);
      glBindVertexArray(VAO);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

auto MeshRender::render_finalize() -> int {
  // Cleanup
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glfwTerminate();
  return 0;
}

void MeshRender::add_indices(const std::vector<unsigned int> &new_indices) {
  faces.resize(faces.size() + new_indices.size());
  std::copy(new_indices.begin(), new_indices.end(),
            faces.begin() + (long)n_total_indices);
  n_total_indices +=
      (long)new_indices.size(); // sturdier if different number of attr
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

  n_total_indices += (long)new_indices.size() - obj.faces_indices_length;
  obj.faces_indices_length = (long)new_indices.size();
  resize_EBO();
}

void MeshRender::add_vertices(const std::vector<double> &new_vertices) {
  vertices_attr.resize(vertices_attr.size() + new_vertices.size() * 2);

  for (unsigned int i = 0; i < new_vertices.size() / 3; ++i) {
    for (unsigned int j = 0; j < 3; ++j) {
      vertices_attr.at((n_total_vertices + i) * 6 + j) =
          (float)new_vertices.at(i * 3 + j);
    }
    vertices_attr.at((n_total_vertices + i) * 6 + 4) = 0.7; // default colors
    vertices_attr.at((n_total_vertices + i) * 6 + 5) = 0.8;
  }
  n_total_vertices += (long)new_vertices.size() / 3;
  resize_VAO();
}

void MeshRender::add_vertices(const std::vector<double> &new_vertices,
                              const std::vector<double> &colors) {

  vertices_attr.resize(vertices_attr.size() + new_vertices.size() * 2);

  for (unsigned int i = 0; i < new_vertices.size() / 3; ++i) {
    for (unsigned int j = 0; j < 3; ++j) {
      vertices_attr.at((n_total_vertices + i) * 6 + j) =
          (float)new_vertices.at(i * 3 + j);
      vertices_attr.at((n_total_vertices + i) * 6 + 3 + j) =
          (float)colors.at(i * 3 + j);
    }
  }
  n_total_vertices += (long)new_vertices.size() / 3;
  resize_VAO();
}

void MeshRender::update_vertices(const std::vector<double> &new_vertices,
                                 Object &obj) {
  // update vertices, can change the number of vertices.
  std::vector<float> attr_tmp(vertices_attr);
  long int new_attr_length =
      obj.total_number_attr * (long)new_vertices.size() / 3;
  vertices_attr.resize(vertices_attr.size() +
                       (new_attr_length - obj.attr_length));

  for (unsigned int i = 0; i < new_vertices.size() / 3; ++i) {
    for (unsigned int j = 0; j < 3; ++j) {
      vertices_attr.at(obj.attr_offset + i * obj.total_number_attr + j) =
          (float)new_vertices.at(i * 3 + j);
    }
    // default color
    vertices_attr.at(obj.attr_offset + i * obj.total_number_attr + 4) = 0.7;
    vertices_attr.at(obj.attr_offset + i * obj.total_number_attr + 5) = 0.8;
  }
  std::copy(attr_tmp.begin() + (long)(obj.attr_offset + obj.attr_length),
            attr_tmp.end(),
            vertices_attr.begin() + (long)(obj.attr_offset + new_attr_length));

  n_total_vertices += (long)new_vertices.size() / 3 - obj.n_vertices();
  obj.attr_length = new_attr_length;

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, (long)(vertices_attr.size() * sizeof(float)),
               vertices_attr.data(), GL_STATIC_DRAW);
}

void MeshRender::update_vertices(const std::vector<double> &new_vertices,
                                 const std::vector<double> &colors,
                                 Object &obj) {
  // update vertices, can change the number of vertices.
  std::vector<float> attr_tmp(vertices_attr);
  long int new_attr_length =
      obj.total_number_attr * (long)new_vertices.size() / 3;
  vertices_attr.resize(vertices_attr.size() +
                       (new_attr_length - obj.attr_length));

  for (unsigned int i = 0; i < new_vertices.size() / 3; ++i) {
    for (unsigned int j = 0; j < 3; ++j) {
      vertices_attr.at(obj.attr_offset + i * obj.total_number_attr + j) =
          (float)new_vertices.at(i * 3 + j);
    }
    for (unsigned int j = 0; j < 3; ++j) {
      vertices_attr.at(obj.attr_offset + i * obj.total_number_attr + j + 3) =
          (float)colors.at(i * 3 + j);
    }
  }
  std::copy(attr_tmp.begin() + (long)(obj.attr_offset + obj.attr_length),
            attr_tmp.end(),
            vertices_attr.begin() + (long)(obj.attr_offset + new_attr_length));

  n_total_vertices += (long)new_vertices.size() / 3 - obj.n_vertices();
  obj.attr_length = new_attr_length;

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, (long)(vertices_attr.size() * sizeof(float)),
               vertices_attr.data(), GL_STATIC_DRAW);
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
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * obj.attr_offset,
                  sizeof(float) * obj.attr_length,
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

  update_vertices(ivertices, obj);

  glCheckError();
}

void MeshRender::update_object(const std::vector<double> &ivertices,
                               const std::vector<unsigned int> &ifaces,
                               const std::vector<double> &icolors, int id) {
  /* Update the vertices and faces of an object. */
  Object &obj = objects.at(id);

  // updates faces
  update_indices(ifaces, obj);

  update_vertices(ivertices, icolors, obj);

  glCheckError();
}

auto MeshRender::add_object(const std::vector<double> &ivertices,
                            const std::vector<unsigned int> &ifaces,
                            ObjectType object_type) -> int {

  Object new_mesh(object_type);
  new_mesh.total_number_attr =
      std::reduce(vert_attr_group_length.begin(), vert_attr_group_length.end());
  new_mesh.attr_offset = (long)vertices_attr.size();
  new_mesh.attr_length = (long)ivertices.size() * 2;
  new_mesh.faces_indices_offset = (long)faces.size();
  new_mesh.faces_indices_length = (long)ifaces.size();

  add_indices(ifaces);
  add_vertices(ivertices);

  new_mesh.set_shader_program();
  objects.push_back(new_mesh);
  return (int)objects.size() - 1;
}

auto MeshRender::add_object(const std::vector<double> &ivertices,
                            const std::vector<unsigned int> &ifaces,
                            const std::vector<double> &colors,
                            ObjectType object_type) -> int {

  Object new_mesh(object_type);
  new_mesh.total_number_attr =
      std::reduce(vert_attr_group_length.begin(), vert_attr_group_length.end());

  new_mesh.attr_offset = (long)vertices_attr.size();
  new_mesh.attr_length = (long)ivertices.size() * 2;
  new_mesh.faces_indices_offset = (long)faces.size();
  new_mesh.faces_indices_length = (long)ifaces.size();

  add_indices(ifaces);
  add_vertices(ivertices, colors);

  new_mesh.set_shader_program();
  objects.push_back(new_mesh);
  return (int)objects.size() - 1;
}

auto MeshRender::add_object(const std::vector<double> &ivertices,
                            const std::vector<unsigned int> &ifaces,
                            const std::vector<double> &colors) -> int {

  Object new_mesh(ObjectType::MESH);
  new_mesh.total_number_attr =
      std::reduce(vert_attr_group_length.begin(), vert_attr_group_length.end());

  new_mesh.attr_offset = (long)vertices_attr.size();
  new_mesh.attr_length = (long)ivertices.size() * 2;
  new_mesh.faces_indices_offset = (long)faces.size();
  new_mesh.faces_indices_length = (long)ifaces.size();

  add_indices(ifaces);
  add_vertices(ivertices, colors);

  new_mesh.set_shader_program();
  objects.push_back(new_mesh);
  return (int)objects.size() - 1;
}

auto MeshRender::add_object(const std::vector<double> &ivertices,
                            const std::vector<unsigned int> &ifaces) -> int {

  Object new_mesh(ObjectType::MESH);
  new_mesh.total_number_attr =
      std::reduce(vert_attr_group_length.begin(), vert_attr_group_length.end());

  new_mesh.attr_offset = (long)vertices_attr.size();
  new_mesh.attr_length = (long)ivertices.size() * 2;
  new_mesh.faces_indices_offset = (long)faces.size();
  new_mesh.faces_indices_length = (long)ifaces.size();

  add_indices(ifaces);
  add_vertices(ivertices);

  new_mesh.set_shader_program();
  objects.push_back(new_mesh);
  return (int)objects.size() - 1;
}

auto MeshRender::add_vectors(const std::vector<double> &coords,
                             const std::vector<double> &directions,
                             const std::vector<double> &colors) -> int {
  // Draws a set of colored vectors or a single colored vector
  int obj_id =
      add_object(VectorInstance::vector_instance_vertices,
                 VectorInstance::vector_instance_faces, ObjectType::VECTOR);

  Object &obj = objects.at(obj_id);
  obj.n_instances = (int)coords.size() / 3;

  std::vector<float> instances_attr(coords.size() * 3);
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

  unsigned int vector_VBO{0};
  glGenBuffers(1, &vector_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, vector_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * instances_attr.size(),
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

  int obj_id =
      add_object(VectorInstance::vector_instance_vertices,
                 VectorInstance::vector_instance_faces, ObjectType::VECTOR);

  Object &obj = objects.at(obj_id);
  obj.n_instances = (int)coords.size() / 3;

  std::vector<float> instances_attr(coords.size() * 3);
  for (int i = 0; i < (int)coords.size(); i += 3) {
    instances_attr.at(i * 3) = (float)coords.at(i);
    instances_attr.at(i * 3 + 1) = (float)coords.at(i + 1);
    instances_attr.at(i * 3 + 2) = (float)coords.at(i + 2);
    instances_attr.at(i * 3 + 3) = (float)directions.at(i);
    instances_attr.at(i * 3 + 4) = (float)directions.at(i + 1);
    instances_attr.at(i * 3 + 5) = (float)directions.at(i + 2);

    instances_attr.at(i * 3 + 7) = 0.8;
    instances_attr.at(i * 3 + 8) = 0.7;
  }

  unsigned int vector_VBO{0};
  glGenBuffers(1, &vector_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, vector_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * instances_attr.size(),
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

auto MeshRender::add_curve(const std::vector<double> &coords,
                           const std::vector<double> &tangents, CurveType type,
                           double width) -> int {
  // Draws a curve

  ObjectType obtype{0};
  switch (type) {
  case CurveType::QUAD_CURVE:
    obtype = ObjectType::QUAD_CURVE;
    break;
  case CurveType::TUBE_CURVE:
    obtype = ObjectType::TUBE_CURVE;
    break;
  default:
    throw;
    break;
  }

  std::vector<unsigned int> curve_indices(4 * (coords.size() / 3 - 3));
  for (unsigned int i = 1; i < curve_indices.size() / 4; ++i) {
    curve_indices.at(i * 4) = i;
    curve_indices.at(i * 4 + 1) = i + 1;
    curve_indices.at(i * 4 + 2) = i + 2;
    curve_indices.at(i * 4 + 3) = i + 3;
  }
  int obj_id = add_object(coords, curve_indices, tangents, obtype);

  Object &obj = objects.at(obj_id);
  obj.vertices_per_face = 4;
  glUniform1f(glGetUniformLocation(obj.shader_program, "r"), (float)width);

  return obj_id;
}

auto MeshRender::add_curves(const std::vector<double> &coords,
                            const std::vector<double> &tangents,
                            const std::vector<unsigned int> &curves_indices,
                            CurveType type, double width) -> int {
  // Draws multiples curves.
  ObjectType obtype{0};
  switch (type) {
  case CurveType::QUAD_CURVE:
    obtype = ObjectType::QUAD_CURVE;
    break;
  case CurveType::TUBE_CURVE:
    obtype = ObjectType::TUBE_CURVE;
    break;
  default:
    throw;
    break;
  }

  int obj_id = add_object(coords, curves_indices, tangents, obtype);

  Object &obj = objects.at(obj_id);
  obj.vertices_per_face = 4;
  glUniform1f(glGetUniformLocation(obj.shader_program, "r"), (float)width);

  return obj_id;
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

void MeshRender::resize_VAO() {
  // Resize the VAO and update vertex attributes data
  size_t total_size_vertice_attr =
      sizeof(float) *
      std::reduce(vert_attr_group_length.begin(), vert_attr_group_length.end());
  size_t n_vertice_attr = vert_attr_group_length.size();
  size_t offset{0 * sizeof(float)};

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, n_total_vertices * total_size_vertice_attr,
               vertices_attr.data(), GL_STATIC_DRAW);

  for (size_t i = 0; i < n_vertice_attr; ++i) {
    // TODO do not hardcode 3 !
    glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, total_size_vertice_attr,
                          (void *)(offset));
    glEnableVertexAttribArray(i);
    offset += vert_attr_group_length[i] * sizeof(float);
  }
}

void MeshRender::update_vertex_colors(std::vector<double> &colors,
                                      unsigned int object_idx) {
  // TODO vertices attr numbers might vary
  size_t n_vertice_attr =
      std::reduce(vert_attr_group_length.begin(), vert_attr_group_length.end());
  Object &obj = objects.at(object_idx);
  for (long int i = 0; i < obj.n_vertices(); ++i) {
    for (long int j = 0; j < 3; ++j) {
      // copies the precedent attributes
      vertices_attr.at(obj.attr_offset + i * n_vertice_attr + n_vertice_attr -
                       3 + j) = (float)colors.at(i * 3 + j);
    }
  }

  size_t total_size_vertice_attr =
      sizeof(float) *
      std::reduce(vert_attr_group_length.begin(), vert_attr_group_length.end());
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // TODO do not reload all data
  glBufferData(GL_ARRAY_BUFFER,
               (long)total_size_vertice_attr * n_total_vertices,
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

void cursor_callback(GLFWwindow *window, double xpos, double ypos) {
  static double x_old{0};
  static double y_old{0};
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    auto *render = (MeshRender *)glfwGetWindowUserPointer(window);
    double dx = xpos - x_old;
    double dy = ypos - y_old;
    dx = dx > 15 ? 0.1 : dx;
    dy = dy > 15 ? 0.1 : dy;
    // std::cout << dx << " , " << dy << std::endl;
    double norm_dm = pow(pow(dy, 2) + pow(dx, 2), 0.5);

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
