#pragma once
struct Renderable
{
	// Main Render call to be defined by renderable, should issue draw call
	virtual void Render() = 0;
	// Should issue instanced draw called defined by renderable
	virtual void RenderInstanced(const std::vector<glm::mat4>& instanceMatrices) {}
	virtual ~Renderable() = default;
	// Can be overwritten by renderables with geomerty defined in .csv
	virtual void readFromCSV() {};
};