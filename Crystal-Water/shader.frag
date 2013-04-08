#version 420

struct Light {
    vec3 lightPos;
    vec3 lightAmb;
    vec3 lightDiff;
    vec3 lightSpec;
};

struct Material {
    vec3 matAmb;
    vec3 matDiff;
    vec3 matSpec;
    float matShiny;
};

uniform Light light0;
uniform Material mat;
uniform sampler2D tex;

in vec3 v;
in vec3 N;
in vec2 texCoord;

out vec4 phongColor;

void main() {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec4 texture;
    
    vec3 L = normalize(light0.lightPos - v);
    vec3 R = normalize(reflect(-L, N));
    vec3 V = normalize(-v);
    
    ambient = light0.lightAmb * mat.matAmb;
    
    diffuse = clamp(light0.lightDiff * mat.matDiff * max(dot(N, L), 0.0), 0.0, 1.0);
    texture = texture2D(tex, vec2(texCoord.s, texCoord.t)); 
    
    specular = clamp(light0.lightSpec * mat.matSpec * pow(max(dot(R, V), 0.0), mat.matShiny), 0.0, 1.0);
    
    phongColor = vec4(clamp(ambient + (diffuse * texture.rgb) + specular, 0.0, 1.0), 1.0);
}
