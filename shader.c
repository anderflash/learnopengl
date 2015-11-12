#include "shader.h"
#include <stdio.h>
#include <stdlib.h>

char*
read_file(const char* filename){
  // Open file
  long lSize;
  size_t result;
  FILE* fp = fopen(filename, "r");
  if(!fp) return NULL;
  // Get size
  fseek(fp,0,SEEK_END);
  lSize = ftell(fp);
  rewind(fp);
  // Alloc and read
  char* buffer = (char*)malloc(lSize+1);
  result = fread(buffer, 1, lSize, fp);
  if(result != lSize){
    free(buffer);
    return NULL;
  }
  buffer[lSize] = 0;
  // Return
  fclose(fp);
  return buffer;
}

static void
compile_shader(const char* shaderSource, GLuint* id, int shader_type){
  *id = glCreateShader(shader_type);
  glShaderSource(*id, 1, (const GLchar* const*)&shaderSource, NULL);
  glCompileShader(*id);
  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(*id, GL_COMPILE_STATUS, &success);
  if(!success)
  {
    GLint maxLength = 0;
    glGetShaderiv(*id, GL_INFO_LOG_LENGTH, &maxLength);
    glGetShaderInfoLog(*id, maxLength, &maxLength, infoLog);
    printf("Error compilation shader: %s\n", infoLog);
    glDeleteShader(*id);
  }
}

GrfShader*
grf_shader_new_from_file(const char* filename,int type){
  return grf_shader_new_from_string(read_file(filename),type);
}

GrfShader*
grf_shader_new_from_string(const char* text, int type){
  GrfShader* shader     = malloc(sizeof(GrfShader));
  shader->shader_source = (char*)text;
  shader->shader_type   = type;
  compile_shader(shader->shader_source,&shader->shader_id,type);
  return shader;
}

void
grf_shader_free(GrfShader** shader){
  free((*shader)->shader_source);
  glDeleteShader((*shader)->shader_id);
  free(*shader);
  *shader = NULL;
}
