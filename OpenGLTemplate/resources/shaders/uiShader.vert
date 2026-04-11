#version 400 core
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;

uniform mat4 projMatrix;
uniform mat4 modelMatrix;

out vec2 vTexCoord;

void main()
{
    gl_Position = projMatrix * modelMatrix * vec4(inPosition, 1.0);
    vTexCoord = inCoord;
}