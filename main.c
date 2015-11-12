#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
  if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window,GL_TRUE);
}
char* read_file(const char* filename){
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

void compile_shader(const char* source, GLuint* id, int shader_type){
  GLchar* shaderSource = (GLchar*)read_file(source);
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
  free(shaderSource);
}

void init_shaders(GLuint* shaderProgram){
  GLuint vertexShader, fragmentShader;
  compile_shader("vertex.glsl",&vertexShader,GL_VERTEX_SHADER);
  compile_shader("fragment.glsl",&fragmentShader,GL_FRAGMENT_SHADER);

  *shaderProgram = glCreateProgram();
  glAttachShader(*shaderProgram, vertexShader);
  glAttachShader(*shaderProgram, fragmentShader);
  glLinkProgram (*shaderProgram);

  GLint success;
  GLchar infoLog[512];
  glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
  if(!success)
  {
    glGetProgramInfoLog(*shaderProgram, 512, NULL, infoLog);
    printf("Error compilation program: %s\n", infoLog);
  }

  glUseProgram  (*shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}




int main(void)
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow* window = glfwCreateWindow(800,600,"LearnOpenGL", NULL, NULL);
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
    // Positions        // Colors
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
     0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
  };
  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid*)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));


  GLuint shaderProgram;
  init_shaders(&shaderProgram);

  glViewport(0,0,800,600);

  glfwSetKeyCallback(window, key_callback);

  while(!glfwWindowShouldClose(window)){
    // Check and call events
    glfwPollEvents();

    // Render
    // Clear the colorbuffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Be sure to activate the shader
    glUseProgram(shaderProgram);

    // Update the uniform color
    GLfloat timeValue = glfwGetTime();
    GLfloat greenValue = (sin(timeValue)/2)+0.5;
    GLint vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
    glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

    // Now draw the triangle
    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
  }

  glfwTerminate();

  return 0;
}

