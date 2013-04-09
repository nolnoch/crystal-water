/**
 * shaderobj.cpp
 *
 *    Created on: Apr 8, 2013
 *   Last Update: Apr 8, 2013
 *  Orig. Author: Wade Burch (nolnoch@cs.utexas.edu)
 *  Contributors: [none]
 */

#include "./shaderobj.hpp"

using namespace std;


/**
 * Primary Constructor.
 */
Shader::Shader(string fName, int type)
: fileName(fName),
  shaderId(0),
  shaderType(type),
  sourceString(""),
  valid(0) {
  this->fileToString();
}

/**
 * Default Destructor.
 */
Shader::~Shader() {
  // Not necessary (yet).
}

/**
 * Assigns the ID given by shader init.
 * @param idAssigned - the init-provided ID
 */
void Shader::setId(GLenum idAssigned) {
  shaderId = idAssigned;
}

/**
 * Accessor function for the assigned ID.
 * @return the init-provided ID
 */
GLenum Shader::id() {
  return this->shaderId;
}

/**
 * Accessor function for the shader type.
 * @return the shader type
 */
GLuint Shader::type() {
  return this->shaderType;
}

/**
 * Accessor function for the shader source parsed into a string.
 * @return the string representation of the shader source
 */
string& Shader::source() {
  return this->sourceString;
}

/**
 * Used by Program object to check valid loading of the shader file.
 * @return 1 if successful or 0 if invalid file
 */
int Shader::isValid() {
  return this->valid;
}

/**
 * Converts the shader source file to a string for loading by the program.
 */
void Shader::fileToString() {
  fstream shaderFile(this->fileName.c_str(), ios::in);

  if (shaderFile.is_open()) {
    ostringstream buffer;
    buffer << shaderFile.rdbuf();
    this->sourceString = string(buffer.str());
    this->valid = 1;
  }
}

