/*
 * face.h
 *
 *  Created on: Mar 15, 2013
 *      Author: nolnoch
 */

#ifndef FACE_H_
#define FACE_H_

#include <glm/glm.hpp>
#include <vector>
#include <string>

enum {VTN = 0x111, VT = 0x110, VN = 0x101};

using namespace std;

class Face {
public:
  Face();
  Face(const vector<int>& lV, const vector<int>& lT, const vector<int>& lN) {
    for (int i = 0; i < lV.size(); i++) {
      this->fV.push_back(lV[i]);
      this->fT.push_back(lT[i]);
      this->fN.push_back(lN[i]);
    }

    materialID = -1;
    origFormat = VTN;
  }
  Face(const vector<int>& lV, const vector<int>& lX, int format) {

    if (format == VT) {
      for (int i = 0; i < lV.size(); i++) {
        this->fV.push_back(lV[i]);
        this->fT.push_back(lX[i]);
        this->fN.push_back(0);
      }
    } else if (format == VN) {
      for (int i = 0; i < lV.size(); i++) {
        this->fV.push_back(lV[i]);
        this->fT.push_back(0);
        this->fN.push_back(lX[i]);
      }
    }

    materialID = -1;
    origFormat = format;
  }

  vector<int> getVertexInfo(int x) {
    vector<int> vRet;

    if (x < fV.size()) {
      vRet.push_back(this->fV[x]);
      vRet.push_back(this->fT[x]);
      vRet.push_back(this->fN[x]);
    }

    return vRet;
  }

  vector<int> vertices() {
    return fV;
  }

  vector<int> textures() {
    return fT;
  }

  vector<int> normals() {
    return fN;
  }

  int size() {
    return this->fV.size();
  }

  int format() {
    return this->origFormat;
  }

  glm::vec3 getNormal() {
    return this->normal;
  }

  void setNormal(glm::vec3 norm) {
    this->normal = norm;
  }

  bool contains(int a) {
    for (int i = 0; i < this->fV.size(); i++) {
      if (this->fV[i] == a)
        return true;
    }

    return false;
  }


private:
  int origFormat;
  int materialID;
  glm::vec3 normal;

  // Vertex, Texture, and Normal vectors
  vector<int> fV;
  vector<int> fT;
  vector<int> fN;

};


#endif /* FACE_H_ */
