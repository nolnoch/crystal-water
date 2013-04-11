#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "./face.h"

class Material {
 public:
  Material();
  explicit Material(const std::string& name);

  void set_ambient(const glm::vec3& ambient);
  void set_diffuse(const glm::vec3& diffuse);
  void set_specular(const glm::vec3& specular);
  void set_specular_coeff(const float& coeff);

  const std::string& name() const { return _name; }
  const glm::vec3& ambient() const { return _ambient; }
  const glm::vec3& diffuse() const { return _diffuse; }
  const glm::vec3& specular() const { return _specular; }
  const float& specular_coeff() const { return _specular_coeff; }

 private:
  std::string _name;
  glm::vec3 _ambient;
  glm::vec3 _diffuse;
  glm::vec3 _specular;
  float _specular_coeff;
};

#endif
