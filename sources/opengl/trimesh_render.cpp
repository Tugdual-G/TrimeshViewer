#include "trimesh_render.hpp"
extern "C" {
#include "compileShader.h"
#include "display_window.h"
}
#include "glad/include/glad/glad.h" // glad should be included before glfw3
#include "math.h"
#include "quatern_transform.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <numeric>
#include <stdlib.h>

GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)

#define SHADER_PATH "shaders/"
#define SMOOTH_SHADE_NAME "smooth_shading_"
#define FLAT_SHADE_NAME "flat_shading_"
#define AXIS_CROSS_NAME "axis_cross_"
#define MOUSE_SENSITIVITY 0.005
#define SCROLL_SENSITIVITY 0.05

void keyboard_callback(GLFWwindow *window, int key, int scancode, int action,
                       int mods);
void cursor_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void MeshRender::init_window() {
  // Init Window
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(width, height, "MeshRender", NULL, NULL);
  if (window == NULL) {
    printf("Error, failed to create GLFW window\n");
    glfwTerminate();
    exit(1);
  }
  glfwMakeContextCurrent(window);
  glfwSetWindowUserPointer(window, userpointer);

  glfwSetKeyCallback(window, keyboard_callback);
  glfwSetCursorPosCallback(window, cursor_callback);
  glfwSetScrollCallback(window, scroll_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    exit(1);
  }
}

void MeshRender::Object::set_shader_program() {
  // Compile shaders
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
}

