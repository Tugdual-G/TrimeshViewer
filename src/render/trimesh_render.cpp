#include "trimesh_render.hpp"
extern "C" {
#include "compileShader.h"
}
#include "glad/include/glad/glad.h" // glad should be included before glfw3
#include "quatern_transform.hpp"
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <numeric>

auto glCheckError_(const char *file, int line) -> GLenum;
#define glCheckError() glCheckError_(__FILE__, __LINE__)

#define SHADER_PATH "shaders/"
#define SMOOTH_SHADE_NAME "smooth_shading_"
#define FLAT_SHADE_NAME "flat_shading_"
#define AXIS_CROSS_NAME "axis_cross_"

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
  unsigned int vertexShader{0};
  unsigned int fragmentShader{0};
  switch (program_type) {
  case ShaderProgramType::FLAT_FACES:
    vertexShader =
        compileVertexShader(SHADER_PATH FLAT_SHADE_NAME "vertex_shader.glsl");
    fragmentShader = compileFragmentShader(SHADER_PATH FLAT_SHADE_NAME
                                           "fragment_shader.glsl");
    break;
  case ShaderProgramType::SMOOTH_FACES:
    vertexShader =
        compileVertexShader(SHADER_PATH SMOOTH_SHADE_NAME "vertex_shader.glsl");
    fragmentShader = compileFragmentShader(SHADER_PATH SMOOTH_SHADE_NAME
                                           "fragment_shader.glsl");
    break;
  case ShaderProgramType::AXIS_CROSS_FLAT:
    vertexShader = compileVertexShader(
        SHADER_PATH AXIS_CROSS_NAME FLAT_SHADE_NAME "vertex_shader.glsl");
    fragmentShader = compileFragmentShader(SHADER_PATH FLAT_SHADE_NAME
                                           "fragment_shader.glsl");
    break;

  case ShaderProgramType::AXIS_CROSS_SMOOTH:
    vertexShader = compileVertexShader(
        SHADER_PATH AXIS_CROSS_NAME SMOOTH_SHADE_NAME "vertex_shader.glsl");
    fragmentShader = compileFragmentShader(SHADER_PATH SMOOTH_SHADE_NAME
                                           "fragment_shader.glsl");
    break;
  }

  shader_program = linkShaders(vertexShader, fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

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
               sizeof(unsigned int) * n_total_faces * 3, faces.data(),
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

      flag = data_update_function(fargs);

      // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      for (auto &obj : objects) {
        draw(obj);
      }
      glfwSwapBuffers(window);
      glfwPollEvents();
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

void MeshRender::update_object(std::vector<double> &ivertices, int id) {
  /* Update the vertices positions of an object. */
  Object &obj = objects.at(id);

  for (unsigned int i = 0; i < obj.n_vertices; ++i) {
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

void MeshRender::update_object(std::vector<double> &ivertices,
                               std::vector<unsigned int> &ifaces, int id) {
  /* Update the vertices and faces of an object. */
  Object &obj = objects.at(id);

  // updates faces
  std::vector<unsigned int> faces_tmp(faces);

  faces.resize(faces.size() + (ifaces.size() - obj.faces_indices_length));

  std::copy(ifaces.begin(), ifaces.end(),
            faces.begin() + (long)obj.faces_indices_offset);

  std::copy(faces_tmp.begin() +
                (long)(obj.faces_indices_offset + obj.faces_indices_length),
            faces_tmp.end(),
            faces.begin() + (long)(obj.faces_indices_offset + ifaces.size()));

  // update vertices
  std::vector<float> attr_tmp(vertices_attr);
  long int new_attr_length = obj.total_number_attr * (long)ivertices.size() / 3;
  vertices_attr.resize(vertices_attr.size() +
                       (new_attr_length - obj.attr_length));

  for (unsigned int i = 0; i < ivertices.size() / 3; ++i) {
    for (unsigned int j = 0; j < 3; ++j) {
      vertices_attr.at(obj.attr_offset + i * obj.total_number_attr + j) =
          (float)ivertices.at(i * 3 + j);
    }
  }
  std::copy(attr_tmp.begin() + (long)(obj.attr_offset + obj.attr_length),
            attr_tmp.end(),
            vertices_attr.begin() + (long)(obj.attr_offset + new_attr_length));

  n_total_faces += (long)ifaces.size() / 3 - obj.n_faces;
  n_total_vertices += (long)ivertices.size() / 3 - obj.n_vertices;
  obj.attr_length = new_attr_length;
  obj.faces_indices_length = (long)ifaces.size();
  obj.n_faces = (long)ifaces.size() / 3;
  obj.n_vertices = (long)ivertices.size() / 3;
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertices_attr.size(),
                  vertices_attr.data());

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(unsigned int) * n_total_faces * 3, faces.data(),
               GL_STATIC_DRAW);

  glCheckError();
}

