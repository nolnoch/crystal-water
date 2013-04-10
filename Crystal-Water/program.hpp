/**
 * program.hpp
 *
 *    Created on: Apr 8, 2013
 *   Last Update: Apr 8, 2013
 *  Orig. Author: Wade Burch (nolnoch@cs.utexas.edu)
 *  Contributors: [none]
 *
 *  This class was created to automate (and generally make easier) the use
 *  of GLSL programs through the GLEW extension libraries.
 *
 *  Notes:
 *
 *    You may call addShader for every file you want to attach to each
 *    program, and all will be processed and used for that program's
 *    lifespan.  Create multiple programs to separate your shaders for
 *    modular loading and use.
 *
 *    The construction and usage of the program is managed in sequence-
 *    protected stages. Error messages will be printed if functions are
 *    called out of order.  The correct order is:
 *
 *          addShader()         // as many as you need
 *          init()              // called once
 *          [bindAttribute()]   // only if you wish, for VAO/VBOs
 *          linkAndValidate()   // must be run before using program
 *          addSampler()        // called after program is linked for safety
 *          enable()            // to actually use
 *          disable()           // when you're done
 *
 *    At the moment, the samplers may only be specified when calling
 *    setTexure() by remembering the order in which you added them with
 *    addSampler().
 *
 *  This is a work in progress and will be continually improved as I use it.
 *
 *  Feel free to share, expand, and modify as you see fit with attribution
 *  to the original author (me) and any who have added since.
 *
 *  -Wade Burch
 */

#ifndef PROGRAM_HPP_
#define PROGRAM_HPP_

#include <GL/glew.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
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
  void setTexture(std::string samplerName, GLuint texUnit, GLuint texId,
      int sampler);

  GLenum getProgramId();

  void displayLogProgram();
  void displayLogShader(GLenum shader);

 private:
  GLenum programId;
  std::vector<Shader> shaders;
  std::vector<GLuint> *samplers;
  int stage;
};

#endif /* PROGRAM_H_ */
