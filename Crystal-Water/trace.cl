struct VBOVertex {
  float3 vPosition;
  float3 vNormal;
  float2 vTexCoord;
  float3 vDiffuse;
  float3 vSpecular;
  float vShiny;
  float align;
};

float3 vRay;
float3 pEye;
float3 pA;
float3 vUp;
int winWidth, winHeight, id;


void findIdx(float2 frustIdx) {
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

void calculate_ray () {
  float3 pFrust, U, V, W;
  float cU, cV;
  
  W = distance (pEye, pA);
  U = vUp * W;
  
  normalize (W);
  normalize (U);
  V = cross (W, U);
  
  getCuCv (&cU, &cV);

  pFrust = pA + (U * cU) + (V * cV);
  
  vRay = distance (pFrust, pEye); 
  normalize (vRay);
}

float test_intersect (float3 v1, float3 v2, float3 v3, float3 *bar) {
  float3 vNorm, n, p0;
  float t, denom, numer;
  float a, b, c;
  
  float3 vA = distance (v2, v1);
  float3 vB = distance (v3, v1);
  vNorm = cross (vA, vB);
  normalize (vNorm);
  
  denom = dot (vNorm, vRay);
  if (denom < 0.001 && denom > -0.001)
    return -1;
  numer = (dot (vNorm, v1 - vEye));
  t = numer / denom;
  if (t < 0.00001)
    return -1;
  
  p0 = vEye + (t * vRay);
  n = dot (vNorm, vNorm);
  
  float3 va1 = distance (v3, v2);
  float3 va2 = distance (p0, v2);
  float3 vb1 = distance (v1, v3);
  float3 vb2 = distance (p0, v3);
  float3 vc1 = distance (v2, v1);
  float3 vc2 = distance (p0, v1);
  va3 = cross (va1, va2);
  vb3 = cross (vb1, vb2);
  vc3 = cross (vc1, vc2);
  
  a = dot (va3, vNorm);
  if (a < 0.00001)
    return -1;
  b = dot (vb3, vNorm);
  if (b < 0.00001)
    return -1;
  c = dot (vc3, vNorm);
  if (c < 0.00001)
    return -1;
  
  bar[0] = a / n;
  bar[1] = b / n;
  bar[2] = c / n;
  
  return t;
}

void get_texture (float3 bar, int vboIdx, float3 frag_color) {

  // Fill me.
}

float3 reflect_ray () {
  float3 vTmp;
  float nTmp = 2 * dot (vRay, vNorm);
  
  vTmp = nTmp * vNorm;
  
  vReflected = distance (vRay, vTmp);
}

float3 refract_ray () {

  // Fill me.
  
  // (n1 / n2) * sin(theta) = phi
  
  // -vNorm + phi = vRefracted
}

void process_frag (float3 bar, int vboIdx, float3 frag_color) {
  float3 vReflected, vRefracted, vNorm;
  
  if (vbo_array[vboIdx].align) {
    get_texture (bar, vboIdx, frag_color);
    return;
  }
  
  vNorm = (bar.x * vbo_array[vboIdx + 0].vNormal) +
          (bar.y * vbo_array[vboIdx + 1].vNormal) +
          (bar.z * vbo_array[vboIdx + 2].vNormal);
  
  reflect_ray ();
  refract_ray ();
  
  // And?...
  
}

__kernel void main (__global struct VBOVertex vbo_array[],
                    __global int **ibo_arrays, __global float3 eye) {
  float3 frag_color;
  float t;
  int i;
  
  pEye = eye;
  calculate_ray ();
  
  for (i = 0; i < vboSize; i += 3) {
    t = test_intersect (vbo_array[i].vPosition, vbo_array[i+1].vPosition,
                        vbo_array[i+2].vPosition, bar);
    if (t > 0) {
      process_frag (t, bar, i, frag_color);
    }
  }
  
}
