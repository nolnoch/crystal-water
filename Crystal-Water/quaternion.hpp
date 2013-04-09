/*
 * quaternion.hpp
 *
 *    Created on: Mar 8, 2013
 *   Last Update: Mar 17, 2013
 *  Orig. Author: Wade Burch (nolnoch@cs.utexas.edu)
 *  Contributors: <none>
 */

#ifndef QUATERNION_HPP_
#define QUATERNION_HPP_

#include <vector>
#include <glm/glm.hpp>

enum { EULER = 2, EXPLICIT = 4, ANGLE_AXIS = 8, MATRIX = 16 };
enum { DEG = 1, RAD = 0 };

using namespace std;

class Quaternion {
 private:
  // Data
  float W, X, Y, Z;
  int dirty;
  vector<float> rotMatrix;

  // Constructor Init -- UT Servers use GCC 4.6 with no ctor delegation.
  void initQAngleAxis(float theta, float *v);
  void initQEulerAngles(float vA, float vB, float vC);
  void initQRotationMatrix(float *m);
  void initQArrayUnknown(float *m, int con, int deg_rad);

  // Internal operations
  float magnitude();
  void normalize();
  void makeMatrix();
  void loadMatrix(float *m);
  void normalizeVector(float *v);
  Quaternion inverse();
  Quaternion pureQuaternion(float *v);
  vector<float> rotateVector(float *v);

 public:
  // Constructors
  Quaternion();
  Quaternion(const Quaternion &b);
  explicit Quaternion(vec3 m, int deg_rad);
  explicit Quaternion(float w, float x, float y, float z);
  explicit Quaternion(float theta, vector<float> axis, int deg_rad);
  explicit Quaternion(float theta, Vec3f axis, int deg_rad);
  explicit Quaternion(float theta, float axis[3], int deg_rad);
  explicit Quaternion(float yaw, float pitch, float roll, int deg_rad);
  explicit Quaternion(float *m, int con, int deg_rad);
  explicit Quaternion(vector<float> m, int con, int deg_rad);

  // Destructor
  virtual ~Quaternion();

  // Public
  vector<float> matrix();
  vector<float> rotate(vector<float> v);
  vector<float> rotate(float v[3]);
  Vec3f rotate(Vec3f v);
  void toString();
  void matrixToString();
  void zero();

  // Operators
  Quaternion operator+ (const Quaternion& b);
  Quaternion operator* (const Quaternion& b);
  Quaternion& operator= (const Quaternion& b);
  Quaternion& operator+= (const Quaternion& b);
  Quaternion& operator*= (const Quaternion& b);
};

#endif /* QUATERNION_H_ */
