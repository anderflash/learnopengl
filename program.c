#include "program.h"
#include <stdio.h>
#include <stdlib.h>

static void
compile_program(GLuint* program_id, GrfShader *vertexShader, GrfShader *fragmentShader){
  *program_id = glCreateProgram();
  glAttachShader(*program_id, vertexShader->shader_id);
  glAttachShader(*program_id, fragmentShader->shader_id);
  glLinkProgram (*program_id);

  GLint success;
  GLchar infoLog[512];
  glGetProgramiv(*program_id, GL_LINK_STATUS, &success);
  if(!success)
  {
    glGetProgramInfoLog(*program_id, 512, NULL, infoLog);
    printf("Error compilation program: %s\n", infoLog);
  }
}

GrfProgram*
grf_program_new(){
  return malloc(sizeof(GrfProgram));
}

GrfProgram*
grf_program_new_from_files(char* vertexShaderFile, char* fragmentShaderFile){
  GrfProgram* program     = grf_program_new();
  program->vertexShader   = grf_shader_new_from_file(vertexShaderFile,GL_VERTEX_SHADER);
  program->fragmentShader = grf_shader_new_from_file(fragmentShaderFile,GL_FRAGMENT_SHADER);
  compile_program(&program->program_id,program->vertexShader, program->fragmentShader);
  return program;
}

GrfProgram*
grf_program_new_from_text(char* vertexShaderText, char* fragmentShaderText){
  GrfProgram* program     = grf_program_new();
  program->vertexShader   = grf_shader_new_from_string(vertexShaderText,GL_VERTEX_SHADER);
  program->fragmentShader = grf_shader_new_from_string(fragmentShaderText,GL_FRAGMENT_SHADER);
  compile_program(&program->program_id,program->vertexShader, program->fragmentShader);
  return program;
}

void
grf_program_free(GrfProgram** programV){
  GrfProgram* program = *programV;
  glDeleteShader(program->vertexShader->shader_id);
  glDeleteShader(program->fragmentShader->shader_id);
  glDeleteProgram(program->program_id);
  free(program);
  *programV = NULL;
}

void
grf_program_use(GrfProgram* program){
  glUseProgram(program->program_id);
}
