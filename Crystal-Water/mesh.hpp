/**
 * mesh.hpp
 *
 *    Created on: Apr 16, 2013
 *   Last Update: Apr 16, 2013
 *  Orig. Author: Wade Burch (nolnoch@cs.utexas.edu)
 *  Contributors: [none]
 */


#ifndef __MESH_HPP__
#define __MESH_HPP__

#include <GL/glew.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>
#include <SOIL/SOIL.h>

#include <string>


// Vertex attributes aligned to a 64-byte block.
typedef struct {
  GLfloat position[3];
  GLfloat normal[3];
  GLfloat texture[2];
  GLfloat diffuse[3];
  GLfloat specular[3];
  GLfloat shininess;
  GLfloat align;
} VBOVertex;

typedef struct {
  GLint texUnit;
  GLuint texID;
} TexInfo;


class Mesh {
 public:
  Mesh();
  ~Mesh();

  bool loadFile(const std::string& filename);

  int vboSize();
  int numIBOs();
  std::vector<int>& iboSizes();
  std::vector<TexInfo>& getTextures();
  std::vector<VBOVertex>& getVBOVertexArray();
  std::vector<std::vector<GLuint> >& getIBOIndexArrays();

  void setTexturePath(std::string path);
  void freeArrays();

 private:
  std::string filePath;
  std::vector<TexInfo> *textures;
  std::vector<VBOVertex> *vboArray;
  std::vector<std::vector<GLuint> > *iboArrays;
  std::vector<int> _iboSizes;
  int nVBO, nIBOs;
  bool loaded;

  void ProcessScene(const aiScene *s);
  GLuint LoadTexture(std::string filename, int texUnit);

  void Reset();
};

#endif
