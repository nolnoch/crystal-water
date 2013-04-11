#include <string>
#include "./material.h"

Material::Material()
    : _ambient(glm::vec3(.1, .1, .1)),
      _diffuse(glm::vec3(.3, .3, .3)),
      _specular(glm::vec3(.1, .1, .1)), _specular_coeff(10) {
}

Material::Material(const string& name)
    : _name(name),
      _ambient(glm::vec3(.1, .1, .1)),
      _diffuse(glm::vec3(.3, .3, .3)),
      _specular(glm::vec3(.1, .1, .1)), _specular_coeff(10) {
}

void Material::set_ambient(const glm::vec3& ambient) {
  _ambient = ambient;
}
void Material::set_diffuse(const glm::vec3& diffuse) {
  _diffuse = diffuse;
}
void Material::set_specular(const glm::vec3& specular) {
  _specular = specular;
}
void Material::set_specular_coeff(const float& coeff) {
  _specular_coeff = coeff;
}
