/**
 * mesh.hpp
 *
 *    Created on: Apr 16, 2013
 *   Last Update: Apr 21, 2013
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


/**
 * Interleaved position, normal, texture, and material data for the VBO.
 * Aligned to a 64-byte block for performance.
 */
typedef struct {
  GLfloat position[3];      /**< Vertex position */
  GLfloat normal[3];        /**< Vertex normal */
  GLfloat texture[2];       /**< Vertex texture coordinates */
  GLfloat diffuse[3];       /**< Vertex material diffuse property */
  GLfloat specular[3];      /**< Vertex material specular property */
  GLfloat shininess;        /**< Vertex material shininess coefficient */
  GLfloat align;            /**< 4 empty bytes for alignment */
} VBOVertex;

/**
 * Container for managing loaded textures.
 */
typedef struct {
  GLint texUnit;            /**< Texture unit in which texture was bound */
  GLuint texID;             /**< Generated texture ID */
} TexInfo;


/**
 * Class representing a loaded object and material file. Somewhat misnamed in
 * that the driver for the class, Assimp, subdivides such files into multiple
 * meshes for each material. This is largely abstracted, however, as this
 * detail is not visible from outside the class.
 */
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
