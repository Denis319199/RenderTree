#include "ShaderProgram.hpp"

ShaderProgram::ShaderProgram(const char *vertex_shader_path,
                             const char *fragment_shader_path) {
  CompileAndLinkShaders(vertex_shader_path, fragment_shader_path);
}

bool ShaderProgram::CompileAndLinkShaders(const char *vertex_shader_path,
                                          const char *fragment_shader_path) {
  if (!CompileShader(vertex_shader_path, GL_VERTEX_SHADER))
    return false;
  if (!CompileShader(fragment_shader_path, GL_FRAGMENT_SHADER))
    return false;
  if (!LinkShaders())
    return false;

  return true;
}

bool ShaderProgram::CompileShader(const char *shader_path, GLenum shader) {
  std::ifstream shader_file(shader_path);
  if (!shader_file.is_open())
    return false;

  shader_file.seekg(0, std::ios::end);
  std::size_t n_char = shader_file.tellg();
  shader_file.seekg(0, std::ios::beg);

  std::string shader_code;
  shader_code.resize(n_char + 1);

  shader_file.read(&shader_code[0], n_char);
  shader_code[n_char] = '\0';
  const char *shader_code_ptr = shader_code.c_str();

  unsigned int compiled_shader = glCreateShader(shader);
  glShaderSource(compiled_shader, 1, &shader_code_ptr, nullptr);
  glCompileShader(compiled_shader);

  int success;
  char info_log[512];
  glGetShaderiv(compiled_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(compiled_shader, 512, nullptr, info_log);
    switch (shader) { 
    case GL_VERTEX_SHADER:
        std::cout << "ERROR::VERTEX_SHADER::COMPILATION_FAILED\n";
        break;
    case GL_FRAGMENT_SHADER:
      std::cout << "ERROR::FRAGMENT_SHADER::COMPILATION_FAILED\n";
      break;
    }
    
    std::cout << info_log << '\n';
    return false;
  }

  if (shader == GL_VERTEX_SHADER)
    vertex_shader = compiled_shader;
  else
    fragment_shader = compiled_shader;

  shader_file.close();
  return true;
}

bool ShaderProgram::LinkShaders() {
  shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  int success;
  char info_log[512];
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader_program, 512, NULL, info_log);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_ERROR\n" << info_log << '\n';
    return false;
  }

  return true;
}

ShaderProgram::~ShaderProgram() {
  glDeleteProgram(shader_program);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}
