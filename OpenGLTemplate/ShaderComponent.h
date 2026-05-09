#pragma once
#include "Component.h"
#include "Common.h"

// Component that allows to specify specific shader for parent entity
class ShaderComponent : public Component
{
	void Init() override;

	void AddRenderData(std::vector<RenderData>& renderQueue) override;

	void Update(float dt) override;
public:
	void Apply(const PropertyMap& props);
	std::string GetShaderName() const { return shaderName; };
private:
	// properties
	std::string shaderName;
};