#ifndef SHADER_H
#define SHADER_H
#include <GL/glew.h>

typedef struct _GrfShader{
  GLuint shader_id;
  char*  shader_source;
  int    shader_type;
}GrfShader;

char*
read_file(const char* filename);

GrfShader*
grf_shader_new_from_file(const char* filename, int type);

GrfShader*
grf_shader_new_from_string(const char* text, int type);

void
grf_shader_free(GrfShader**);

#endif
