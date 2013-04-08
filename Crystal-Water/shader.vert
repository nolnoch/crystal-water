#version 420

in vec3 vertexLocation;
in vec3 vertexNormal;
in vec2 vertexTexCoord;

uniform mat4 rotationMatrix;
uniform mat4 modelviewMatrix;
uniform mat4 projectionMatrix;

out vec3 v;
out vec3 N;
out vec2 texCoord;

void main() {
    vec4 vLoc = vec4(vertexLocation, 1.0);
    mat4 rotatedModelview = modelviewMatrix * rotationMatrix;
    mat3 normalMatrix = mat3(transpose(inverse(rotatedModelview)));

    v = (modelviewMatrix * rotationMatrix * vLoc).xyz;
    N = normalize(normalMatrix * vertexNormal);
    
    texCoord = vertexTexCoord;
        
    gl_Position = projectionMatrix * rotatedModelview * vLoc;
}
