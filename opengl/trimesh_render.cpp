#include "trimesh_render.h"
extern "C" {
#include "compileShader.h"
#include "display_window.h"
}
#include "include/glad/glad.h" // glad should be included before glfw3
#include <GLFW/glfw3.h>
#include <iostream>
#include <numeric>
#include <stdlib.h>

GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)

#define SHADER_PATH "shaders/"
#define SMOOTH_SHADE_NAME SHADER_PATH "smooth_shading_"
#define FLAT_SHADE_NAME SHADER_PATH "flat_shading_"

void MeshRender::init_window() {

  // Init Window
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(width, height, "Budack", NULL, NULL);
  if (window == NULL) {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    exit(1);
  }
  glfwMakeContextCurrent(window);
  glfwSetWindowUserPointer(window, userpointer);
  if (keyboard_callback) {
    glfwSetKeyCallback(window, keyboard_callback);
  }
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
  double time = 0;
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glUseProgram(shader_program);
  unsigned int time_loc = glGetUniformLocation(shader_program, "time");
  glUniform1f(time_loc, 0.01);
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
      glUniform1f(time_loc, time);

      glBindVertexArray(VAO);

      glDrawElements(GL_TRIANGLES, n_faces * 3, GL_UNSIGNED_INT, 0);
      glfwSwapBuffers(window);
      glfwPollEvents();
      time += 0.001;
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

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  static double x_old{0}, y_old{0};
  double dx = xpos - x_old, dy = ypos - y_old;
  dx = dx > 50 ? 0 : dx;
  dy = dy > 50 ? 0 : dy;
  double axis[3] = {0};
  axis[0] = -dy;
  axis[1] = dx;
}

void keyboard_callback(__attribute__((unused)) GLFWwindow *window, int key,
                       __attribute__((unused)) int scancode, int action,
                       __attribute__((unused)) int mods) {

  // Render_object *rdr = glfwGetWindowUserPointer(window);
  if (action == GLFW_PRESS) {
    switch (key) {
    case GLFW_KEY_RIGHT:
      break;

    case GLFW_KEY_LEFT:
      break;

    case GLFW_KEY_DOWN:
      break;
    case GLFW_KEY_UP:
      break;
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
