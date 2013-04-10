#version 420

uniform mat4 modelviewMatrix;
uniform mat4 projectionMatrix;

out vec3 v;
out vec3 N;

void main() {
    mat3 normalMatrix = mat3(transpose(inverse(modelviewMatrix)));

    v = (modelviewMatrix * gl_Vertex).xyz;
    N = normalize(normalMatrix * gl_Normal);
        
    gl_Position = projectionMatrix * modelviewMatrix * gl_Vertex;
}
