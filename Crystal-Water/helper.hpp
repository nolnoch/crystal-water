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
// #include <CL/cl.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL/SOIL.h>
// #include <assimp/Importer.hpp>
// #include <assimp/scene.h>

#include "./program.hpp"
#include "./quaternion.hpp"
#include "./io.h"


#define NULL_PTR        reinterpret_cast<char *>(NULL)
#define OFFSET_PTR(n)   (reinterpret_cast<GLvoid *>(NULL_PTR + (n)))


/*********************************
 * Global Constants and Variables
 */

// Shader Program
Program progSky, progCube;

// Vertex Buffers
GLuint vboID, uboID;
vector<GLuint *> iboIDs;

// Objects
Mesh meshSky;
vector<GLuint> texIds;
bool useSkyBox;

// Lighting
glm::vec4 light_ambient(0.1f, 0.1f, 0.1f, 1.0f);
glm::vec4 light_diffuse(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 light_specular(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 light_position(70.0f, 5.0f, 50.0f, 1.0f);

// Material
glm::vec3 material_ambient(0.1f, 0.2f, 0.3f);
glm::vec3 material_diffuse(0.2f, 0.3f, 0.5f);
glm::vec3 material_specular(0.3f, 0.6f, 0.8f);
GLfloat material_shininess = 100;

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
void PushStaticUniformsCube();
void PushVerticesCube();
void PushStaticUniformsSky();
void PushVerticesSky();
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

GLuint LoadTexture(string filename, int texUnit) {
  GLuint id;

  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0 + texUnit);

  id = SOIL_load_OGL_texture(filename.c_str(),
                             SOIL_LOAD_AUTO,
                             SOIL_CREATE_NEW_ID,
                             SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);

  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  return id;
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

  return glm::acos(dotProd) * 1.20f;
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