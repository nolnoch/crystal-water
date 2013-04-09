/**
 * shaderobj.hpp
 *
 *    Created on: Apr 8, 2013
 *   Last Update: Apr 8, 2013
 *  Orig. Author: Wade Burch (nolnoch@cs.utexas.edu)
 *  Contributors: [none]
 */

#ifndef SHADEROBJ_HPP_
#define SHADEROBJ_HPP_

#include <GL/glew.h>
#include <cstring>
#include <fstream>
#include <sstream>

class Shader {
 public:
  Shader(std::string fName, int type);
  virtual ~Shader();

  void setId(GLenum idAssigned);

  GLenum id();
  GLuint type();
  std::string& source();
  int isValid();

 private:
  GLenum shaderId;
  GLuint shaderType;
  std::string fileName;
  std::string sourceString;
  int valid;

  void fileToString();
};

#endif /* SHADER_HPP_ */