auto MeshRender::add_object(std::vector<double> &ivertices,
                            std::vector<unsigned int> &ifaces,
                            ShaderProgramType shader_type) -> int {

  Object new_mesh(std::reduce(vert_attr_group_length.begin(),
                              vert_attr_group_length.end()));

  new_mesh.attr_offset = vertices_attr.size();
  new_mesh.attr_length = ivertices.size() * 2;
  new_mesh.faces_indices_offset = faces.size();
  new_mesh.faces_indices_length = ifaces.size();
  new_mesh.n_faces = ifaces.size() / 3;
  new_mesh.n_vertices = ivertices.size() / 3;
  new_mesh.program_type = shader_type;

  vertices_attr.resize(vertices_attr.size() + ivertices.size() * 2);
  faces.resize(faces.size() + ifaces.size());

  std::copy(ifaces.begin(), ifaces.end(),
            faces.begin() + (long)n_total_faces * 3);
  n_total_faces += ifaces.size() / 3; // sturdier if different number of attr

  for (unsigned int i = 0; i < ivertices.size() / 3; ++i) {
    for (unsigned int j = 0; j < 3; ++j) {
      vertices_attr.at((n_total_vertices + i) * 6 + j) =
          (float)ivertices.at(i * 3 + j);
    }
    vertices_attr.at((n_total_vertices + i) * 6 + 4) = 0.7; // intial colors
    vertices_attr.at((n_total_vertices + i) * 6 + 5) = 0.8;
  }
  n_total_vertices += ivertices.size() / 3;
  new_mesh.set_shader_program();
  objects.push_back(new_mesh);
  resize_VAO();
  resize_EBO();
  return (int)objects.size() - 1;
}

auto MeshRender::add_object(std::vector<double> &ivertices,
                            std::vector<unsigned int> &ifaces,
                            std::vector<double> colors,
                            ShaderProgramType shader_type) -> int {

  Object new_mesh(std::reduce(vert_attr_group_length.begin(),
                              vert_attr_group_length.end()));

  new_mesh.attr_offset = vertices_attr.size();
  new_mesh.attr_length = ivertices.size() * 2;
  new_mesh.faces_indices_offset = faces.size();
  new_mesh.faces_indices_length = ifaces.size();
  new_mesh.n_faces = ifaces.size() / 3;
  new_mesh.n_vertices = ivertices.size() / 3;
  new_mesh.program_type = shader_type;

  vertices_attr.resize(vertices_attr.size() + ivertices.size() * 2);
  faces.resize(faces.size() + ifaces.size());

  std::copy(ifaces.begin(), ifaces.end(),
            faces.begin() + (long)n_total_faces * 3);
  n_total_faces += ifaces.size() / 3; // sturdier if different number of attr

  for (unsigned int i = 0; i < ivertices.size() / 3; ++i) {
    for (unsigned int j = 0; j < 3; ++j) {
      vertices_attr.at((n_total_vertices + i) * 6 + j) =
          (float)ivertices.at(i * 3 + j);
      vertices_attr.at((n_total_vertices + i) * 6 + 3 + j) =
          (float)colors.at(i * 3 + j); // intial colors
    }
  }
  n_total_vertices += ivertices.size() / 3;
  new_mesh.set_shader_program();
  objects.push_back(new_mesh);
  resize_VAO();
  resize_EBO();
  return (int)objects.size() - 1;
}

