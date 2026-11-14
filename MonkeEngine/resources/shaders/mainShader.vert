#version 400 core

uniform struct Matrices
{
    mat4 projMatrix;
    mat4 modelViewMatrix; 
    mat3 normalMatrix;
} matrices;

#include "light.glsl"

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

out vec3 vColour;    
out vec2 vTexCoord;    
out vec3 worldPosition;

uniform mat4 lightSpaceMatrix;
uniform bool isTerrain;
uniform mat4 modelMatrix;
out vec4 FragPosLightSpace;
// Pseudo-random hash
vec2 randomGradient(vec2 p) 
{
    p = vec2(dot(p, vec2(145.6, 291.2)), dot(p, vec2(312.2, 169.3)));
    return normalize(-1.0 + 2.0 * fract(sin(p) * 64671.217333)); // normalize to -1, 1 range so that arrows can point anywhere
}

vec2 perlinFadeFunc(vec2 t) 
{
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float perlinNoise(vec2 uv)
{
    vec2 gridId = floor(uv);

    vec2 fraction = fract(uv);

    vec2 gradBottomLeft = randomGradient(gridId + vec2(0.0, 0.0));
    vec2 gradBottomRight = randomGradient(gridId + vec2(1.0, 0.0));
    vec2 gradTopLeft = randomGradient(gridId + vec2(0.0, 1.0));
    vec2 gradTopRight = randomGradient(gridId + vec2(1.0, 1.0));

    vec2 distBottomLeft  = fraction - vec2(0.0, 0.0);
    vec2 distBottomRight = fraction - vec2(1.0, 0.0);
    vec2 distTopLeft = fraction - vec2(0.0, 1.0);
    vec2 distTopRight = fraction - vec2(1.0, 1.0);

    float dotBottomLeft = dot(gradBottomLeft, distBottomLeft);
    float dotBottomRight = dot(gradBottomRight, distBottomRight);
    float dotTopLeft = dot(gradTopLeft, distTopLeft);
    float dotTopRight = dot(gradTopRight, distTopRight);

    vec2 smoothedFraction = perlinFadeFunc(fraction);

    float mixBottom = mix(dotBottomLeft, dotBottomRight, smoothedFraction.x);
    float mixTop = mix(dotTopLeft, dotTopRight, smoothedFraction.x);

    return mix(mixBottom, mixTop, smoothedFraction.y);
}

void main()
{   
    vec3 vEyeNorm;
    vec4 vEyePosition;
    vec4 trueWorldPos;

    if (isTerrain)
    {
        float noiseScale = 0.007;
        float epsilon = 0.1;
        float mountainHegiht = 50.0;

        vec2 noiseCoord = inPosition.xz * noiseScale;
        float y0 = perlinNoise(noiseCoord) * mountainHegiht;

        vec3 p0 = vec3(inPosition.x, y0, inPosition.z);

        // Small step on x axis
        vec2 noiseCoordsX = (inPosition.xz + vec2(epsilon, 0.0)) * noiseScale;
        float y1 = perlinNoise(noiseCoordsX) * mountainHegiht;
        vec3 p1 = vec3(inPosition.x + epsilon, y1, inPosition.z);

        // Small step on z axis
        vec2 noiseCoordsZ = (inPosition.xz + vec2(0.0, epsilon)) * noiseScale;
        float y2 = perlinNoise(noiseCoordsZ) * mountainHegiht;
        vec3 p2 = vec3(inPosition.x, y2, inPosition.z + epsilon);

        // Use this new vectors to calculate slope and use cross product to find the normal
        vec3 rightSlope = p1 - p0;
        vec3 forwardSlope = p2 - p0;

        trueWorldPos = modelMatrix * vec4(p0, 1.0);
        worldPosition = p0;
        gl_Position = matrices.projMatrix * matrices.modelViewMatrix * vec4(p0, 1.0f);

        vec3 newNormal = normalize(cross(forwardSlope, rightSlope));

        vEyeNorm = normalize(matrices.normalMatrix * newNormal);
        vEyePosition = matrices.modelViewMatrix * vec4(p0, 1.0f);
    }
    else
    {
        trueWorldPos = modelMatrix * vec4(inPosition, 1.0);
        worldPosition = inPosition;
        gl_Position = matrices.projMatrix * matrices.modelViewMatrix * vec4(inPosition, 1.0f);
        vEyeNorm = normalize(matrices.normalMatrix * inNormal);
        vEyePosition = matrices.modelViewMatrix * vec4(inPosition, 1.0f);
    }
    
    FragPosLightSpace = lightSpaceMatrix * trueWorldPos;
        
    vColour = PhongModel(vEyePosition, vEyeNorm);
    vTexCoord = inCoord;
}