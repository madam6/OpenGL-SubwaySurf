#pragma once


#include "Game.h"
class Renderer
{
	friend class Game;
	void Render(const FrameData& frameData);

	std::vector<RenderData> m_RenderQueue;
};