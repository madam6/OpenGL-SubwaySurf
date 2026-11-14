#version 400 core
in vec2 vTexCoord;
out vec4 vOutputColour;

uniform sampler2D sampler0;

void main()
{
    vec4 texColour = texture(sampler0, vTexCoord);
    
    if(texColour.a < 0.1) discard; 
    
    vOutputColour = texColour;
}