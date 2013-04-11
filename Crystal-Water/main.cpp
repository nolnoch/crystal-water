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

  if (useSkyBox) {
    progSky.enable();
    PushStaticUniformsSky();
    PushVerticesSky();
    progSky.disable();
  }

  progCube.enable();
  PushStaticUniformsCube();
  PushVerticesCube();
  progCube.disable();

  glFlush();
  glutSwapBuffers();
}


/*********************************
 * Rendering
 */

void PushStaticUniformsSky() {
  // Load matrices.
  progSky.setUniformMatrix(4, "modelviewMatrix", glm::value_ptr(mModel));
  progSky.setUniformMatrix(4, "projectionMatrix", glm::value_ptr(mProj));

  // Load the (static) location/properties of the light.
  progSky.setUniformv(3, GL_FLOAT, "light0.lightPos", glm::value_ptr(light_position));
  progSky.setUniformv(3, GL_FLOAT, "light0.lightAmb", glm::value_ptr(light_ambient));
  progSky.setUniformv(3, GL_FLOAT, "light0.lightDiff", glm::value_ptr(light_diffuse));
  progSky.setUniformv(3, GL_FLOAT, "light0.lightSpec", glm::value_ptr(light_specular));
}

void PushVerticesSky() {
  bool useMaterials = (meshSky.getUsedMaterials().size() != 0);
  vector<int>& iboSizes = meshSky.iboSizes();
  int nIBOs = meshSky.numIBOs();

  glBindBuffer(GL_ARRAY_BUFFER, vboID);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), OFFSET_PTR(0));
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), OFFSET_PTR(12));
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), OFFSET_PTR(20));

  // Load materials specific to each IBO and render those elements.
  for (int i = 0; i < nIBOs; i++) {
    if (useMaterials) {
      int mat = meshSky.getUsedMaterials()[i];
      Material& mCurrent = meshSky.getMaterials()[mat];

      progSky.setUniformv(3, GL_FLOAT, "mat.matAmb", glm::value_ptr(mCurrent.ambient()));
      progSky.setUniformv(3, GL_FLOAT, "mat.matDiff", glm::value_ptr(mCurrent.diffuse()));
      progSky.setUniformv(3, GL_FLOAT, "mat.matSpec", glm::value_ptr(mCurrent.specular()));
      progSky.setUniform(GL_FLOAT, "mat.matShiny",  mCurrent.specular_coeff());

      glEnable(GL_TEXTURE_2D);
      progSky.setTexture("tex", 0, texIds[0], 0);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(iboIDs[i]));
    glDrawElements(GL_TRIANGLES, iboSizes[i], GL_UNSIGNED_INT, OFFSET_PTR(0));
  }

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PushStaticUniformsCube() {
  // Load matrices.
  progCube.setUniformMatrix(4, "modelviewMatrix", glm::value_ptr(mModel));
  progCube.setUniformMatrix(4, "projectionMatrix", glm::value_ptr(mProj));

  // Load the (static) location/properties of the light.
  progCube.setUniformv(3, GL_FLOAT, "light0.lightPos", glm::value_ptr(light_position));
  progCube.setUniformv(3, GL_FLOAT, "light0.lightAmb", glm::value_ptr(light_ambient));
  progCube.setUniformv(3, GL_FLOAT, "light0.lightDiff", glm::value_ptr(light_diffuse));
  progCube.setUniformv(3, GL_FLOAT, "light0.lightSpec", glm::value_ptr(light_specular));

  // Load material properties.
  progCube.setUniformv(3, GL_FLOAT, "mat.matAmb", glm::value_ptr(material_ambient));
  progCube.setUniformv(3, GL_FLOAT, "mat.matDiff", glm::value_ptr(material_diffuse));
  progCube.setUniformv(3, GL_FLOAT, "mat.matSpec", glm::value_ptr(material_specular));
  progCube.setUniform(    GL_FLOAT, "mat.matShiny", material_shininess);
}

void PushVerticesCube() {
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
  // Needed?

  glutPostRedisplay();
}


/*********************************
 * Init Functions
 */

void BufferInit() {
  std::vector<VBOVertex>& vboArray = meshSky.getVBOVertexArray();
  std::vector<vector<GLuint> >& iboArrays = meshSky.getIBOIndexArrays();
  int nVBO = vboArray.size();
  int nIBOs = iboArrays.size();

  // Vertex Buffer Object
  glGenBuffers(1, &vboID);
  glBindBuffer(GL_ARRAY_BUFFER, vboID);
  glBufferData(GL_ARRAY_BUFFER, sizeof(VBOVertex)*nVBO, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VBOVertex)*nVBO, &vboArray[0]);

  glNormalPointer(GL_FLOAT, sizeof(VBOVertex), OFFSET_PTR(0));
  glTexCoordPointer(2, GL_FLOAT, sizeof(VBOVertex), OFFSET_PTR(12));
  glVertexPointer(3, GL_FLOAT, sizeof(VBOVertex), OFFSET_PTR(20));

  // Index Buffer Objects
  for (int i = 0; i < nIBOs; i++) {
    iboIDs.push_back(new GLuint);
    glGenBuffers(1, iboIDs[i]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(iboIDs[i]));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, iboArrays[i].size() * sizeof(GLuint),
        &iboArrays[i][0], GL_STATIC_DRAW);
  }
  // Data should now be in GPU memory (server-side), so free heap memory.
  meshSky.freeArrays();
}

void ShaderInit() {
  GLint texLoad;
  string t0 = "../tex/skybox1.jpg";

  if (useSkyBox) {
    progSky.addShader("shader.vert0", GL_VERTEX_SHADER);
    progSky.addShader("shader.frag0", GL_FRAGMENT_SHADER);
    progSky.init();
    progSky.bindAttribute(0, "vertexNormal");
    progSky.bindAttribute(1, "vertexTexCoord");
    progSky.bindAttribute(2, "vertexLocation");
    progSky.linkAndValidate();
    progSky.addSampler();

    texIds.push_back(texLoad = LoadTexture(t0, 0));
    if (!texLoad) {
      cout << "SOIL: Error loading texture from " << t0 << endl;
      exit(-1);
    }
  }

  progCube.addShader("shader.vert1", GL_VERTEX_SHADER);
  progCube.addShader("shader.frag1", GL_FRAGMENT_SHADER);
  progCube.init();
  progCube.linkAndValidate();
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

  if (argc > 1 && (useSkyBox = (string(argv[1]) == "-s"))) {
    ParseObj("skybox.obj", meshSky);
    meshSky.compute_normals();
    for (int i = 0; i < meshSky.num_materials(); ++i)
      Material& material = meshSky.material(i);
    meshSky.loadVBOArrays();
    BufferInit();
  }

  OpenGLInit();
  ShaderInit();

  glutMainLoop();

  return 0;
}
