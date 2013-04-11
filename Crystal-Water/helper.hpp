/**
 * helper.hpp
 *
 *    Created on: Apr 10, 2013
 *   Last Update: Apr 10, 2013
 *  Orig. Author: Wade Burch (nolnoch@cs.utexas.edu)
 *  Contributors: [none]
 */

#ifndef HELPER_HPP_
#define HELPER_HPP_


#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
glm::vec4 light_position(40.0f, 5.0f, 20.0f, 1.0f);

// Material
glm::vec3 material_ambient(0.1f, 0.2f, 0.3f);
glm::vec3 material_diffuse(0.2f, 0.3f, 0.5f);
glm::vec3 material_specular(0.3f, 0.6f, 0.8f);
GLfloat material_shininess = 70;

// Window
const GLint WIN_WIDTH = 800;
const GLint WIN_HEIGHT = 600;

// View and Transformation
GLfloat fovy, aspect, zNear, zFar;
glm::vec3 vEye, vCenter, vUp;
glm::mat4 mModel, mProj, mRot, mTrans, mLook;
Quaternion qTotalRotation;

// Input
glm::vec3 zoomAnchor;
glm::vec3 orbitAnchor, orbitDest;
bool stateOrbiting;



/*********************************
 * Function Stubs
 */

void Display();
void PushUniforms();
void PushVertices();
void MouseClick(int button, int state, int x, int y);
void MouseMotion(int x, int y);
void MouseWheel(int wheel, int direction, int x, int y);
void Keyboard(unsigned char key, int x, int y);
void Idle();
void BufferInit();
void ShaderInit();
void OpenGLInit();


/*********************************
 * Helper Functions
 */

void CollapseMatrices() {
  mLook = glm::lookAt(vEye, vCenter, vUp);
  mRot = glm::make_mat4(&qTotalRotation.matrix()[0]);

  mModel = mLook * mTrans * mRot;
}

GLfloat FindRotationAngle(glm::vec3 startVec, glm::vec3 endVec) {
  GLfloat angle, zA, zB, xA, xB, dotProd;
  GLfloat width = WIN_WIDTH / 2.0f;

  xA = glm::sin(((startVec.x - width) / width) * PI / 2);
  xB = glm::sin(((endVec.x - width) / width) * PI / 2);

  zA = glm::sqrt(1.0f - (xA * xA));
  zB = glm::sqrt(1.0f - (xB * xB));

  glm::vec3 vA(xA, 0.0, zA);
  glm::vec3 vB(xB, 0.0, zB);

  dotProd = glm::dot(glm::normalize(vA), glm::normalize(vB));

  return glm::acos(dotProd) * (vEye.z / 42.0f);
}

void SetAnchor(float x, float y) {
  float wX, wY, wZ;

  wX = x;
  wY = WIN_HEIGHT - y;
  glReadPixels(wX, wY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wZ);

  glm::vec4 vView(0, 0, WIN_WIDTH, WIN_HEIGHT);
  glm::vec3 vWin(wX, wY, wZ);
  zoomAnchor = glm::unProject(vWin, mModel, mProj, vView);

  cout << zoomAnchor.x << ", " << zoomAnchor.y << ", " << zoomAnchor.z << endl;
}

void CameraInit() {
  vEye = glm::vec3(0.0f, 5.0f, 50.0f);
  vCenter = glm::vec3(0.0f, 0.0f, 0.0f);
  vUp = glm::vec3(0.0f, 1.0f, 0.0f);
}

void MatrixInit() {
  mProj = glm::perspective(fovy, aspect, zNear, zFar);
  mLook = glm::lookAt(vEye, vCenter, vUp);
  mModel = glm::mat4(1.0);
  mRot = glm::mat4(1.0);
  mTrans = glm::mat4(1.0);
}


#endif /* HELPER_HPP_ */
