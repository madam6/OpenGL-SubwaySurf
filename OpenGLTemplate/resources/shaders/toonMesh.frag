#version 400 core

in vec3 vEyeNorm;
in vec4 vEyePosition;
in vec2 vTexCoord;
in vec4 FragPosLightSpace;

out vec4 vOutputColour;

uniform sampler2D sampler0;
#include "light.glsl"

const float levels = 3.0;
const float scaleFactor = 1.0 / levels;
uniform bool bIsDepthPass;

uniform sampler2D shadowMap;

// Code adapted from https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0) return 0.0;

    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;

    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.0005);

    return currentDepth - bias > closestDepth ? 1.0 : 0.0;
}

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
        
        float shadow = 0.0;
        if (i == 1) 
        { 
            shadow = ShadowCalculation(FragPosLightSpace, n, s);
        }

        float sDotN = max(dot(s, n), 0.0);
        diffuseTotal += lights[i].Ld * material1.Md * floor(sDotN * levels) * scaleFactor* (1.0 - shadow);

        vec3 halfVector = normalize(s + v);
        float spec = pow(max(dot(n, halfVector), 0.0), material1.shininess); 

        if (spec > 0.4 && sDotN > 0.5) 
        {
            specularTotal += lights[i].Ls * material1.Ms * (1.0 - shadow);
        }
    }

    vec4 texColour = texture(sampler0, vTexCoord);
    vec3 finalColor = texColour.rgb * (ambientTotal + diffuseTotal) + specularTotal;

    finalColor = clamp(finalColor, 0.0, 1.0);
    vOutputColour = vec4(finalColor, texColour.a);
}