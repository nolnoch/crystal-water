struct VBOVertex {
  float vPosition[3];
  float vNormal[3];
  float vTexCoord[2];
  float vDiffuse[3];
  float vSpecular[3];
  float vShiny;
  float align;
};

__kernel void main(__global struct VBOVertex *vbo_array,
    __global float *ibo_array, __global float matModel[16]) {  
  const int id = get_global_id(0);
  float rayVector[3];
  float eyePos[3];
  
  
}