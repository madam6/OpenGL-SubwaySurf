#version 400 core

in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vPosition;

out vec4 vOutputColour;

uniform sampler2D sampler0;

uniform struct Material {
    vec3 Ma;
    vec3 Md;
    vec3 Ms;
    float shininess;
} material;

uniform struct Light {
    vec4 position;
    vec3 La, Ld, Ls;
} lights[1];

void main()
{
    vec4 texColour = texture(sampler0, vTexCoord);

    vec3 n = normalize(vNormal);
    vec3 s = normalize(vec3(lights[0].position) - vPosition);
    vec3 v = normalize(-vPosition);
    vec3 h = normalize(s + v);

    float sDotN = max(dot(s, n), 0.0);
    vec3 ambient = lights[0].La * material.Ma;
    vec3 diffuse = lights[0].Ld * material.Md * sDotN;
    vec3 specular = vec3(0.0);

    if (sDotN > 0.0) 
    {
        specular = lights[0].Ls * material.Ms * pow(max(dot(h, n), 0.0), material.shininess);
    }

    vOutputColour = texColour * vec4(ambient + diffuse + specular, 1.0);
}