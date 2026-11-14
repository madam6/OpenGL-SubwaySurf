#version 400 core

uniform struct Matrices {
    mat4 projMatrix;
    mat4 viewMatrix;
} matrices;

layout (location = 0) in vec3 inPosition;
layout (location = 2) in vec3 inNormal;
layout (location = 5) in mat4 instanceModelMatrix;

void main() 
{
    float outlineThickness = 0.15; 
    
    vec3 expandedPos = inPosition + (inNormal * outlineThickness);
    
    mat4 modelViewMat = matrices.viewMatrix * instanceModelMatrix;
    
    gl_Position = matrices.projMatrix * modelViewMat * vec4(expandedPos, 1.0);
}