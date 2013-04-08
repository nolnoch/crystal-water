/*
 * main.cpp
 *
 *    Created on: Apr 8, 2013
 *   Last Update: Apr 8, 2013
 *  Orig. Author: Wade Burch (nolnoch@cs.utexas.edu)
 *  Contributors: <none>
 */

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <iostream>

#include "./quaternion.h"


/*********************************
 * Global Constants and Variables
 */

// Shader Program


// Lighting


// Materials


// Window
const GLint WIN_WIDTH = 800;
const GLint WIN_HEIGHT = 600;

// Input



/*********************************
 * Function Stubs
 */



/*********************************
 * Display
 */

void Display() {

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

}

void ShaderInit() {

}

void OpenGLInit() {

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
