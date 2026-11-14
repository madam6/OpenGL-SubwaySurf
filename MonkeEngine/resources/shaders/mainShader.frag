#version 400 core

in vec3 vColour;			// Interpolated colour using colour calculated in the vertex shader
in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader
in vec3 worldPosition;
in vec4 FragPosLightSpace;

out vec4 vOutputColour;		// The output colour

uniform sampler2D sampler0;  // The texture sampler
uniform sampler2D sampler1;
uniform sampler2D sampler2;
uniform sampler2D shadowMap;

uniform float fMinHeight;
uniform float fMaxHeight;

uniform samplerCube CubeMapTex;
uniform bool bUseTexture;    // A flag indicating if texture-mapping should be applied
uniform bool renderSkybox;
uniform bool isTerrain;
uniform bool bIsDepthPass;

// Code adapted from https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0) return 0.0;
    
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    
    float bias = 0.001; 
    return currentDepth - bias > closestDepth ? 1.0 : 0.0;
}

void main()
{
	if (bIsDepthPass)
	{
		vOutputColour = vec4(1.0);
		return;
	}

	if (renderSkybox) {
		vOutputColour = texture(CubeMapTex, worldPosition);
	} else {
        
        float shadow = ShadowCalculation(FragPosLightSpace);

		vec4 vTexColour = texture(sampler0, vTexCoord);	
        vec4 resultColor = vTexColour;

		if (bUseTexture)
		{
			if (isTerrain)
			{
				vec4 vTexColour1 = texture(sampler1, vTexCoord);	
				vec4 vTexColour2 = texture(sampler2, vTexCoord);
				float f = clamp(2.0 * (worldPosition.y - fMinHeight) / (fMaxHeight - fMinHeight), 0.0, 2.0);
				
				if (f < 1.0) resultColor = mix(vTexColour, vTexColour1, f);
				else resultColor = mix(vTexColour1, vTexColour2, f - 1.0);
			}
		}
		else
		{
			resultColor = vec4(1.0f);	
		}
        
        vec3 finalLighting = vColour * (1.0 - (shadow * 0.5));
        vOutputColour = resultColor * vec4(finalLighting, 1.0f);
	}
}