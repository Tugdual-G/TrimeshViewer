#include "compile_shader.hpp"
#include "glad/include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

auto compile_shader(const char *source_fname, GLenum type) -> int {

  std::ifstream file;
  file.open(source_fname, std::ios::binary | std::ios::in);
  if (file.fail()) {
    std::cout << "Error, can't open : " << source_fname << "\n";
    exit(1);
  }
  std::ostringstream contents;
  contents << file.rdbuf();
  file.close();

  int success{0};
  int v_shader{0};

  const std::string source = contents.str();
  const char *s = source.c_str();

  // Compile shaders
  v_shader = glCreateShader(type);
  glShaderSource(v_shader, 1, &s, nullptr);
  glCompileShader(v_shader);
  glGetShaderiv(v_shader, GL_COMPILE_STATUS, &success);

  std::vector<char> infoLog(1000, '\0');
  if (!success) {
    glGetShaderInfoLog(v_shader, 512, nullptr, infoLog.data());
    std::cout << "ERROR: SHADER COMPILATION_FAILED\n";
    std::cout << infoLog.data() << "\n";
  }
  return v_shader;
}

auto link_shaders(std::vector<int> &shaders) -> int {
  int shader_program{0};
  int success{0};

  std::vector<char> infoLog(1000, '\0');
  shader_program = glCreateProgram();
  for (auto &shader : shaders) {
    glAttachShader(shader_program, shader);
  }

  glLinkProgram(shader_program);
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader_program, 512, nullptr, infoLog.data());
    std::cout << ("ERROR:: linking shader failed \n");
    std::cout << infoLog.data() << "\n";
  }
  for (auto &shader : shaders) {
    glDeleteShader(shader);
  }
  return shader_program;
}