void MeshRender::init_render() {

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  keep_aspect_ratio(window, width, height);

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

int MeshRender::render_loop(int (*data_update_function)(void *fargs),
                            void *fargs) {
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  int flag = 1;
  glClearColor(0.0, 0.0, 0.0, 0.0);
  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  // if (data_update_function != NULL) {
  //   while (!glfwWindowShouldClose(window) && flag) {
  //     keep_aspect_ratio(window, width, height);
  //     processInput(window);
  //     glClear(GL_COLOR_BUFFER_BIT);
  //     flag = data_update_function(fargs);
  //     // render container
  //     glBindVertexArray(VAO);
  //     glDrawElements(GL_TRIANGLES, n_faces * 3, GL_INT, 0);
  //     glfwSwapBuffers(window);
  //     glfwPollEvents();
  //   }

  // } else {
  while (!glfwWindowShouldClose(window) && flag) {

    glBindVertexArray(VAO);

    keep_aspect_ratio(window, width, height);
    processInput(window);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // objects.at(0).draw(); // axis cross

    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for (auto &obj : objects) {
      obj.draw();
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  //}
  glCheckError();
  return 0;
}

int MeshRender::render_finalize() {
  // Cleanup
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glfwTerminate();
  return 0;
}

int MeshRender::add_object(std::vector<double> &ivertices,
                           std::vector<unsigned int> ifaces,
                           ShaderProgramType shader_type) {

  Object new_mesh(
      q, q_inv, zoom_level,
      std::reduce(vert_attr_numbers.begin(), vert_attr_numbers.end()));
  new_mesh.attr_offset = vertices_attr.size();
  new_mesh.attr_length = ivertices.size() * 2;
  new_mesh.faces_indices_offset = faces.size();
  new_mesh.faces_indices_length = ifaces.size();
  new_mesh.n_faces = ifaces.size() / 3;
  new_mesh.n_vertices = ivertices.size() / 3;
  new_mesh.program_type = shader_type;

  vertices_attr.resize(vertices_attr.size() + ivertices.size() * 2);
  faces.resize(faces.size() + ifaces.size());

  std::copy(ifaces.begin(), ifaces.end(), faces.begin() + n_total_faces * 3);
  n_total_faces += ifaces.size() / 3; // sturdier if different number of attr

  for (unsigned int i = 0; i < ivertices.size() / 3; ++i) {
    for (unsigned int j = 0; j < 3; ++j) {
      vertices_attr.at((n_total_vertices + i) * 6 + j) =
          ivertices.at(i * 3 + j);
    }
    vertices_attr.at((n_total_vertices + i) * 6 + 4) = 0.7; // intial colors
    vertices_attr.at((n_total_vertices + i) * 6 + 5) = 0.8;
  }
  n_total_vertices += ivertices.size() / 3;
  new_mesh.set_shader_program();
  objects.push_back(new_mesh);
  resize_VAO();
  resize_EBO();
  return objects.size() - 1;
}

int MeshRender::add_object(std::vector<double> &ivertices,
                           std::vector<unsigned int> ifaces,
                           std::vector<double> colors,
                           ShaderProgramType shader_type) {

  Object new_mesh(
      q, q_inv, zoom_level,
      std::reduce(vert_attr_numbers.begin(), vert_attr_numbers.end()));
  new_mesh.attr_offset = vertices_attr.size();
  new_mesh.attr_length = ivertices.size() * 2;
  new_mesh.faces_indices_offset = faces.size();
  new_mesh.faces_indices_length = ifaces.size();
  new_mesh.n_faces = ifaces.size() / 3;
  new_mesh.n_vertices = ivertices.size() / 3;
  new_mesh.program_type = shader_type;

  vertices_attr.resize(vertices_attr.size() + ivertices.size() * 2);
  faces.resize(faces.size() + ifaces.size());

  std::copy(ifaces.begin(), ifaces.end(), faces.begin() + n_total_faces * 3);
  n_total_faces += ifaces.size() / 3; // sturdier if different number of attr

  for (unsigned int i = 0; i < ivertices.size() / 3; ++i) {
    for (unsigned int j = 0; j < 3; ++j) {
      vertices_attr.at((n_total_vertices + i) * 6 + j) =
          ivertices.at(i * 3 + j);
      vertices_attr.at((n_total_vertices + i) * 6 + 3 + j) =
          colors.at(i * 3 + j); // intial colors
    }
  }
  n_total_vertices += ivertices.size() / 3;
  new_mesh.set_shader_program();
  objects.push_back(new_mesh);
  resize_VAO();
  resize_EBO();
  return objects.size() - 1;
}

void MeshRender::resize_VAO() {
  // Resize the VAO and update vertex attributes data
  unsigned int total_size_vertice_attr =
      std::reduce(vert_attr_sizes.begin(), vert_attr_sizes.end());
  unsigned int n_vertice_attr = vert_attr_sizes.size();
  long unsigned int offset{0 * sizeof(double)};

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, n_total_vertices * total_size_vertice_attr,
               vertices_attr.data(), GL_STATIC_DRAW);

  for (unsigned int i = 0; i < n_vertice_attr; ++i) {
    // TODO do not hardcode 3 !
    glVertexAttribPointer(i, 3, GL_DOUBLE, GL_FALSE, total_size_vertice_attr,
                          (void *)(offset));
    glEnableVertexAttribArray(i);
    offset += vert_attr_sizes[i];
  }
}

void MeshRender::update_vertex_colors(std::vector<double> &colors,
                                      unsigned int object_idx) {
  // TODO vertices attr numbers might vary
  unsigned int n_vertice_attr =
      std::reduce(vert_attr_numbers.begin(), vert_attr_numbers.end());
  Object &obj = objects.at(object_idx);
  for (unsigned int i = 0; i < obj.n_vertices; ++i) {
    for (unsigned int j = 0; j < 3; ++j) {
      // copies the precedent attributes
      vertices_attr.at(obj.attr_offset + i * n_vertice_attr + n_vertice_attr -
                       3 + j) = colors.at(i * 3 + j);
    }
  }

  unsigned int total_size_vertice_attr =
      std::reduce(vert_attr_sizes.begin(), vert_attr_sizes.end());
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // TODO do not reload all data
  glBufferData(GL_ARRAY_BUFFER, total_size_vertice_attr * n_total_vertices,
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
  static double x_old{0}, y_old{0};
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    MeshRender *render = (MeshRender *)glfwGetWindowUserPointer(window);
    double dx = xpos - x_old, dy = ypos - y_old;
    dx = dx > 15 ? 0.1 : dx;
    dy = dy > 15 ? 0.1 : dy;
    // std::cout << dx << " , " << dy << std::endl;
    double norm_dm = pow(pow(dy, 2) + pow(dx, 2), 0.5);

    // quaternionic transform
    double sin_dm = sin(norm_dm * MOUSE_SENSITIVITY) / norm_dm;
    // std::cout << " sin_dm :" << sin_dm << std::endl;
    Quaternion q_new(cos(norm_dm * MOUSE_SENSITIVITY), dy * sin_dm, dx * sin_dm,
                     0);

    render->q = q_new * render->q;
    render->q_inv = render->q_inv * q_new.inv();
  }

  x_old = xpos;
  y_old = ypos;
}

void scroll_callback(GLFWwindow *window, __attribute__((unused)) double xoffset,
                     double yoffset) {
  MeshRender *rdr = (MeshRender *)glfwGetWindowUserPointer(window);
  rdr->zoom_level *= 1.0 + yoffset * SCROLL_SENSITIVITY;
}

void keyboard_callback(__attribute__((unused)) GLFWwindow *window, int key,
                       __attribute__((unused)) int scancode, int action,
                       __attribute__((unused)) int mods) {

  MeshRender *rdr = (MeshRender *)glfwGetWindowUserPointer(window);
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
      // case GLFW_KEY_RIGHT:
      //   break;

      // case GLFW_KEY_LEFT:
      //   break;

      // case GLFW_KEY_DOWN:
      //   break;
      // case GLFW_KEY_UP:
      //   break;
    }
  }
}

GLenum glCheckError_(const char *file, int line) {
  GLenum errorCode;
  while ((errorCode = glGetError()) != GL_NO_ERROR) {
    switch (errorCode) {
    case GL_INVALID_ENUM: {
      printf(" ERROR : INVALID_ENUM , file: %s  ,line: %i \n", file, line);
      break;
    }
    case GL_INVALID_VALUE: {
      printf(" ERROR : INVALID_VALUE , file: %s  ,line: %i \n", file, line);
      break;
    }
    case GL_INVALID_OPERATION: {
      printf(" ERROR : INVALID_OPERATION , file: %s  ,line: %i \n", file, line);
      break;
    }
    case GL_STACK_OVERFLOW: {
      printf(" ERROR : STACK_OVERFLOW , file: %s  ,line: %i \n", file, line);
      break;
    }
    case GL_STACK_UNDERFLOW: {
      printf(" ERROR : STACK_UNDERFLOW , file: %s ,line: %i \n", file, line);
      break;
    }
    case GL_OUT_OF_MEMORY: {
      printf(" ERROR : OUT_OF_MEMORY , file: %s ,line: %i \n", file, line);
      break;
    }
    case GL_INVALID_FRAMEBUFFER_OPERATION: {
      printf(" ERROR : INVALID_FRAMEBUFFER_OPERATION , file: %s ,line: %i \n",
             file, line);
      break;
    }
    }
  }
  return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)
