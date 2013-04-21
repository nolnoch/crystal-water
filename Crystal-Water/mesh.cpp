/**
 * mesh.cpp
 *
 *    Created on: Apr 16, 2013
 *   Last Update: Apr 21, 2013
 *  Orig. Author: Wade Burch (nolnoch@cs.utexas.edu)
 *  Contributors: [none]
 */


#include <iostream>
#include <vector>
#include <cstring>

#include "./mesh.hpp"

using namespace std;

/**
 * Default constructor,
 */
Mesh::Mesh()
: loaded(false),
  vboArray(NULL),
  iboArrays(NULL),
  textures(NULL),
  nVBO(0),
  nIBOs(0) {
}

/**
 * Resets the Mesh to an empty state.
 */
void Mesh::Reset() {
  this->freeArrays();
  loaded = false;
  vboArray = NULL;
  iboArrays = NULL;
  textures = NULL;
  nVBO = 0;
  nIBOs = 0;
}

/**
 * Default destructor.
 */
Mesh::~Mesh() {
  textures->~vector();
}

/**
 * Loads an object or mesh file from many, many formats. To see a complete
 * list, visit: http://assimp.sourceforge.net/main_features_formats.html.
 * @param filename - the file (including path) to be loaded
 * @return true if load is successful, false if it fails
 */
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

/**
 * The main processing of the class. This takes the scene object returned from
 * Assimp and interleaves the vertex and material data into a single VBO and
 * an IBO for each sub-mesh. These meshes are delineated by material changes
 * in the loaded file. Any included texture files will also be loaded here.
 * [Currently only supports diffuse textures]
 * @param s - the Assimp scene object
 */
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
    int lastIdx;
    bool tex;

    // Load texture.
    int m = i + 1;
    if (s->HasMaterials() && s->mNumMaterials > 1) {
      aiMaterial* mat = s->mMaterials[m];
      aiString fileName;
      if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &fileName) == AI_SUCCESS) {
        string fullPath = this->filePath + fileName.data;
        GLint texLoad = this->LoadTexture(fullPath, m);
        if (texLoad) {
          TexInfo texInfo;
          texInfo.texID = texLoad;
          texInfo.texUnit = m;
          texInfo.present = true;

          this->textures->push_back(texInfo);
          tex = true;
          cout << "Loaded " << fileName.C_Str() << "." << endl;
        } else {
          cout << "SOIL: Error loading texture from " << fullPath << endl;
        }
      } else {
        cout << "AssImp: Error retrieving texture file name from material "
             << m << "." << endl;
        TexInfo texInfo;
        texInfo.present = false;
        this->textures->push_back(texInfo);
        tex = false;
      }

      // Load material.
      if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, diff) != AI_SUCCESS)
        cout << "No diffuse color found in material " << m << "." << endl;
      if (mat->Get(AI_MATKEY_COLOR_SPECULAR, spec) != AI_SUCCESS)
        cout << "No specular color found in material " << m << "." << endl;
      if (mat->Get(AI_MATKEY_SHININESS, shiny) != AI_SUCCESS)
        cout << "No shininess value found in material " << m << "." << endl;
    }

    lastIdx = 0;
    for (int n = 0; n < i; n++) {
      lastIdx += (*iboArrays)[n].size();
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
        vbo.texture[0] = tex ? mesh->mTextureCoords[0][texCIdx].x : 0;
        vbo.texture[1] = tex ? mesh->mTextureCoords[0][texCIdx].y : 0;
        vbo.diffuse[0] = diff.r;
        vbo.diffuse[1] = diff.g;
        vbo.diffuse[2] = diff.b;
        vbo.specular[0] = spec.r;
        vbo.specular[1] = spec.g;
        vbo.specular[2] = spec.b;
        vbo.shininess = shiny;
        vbo.align = 0;

        vboArray->push_back(vbo);
        (*iboArrays)[i].push_back(face.mIndices[k] + lastIdx);
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

/**
 * Uses SOIL to load textures discovered by Assimp in the loading of an object
 * or mesh file's materials.
 * @param filename - the filename of the compressed image texture
 * @param texUnit - the texture unit to bind this texture under (mesh index)
 * @return the texture ID assigned by OpenGL via SOIL
 */
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

/**
 * Allows user to specify a non-local path to the directory where the textures
 * are stored.
 * @param path - string value of the path to the non-local texture directory
 */
void Mesh::setTexturePath(string path) {
  // TODO Perform checking for (and conditional appending of) trailing '/'
  this->filePath = path;
}

/**
 * Allows explicit freeing of the texture arrays once VBO and IBOs are loaded
 * without losing all state information of the mesh.
 */
void Mesh::freeArrays() {
  vboArray->~vector();
  iboArrays->~vector();
}

/**
 * Retrieves stored size of the VBO array.
 * @return VBO array size
 */
int Mesh::vboSize() {
  return this->nVBO;
}

/**
 * Retrieves stored number of IBO arrays.
 * @return number of IBO arrays
 */
int Mesh::numIBOs() {
  return this->nIBOs;
}

/**
 * Retrieves array of IBO sizes.
 * @return STL vector of sizes for each IBO
 */
std::vector<int>& Mesh::iboSizes() {
  return this->_iboSizes;
}

/**
 * Retrieves the interleaved VBO array.
 * @return a reference to the VBO array
 */
std::vector<VBOVertex>& Mesh::getVBOVertexArray() {
  return *(this->vboArray);
}

/**
 * Retrieves all IBO arrays constructed.
 * @return reference to an STL vector of all IBO arrays
 */
std::vector<vector<GLuint> >& Mesh::getIBOIndexArrays() {
  return *(this->iboArrays);
}

/**
 * Retrieves all loaded texture IDs with associated texture units.
 * @return reference to an STL vector of TexInfo pairings
 */
std::vector<TexInfo>& Mesh::getTextures() {
  return *(this->textures);
}
