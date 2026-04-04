#include "ShaderComponent.h"

void ShaderComponent::Apply(const PropertyMap& props)
{
	auto it = props.find("shader");
	if (it != props.end())
	{
		shaderName = it->second;
	}
}
