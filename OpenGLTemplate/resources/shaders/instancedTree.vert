#version 400 core

uniform struct Matrices {
    mat4 projMatrix;
    mat4 viewMatrix;
} matrices;

#include "light.glsl"

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 5) in mat4 instanceModelMatrix;

out vec3 vColour;    
out vec2 vTexCoord;    
out vec3 worldPosition;
out vec4 FragPosLightSpace;

uniform mat4 lightSpaceMatrix;


// Terrain noise funcs (copied from mainShader.vert)
vec2 randomGradient(vec2 p) 
{
    p = vec2(dot(p, vec2(145.6, 291.2)), dot(p, vec2(312.2, 169.3)));
    return normalize(-1.0 + 2.0 * fract(sin(p) * 64671.217333));
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
    vec4 baseInstancePos = instanceModelMatrix * vec4(0.0, 0.0, 0.0, 1.0);

    float noiseScale = 0.007;
    float mountainHeight = 50.0;
    vec2 noiseCoord = baseInstancePos.xz * noiseScale;
    float yOffset = perlinNoise(noiseCoord) * mountainHeight;

    mat4 displacedInstanceMatrix = instanceModelMatrix;
    displacedInstanceMatrix[3][1] += yOffset;

    worldPosition = (displacedInstanceMatrix * vec4(inPosition, 1.0f)).xyz;
    
    mat4 modelViewMat = matrices.viewMatrix * displacedInstanceMatrix;
    gl_Position = matrices.projMatrix * modelViewMat * vec4(inPosition, 1.0f);
    
    vec3 vEyeNorm = normalize(mat3(modelViewMat) * inNormal);
    vec4 vEyePosition = modelViewMat * vec4(inPosition, 1.0f);
        
    FragPosLightSpace = lightSpaceMatrix * instanceModelMatrix * vec4(inPosition, 1.0);
    vColour = PhongModel(vEyePosition, vEyeNorm);
    vTexCoord = inCoord;
}