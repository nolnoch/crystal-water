/**
 * main.cpp
 *
 *    Created on: Apr 8, 2013
 *   Last Update: Apr 10, 2013
 *  Orig. Author: Wade Burch (nolnoch@cs.utexas.edu)
 *  Contributors: [none]
 */

#include "./helper.hpp"


/*********************************
 * Display
 */

void Display() {
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  CollapseMatrices();

  progShader.enable();
  PushUniforms();
  PushVertices();
  progShader.disable();

  glFlush();
  glutSwapBuffers();
}


/*********************************
 * Rendering
 */

void PushUniforms() {
  // Load matrices.
  progShader.setUniformMatrix(4, "modelviewMatrix", glm::value_ptr(mModel));
  progShader.setUniformMatrix(4, "projectionMatrix", glm::value_ptr(mProj));

  // Load the (static) location/properties of the light.
  progShader.setUniformv(3, GL_FLOAT, "light0.lightPos", glm::value_ptr(light_position));
  progShader.setUniformv(3, GL_FLOAT, "light0.lightAmb", glm::value_ptr(light_ambient));
  progShader.setUniformv(3, GL_FLOAT, "light0.lightDiff", glm::value_ptr(light_diffuse));
  progShader.setUniformv(3, GL_FLOAT, "light0.lightSpec", glm::value_ptr(light_specular));

  // Load material properties.
  progShader.setUniformv(3, GL_FLOAT, "mat.matAmb", glm::value_ptr(material_ambient));
  progShader.setUniformv(3, GL_FLOAT, "mat.matDiff", glm::value_ptr(material_diffuse));
  progShader.setUniformv(3, GL_FLOAT, "mat.matSpec", glm::value_ptr(material_specular));
  progShader.setUniform(    GL_FLOAT, "mat.matShiny", material_shininess);
}

void PushVertices() {
  glutSolidCube(15.0);
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
      orbitAnchor = glm::vec3(x, WIN_HEIGHT - y, vEye.z);
      orbitDest = glm::vec3(x, WIN_HEIGHT - y, vEye.z);
      stateOrbiting = true;
    } else {
      stateOrbiting = false;
    }
  } else if (button == GLUT_RIGHT_BUTTON) {
    // Right click action?

  } else if (button == 3 || button == 4) {
    float step = 1.0f;

    vEye.z += (button == 4) ? step : -step;
  }

  glutPostRedisplay();
}

/**
 * Disc rotation with a slide: orbit left/right, translate up/down.
 */
void MouseMotion(int x, int y) {
  if (stateOrbiting) {
    orbitAnchor = orbitDest;
    orbitDest = glm::vec3(x, WIN_HEIGHT - y, vEye.z);

    // Vertical Slide
    if (orbitAnchor.y != orbitDest.y) {
      float slideFactor = vEye.z / WIN_WIDTH;
      GLfloat slideDist = slideFactor * (orbitDest.y - orbitAnchor.y);
      mTrans = glm::translate(mTrans, glm::vec3(0.0, slideDist, 0.0));
    }

    // Horizontal Orbit
    if (orbitAnchor.x != orbitDest.x) {
      float signAxis = orbitAnchor.x < orbitDest.x ? 1.0f : -1.0f;
      glm::vec3 orbitAxis = glm::vec3(0.0, signAxis, 0.0);
      GLfloat orbitAngle = FindRotationAngle(orbitDest, orbitAnchor);
      Quaternion qOrbitRot = Quaternion(orbitAngle, orbitAxis, RAD);
      qTotalRotation = qOrbitRot * qTotalRotation;
    }
  }

  glutPostRedisplay();
}

/**
 * Zoom to/from cursor on mouse-wheel scroll.
 */
void MouseWheel(int wheel, int direction, int x, int y) {
  cout << "Wheel signal received." << endl;

  // This function, although registered, is not being called.
}

void Keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'r':
      CameraInit();
      MatrixInit();
      break;
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


/*********************************
 * Init Functions
 */

void BufferInit() {
  // Fill when we have data to send to buffer.
}

void ShaderInit() {
  progShader.addDefaultShaders();
  progShader.init();
  // progShader.bindAttribute(0, "someAttribute");
  progShader.linkAndValidate();
  // progShader.addSampler();
}

void OpenGLInit() {
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  glEnable(GL_NORMALIZE);
  glEnable(GL_RESCALE_NORMAL);

  // View/Projection
  fovy = 40.0f;
  aspect = WIN_WIDTH / static_cast<float>(WIN_HEIGHT);
  zNear = 1.0f;
  zFar = 800.0f;

  stateOrbiting = false;

  CameraInit();
  MatrixInit();
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
  glutIdleFunc(NULL);

  // Initialize GLEW
  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    cout << "GLEW initialization failed. Aborting program..." << endl;
    return -1;
  }

  OpenGLInit();
  ShaderInit();
  BufferInit();

  glutMainLoop();

  return 0;
}
