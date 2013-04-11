#include <iostream>
#include <vector>
#include <cstring>

#include "./mesh.h"

using namespace std;

Mesh::Mesh() {
  _cur_mtl = -1;
  loaded = false;
  vboArray = NULL;
  iboArrays = NULL;
  textureTemp = NULL;
  nVBO = 0;
  nIBOs = 0;
}

/**
 * This will be called by the obj parser.
 */
void Mesh::AddVertex(const glm::vec3& v) {
  this->_vertices.push_back(v);
}

/**
 * This **may** be called by the obj parser.
 * We can support vertex weighting for "trimming curves" at a later date.
 * Until then, shorten the glm::vec4 to a glm::vec3, and deal with it normally.
 */
void Mesh::AddVertex(const glm::vec4& v) {
  glm::vec3 v3 = glm::vec3(v[0], v[1], v[2]);
  this->_vertices.push_back(v3);
}

/**
 * This will be called by the obj parser.
 */
void Mesh::AddTextureVertex(const glm::vec3& v) {
  this->_textures.push_back(v);
}

/**
 * This **may** be called by the obj parser (forward compatible).
 */
void Mesh::AddNormalVertex(const glm::vec3& v) {
//  this->_normals.push_back(v);
}

/**
 * p is the list of indices of vertices for this polygon.  For example,
 * if p = {0, 1, 2} then the polygon is a triangle with the zeroth, first and
 * second vertices added using AddVertex.
 *
 * pt is the list of texture indices for this polygon, similar to the
 * actual vertices described above.
 */
void Mesh::AddPolygon(const std::vector<int>& p, const std::vector<int>& pt) {
  Face f = Face(p, pt, VT);

  this->_faces.push_back(f);

  // updates the poly2mat map
  _polygon2material.push_back(_cur_mtl);
}

/**
 * Computes the normals for each vertex.
 * Whether the normals are loaded from the obj file or generated here,
 * this function must be called before the Mesh may be rendered (safety).
 */
void Mesh::compute_normals() {
  int size = this->_vertices.size();
  vector<glm::vec3> sumNormals = vector<glm::vec3>(size, glm::vec3(0, 0, 0));
  vector<int> numNormals = vector<int>(size, 0);

  // It's possible that the normals were already read in by the obj parser.
  if (this->_normals.size() != 0) {
    cout << "Normals included. Skipping computation." << endl;
    this->loaded = true;
    return;
  }

  // The normal for each face is the cross product of two vectors
  // defined in counter-clockwise order.
  for (int i = 0; i < this->_faces.size(); i++) {
    Face f = this->_faces[i];
    vector<int> v = f.vertices();

    glm::vec3 v0 = this->_vertices[v[0]];
    glm::vec3 v1 = this->_vertices[v[1]];
    glm::vec3 v2 = this->_vertices[v[2]];

    glm::vec3 vecA = v2 - v1;
    glm::vec3 vecB = v0 - v1;

    f.setNormal(glm::normalize(glm::cross(vecA, vecB)));

    // The normal for each Vertex is the average of the normals of
    // all faces containing that vertex. Sum them here.
    for (vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
      sumNormals[*it] += f.getNormal();
      numNormals[*it]++;
    }
  }

  // Now divide each vertex's sumNormals by the numNormals contributed.
  // Each normal should already be normalized (from faces' unit normals).
  for (int i = 0; i < sumNormals.size(); i++)
    this->_normals.push_back(glm::vec3(sumNormals[i].x / numNormals[i],
                                       sumNormals[i].y / numNormals[i],
                                       sumNormals[i].z / numNormals[i]));

  this->loaded = true;
}

/**
 * Set up the Mesh-wide VBO array and an IBO array for each material.
 */
