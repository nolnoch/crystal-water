/*
 * shaderobj.cpp
 *
 *    Created on: Apr 8, 2013
 *   Last Update: Apr 8, 2013
 *  Orig. Author: Wade Burch (nolnoch@cs.utexas.edu)
 *  Contributors: <none>
 */

#include "./shaderobj.hpp"

using namespace std;

Shader::Shader(string fName, int type)
: fileName(fName),
  shaderId(0),
  shaderType(type),
  sourceString(""),
  valid(0) {
  this->fileToString();
}

void Shader::setId(GLenum idAssigned) {
  shaderId = idAssigned;
}

GLenum Shader::id() {
  return this->shaderId;
}

GLuint Shader::type() {
  return this->shaderType;
}

string& Shader::source() {
  return this->sourceString;
}

int Shader::isValid() {
  return this->valid;
}

// Read shader sources from files.
void Shader::fileToString() {
  fstream shaderFile(this->fileName.c_str(), ios::in);

  if (shaderFile.is_open()) {
    ostringstream buffer;
    buffer << shaderFile.rdbuf();
    this->sourceString = string(buffer.str());
    this->valid = 1;
  }
}

Shader::~Shader() {
  // TODO Auto-generated destructor stub
}

