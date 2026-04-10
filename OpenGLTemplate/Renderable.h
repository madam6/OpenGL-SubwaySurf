#pragma once
struct Renderable
{
	virtual void Render() = 0;
	virtual void RenderInstanced(const std::vector<glm::mat4>& instanceMatrices) {}
	virtual ~Renderable() = default;
};