auto MeshRender::add_object(std::vector<double> &ivertices,
                            std::vector<unsigned int> &ifaces,
                            std::vector<double> &colors) -> int {

  Object new_mesh(std::reduce(vert_attr_group_length.begin(),
                              vert_attr_group_length.end()));

  new_mesh.attr_offset = vertices_attr.size();
  new_mesh.attr_length = ivertices.size() * 2;
  new_mesh.faces_indices_offset = faces.size();
  new_mesh.faces_indices_length = ifaces.size();
  new_mesh.n_faces = ifaces.size() / 3;
  new_mesh.n_vertices = ivertices.size() / 3;
  new_mesh.program_type = ShaderProgramType::FLAT_FACES;

  vertices_attr.resize(vertices_attr.size() + ivertices.size() * 2);
  faces.resize(faces.size() + ifaces.size());

  std::copy(ifaces.begin(), ifaces.end(),
            faces.begin() + (long)n_total_faces * 3);
  n_total_faces += ifaces.size() / 3; // sturdier if different number of attr

  for (unsigned int i = 0; i < ivertices.size() / 3; ++i) {
    for (unsigned int j = 0; j < 3; ++j) {
      vertices_attr.at((n_total_vertices + i) * 6 + j) =
          (float)ivertices.at(i * 3 + j);
      vertices_attr.at((n_total_vertices + i) * 6 + 3 + j) =
          (float)colors.at(i * 3 + j);
    }
  }
  n_total_vertices += ivertices.size() / 3;
  new_mesh.set_shader_program();
  objects.push_back(new_mesh);
  resize_VAO();
  resize_EBO();
  return (int)objects.size() - 1;
}

auto MeshRender::add_object(std::vector<double> &ivertices,
                            std::vector<unsigned int> &ifaces) -> int {

  Object new_mesh(std::reduce(vert_attr_group_length.begin(),
                              vert_attr_group_length.end()));

  new_mesh.attr_offset = vertices_attr.size();
  new_mesh.attr_length = ivertices.size() * 2;
  new_mesh.faces_indices_offset = faces.size();
  new_mesh.faces_indices_length = ifaces.size();
  new_mesh.n_faces = ifaces.size() / 3;
  new_mesh.n_vertices = ivertices.size() / 3;
  new_mesh.program_type = ShaderProgramType::FLAT_FACES;

  vertices_attr.resize(vertices_attr.size() + ivertices.size() * 2);
  faces.resize(faces.size() + ifaces.size());

  std::copy(ifaces.begin(), ifaces.end(),
            faces.begin() + (long)n_total_faces * 3);
  n_total_faces += ifaces.size() / 3; // sturdier if different number of attr

  for (unsigned int i = 0; i < ivertices.size() / 3; ++i) {
    for (unsigned int j = 0; j < 3; ++j) {
      vertices_attr.at((n_total_vertices + i) * 6 + j) =
          (float)ivertices.at(i * 3 + j);
    }
    vertices_attr.at((n_total_vertices + i) * 6 + 4) = 0.7; // intial colors
    vertices_attr.at((n_total_vertices + i) * 6 + 5) = 0.8;
  }
  n_total_vertices += ivertices.size() / 3;
  new_mesh.set_shader_program();
  objects.push_back(new_mesh);
  resize_VAO();
  resize_EBO();
  return (int)objects.size() - 1;
}

void MeshRender::draw(Object &obj) {
  glUseProgram(obj.shader_program);
  // Mouse rotation
  glUniform4f(obj.q_loc, (float)q[0], (float)q[1], (float)q[2], (float)q[3]);
  glUniform4f(obj.q_inv_loc, (float)q_inv[0], (float)q_inv[1], (float)q_inv[2],
              (float)q_inv[3]);
  // Zoom
  glUniform1f(obj.zoom_loc, zoom_level);

  glUniform2f(obj.viewport_size_loc, (float)width, (float)height);
  // glDrawElements(GL_TRIANGLES, faces_indices_length, GL_UNSIGNED_INT,
  //                (void *)(faces_indices_offset * sizeof(unsigned int)));
  glDrawElementsBaseVertex(
      GL_TRIANGLES, obj.faces_indices_length, GL_UNSIGNED_INT,
      (void *)(obj.faces_indices_offset * sizeof(unsigned int)),
      obj.attr_offset / obj.total_number_attr);
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
  for (long int i = 0; i < obj.n_vertices; ++i) {
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
