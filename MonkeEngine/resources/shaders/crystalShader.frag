#version 400 core

in vec3 vColour;
in vec2 vTexCoord;

out vec4 vOutputColour;    
#include "light.glsl"
uniform sampler2D sampler0;
uniform bool bUseTexture;
uniform bool bIsDepthPass;
void main() {
    if (bIsDepthPass)
    {
        vOutputColour = vec4(1.0);
        return;
    }
    if (bUseTexture) 
    {
        vec4 vTexColour = texture(sampler0, vTexCoord);
        vOutputColour = vTexColour * vec4(vColour, 1.0); 
    }
    else
    {
        vOutputColour = vec4(vColour, 1.0);
    }
}