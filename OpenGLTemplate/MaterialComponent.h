#pragma once
#include "Component.h"
#include "Common.h"
class MaterialComponent : public Component
{
	void Init() override;

	void AddRenderData(std::vector<RenderData>& renderQueue) override;

	void Apply(const PropertyMap& props);
	void Update(float dt) override;
public:
	glm::vec3 GetMa() const { return Ma; };
	glm::vec3 GetMd() const { return Md; };
	glm::vec3 GetMs() const { return Ms; };
	float GetShiny() const { return shininess; };
private:
	// properties
	glm::vec3 Ma{ 1.0f };
	glm::vec3 Md{ 1.0f };
	glm::vec3 Ms{ 0.0f };
	float shininess = 1.0f;
};