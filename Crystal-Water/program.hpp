/*
 * program.hpp
 *
 *    Created on: Apr 8, 2013
 *   Last Update: Apr 8, 2013
 *  Orig. Author: Wade Burch (nolnoch@cs.utexas.edu)
 *  Contributors: <none>
 */

#ifndef PROGRAM_HPP_
#define PROGRAM_HPP_

#include <GL/glew.h>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "./shaderobj.hpp"


class Program {
 public:
  Program();
  virtual ~Program();

  int addShader(std::string fName, int type);
  void addDefaultShaders();
  void addSampler();

  void init();
  void bindAttribute(int location, std::string name);
  GLint linkAndValidate();
  void enable();
  void disable();

  void setUniform(int type, std::string name, float n);
  void setUniformv(int count, int type, std::string name, const float *n);
  void setUniformMatrix(int size, std::string name, float *m);
  void setTexture(std::string name, GLuint texUnit, GLuint texId, int sampler);

  GLenum getProgramId();

  void displayLogProgram(GLenum program);
  void displayLogShader(GLenum shader);

 private:
  GLenum programId;
  std::vector<Shader> shaders;
  std::vector<GLuint> *samplers;
  int stage;
};

#endif /* PROGRAM_H_ */
