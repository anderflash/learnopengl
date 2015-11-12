#ifndef PROGRAM_H
#define PROGRAM_H
#include "shader.h"
typedef struct _GrfProgram{
  GLuint program_id;
  GrfShader* vertexShader;
  GrfShader* fragmentShader;
}GrfProgram;

GrfProgram*
grf_program_new_from_files(char* vertexShaderFile, char* fragmentShaderFile);

GrfProgram*
grf_program_new_from_text(char* vertexShaderText, char* fragmentShaderText);

void
grf_program_free(GrfProgram**);

void
grf_program_use(GrfProgram*);
#endif
