/*
 * program.cpp
 *
 *    Created on: Apr 8, 2013
 *   Last Update: Apr 8, 2013
 *  Orig. Author: Wade Burch (nolnoch@cs.utexas.edu)
 *  Contributors: <none>
 */

#include "./program.hpp"

using namespace std;

Program::Program()
: programId(0),
  samplers(0),
  stage(0) {
}

Program::~Program() {
  if (samplers)
    delete samplers;
}

int Program::addShader(string fName, int type) {
  int validFile;

  this->shaders.push_back(Shader(fName, type));
  validFile = this->shaders.back().isValid();

  if (!validFile)
    this->shaders.pop_back();
  else
    this->stage = 1;

  return validFile;
}

void Program::addDefaultShaders() {
  this->addShader("shader.vert", GL_VERTEX_SHADER);
  this->addShader("shader.frag", GL_FRAGMENT_SHADER);
  // this->addShader("shader.geom", GL_GEOMETRY_SHADER);
}

void Program::addSampler() {
  GLuint sample;

  if (!samplers)
    this->samplers = new vector<GLuint>();

  glGenSamplers(1, &sample);

  this->samplers->push_back(sample);
}

void Program::init() {
  int numShaders = this->shaders.size();

  if (!numShaders || !stage) {
    cout << "No shader files associated with program. Aborting..." << endl;
    return;
  }

  // Init program.
  this->programId = glCreateProgram();

  for (int i = 0; i < numShaders; i++) {
    // Init shader
    Shader& shad = this->shaders[i];
    shad.setId(glCreateShader(shad.type()));

    // Load shader sources.
    const GLchar *shaderSource = shad.source().c_str();
    glShaderSource(shad.id(), 1, &shaderSource, NULL);

    // Compile shader from source.
    glCompileShader(shad.id());

    // Display any problems with shader compilation.
    displayLogShader(shad.id());

    // Attach compiled shader to program.
    glAttachShader(this->programId, shad.id());
  }

  this->stage = 2;
}

void Program::bindAttribute(int location, string name) {
  if (stage < 2) {
    cout << "Invalid binding. Must init first." << endl;
    return;
  } else if (stage >= 4) {
    cout << "This attribute binding will not take effect until next linking."
         << endl;
  }

  // Bind explicit attribute locations before linking.
  glBindAttribLocation(this->programId, location, name.c_str());

  this->stage = 3;
}

GLint Program::linkAndValidate() {
  if (stage < 2) {
    cout << "Invalid linking. Must init (and bind attributes) first." << endl;
    return 0;
  }

  GLint programValid;
  // Link the compiled and attached program to this code.
  glLinkProgram(this->programId);

  // Verify program compilation and linkage.
  glValidateProgram(this->programId);
  glGetProgramiv(this->programId, GL_VALIDATE_STATUS, &programValid);
  displayLogProgram(this->programId);

  this->stage = programValid ? 5 : 4;

  return programValid;
}

void Program::enable() {
  if (stage < 5) {
    if (stage < 4)
      cout << "Program not ready to enable: must link before use." << endl;
    else
      cout << "Program not ready to enable: linked but not valid." << endl;

    return;
  }

  glUseProgram(this->programId);
}

void Program::disable() {
  glUseProgram(0);
}

void Program::setUniform(int type, string name, float n) {
  GLint loc = glGetUniformLocation(this->programId, name.c_str());

  if (type == GL_FLOAT) {
        glUniform1f(loc, n);
  } else if (type == GL_INT) {
        glUniform1i(loc, static_cast<int>(n));
  }
}

void Program::setUniformv(int count, int type, string name, const float *n) {
  GLint loc = glGetUniformLocation(this->programId, name.c_str());

  if (type == GL_FLOAT) {
    switch (count) {
      case 1:
        glUniform1fv(loc, 1, n);
        break;
      case 2:
        glUniform2fv(loc, 1, n);
        break;
      case 3:
        glUniform3fv(loc, 1, n);
        break;
      case 4:
        glUniform4fv(loc, 1, n);
        break;
      default:
        break;
    }
  } else if (type == GL_INT) {
    switch (count) {
      case 1:
        glUniform1iv(loc, 1, reinterpret_cast<const int *>(n));
        break;
      case 2:
        glUniform2iv(loc, 1, reinterpret_cast<const int *>(n));
        break;
      case 3:
        glUniform3iv(loc, 1, reinterpret_cast<const int *>(n));
        break;
      case 4:
        glUniform4iv(loc, 1, reinterpret_cast<const int *>(n));
        break;
      default:
        break;
    }
  }
}

void Program::setUniformMatrix(int size, string name, float *m) {
  GLint loc = glGetUniformLocation(this->programId, name.c_str());

  if (size == 4) {
    glUniformMatrix4fv(loc, 1, GL_FALSE, m);
  } else if (size == 3) {
    glUniformMatrix3fv(loc, 1, GL_FALSE, m);
  }
}

void Program::setTexture(string samplerName, GLuint texUnit,
              GLuint texId, int sampler) {
  GLint loc = glGetUniformLocation(this->programId, samplerName.c_str());

  glActiveTexture(GL_TEXTURE0 + texUnit);
  glBindSampler(GL_TEXTURE0 + texUnit, (*this->samplers)[sampler]);

  glBindTexture(GL_TEXTURE_2D, texId);
  glUniform1i(loc, texUnit);
}

GLenum Program::getProgramId() {
  return this->programId;
}

void Program::displayLogProgram(GLenum program) {
  GLsizei logLength;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

  GLsizei MAXLENGTH = 1 << 30;
  GLchar *logBuffer = new GLchar[MAXLENGTH];
  glGetProgramInfoLog(program, MAXLENGTH, &logLength, logBuffer);
  if (strlen(logBuffer)) {
    cout << "************ Begin Program Log ************" << endl;
    cout << logBuffer << endl;
    cout << "************* End Program Log *************" << endl;
  }
  delete[] logBuffer;
}

void Program::displayLogShader(GLenum shader) {
  GLsizei logLength;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

  GLsizei MAXLENGTH = 1 << 30;
  GLchar *logBuffer = new GLchar[MAXLENGTH];
  glGetShaderInfoLog(shader, MAXLENGTH, &logLength, logBuffer);
  if (strlen(logBuffer)) {
    cout << "************ Begin Shader Log ************" << endl;
    cout << logBuffer << endl;
    cout << "************* End Shader Log *************" << endl;
  }
  delete[] logBuffer;
}
