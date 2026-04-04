#pragma once
#include "Component.h"
#include "Common.h"
class ShaderComponent : public Component
{
	void Init() override;

	void AddRenderData(std::vector<RenderData>& renderQueue) override;

	void Apply(const PropertyMap& props);
	void Update(float dt) override;
public:
	std::string GetShaderName() const { return shaderName; };
private:
	// properties
	std::string shaderName;
};