void Mesh::loadVBOArrays() {
  bool textures = this->_textures.size() != 0;
  bool materials = this->_materials.size() != 0;
  int matIdx, prevMatIdx, iboIdx = 0;

  // May not proceed until the normals have been computed or verified.
  if (!loaded) {
    cout << "Computing normals." << endl;
    this->compute_normals();
  }

  /*  Create VBO and array of IBO arrays.  */

  // Create array of "n" IBO index arrays where n = unique materials used.
  int uniqueMats = this->_usedMaterials.size();
  iboArrays = new vector<vector<GLuint> >(uniqueMats ? uniqueMats : 1);
  vboArray = new vector<VBOVertex>;

  // For each face, determine material used and load IBO for that material with
  // the vertices specified. Converts from polygons of v > 3 to triangles.

  vector<glm::vec3>& vV = this->_vertices;
  vector<glm::vec3>& vN = this->_normals;
  vector<glm::vec3>& vT = this->_textures;
  vector<Face>& vF = this->_faces;

  for (int j = 0; j < vF.size(); j++) {
    vector<int> verts = vF[j].vertices();
    vector<int> texes = vF[j].textures();
    vector<int> norms = vF[j].normals();
    int k = 0;

    // Based on material for this polygon, which VBO array do we load?
    matIdx = materials ? polygon2material(j) : 0;
    for (int m = 0; m < uniqueMats; m++) {
      if (matIdx == this->_usedMaterials[m])
        k = m;
    }

    // Construct a VBO vertex for requisite three vertices.
    for (int i = 0; i < 3; i++) {
      VBOVertex vbo;
      int idV = verts[i];
      int idN = norms[i];
      int idT = texes[i];

      vbo.position[0] = vV[idV][0];
      vbo.position[1] = vV[idV][1];
      vbo.position[2] = vV[idV][2];
      vbo.normal[0] = vN[idV][0];
      vbo.normal[1] = vN[idV][1];
      vbo.normal[2] = vN[idV][2];
      vbo.texture[0] = vT[idT][0];
      vbo.texture[1] = 1.0f - vT[idT][1];

      vboArray->push_back(vbo);
      (*iboArrays)[k].push_back(iboIdx++);
    }

    // Construct a VBO vertex for any additional vertices.
    for (int i = 3; i < verts.size(); i++) {
      VBOVertex vbo1;
      int idV = verts[i-1];
      int idN = norms[i-1];
      int idT = texes[i-1];

      vbo1.position[0] = vV[idV][0];
      vbo1.position[1] = vV[idV][1];
      vbo1.position[2] = vV[idV][2];
      vbo1.normal[0] = vN[idV][0];
      vbo1.normal[1] = vN[idV][1];
      vbo1.normal[2] = vN[idV][2];
      vbo1.texture[0] = vT[idT][0];
      vbo1.texture[1] = 1.0f - vT[idT][1];

      vboArray->push_back(vbo1);
      (*iboArrays)[k].push_back(iboIdx++);

      VBOVertex vbo2;
      idV = verts[i];
      idN = norms[i];
      idT = texes[i];

      vbo2.position[0] = vV[idV][0];
      vbo2.position[1] = vV[idV][1];
      vbo2.position[2] = vV[idV][2];
      vbo2.normal[0] = vN[idV][0];
      vbo2.normal[1] = vN[idV][1];
      vbo2.normal[2] = vN[idV][2];
      vbo2.texture[0] = vT[idT][0];
      vbo2.texture[1] = 1.0f - vT[idT][1];

      vboArray->push_back(vbo2);
      (*iboArrays)[k].push_back(iboIdx++);

      VBOVertex vbo3;
      idV = verts[0];
      idN = norms[0];
      idT = texes[0];

      vbo3.position[0] = vV[idV][0];
      vbo3.position[1] = vV[idV][1];
      vbo3.position[2] = vV[idV][2];
      vbo3.normal[0] = vN[idV][0];
      vbo3.normal[1] = vN[idV][1];
      vbo3.normal[2] = vN[idV][2];
      vbo3.texture[0] = vT[idT][0];
      vbo3.texture[1] = 1.0f - vT[idT][1];

      vboArray->push_back(vbo3);
      (*iboArrays)[k].push_back(iboIdx++);
    }
  }

  /*  Save array sizes for rendering so we can free the array memory.  */

  nVBO = vboArray->size();
  nIBOs = iboArrays->size();
  for (int i = 0; i < nIBOs; i++) {
    this->_iboSizes.push_back((*iboArrays)[i].size());
  }
}

void Mesh::freeArrays() {
  vboArray->~vector();
  iboArrays->~vector();
}
