#pragma once

#include <fstream>
#include <iostream>
#include <sstream>

#include "PDH_GLFW_OpenGL.hpp"

class ShaderProgram {
public:
  ShaderProgram(const char *vertex_shader_path,
                const char *fragment_shader_path);

  bool CompileAndLinkShaders(const char *vertex_shader_path,
                             const char *fragment_shader_path);

  bool CompileShader(const char *shader_path, GLenum shader);

  bool LinkShaders();

  ~ShaderProgram();

  unsigned int vertex_shader;
  unsigned int fragment_shader;
  unsigned int shader_program;
};
