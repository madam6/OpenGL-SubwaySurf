#pragma once
#include "Component.h"
#include "Common.h"
class ModelViewComponent : public Component
{
	void Init() override;

	void AddRenderData(std::vector<RenderData>& renderQueue) override;

	void Apply(const PropertyMap& props);
	void Update(float dt) override;
public:
	glm::vec3 GetPosition() const { return position; }
	glm::vec3 GetScale() const { return scale; }

private:
	// properties
	glm::vec3 position;
	glm::vec3 scale;
};