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

Mesh::~Mesh() {
  textures->~vector();
}

bool Mesh::loadFile(const string& filename) {
  Assimp::Importer importer;

  if (loaded) {
    Reset();
    loaded = false;
  }

  const aiScene *scene = importer.ReadFile(filename, aiProcess_Triangulate |
      aiProcess_GenNormals);
  if (!(loaded = scene))
    cout << importer.GetErrorString() << endl;
  else
    this->ProcessScene(scene);

  return loaded;
}

void Mesh::ProcessScene(const aiScene *s) {
  this->iboArrays = new vector<vector<GLuint> >(s->mNumMeshes);
  this->vboArray = new vector<VBOVertex>;
  this->textures = new vector<TexInfo>;

  /**************************************************************************
   * According to doc, materials will correspond to meshes in a 1-to-1 mapping.
   *
   * Not in the doc(!@#$%^&), but the correct material object is 1, not 0.
   *
   * Vertex data is being interleaved in-order, but let's continue to use the
   * IBOs for forward flexibility.
   */

  /*  Construct internal buffers for VBO and IBOs while loading textures  */

  for (int i = 0; i < s->mNumMeshes; i++) {
    aiMesh *mesh = s->mMeshes[i];
    unsigned int numVertices = mesh->mNumVertices;
    aiColor3D spec(0.5f, 0.1f, 0.2f);
    aiColor3D diff(0.5f, 0.1f, 0.2f);
    float shiny = 42;                                 // The answer to LTU&E.
    int verts = 0;

    // Load texture.
    if (s->HasMaterials() && s->mNumMaterials > 1) {
      aiMaterial* mat = s->mMaterials[1];
      aiString fileName;
      if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &fileName) == AI_SUCCESS) {
        string fullPath = this->filePath + fileName.data;
        GLint texLoad = this->LoadTexture(fullPath, i);
        if (texLoad) {
          TexInfo texInfo;
          texInfo.texID = texLoad;
          texInfo.texUnit = i;

          this->textures->push_back(texInfo);
          cout << "Loaded " << fileName.C_Str() << "." << endl;
        } else {
          cout << "SOIL: Error loading texture from " << fullPath << endl;
        }
      } else {
        cout << "AssImp: Error retrieving texture file name from material "
             << i << "." << endl;
      }

      // Load material.
      if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, diff) != AI_SUCCESS)
        cout << "No diffuse color found in material " << i << "." << endl;
      if (mat->Get(AI_MATKEY_COLOR_SPECULAR, spec) != AI_SUCCESS)
        cout << "No specular color found in material " << i << "." << endl;
      if (mat->Get(AI_MATKEY_SHININESS, shiny) != AI_SUCCESS)
        cout << "No shininess value found in material " << i << "." << endl;
    }

    // Add vertex indices to object-specific vectors.
    for (int j = 0; j < mesh->mNumFaces; j++) {
      aiFace& face = mesh->mFaces[j];

      for (int k = 0; k < face.mNumIndices; k++) {
        VBOVertex vbo;
        GLint vertIdx = face.mIndices[k];
        GLint texCIdx = face.mIndices[k];
        GLint normIdx = face.mIndices[k];

        vbo.position[0] = mesh->mVertices[vertIdx][0];
        vbo.position[1] = mesh->mVertices[vertIdx][1];
        vbo.position[2] = mesh->mVertices[vertIdx][2];
        vbo.normal[0] = mesh->mNormals[normIdx][0];
        vbo.normal[1] = mesh->mNormals[normIdx][1];
        vbo.normal[2] = mesh->mNormals[normIdx][2];
        vbo.texture[0] = mesh->mTextureCoords[0][texCIdx].x;
        vbo.texture[1] = mesh->mTextureCoords[0][texCIdx].y;
        vbo.diffuse[0] = diff.r;
        vbo.diffuse[1] = diff.g;
        vbo.diffuse[2] = diff.b;
        vbo.specular[0] = spec.r;
        vbo.specular[1] = spec.g;
        vbo.specular[2] = spec.b;
        vbo.shininess = shiny;
        vbo.align = 0;

        vboArray->push_back(vbo);
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
