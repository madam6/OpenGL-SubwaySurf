#version 400 core

in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vPosition;

out vec4 vOutputColour;

uniform sampler2D sampler0;

#include "light.glsl"

void main()
{
    vec4 texColour = texture(sampler0, vTexCoord);

    vec3 n = normalize(vNormal);
    vec3 v = normalize(-vPosition);
    
    vec3 ambientTotal = vec3(0.0);
    vec3 diffuseTotal = vec3(0.0);
    vec3 specularTotal = vec3(0.0);

    float eps = 0.000001;

    for (int i = 0; i < numLights; i++)
    {
        vec3 s = normalize(vec3(lights[i].position) - vPosition);
        vec3 r = reflect(-s, n);

        float sDotN = max(dot(s, n), 0.0);

        ambientTotal += lights[i].La * material1.Ma;
        diffuseTotal += lights[i].Ld * material1.Md * sDotN;

        if (sDotN > 0.0) 
        {
            specularTotal += lights[i].Ls * material1.Ms * pow(max(dot(r, v), 0.0), material1.shininess + eps);
        }
    }

    vOutputColour = texColour * vec4(ambientTotal + diffuseTotal + specularTotal, 1.0);
}