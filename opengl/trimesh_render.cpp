#include "trimesh_render.h"
extern "C" {
#include "compileShader.h"
#include "display_window.h"
}
#include "include/glad/glad.h" // glad should be included before glfw3
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdlib.h>

GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)

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

void MeshRender::init_render() {
  if (!faces) {
    std::cout << "Error, please provide faces \n";
    exit(1);
  }

  if (!vertices) {
    std::cout << "Error, please provide vertices \n";
    exit(1);
  }

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    exit(1);
  }

  // Compile shaders
  unsigned int vertexShader = compileVertexShader("shaders/vertex_shader.glsl");
  unsigned int fragmentShader =
      compileFragmentShader("shaders/fragment_shader.glsl");
  shader_program = linkShaders(vertexShader, fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  glUseProgram(shader_program);
  // maxval_loc = glGetUniformLocation(shader_program, "maxval");

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  keep_aspect_ratio(window, width, height);

  //// Define a list of points
  // float vertices[] = {
  //// points coord     // texture coords
  //-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // Point 1
  // 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, // Point 2
  // 1.0f,  1.0f,  0.0f, 1.0f, 1.0f, // Point 3
  //-1.0f, 1.0f,  0.0f, 0.0f, 1.0f, // Point 4
  //};
  //
  // unsigned int square_idx[] = {
  // 0, 1, 2, 2, 3, 0,
  //};

  unsigned int EBO;

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(double) * n_vertices * 3, vertices,
               GL_STATIC_DRAW);

  // Square EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * n_faces * 3,
               faces, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(double),
                        (void *)0);
  glEnableVertexAttribArray(0);

  //// Position attrib
  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void
  // *)0); glEnableVertexAttribArray(0);
  //
  // Texture attrib
  // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
  //(void *)(3 * sizeof(float)));
  // glEnableVertexAttribArray(1);

  glCheckError();
}

int MeshRender::render_loop(int (*data_update_function)(void *fargs),
                            void *fargs) {

  int flag = 1;
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glUseProgram(shader_program);
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
      glClear(GL_COLOR_BUFFER_BIT);
      // render container
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

void set_image2D(unsigned int unit, unsigned int *imageID, unsigned int width,
                 unsigned int height, uint16_t *img_data) {

  glGenTextures(1, imageID);
  glBindTexture(GL_TEXTURE_2D, *imageID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R16UI, width, height, 0, GL_RED_INTEGER,
               GL_UNSIGNED_SHORT, img_data);
  glBindImageTexture(unit, *imageID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R16UI);
  glCheckError();
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
