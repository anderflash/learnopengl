#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <grafeo/imgproc.h>
#include "program.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
  if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window,GL_TRUE);
}

GrfArray*
grf_array_eye(uint32_t size, GrfDataType type){
  uint32_t* sizes = (uint32_t*)malloc(2*sizeof(uint32_t));
  sizes[0] = size;sizes[1] = size;
  GrfArray* array = grf_array_zeros(2,sizes,type);
  uint32_t i;
  for(i = 0; i < size; i++) grf_array_set_element_1D(array,i*size+i,1.0f);
  free(sizes);
  return array;
}

GrfArray*
grf_array_translate(GrfArray* array, GrfArray* offset){
  GrfArray* trans = grf_array_copy(array);
  uint32_t size = array->size[0]-1;
  uint32_t index,i;
  long double sum;
  for(i = 0; i < size; i++) {
    index = (i+1)*(size+1)-1;
    sum   = grf_array_get_long_double_1D(array,index)+grf_array_get_long_double_1D(offset,i);
    grf_array_set_element_1D(trans,index,sum);
  }
  return trans;
}

GrfArray*
grf_array_translate_4f(GrfArray* array, float x, float y, float z){
  GrfArray *trans;
  if(array) trans = grf_array_copy(array);
  else      trans = grf_array_eye(4,GRF_FLOAT);
  trans->data_float[ 3] += x;
  trans->data_float[ 7] += y;
  trans->data_float[11] += z;
  return trans;
}

void
grf_array_transpose(GrfArray* array){
  long double tmp;
  uint64_t size = array->size[0];
  uint64_t index2 = size;
  uint64_t index1 = 1, y, x;

  for(y = 1, index2=size; y < size; y++){
    for(x = 0, index1=y,index2=y*size; x < y;  x++, index1 += size, index2++){
      tmp = grf_array_get_long_double_1D(array,index1);
      grf_array_set_element_1D(array,index1,grf_array_get_long_double_1D(array,index2));
      grf_array_set_element_1D(array,index2,tmp);
    }
  }
}

GrfArray*
grf_array_scale(GrfArray* array, GrfArray* scale){
  GrfArray* scaled = grf_array_copy(array);
  uint32_t size = array->size[0]-1;
  uint32_t i, index = 0;
  long double value = 0;
  for(i = 0; i < size; i++,index += size+1){
    value = grf_array_get_long_double_1D(array,index)*grf_array_get_long_double_1D(scale,i);
    grf_array_set_element_1D(scaled,index,value);
  }
  return scaled;
}

GrfArray*
grf_array_scale_4f(GrfArray* array, float x, float y, float z){
  GrfArray *scaled;
  if(array) scaled = grf_array_copy(array);
  else      scaled = grf_array_eye(4,GRF_FLOAT);
  scaled->data_float[ 0] *= x;
  scaled->data_float[ 5] *= y;
  scaled->data_float[10] *= z;
  return scaled;
}

GrfArray*
grf_array_ortho(float left,float right,float bottom,float top,float near,float far){
  GrfArray* ortho = grf_array_eye(4,GRF_FLOAT);
  ortho->data_float[0]  =  2.0f/(right-left);
  ortho->data_float[5]  =  2.0f/(top-bottom);
  ortho->data_float[10] = -2.0f/(far-near);
  ortho->data_float[3]  = -(right+left)/(right-left);
  ortho->data_float[7]  = -(top+bottom)/(top-bottom);
  ortho->data_float[11] = -(far+near  )/(far-near);
  return ortho;
}

GrfArray*
grf_array_perspective(float fov, float aspect, float near, float far){
  float tanHalfFovy = tan(fov/2.0f);
  uint32_t  size[2]     = {4,4};
  GrfArray* perspective = grf_array_zeros(2,size,GRF_FLOAT);
  perspective->data_float[0 ] = 1.0f/(aspect*tanHalfFovy);
  perspective->data_float[5 ] = 1.0f/(tanHalfFovy);
  perspective->data_float[10] = -(far+near)/(far-near);
  perspective->data_float[14] = -1.0f;
  perspective->data_float[11] = -(2.0f*far*near)/(far-near);
  return perspective;
}

GrfArray*
grf_array_dot(GrfArray* array, GrfArray* array2){
  GrfArray* result = grf_array_zeros_like(array);
  float* data1  =  array->data_float;
  float* data2  = array2->data_float;
  float* datar  = result->data_float;
  uint32_t size = array->size[0];
  uint64_t index,index1 = 0, index2;
  uint32_t i, x, y;
  for(y = 0; y < size; y++){
    for(x = 0; x < size; x++, index++){
      index1 = y*size;
      index2 = x;
      for(i = 0; i < size; i++){
        datar[index] += data1[index1] * data2[index2];
        index1++;
        index2 += size;
      }
    }
  }
  return result;
}
GrfArray*
grf_array_normalize(GrfArray* array){
  float* d = array->data_float;
  float* n = array->data_float;
  GrfArray* normalized = grf_array_copy(array);
  float norm = sqrt(d[0]*d[0]+d[1]*d[1]+d[2]*d[2]);
  n[0] /= norm;
  n[1] /= norm;
  n[2] /= norm;
  return n;
}

