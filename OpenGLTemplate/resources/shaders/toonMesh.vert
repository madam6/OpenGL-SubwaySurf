#version 400 core

uniform struct Matrices {
    mat4 projMatrix;
    mat4 viewMatrix;
} matrices;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 5) in mat4 instanceModelMatrix; 

out vec3 vEyeNorm;
out vec4 vEyePosition;
out vec2 vTexCoord;

void main() 
{
    mat4 modelViewMat = matrices.viewMatrix * instanceModelMatrix;

    vEyePosition = modelViewMat * vec4(inPosition, 1.0);

    vEyeNorm = normalize(mat3(matrices.viewMatrix * instanceModelMatrix) * inNormal);

    vTexCoord = inCoord;
    
    gl_Position = matrices.projMatrix * vEyePosition;
}