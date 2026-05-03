#version 400 core

in vec3 vEyeNorm;
in vec4 vEyePosition;

out vec4 vOutputColour;

#include "light.glsl"

const float levels = 3.0;
const float scaleFactor = 1.0 / levels;
uniform bool bIsDepthPass;
void main()
{
    if (bIsDepthPass)
    {
        vOutputColour = vec4(1.0);
        return;
    }
    vec3 n = normalize(vEyeNorm);
    vec3 v = normalize(-vEyePosition.xyz);
    
    vec3 ambientTotal = vec3(0.0);
    vec3 diffuseTotal = vec3(0.0);
    vec3 specularTotal = vec3(0.0);

    for (int i = 0; i < numLights; i++)
    {
        vec3 s = normalize(vec3(lights[i].position - vEyePosition));    

        ambientTotal += (lights[i].La * 0.7) * material1.Ma;
        
        // Adapted from OpenGL 4 shading language cookbook (David Wolf, 2018)
        float sDotN = max(dot(s, n), 0.0);
        diffuseTotal += lights[i].Ld * material1.Md * floor(sDotN * levels) * scaleFactor;

        vec3 halfVector = normalize(s + v);

        float spec = pow(max(dot(n, halfVector), 0.0), material1.shininess); 
        
        if (spec > 0.4 && sDotN > 0.5) 
        {
            specularTotal += lights[i].Ls * material1.Ms; 
        }
    }

    vec3 finalColor = ambientTotal + diffuseTotal + specularTotal;

    finalColor = clamp(finalColor, 0.0, 1.0);

    vOutputColour = vec4(finalColor, 1.0);
}