GrfArray*
grf_array_rotate_4f(GrfArray* array, float angle_rad, float x,float y,float z){
  float a  = angle_rad;
  float c  = cos(a);
  float cc = 1-c;
  float s  = sin(a);

  uint32_t size     = 3;
  uint32_t sizes[2] = {4,4};
  float    v[3]     = {x,y,z};
  grf_array_from_data(v,1,&size,GRF_FLOAT);
  GrfArray* axis    = grf_array_normalize(v);

  float u = axis->data_float[0];
  float v = axis->data_float[1];
  float w = axis->data_float[2];
  float u2 = u*u;
  float v2 = v*v;
  float w2 = w*w;
  float uv = u*v;
  float uw = u*w;
  float vw = v*w;

  GrfArray* rotation = grf_array_zeros(2,sizes,GRF_FLOAT);

  rotation->data_float[0]  = u2+(1-u2)*c;
  rotation->data_float[1]  = uv*cc - w*s;
  rotation->data_float[2]  = uw*cc + v*s;

  rotation->data_float[4]  = uv*cc + w*s;
  rotation->data_float[5]  = v2+(1-v2)*c;
  rotation->data_float[6]  = vw*cc - u*s;

  rotation->data_float[8]  = uw*cc - v*s;
  rotation->data_float[9]  = vw*cc + u*s;
  rotation->data_float[10] = w2+(1-w2)*c;

  rotation->data_float[15] = 1;

  if(!array) return rotation;
  GrfArray* result = grf_array_dot(array,rotation);
  grf_array_free(rotation);
  return result;
}

int main(void)
{
  int width = 800, height=600;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow* window = glfwCreateWindow(width,height,"LearnOpenGL", NULL, NULL);
  if(window == NULL){
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE;
  if(glewInit() != GLEW_OK){
    printf("Failed to initialize GLEW\n");
    glfwTerminate();
    return -1;
  }

  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  GLfloat vertices[] = {
    0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // Top Right
    0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // Bottom Right
   -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // Bottom Left
   -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // Top Left
  };
  GLuint indices[] = {
    0, 1, 3,
    1, 2, 3
  };

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));

  GLuint EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  GrfProgram* shaderProgram = grf_program_new_from_files("vertex.glsl","fragment.glsl");

  glViewport(0,0,width,height);

  glfwSetKeyCallback(window, key_callback);

  GLuint texture1, texture2;

  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  GrfArray* image = grf_image_read("container.jpg");
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->size[1], image->size[0], 0, GL_RGB, GL_UNSIGNED_BYTE, image->data_uint8);
  glGenerateMipmap(GL_TEXTURE_2D);
  grf_array_free(image);
  image = NULL;

  glGenTextures(1,&texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  image = grf_image_read("awesomeface.png");
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->size[1], image->size[0], 0, GL_RGB, GL_UNSIGNED_BYTE, image->data_uint8);
  glGenerateMipmap(GL_TEXTURE_2D);
  grf_array_free(image);
  image = NULL;

  glBindTexture(GL_TEXTURE_2D, 0);

  // Model Matrix
  GrfArray* model      = grf_array_eye(4,GRF_FLOAT);
  grf_array_transpose(model);

  // View Matrix
  GrfArray* view      = grf_array_translate_4f(NULL,0.0f, 0.0f, -3.0f);
  grf_array_transpose(view);

  // Projection Matrix
  GrfArray* projection = grf_array_perspective(45.0f, (float)width/(float)height,0.1f, 100.0f);
  grf_array_transpose(projection);

  grf_program_use(shaderProgram);
  GLuint MLoc = glGetUniformLocation(shaderProgram->program_id,"M");
  GLuint VLoc = glGetUniformLocation(shaderProgram->program_id,"V");
  GLuint PLoc = glGetUniformLocation(shaderProgram->program_id,"P");

  glUniformMatrix4fv(MLoc, 1, GL_FALSE, model->data_float);
  glUniformMatrix4fv(VLoc, 1, GL_FALSE, view->data_float);
  glUniformMatrix4fv(PLoc, 1, GL_FALSE, projection->data_float);

  grf_array_free(model);
  grf_array_free(view);
  grf_array_free(projection);

  while(!glfwWindowShouldClose(window)){
    // Check and call events
    glfwPollEvents();

    // Render
    // Clear the colorbuffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Be sure to activate the shader
    grf_program_use(shaderProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glUniform1i(glGetUniformLocation(shaderProgram->program_id,"ourTexture1"),0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glUniform1i(glGetUniformLocation(shaderProgram->program_id,"ourTexture2"),1);

    // Now draw the triangle
    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
  }

  glfwTerminate();
  grf_program_free(&shaderProgram);

  return 0;
}

