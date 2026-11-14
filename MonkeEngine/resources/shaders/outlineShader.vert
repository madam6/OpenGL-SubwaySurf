#version 400 core

uniform struct Matrices 
{
    mat4 projMatrix;
    mat4 viewMatrix; 
} matrices;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 4) in mat4 instanceModelMatrix; 

uniform float uTime;

void main() 
{
    float rotAngle = uTime * 2.0;
    float yOffset = (sin(uTime * 3.0) * 1.0) + 1.0;
    float s = sin(rotAngle);
    float c = cos(rotAngle);

    mat4 localRotY = mat4(
        c, 0.0, -s, 0.0,
        0.0, 1.0, 0.0, 0.0,
        s, 0.0, c, 0.0,
        0.0, 0.0, 0.0, 1.0
    );

    vec3 expandedPos = inPosition * 1.15;
    
    vec4 localPos = localRotY * vec4(expandedPos, 1.0);
    localPos.y += yOffset;

    vec4 worldPos = instanceModelMatrix * localPos;
    gl_Position = matrices.projMatrix * matrices.viewMatrix * worldPos;
}