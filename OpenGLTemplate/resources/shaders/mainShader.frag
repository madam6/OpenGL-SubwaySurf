#version 400 core

in vec3 vColour;			// Interpolated colour using colour calculated in the vertex shader
in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader

out vec4 vOutputColour;		// The output colour

uniform sampler2D sampler0;  // The texture sampler
uniform sampler2D sampler1;
uniform sampler2D sampler2;

uniform float fMinHeight;
uniform float fMaxHeight;

uniform samplerCube CubeMapTex;
uniform bool bUseTexture;    // A flag indicating if texture-mapping should be applied
uniform bool renderSkybox;
uniform bool isTerrain;
in vec3 worldPosition;
uniform bool bIsDepthPass;

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

		// Get the texel colour from the texture sampler
		vec4 vTexColour = texture(sampler0, vTexCoord);	

		if (bUseTexture)
		{
			if (isTerrain)
			{
				vec4 vTexColour1 = texture(sampler1, vTexCoord);	
				vec4 vTexColour2 = texture(sampler2, vTexCoord);
				float f = clamp(2 * (worldPosition.y - fMinHeight) / (fMaxHeight - fMinHeight), 0, 2);
				vec4 resultColor;
				if (f < 1)
					resultColor = mix(vTexColour, vTexColour1, f);
				else
					resultColor = mix(vTexColour1, vTexColour2, f - 1.0);
				vOutputColour = resultColor * vec4(vColour, 1.0f);

			}
			else
			{
				vOutputColour = vTexColour*vec4(vColour, 1.0f);
			}
		}
		else
		{
			vOutputColour = vec4(vColour, 1.0f);	// Just use the colour instead
		}
	}
	
	
}
