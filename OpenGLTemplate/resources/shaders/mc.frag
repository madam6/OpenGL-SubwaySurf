#version 400 core

in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vPosition;
in vec4 FragPosLightSpace;

out vec4 vOutputColour;

uniform sampler2D sampler0;
uniform int uIsRecovering; 
uniform float uTime;
#include "light.glsl"
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

        float shadow = 0.0;
        if (i == 1) 
        { 
            shadow = ShadowCalculation(FragPosLightSpace, n, s);
        }

        float sDotN = max(dot(s, n), 0.0);

        ambientTotal += lights[i].La * material1.Ma;
        diffuseTotal += lights[i].Ld * material1.Md * sDotN * (1.0 - shadow);


        if (sDotN > 0.0) 
        {
            specularTotal += lights[i].Ls * material1.Ms * pow(max(dot(r, v), 0.0), material1.shininess + eps) * (1.0 - shadow);
        }
    }

    
    vec4 finalColor = texColour * vec4(ambientTotal + (diffuseTotal + specularTotal*2), 1.0);

    if (uIsRecovering == 1) 
    {
        if (sin(uTime * 30.0) > 0.0) 
        {
            finalColor = mix(finalColor, vec4(1.0, 0.0, 0.0, 1.0), 0.5); 
        } 
        else 
        {
            discard; 
        }
    }
    
    vOutputColour = finalColor;
}