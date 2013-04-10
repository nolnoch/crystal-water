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
void Render();
void CollapseMatrices();
void MouseClick(int button, int state, int x, int y);
void MouseMotion(int x, int y);
void MouseWheel(int wheel, int direction, int x, int y);
void Keyboard(unsigned char key, int x, int y);
void Idle();
void SetAnchor(float x, float y);
GLfloat FindRotationAngle(glm::vec3 startVec, glm::vec3 endVec);
void BufferInit();
void ShaderInit();
void OpenGLInit();


/*********************************
 * Display
 */

void Display() {
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  CollapseMatrices();

  Render();

  glFlush();
  glutSwapBuffers();
}


/*********************************
 * Rendering
 */

void Render() {
  progShader.enable();

  // Load matrices.
  progShader.setUniformMatrix(4, "modelviewMatrix", glm::value_ptr(mModel));
  progShader.setUniformMatrix(4, "projectionMatrix", glm::value_ptr(mProj));

  // Load the (static) location/properties of the light.
  progShader.setUniformv(3, GL_FLOAT, "light0.lightPos",
      glm::value_ptr(light_position));
  progShader.setUniformv(3, GL_FLOAT, "light0.lightAmb",
      glm::value_ptr(light_ambient));
  progShader.setUniformv(3, GL_FLOAT, "light0.lightDiff",
      glm::value_ptr(light_diffuse));
  progShader.setUniformv(3, GL_FLOAT, "light0.lightSpec",
      glm::value_ptr(light_specular));

  // Load material properties.
  progShader.setUniformv(3, GL_FLOAT, "mat.matAmb",
      glm::value_ptr(material_ambient));
  progShader.setUniformv(3, GL_FLOAT, "mat.matDiff",
      glm::value_ptr(material_diffuse));
  progShader.setUniformv(3, GL_FLOAT, "mat.matSpec",
      glm::value_ptr(material_specular));
  progShader.setUniform(GL_FLOAT, "mat.matShiny", material_shininess);

  glutSolidCube(15.0);

  progShader.disable();
}

void CollapseMatrices() {
  mLook = glm::lookAt(vEye, vCenter, vUp);
  mRot = glm::make_mat4(&qTotalRotation.matrix()[0]);

  mModel = mLook * mTrans * mRot;
}


/*********************************
 * Interaction
 */

/**
 * Disc rotation with a slide: orbit left/right, translate up/down.
 */
void MouseClick(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
      orbitAnchor = glm::normalize(glm::vec3(x, WIN_HEIGHT - y, vEye.z));
      orbitDest = glm::normalize(glm::vec3(x, WIN_HEIGHT - y, vEye.z));
      stateOrbiting = true;
    } else {
      stateOrbiting = false;
    }
  }
}

/**
 * Disc rotation with a slide: orbit left/right, translate up/down.
 */
void MouseMotion(int x, int y) {
  if (stateOrbiting) {
    orbitAnchor = orbitDest;
    orbitDest = glm::normalize(glm::vec3(x, WIN_HEIGHT - y, vEye.z));

    GLfloat slideDist = orbitDest.y - orbitAnchor.y;

    glm::vec3 orbitAxis = glm::vec3(0.0, 1.0, 0.0);
    GLfloat orbitAngle = FindRotationAngle(orbitAnchor, orbitDest);
    Quaternion qOrbitRot = Quaternion(orbitAngle, orbitAxis, RAD);

    qOrbitRot.toString();

    qTotalRotation = qOrbitRot * qTotalRotation;
    mTrans = glm::translate(mTrans, glm::vec3(0.0, slideDist, 0.0));
  }

  glutPostRedisplay();
}

/**
 * Zoom to/from cursor on mouse-wheel scroll.
 */
void MouseWheel(int wheel, int direction, int x, int y) {
  float step = 0.1f;

  cout << "Wheel signal received." << endl;

  SetAnchor(x, y);

  // Gradually move lookAt center to anchor for any zoom.
  glm::vec3 startCenter = vCenter;
  vCenter = (step * zoomAnchor) + ((1.0f - step) * startCenter);

  // Gradually scale lookAt eye distance with respect to center.
  glm::vec3 eyeDistance = vEye;
  eyeDistance *= direction > 0 ? 1.0f + step : 1.0f - step;
  vEye = vCenter + eyeDistance;

  glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'q':
    case 27:
      exit(0);
      break;
    default:
      break;
  }
}

void Idle() {
  // Needed?

  glutPostRedisplay();
}

GLfloat FindRotationAngle(glm::vec3 startVec, glm::vec3 endVec) {
  GLfloat angle, zA, zB, xA, xB, dotProd;

  xA = startVec.x - (WIN_WIDTH / 2.0f);
  xB = endVec.x - (WIN_WIDTH / 2.0f);

  cout << xA << ", " << xB << ", " << vEye.z << endl;

  zA = glm::sqrt((vEye.z * vEye.z) - (xA * xA));
  zB = glm::sqrt((vEye.z * vEye.z) - (xB * xB));

  glm::vec3 vA(xA, 0.0, zA);
  glm::vec3 vB(xB, 0.0, zB);

  cout << vA.x << ", " << vA.z << endl;
  cout << vB.x << ", " << vB.z << endl;

  dotProd = glm::dot(glm::normalize(vA), glm::normalize(vB));

  cout << dotProd << endl;

  return glm::acos(dotProd);
}

void SetAnchor(float x, float y) {
  float wX, wY, wZ;

  // Read the depth buffer for pixel at (x, y).
  wX = x;
  wY = WIN_HEIGHT - y;
  glReadPixels(wX, wY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wZ);

  // Construct Viewport and Window coord vectors.
  glm::vec4 vView(0, 0, WIN_WIDTH, WIN_HEIGHT);
  glm::vec3 vWin(wX, wY, wZ);

  // Unproject to get our zoom anchor point.
  zoomAnchor = glm::unProject(vWin, mModel, mProj, vView);
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

  // Projection
  fovy = 40.0f;
  aspect = WIN_WIDTH / static_cast<float>(WIN_HEIGHT);
  zNear = 1.0f;
  zFar = 800.0f;

  // Camera
  vEye = glm::vec3(0.0f, 5.0f, 50.0f);
  vCenter = glm::vec3(0.0f, 0.0f, 0.0f);
  vUp = glm::vec3(0.0f, 1.0f, 0.0f);

  stateOrbiting = false;

  // Matrices
  mProj = glm::perspective(fovy, aspect, zNear, zFar);
  mLook = glm::lookAt(vEye, vCenter, vUp);
  mModel = glm::mat4(1.0);
  mRot = glm::mat4(1.0);
  mTrans = glm::mat4(1.0);
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
