#ifndef __MESH_H__
#define __MESH_H__

#include <GL/glew.h>
#include "./material.h"

// Basic vertex attributes aligned to a 32-byte block.
typedef struct {
  GLfloat normal[3];
  GLfloat texture[2];
  GLfloat position[3];
} VBOVertex;

// This class represents a mesh, which contains vertices, polygons,
// and material properties for each polygon.
class Mesh {
 public:
  Mesh();

  void AddVertex(const glm::vec3& v);
  void AddVertex(const glm::vec4& v);
  void AddTextureVertex(const glm::vec3& v);
  void AddNormalVertex(const glm::vec3& v);
  void AddPolygon(const std::vector<int>& p, const std::vector<int>& pt);

  void render();
  void loadVBOArrays();
  void freeArrays();

  int vboSize() {
    return nVBO;
  }

  int numIBOs() {
    return nIBOs;
  }

  std::vector<int>& iboSizes() {
    return _iboSizes;
  }

  std::vector<VBOVertex>& getVBOVertexArray() {
    return *(this->vboArray);
  }

  std::vector<vector<GLuint> >& getIBOIndexArrays() {
    return *(this->iboArrays);
  }

  std::vector<int>& getUsedMaterials() {
    return this->_usedMaterials;
  }

  std::vector<Material>& getMaterials() {
    return this->_materials;
  }

  void new_material(int material_idx, const std::string& name) {
    _materials.push_back(Material(name));
  }

  void set_cur_material(const string& name) {
    for (int i = 0; i < _materials.size(); ++i) {
      if (_materials[i].name() == name) {
        set_cur_material(i);
      }
    }
  }

  void set_cur_material(int cur_mtl) {
    int found = 0;
    _cur_mtl = cur_mtl;
    for (int i = 0; !found && i < _usedMaterials.size(); ++i) {
      if (_usedMaterials[i] == cur_mtl)
        found++;
    }

    if (!found)
      _usedMaterials.push_back(cur_mtl);
  }

  void set_ambient(int material_idx, const glm::vec3& ambient) {
    _materials[material_idx].set_ambient(ambient);
  }

  void set_diffuse(int material_idx, const glm::vec3& diffuse) {
    _materials[material_idx].set_diffuse(diffuse);
  }

  void set_specular(int material_idx, const glm::vec3& specular) {
    _materials[material_idx].set_specular(specular);
  }

  void set_specular_coeff(int material_idx, const float& coeff) {
    _materials[material_idx].set_specular_coeff(coeff);
  }

  /**
   * You don't need to modify this function, but it will be useful
   * in your Display() function.
   *
   * Given a polygon index, returns the index of the material
   * that should be used for that polygon.
   */
  int polygon2material(int polygon_idx) const {
    return _polygon2material[polygon_idx];
  }

  const Material& material(int i) const { return _materials[i]; }

  Material& material(int i) { return _materials[i]; }

  int num_materials() const { return _materials.size(); }

  void compute_normals();

 private:
  std::vector<glm::vec3> _vertices;
  std::vector<glm::vec3> _textures;
  std::vector<glm::vec3> _normals;
  std::vector<Face> _faces;

  std::vector<VBOVertex> *vboArray;
  std::vector<vector<GLuint> > *iboArrays;
  std::vector<glm::vec3> *textureTemp;
  std::vector<int> _iboSizes;
  int nVBO, nIBOs;

  std::vector<Material> _materials;
  std::vector<int> _usedMaterials;
  std::vector<int> _polygon2material;
  int _cur_mtl;

  bool loaded;
};

#endif
