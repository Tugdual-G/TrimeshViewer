#ifndef COMPILE_SHADER_H_
#define COMPILE_SHADER_H_
#include "glad/include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <map>
#include <string>

enum class ShaderProgramType : int {
  FLAT_FACES,
  SMOOTH_FACES,
  VECTOR_INSTANCE,
  QUAD_CURVE,
  TUBE_CURVE,
  AXIS_CROSS,
};

// Contains the list of sources directory names for each shader type;
std::map<ShaderProgramType, std::string> const SHADER_DIR_MAP{
    {ShaderProgramType::AXIS_CROSS, "shaders/axis_cross/"},
    {ShaderProgramType::FLAT_FACES, "shaders/flat_faces/"},
    {ShaderProgramType::SMOOTH_FACES, "shaders/smooth_faces/"},
    {ShaderProgramType::VECTOR_INSTANCE, "shaders/vector_instance/"},
    {ShaderProgramType::QUAD_CURVE, "shaders/quad_curve/"},
    {ShaderProgramType::TUBE_CURVE, "shaders/tube_curve/"}};

auto create_program(ShaderProgramType type) -> int;

#endif // COMPILE_SHADER_H_
