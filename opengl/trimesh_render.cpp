#include "trimesh_render.hpp"
extern "C" {
#include "compileShader.h"
#include "display_window.h"
}
#include "include/glad/glad.h" // glad should be included before glfw3
#include "math.h"
#include "quatern_transform.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <numeric>
#include <stdlib.h>

GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)

#define SHADER_PATH "shaders/"
#define SMOOTH_SHADE_NAME SHADER_PATH "smooth_shading_"
#define FLAT_SHADE_NAME SHADER_PATH "flat_shading_"
#define MOUSE_SENSITIVITY 0.005

void keyboard_callback(__attribute__((unused)) GLFWwindow *window, int key,
                       __attribute__((unused)) int scancode, int action,
                       __attribute__((unused)) int mods);
void cursor_callback(GLFWwindow *window, double xpos, double ypos);
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
}

void MeshRender::set_shader_program() {
  // Compile shaders
  unsigned int vertexShader;
  unsigned int fragmentShader;
  switch (program_type) {
  case FLAT_FACES:
    vertexShader = compileVertexShader(FLAT_SHADE_NAME "vertex_shader.glsl");
    fragmentShader =
        compileFragmentShader(FLAT_SHADE_NAME "fragment_shader.glsl");
    break;
  case SMOOTH_FACES:
    vertexShader = compileVertexShader(SMOOTH_SHADE_NAME "vertex_shader.glsl");
    fragmentShader =
        compileFragmentShader(SMOOTH_SHADE_NAME "fragment_shader.glsl");
    break;
  }

  shader_program = linkShaders(vertexShader, fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  glUseProgram(shader_program);

  q_loc = glGetUniformLocation(shader_program, "q");
  q_inv_loc = glGetUniformLocation(shader_program, "q_inv");
}

void MeshRender::init_render() {
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    exit(1);
  }

  // Compile shaders
  set_shader_program();

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  keep_aspect_ratio(window, width, height);

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  resize_VAO();

  // Square EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * n_faces * 3,
               faces.data(), GL_STATIC_DRAW);

  glCheckError();
}

int MeshRender::render_loop(int (*data_update_function)(void *fargs),
                            void *fargs) {
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  int flag = 1;
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glUseProgram(shader_program);
  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  if (data_update_function != NULL) {
    while (!glfwWindowShouldClose(window) && flag) {
      keep_aspect_ratio(window, width, height);
      processInput(window);
      glClear(GL_COLOR_BUFFER_BIT);
      flag = data_update_function(fargs);
      // render container
      glBindVertexArray(VAO);
      glDrawElements(GL_TRIANGLES, n_faces * 3, GL_INT, 0);
      glfwSwapBuffers(window);
      glfwPollEvents();
    }

  } else {
    while (!glfwWindowShouldClose(window) && flag) {
      keep_aspect_ratio(window, width, height);
      processInput(window);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glUniform4f(q_loc, (float)q[0], (float)q[1], (float)q[2], (float)q[3]);
      glUniform4f(q_inv_loc, (float)q_inv[0], (float)q_inv[1], (float)q_inv[2],
                  (float)q_inv[3]);

      glBindVertexArray(VAO);

      glDrawElements(GL_TRIANGLES, n_faces * 3, GL_UNSIGNED_INT, 0);
      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  }
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

void MeshRender::resize_VAO() {
  unsigned int total_size_vertice_attr =
      std::reduce(vert_attr_sizes.begin(), vert_attr_sizes.end());
  unsigned int n_vertice_attr = vert_attr_sizes.size();
  long unsigned int offset{0 * sizeof(double)};

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, n_vertices * total_size_vertice_attr,
               vertices_attr.data(), GL_STATIC_DRAW);

  for (unsigned int i = 0; i < n_vertice_attr; ++i) {
    // TODO do not hardcode 3 !
    glVertexAttribPointer(i, 3, GL_DOUBLE, GL_FALSE, total_size_vertice_attr,
                          (void *)(offset));
    glEnableVertexAttribArray(i);
    offset += vert_attr_sizes[i];
  }
}

void MeshRender::add_vertex_colors(std::vector<double> &colors) {
  vert_attr_sizes.push_back(3 * sizeof(double));
  vert_attr_numbers.push_back(3);

  unsigned int n_vertice_attr =
      std::reduce(vert_attr_numbers.begin(), vert_attr_numbers.end());

  std::vector<double> vertexes_attr_tmp(vertices_attr);

  vertices_attr.resize(vertices_attr.size() + (n_vertices * 3),
                       -999); // -999 for debug

  for (unsigned int i = 0; i < n_vertices; ++i) {
    for (unsigned int j = 0; j < n_vertice_attr - 3; ++j) {
      // copies the precedent attributes
      vertices_attr.at(i * n_vertice_attr + j) =
          vertexes_attr_tmp.at(i * (n_vertice_attr - 3) + j);
    }
    for (unsigned int j = 0; j < 3; ++j) {
      // adds colors
      vertices_attr.at(i * n_vertice_attr + j + n_vertice_attr - 3) =
          colors.at(i * 3 + j);
    }
  }
  resize_VAO();
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

  MeshRender *render = (MeshRender *)glfwGetWindowUserPointer(window);
  static double x_old{0}, y_old{0};
  double dx = xpos - x_old, dy = ypos - y_old;
  x_old = xpos;
  y_old = ypos;

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

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
  // std::cout << "q : ";

  // render->q.print_quaternion();
  // std::cout << std::endl;
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
