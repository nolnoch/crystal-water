/**
 * main.cpp
 *
 *    Created on: Apr 8, 2013
 *   Last Update: Apr 8, 2013
 *  Orig. Author: Wade Burch (nolnoch@cs.utexas.edu)
 *  Contributors: <none>
 */

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <iostream>

#include "./program.hpp"
#include "./quaternion.hpp"


/*********************************
 * Global Constants and Variables
 */

// Shader Program
Program progShader;

// Lighting
glm::vec4 light_ambient(0.1f, 0.1f, 0.1f, 1.0f);
glm::vec4 light_diffuse(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 light_specular(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 light_position(40, 40, 40, 1.0f);

// Window
const GLint WIN_WIDTH = 800;
const GLint WIN_HEIGHT = 600;

// View and Transformation
GLfloat fovy, aspect, zNear, zFar;
glm::mat4 mModel, mProj, mRot;

// Input



/*********************************
 * Function Stubs
 */

void Display();
void DrawSomething();
void MouseClick(int button, int state, int x, int y);
void MouseMotion(int x, int y);
void MouseWheel(int wheel, int direction, int x, int y);
void Keyboard(unsigned char key, int x, int y);
void Idle();
void BufferInit();
void ShaderInit();
void OpenGLInit();


/*********************************
 * Display
 */

void Display() {
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  DrawSomething();

  glFlush();
  glutSwapBuffers();
}


/*********************************
 * Rendering
 */

void DrawSomething() {
  progShader.enable();

  progShader.setUniformMatrix(4, "rotationMatrix", glm::value_ptr(mRot));
  progShader.setUniformMatrix(4, "modelviewMatrix", glm::value_ptr(mModel));
  progShader.setUniformMatrix(4, "projectionMatrix", glm::value_ptr(mProj));

  // Render here.

  progShader.disable();
}


/*********************************
 * Interaction
 */

void MouseClick(int button, int state, int x, int y) {

}

void MouseMotion(int x, int y) {

}

void MouseWheel(int wheel, int direction, int x, int y) {

}

void Keyboard(unsigned char key, int x, int y) {

}

void Idle() {

}


/*********************************
 * Init Functions
 */

void BufferInit() {
  // Fill when we have data to send to buffer.
}

void ShaderInit() {
  // Add Shaders
  progShader.addDefaultShaders();

  // Init Program object
  progShader.init();

  // Bind Attributes
  // progShader.bindAttribute(0, "someAttribute");

  // Link and Validate program
  progShader.linkAndValidate();

  // Generate Sampler2D uniform binding target
  // progShader.addSampler();
}

void OpenGLInit() {
  // Depth Testing
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);

  // Blending
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Clear Color
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  // Scale Normalization
  glEnable(GL_NORMALIZE);
  glEnable(GL_RESCALE_NORMAL);

  //Variables
  fovy = 40.0f;
  aspect = WIN_WIDTH / static_cast<float>(WIN_HEIGHT);
  zNear = 1.0f;
  zFar = 800.0f;

  // Matrices
  mProj = glm::perspective(fovy, aspect, zNear, zFar);
  mModel = glm::mat4(1.0);
  mRot = glm::mat4(1.0);
}

int main(int argc, char* argv[]) {
  // Initialize freeglut
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
  glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
  glutInitWindowPosition(50, 50);

  glutCreateWindow("Crystal-Water");
  glutDisplayFunc(Display);
  glutMouseFunc(MouseClick);
  glutMotionFunc(MouseMotion);
  glutMouseWheelFunc(MouseWheel);
  glutKeyboardFunc(Keyboard);
  glutIdleFunc(Idle);

  // Initialize GLEW
  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    cout << "GLEW initialization failed. Aborting program..." << endl;
    return -1;
  }

  // Initialize OpenGL state
  OpenGLInit();

  // Initialize GLSL program
  ShaderInit();

  // Initialize buffer/array objects
  BufferInit();

  glutMainLoop();

  return 0;
}
