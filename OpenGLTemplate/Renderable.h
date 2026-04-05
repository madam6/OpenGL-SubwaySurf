#pragma once
struct Renderable
{
	virtual void Render() = 0;
	virtual ~Renderable() = default;
};