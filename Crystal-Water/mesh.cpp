/**
 * mesh.cpp
 *
 *    Created on: Apr 16, 2013
 *   Last Update: Apr 16, 2013
 *  Orig. Author: Wade Burch (nolnoch@cs.utexas.edu)
 *  Contributors: [none]
 */


#include <iostream>
#include <vector>
#include <cstring>

#include "./mesh.hpp"

using namespace std;

Mesh::Mesh()
: loaded(false),
  vboArray(NULL),
  iboArrays(NULL),
  textures(NULL),
  nVBO(0),
  nIBOs(0) {
}

void Mesh::Reset() {
  this->freeArrays();
  loaded = false;
  vboArray = NULL;
  iboArrays = NULL;
  textures = NULL;
  nVBO = 0;
  nIBOs = 0;
}

bool Mesh::loadFile(const string& filename) {
  Assimp::Importer importer;

  if (loaded) {
    Reset();
    loaded = false;
  }

  const aiScene *scene = importer.ReadFile(filename,
      aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
  if ((loaded = !scene))
    cout << importer.GetErrorString() << endl;
  else
    this->ProcessScene(scene);

  return loaded;
}

void Mesh::ProcessScene(const aiScene *s) {
  this->textures = new vector<TexInfo>(s->mNumMeshes);
  this->iboArrays = new vector<vector<GLuint> >(s->mNumMeshes);
  this->vboArray = new vector<VBOVertex>;

  /*  Construct internal buffers for VBO and IBOs while loading textures  */

  /* TODO *******************************************************************
   * Meshes may have n materials such that n == 0 || n > 1. Cover these cases.
   *
   * Texture coordinates do not (necessarily) match vertices, as in our test
   * file.  WE ARE SEG FAULTING below.  Fix this.
   *
   * Is that the royal 'we'?
   *
   * I need to shower.
   */

  for (int i = 0; i < s->mNumMeshes; i++) {
    aiMesh *mesh = s->mMeshes[i];
    unsigned int numVertices = mesh->mNumVertices;
    aiColor3D spec(0.0f, 0.0f, 0.0f);
    aiColor3D diff(0.0f, 0.0f, 0.0f);
    aiMaterial* mat = s->mMaterials[i];
    float shiny;

    // Load mesh's texture if it exists and retrieve color properties.
    if (mat->GetTextureCount(aiTextureType_DIFFUSE)) {
      aiString fileName;
      if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &fileName,
          NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
        string fullPath = this->filePath + fileName.data;
        GLint texLoad = this->LoadTexture(fullPath, i);
        if (texLoad) {
          TexInfo tex;
          tex.texID = texLoad;
          tex.texUnit = i;

          this->textures->push_back(tex);
        } else {
          cout << "SOIL: Error loading texture from " << fullPath << endl;
          exit(-1);
        }
      } else {
        cout << "AssImp: Error retrieving texture file name." << endl;
        exit(-1);
      }
    }

    mat->Get(AI_MATKEY_COLOR_SPECULAR, spec);
    mat->Get(AI_MATKEY_COLOR_DIFFUSE, diff);
    mat->Get(AI_MATKEY_SHININESS, shiny);

    // Dump all vertices into the VBO array.
    for (int j = 0; j < numVertices; j++) {
      VBOVertex vbo;

      vbo.position[0] = mesh->mVertices[j][0];
      vbo.position[1] = mesh->mVertices[j][1];
      vbo.position[2] = mesh->mVertices[j][2];
      vbo.normal[0] = mesh->mNormals[j][0];
      vbo.normal[1] = mesh->mNormals[j][1];
      vbo.normal[2] = mesh->mNormals[j][2];
      vbo.texture[0] = mesh->mTextureCoords[j]->x;
      vbo.texture[1] = mesh->mTextureCoords[j]->y;
      vbo.specular[0] = spec.r;
      vbo.specular[1] = spec.g;
      vbo.specular[2] = spec.b;
      vbo.diffuse[0] = diff.r;
      vbo.diffuse[1] = diff.g;
      vbo.diffuse[2] = diff.b;
      vbo.shininess = shiny;
      vbo.align = 0;

      vboArray->push_back(vbo);
    }

    // Add vertex indices to object-specific vectors.
    for (int j = 0; j < mesh->mNumFaces; j++) {
      aiFace& face = mesh->mFaces[j];
      for (int k = 0; k < face.mNumIndices; k++) {
        (*iboArrays)[i].push_back(face.mIndices[k]);
      }
    }
  }

  /*  Save sizes for rendering so we can free the arrays after pushing.  */

  nVBO = vboArray->size();
  nIBOs = iboArrays->size();
  for (int i = 0; i < nIBOs; i++) {
    this->_iboSizes.push_back((*iboArrays)[i].size());
  }
}

GLuint Mesh::LoadTexture(string filename, int texUnit) {
  GLuint id;

  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0 + texUnit);

  id = SOIL_load_OGL_texture(filename.c_str(),
                             SOIL_LOAD_AUTO,
                             SOIL_CREATE_NEW_ID,
                             SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);

  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  return id;
}

void Mesh::setTexturePath(string path) {
  // TODO Perform checking for (and conditional appending of) trailing '/'
  this->filePath = path;
}

void Mesh::freeArrays() {
  vboArray->~vector();
  iboArrays->~vector();
  textures->~vector();
}

int Mesh::vboSize() {
  return this->nVBO;
}

int Mesh::numIBOs() {
  return this->nIBOs;
}

std::vector<int>& Mesh::iboSizes() {
  return this->_iboSizes;
}

std::vector<VBOVertex>& Mesh::getVBOVertexArray() {
  return *(this->vboArray);
}

std::vector<vector<GLuint> >& Mesh::getIBOIndexArrays() {
  return *(this->iboArrays);
}

std::vector<TexInfo>& Mesh::getTextures() {
  return *(this->textures);
}
