/**
 * main.cpp
 *
 *    Created on: Apr 8, 2013
 *   Last Update: Apr 21, 2013
 *  Orig. Author: Wade Burch (nolnoch@cs.utexas.edu)
 *  Contributors: [none]
 */

#include "./helper.hpp"


/*********************************
 * Display
 */

void CrystalDisplay() {
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  CollapseMatrices();

  // OpenCL program
//  glFinish();
//  clEnqueueAcquireGLObjects(clQueue, 1, &clVBObuffer, 0, NULL, NULL);
//  clEnqueueNDRangeKernel(clQueue, clKernel, 1, NULL, &clGlobalSize, NULL, 0, 0, 0);
//  clEnqueueReleaseGLObjects(clQueue, 1, &clVBObuffer, 0, NULL, NULL);
//  clFinish(clQueue);

  // OpenGL program
  progSky.enable();
  RenderMesh();
  progSky.disable();

  /* TODO
   * We need a simple heuristic to separate cube from skybox, but we ought
   * to design something flexible for future projects (not relying on all
   * skyboxes to be 256x256 centered on origin).
   */

  glFlush();
  glutSwapBuffers();
}


/*********************************
 * Rendering
 */


void RenderMesh() {
  vector<TexInfo>& texIds = mesh.getTextures();
  vector<int>& iboSizes = mesh.iboSizes();
  int nIBOs = mesh.numIBOs();
  GLuint locLight0;
  GLuint blockBindingLight0 = 1;

  // Load matrices.
  progSky.setUniformMatrix(4, "modelviewMatrix", glm::value_ptr(mModel));
  progSky.setUniformMatrix(4, "projectionMatrix", glm::value_ptr(mProj));

  // Bind the (static) location/properties of the light.
  glBindBuffer(GL_UNIFORM_BUFFER, uboID);
  locLight0 = glGetUniformBlockIndex(progSky.getProgramId(), "Light");
  glUniformBlockBinding(progSky.getProgramId(), locLight0, blockBindingLight0);
  glBindBufferBase(GL_UNIFORM_BUFFER, blockBindingLight0, uboID);

  // Load the VBO.
  glBindBuffer(GL_ARRAY_BUFFER, vboID);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glEnableVertexAttribArray(4);
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), OFFSET_PTR(0));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), OFFSET_PTR(12));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), OFFSET_PTR(24));
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), OFFSET_PTR(32));
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), OFFSET_PTR(44));
  glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), OFFSET_PTR(56));

  // Load each IBO and draw elements. Loads one texture per IBO.
  for (int i = 0; i < nIBOs; i++) {
    if (texIds[i].present) {
      glEnable(GL_TEXTURE_2D);
      progSky.setTexture(0, texIds[i]);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(iboIDs[i]));
    glDrawElements(GL_TRIANGLES, iboSizes[i], GL_UNSIGNED_INT, OFFSET_PTR(0));
  }

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(3);
  glDisableVertexAttribArray(4);
  glDisableVertexAttribArray(5);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
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
    float step = vEye.z / 15.0f;

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
      glutPostRedisplay();
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
  // Currently not registered. Needed?

  glutPostRedisplay();
}


/*********************************
 * Init Functions
 */

void OpenCLInit() {
  cl_int errorCode;

  // Redirect Vertex Buffer to OpenCL
  clVBObuffer = clCreateFromGLBuffer(clContext, CL_MEM_READ_WRITE, vboID, &errorCode);
  if (errorCode != CL_SUCCESS) {
    exit(ProcessErrorCL(errorCode));
  }
  clQueue = clCreateCommandQueue(clContext, clDeviceId, 0, &errorCode);
  if (errorCode != CL_SUCCESS) {
    exit(ProcessErrorCL(errorCode));
  }

  // TODO Specify CL program to execute on kernel and release object.
}

void BufferInit() {
  std::vector<VBOVertex>& vboArray = mesh.getVBOVertexArray();
  std::vector<vector<GLuint> >& iboArrays = mesh.getIBOIndexArrays();
  int nVBO = vboArray.size();
  int nIBOs = iboArrays.size();
  GLfloat align = 0.0f;

  // Vertex Buffer Object
  glGenBuffers(1, &vboID);
  glBindBuffer(GL_ARRAY_BUFFER, vboID);
  glBufferData(GL_ARRAY_BUFFER, sizeof(VBOVertex)*nVBO, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VBOVertex)*nVBO, &vboArray[0]);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), OFFSET_PTR(0));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), OFFSET_PTR(12));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), OFFSET_PTR(24));
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), OFFSET_PTR(32));
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), OFFSET_PTR(44));
  glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), OFFSET_PTR(56));

  // Uniform Buffer Object
  GLfloat uLight0[16] = { light_position.x, light_position.y, light_position.z, align,
                          light_ambient.x, light_ambient.y, light_ambient.z, align,
                          light_diffuse.x, light_diffuse.y, light_diffuse.z, align,
                          light_specular.x, light_specular.y, light_specular.z, align
                        };
  glGenBuffers(1, &uboID);
  glBindBuffer(GL_UNIFORM_BUFFER, uboID);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(uLight0), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(uLight0), uLight0);

  // Index Buffer Objects
  for (int i = 0; i < nIBOs; i++) {
    iboIDs.push_back(new GLuint);
    glGenBuffers(1, iboIDs[i]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(iboIDs[i]));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, iboArrays[i].size() * sizeof(GLuint),
        &iboArrays[i][0], GL_STATIC_DRAW);
  }
  // Data should now be in GPU memory (server-side), so free heap memory.
  // TODO This data will not be assigned to any buffer yet when using OpenCL.
  mesh.freeArrays();
}

void ShaderInit() {
  GLint texLoad;

  progSky.addShader("shader0.vert", GL_VERTEX_SHADER);
  progSky.addShader("shader0.frag", GL_FRAGMENT_SHADER);
  progSky.init();
  progSky.bindAttribute(0, "vertexLocation");
  progSky.bindAttribute(1, "vertexNormal");
  progSky.bindAttribute(2, "vertexTexCoord");
  progSky.bindAttribute(3, "vertexMatDiffuse");
  progSky.bindAttribute(4, "vertexMatSpecular");
  progSky.bindAttribute(5, "vertexShininess");
  progSky.linkAndValidate();
  progSky.addSampler("tex");
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
  glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);
  glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
  glutInitWindowPosition(50, 50);

  glutCreateWindow("Crystal-Water");
  glutDisplayFunc(CrystalDisplay);
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

  // Initialize OpenCL
  clGetPlatformIDs(1, &clPlatformId, NULL);
  clGetDeviceIDs(clPlatformId, CL_DEVICE_TYPE_GPU, 1, &clDeviceId, NULL);
  cl_context_properties properties[] = {
      CL_GL_CONTEXT_KHR, (cl_context_properties) glXGetCurrentContext(),
      CL_GLX_DISPLAY_KHR, (cl_context_properties) glXGetCurrentDisplay(),
      CL_CONTEXT_PLATFORM, (cl_context_properties) clPlatformId, 0
  };
  cl_int contextError;
  clContext = clCreateContext(properties, 1, &clDeviceId, NULL, NULL, &contextError);
  if (contextError != CL_SUCCESS) {
    return ProcessErrorCL(contextError);
  }

  // Load skybox mesh
  mesh.setTexturePath("../tex/");
  if (!mesh.loadFile("skybox.obj")) {
    cout << "Error loading object/mesh file. Aborting program..." << endl;
    return -1;
  }

  OpenGLInit();
  ShaderInit();
  BufferInit();
//  OpenCLInit();

  glutMainLoop();

  return 0;
}
