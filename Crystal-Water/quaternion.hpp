/**
 * quaternion.hpp
 *
 *    Created on: Mar 8, 2013
 *   Last Update: Mar 17, 2013
 *  Orig. Author: Wade Burch (nolnoch@cs.utexas.edu)
 *  Contributors: <none>
 *
 *  This Quaternion class was designed for use in CS354 Computer Graphics.
 *  This may not be the most efficient way to reinvent the wheel, but I
 *  wanted to understand each and every manipulation as deeply as I could,
 *  and that required writing it in my own style without any use of copy
 *  and paste. Some notes about the implementation:
 *
 *  [Quaternion Representation]
 *
 *  One can visualize a quaternion's components in the formats:
 *
 *        Q = [ X, Y, Z, W ]        or         Q = [ W | X, Y, Z ]
 *
 *  where the former is more similar to a vector in homogeneous coordinates
 *  and the latter is more in line with angle-axis representation. Because
 *  it helps me to think of a quaternion as a physical representation of a
 *  rotation, I've chosen the latter in this code.
 *
 *
 *  [Return Types]
 *
 *  There was an interesting challenge with return types. For instance, the
 *  matrix representation of the quaternion is expected in OpenGL as a float
 *  array. To return an array, I'd have to allocate it to the heap (No! I
 *  don't trust programmers to free it later), pass a pointer to a permanent
 *  local copy (No...it solves the memory leak but opens our class copy to
 *  being tampered with), or return some kind of object that could cast to
 *  a float array.  Luckily, OpenGL technically expects,
 *
 *      "a pointer to 16 consecutive values, which are used as the
 *      elements of a 4x4 column-major matrix,"
 *
 *  which an STL vector certainly satisfies. Thus, all array returns are
 *  passed as copies of an STL vector, leaving memory clean and our
 *  variables safe.
 *
 *
 *  [Vector/vertex Rotation]
 *
 *  It would make sense to overload the operator* to handle the application
 *  of a quaternion to a vector (v' = q*v*q^), but I've chosen to make it
 *  a public function to reinforce the concept of the quaternion being
 *  the mathematical embodiment of a rotation which acts upon vector. This
 *  will work as well for 'concatenated' quaternions. Example:
 *
 *      v2 = (qA * qB * qC).rotate(v1);         // v' = Ra.Rb.Rc.v
 *
 *
 *  [Constructor Overload]
 *
 *  Pun intended. I'm trying to cover all bases for the instantiation of a
 *  quaternion from different representations of rotations and even different
 *  methods of passing array values. When it becomes safe to depend on
 *  constructor delegation (GCC 4.7), I'll refactor for a cleaner class.
 *  I also considered the class template format, but that prohibits the split
 *  of code between a .cpp and a .h file. I'd rather be well-documented and
 *  organized for this learning experience.
 *
 *
 *  [Miscellaneous]
 *
 *  The liberal use of the keyword "this" is a personal style choice for
 *  enforced correctness and clarity.
 *
 *
 *  [Resources]
 *
 *  My four favorite resources for understanding quaternions (in addition
 *  to Wikipedia) are listed below:
 *
 *  http://www.youtube.com/watch?v=LB6U849kwXc
 *  http://www.euclideanspace.com/maths/geometry/rotations/conversions/
 *  http://www.cs.princeton.edu/~gewang/projects/darth/stuff/quat_faq.html
 *  http://content.gpwiki.org/index.php/
 *                 OpenGL:Tutorials:Using_Quaternions_to_represent_rotation
 *
 *
 *  If this can become public domain, feel free to alter and optimize as you
 *  see fit, giving credit where credit is due.
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
