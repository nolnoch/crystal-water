struct VBOVertex {
  float vPosition[3];
  float vNormal[3];
  float vTexCoord[2];
  float vDiffuse[3];
  float vSpecular[3];
  float vShiny;
  float align;
};

float vRay[3];
float pEye[3];
float pA[3];
float vUp[3];
int winWidth, winHeight;

void normalize (float *v) {
  float mag2 = dot (v, v);
  float mag = sqrt(mag2);
  
  v[0] = v[0] / mag;
  v[1] = v[1] / mag;
  v[2] = v[2] / mag;
  
  return;
}

float dot (float *v1, float *v2) {
  return (v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]);
}

void cross (float *v1, float *v2, float *vOut) {
  vOut[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
  vOut[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
  vOut[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
}

void findIdx(float *frustIdx) {
  const int id = get_global_id (0);
  
  frustIdx[0] = id / winWidth;
  frustIdx[1] = mod(id, winWidth);
}

void getCuCv (float *cU, float *cV) {
  int frustIdx[2];
  
  findIdx (frustIdx);
  
  *cU = frustIdx[0] + 0.5 - (winWidth / 2.0);
  *cV = frustIdx[1] + 0.5 - (winHeight / 2.0);
}

void calculate_ray (const int id) {
  float pFrust[3], U[3], V[3], W[3];
  float cU, cV;
  
  W[0] = pEye[0] - pA[0];
  W[1] = pEye[1] - pA[1];
  W[2] = pEye[2] - pA[2];
  
  U[0] = vUp[0] * W[0];
  U[1] = vUp[1] * W[1];
  U[2] = vUp[2] * W[2];
  
  normalize (W);
  normalize (U);
  cross (W, U, V);
  
  getCuCv (id, &cU, &cV)

  pFrust[0] = pA[0] + (U[0] * cU) + (V[0] * cV);
  pFrust[1] = pA[1] + (U[1] * cU) + (V[1] * cV);
  pFrust[2] = pA[2] + (U[2] * cU) + (V[2] * cV);
  
  vRay[0] = pFrust[0] - pEye[0];
  vRay[1] = pFrust[1] - pEye[1];
  vRay[2] = pFrust[2] - pEye[2]; 
  normalize (vRay);
}

float test_intersect (float *v1, float *v2, float *v3, float *bar) {
  float t, denom, numer;
  float vNorm[3], n[3], p0[3];
  float a, b, c;
  
  float vA[] = {v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2]}
  float vB[] = {v3[0] - v1[0], v3[1] - v1[1], v3[2] - v1[2]}  
  cross (vA, vB, vNorm);
  normalize (vNorm);
  
  denom = dot (vNorm, vRay);
  if (denom < 0.001 && denom > -0.001)
    return -1;
  numer = (dot (vNorm, v1)) - (dot (vNorm, vEye));
  t = numer / denom;
  if (t < 0)
    return -1;
  
  p0[0] = vEye[0] + (t * vRay[0]);
  p0[1] = vEye[1] + (t * vRay[1]);
  p0[2] = vEye[2] + (t * vRay[2]);
  n = dot (vNorm, vNorm);
  
  float va1[] = {v3[0] - v2[0], v3[1] - v2[1], v3[2] - v2[2]}
  float va2[] = {p0[0] - v2[0], p0[1] - v2[1], p0[2] - v2[2]}
  float vb1[] = {v1[0] - v3[0], v1[1] - v3[1], v1[2] - v3[2]}
  float vb2[] = {p0[0] - v3[0], p0[1] - v3[1], p0[2] - v3[2]}
  float vc1[] = {v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2]}
  float vc2[] = {p0[0] - v1[0], p0[1] - v1[1], p0[2] - v1[2]}
  cross (va1, va2, va3);
  cross (vb1, vb2, vb3);
  cross (vc1, vc2, vc3);
  
  a = dot (va3, vNorm);
  if (a < 0)
    return -1;
  b = dot (vb3, vNorm);
  if (b < 0)
    return -1;
  c = dot (vc3, vNorm);
  if (c < 0)
    return -1;
  
  bar[0] = a / n;
  bar[1] = b / n;
  bar[2] = c / n;
  
  return t;
}

void reflect_ray (float *vNorm, float *vReflected) {
  
  
}

void refract_ray (float *vNorm, float *vRefracted) {


}

void process (float *bar, int vboIdx) {


}

__kernel void main (__global struct VBOVertex vbo_array[],
                    __global int **ibo_arrays, __global float eye[3]) {
  float t;
  int i;
  
  pEye[0] = eye[0];
  pEye[1] = eye[1];
  pEye[2] = eye[2];
  
  calculate_ray (id);
  
  for (i = 0; i < vboSize; i += 3) {
    t = test_intersect (vbo_array[i].vPosition, vbo_array[i+1].vPosition,
                        vbo_array[i+2].vPosition, bar);
    if (t > 0) {
      process (bar, i);
    }
  }
  
}
