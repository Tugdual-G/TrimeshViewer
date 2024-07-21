#ifndef COMPILE_SHADER_H_
#define COMPILE_SHADER_H_
#include "glad/include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <vector>

auto compile_shader(const char *source_fname, GLenum type) -> int;
auto link_shaders(std::vector<int> &shaders) -> int;

#endif // COMPILE_SHADER_H_
