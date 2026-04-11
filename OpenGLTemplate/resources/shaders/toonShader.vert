#version 400 core

uniform struct Matrices 
{
    mat4 projMatrix;
    mat4 viewMatrix; 
} matrices;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTextureCoord;
layout (location = 3) in vec3 inColour;
layout (location = 4) in mat4 instanceModelMatrix; 

uniform float uTime;

out vec3 vEyeNorm;
out vec4 vEyePosition;

void main() 
{
    float rotAngle = uTime * 2.0;
    float yOffset = (sin(uTime * 3) * 1.0) + 1.0;

    float s = sin(rotAngle);
    float c = cos(rotAngle);

    mat4 localRotY = mat4(
        c, 0.0, -s, 0.0,
        0.0, 1.0, 0.0, 0.0,
        s, 0.0, c, 0.0,
        0.0, 0.0, 0.0, 1.0
    );

    vec4 localPos = localRotY * vec4(inPosition, 1.0);
    localPos.y += yOffset;

    vec4 worldPos = instanceModelMatrix * localPos;
    gl_Position = matrices.projMatrix * matrices.viewMatrix * worldPos;

    mat4 modelViewMat = matrices.viewMatrix * instanceModelMatrix * localRotY;
    
    vEyePosition = modelViewMat * vec4(inPosition, 1.0);
    vEyeNorm = normalize(mat3(modelViewMat) * inNormal);
}