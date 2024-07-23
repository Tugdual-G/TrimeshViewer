#include "compile_shader.hpp"
#include "glad/include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

auto compile_shader(std::string source_fname, GLenum type) -> int {

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
    glGetShaderInfoLog(v_shader, 999, nullptr, infoLog.data());
    std::cout << "ERROR: SHADER COMPILATION_FAILED in" << source_fname << "\n";
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
    glGetProgramInfoLog(shader_program, 999, nullptr, infoLog.data());
    std::cout << ("ERROR:: linking shader failed \n");
    std::cout << infoLog.data() << "\n";
  }
  for (auto &shader : shaders) {
    glDeleteShader(shader);
  }
  return shader_program;
}

auto create_program(ShaderProgramType type) -> int {

  // map to keep track of the currently available programs;
  static std::map<ShaderProgramType, int> compiled_shader_programs_map;

  // Checks if already compiled.
  if (compiled_shader_programs_map.find(type) !=
      compiled_shader_programs_map.end()) {
    return compiled_shader_programs_map.at(type);
  }

  std::vector<int> shader_id;

  shader_id.push_back(compile_shader(
      SHADER_DIR_MAP.at(type) + "vertex_shader.glsl", GL_VERTEX_SHADER));

  shader_id.push_back(compile_shader(
      SHADER_DIR_MAP.at(type) + "fragment_shader.glsl", GL_FRAGMENT_SHADER));

  switch (type) {
  case ShaderProgramType::TUBE_CURVE:
  case ShaderProgramType::QUAD_CURVE:
    shader_id.push_back(compile_shader(
        SHADER_DIR_MAP.at(type) + "geometry_shader.glsl", GL_GEOMETRY_SHADER));
    break;
  default:
    break;
  }

  compiled_shader_programs_map[type] = link_shaders(shader_id);

  return compiled_shader_programs_map[type];
}
