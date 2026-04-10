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

void main()
{    
    worldPosition = inPosition;
    gl_Position = matrices.projMatrix * matrices.modelViewMatrix * vec4(inPosition, 1.0f);
    
    vec3 vEyeNorm = normalize(matrices.normalMatrix * inNormal);
    vec4 vEyePosition = matrices.modelViewMatrix * vec4(inPosition, 1.0f);
        
    vColour = PhongModel(vEyePosition, vEyeNorm);
    vTexCoord = inCoord;
}