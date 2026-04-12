#pragma once
#include "Component.h"
#include "Common.h"
class ModelViewComponent : public Component
{
	void Init() override;

	void AddRenderData(std::vector<RenderData>& renderQueue) override;
	
	void Update(float dt) override;
public:
	void SetPosition(const glm::vec3& newPosition) { position = newPosition; }
	glm::vec3 GetPosition() const { return position; }
	glm::vec3 GetScale() const { return scale; }
	void SetScale(const glm::vec3& newScale) { scale = newScale; }
	void Apply(const PropertyMap& props);

	void SetOrientation(const glm::mat4& newOrientation) { orientation = newOrientation; }
	glm::mat4 GetOrientation() const { return orientation; }
private:
	// properties
	glm::vec3 position{ 0.0f };
	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
	glm::mat4 orientation{ 1.0f };
};