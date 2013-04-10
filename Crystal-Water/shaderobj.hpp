/**
 * shaderobj.hpp
 *
 *    Created on: Apr 8, 2013
 *   Last Update: Apr 8, 2013
 *  Orig. Author: Wade Burch (nolnoch@cs.utexas.edu)
 *  Contributors: [none]
 *
 *  This was created specifically for my Program class for easier use of
 *  GLSL shaders with the GLEW extension libraries.
 *
 *  This class stores each shader file as a Shader object with the data
 *  necessary to load, compile, attach, link, and use it in an automated
 *  framework.
 *
 *  Feel free to share, expand, and modify as you see fit with attribution
 *  to the original author (me) and any who have added since.
 *
 *  -Wade Burch
 */

#ifndef SHADEROBJ_HPP_
#define SHADEROBJ_HPP_

#include <GL/glew.h>
#include <string>
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
