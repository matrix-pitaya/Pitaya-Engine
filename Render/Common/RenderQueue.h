#pragma once

#include<cstdint>
#include<string>

namespace Pitaya::Render
{
	enum class RenderQueue : uint16_t
	{
		Background = 1000,
		Geometry = 2000,
		AlphaTest = 2450,
		Transparent = 3000,
		Overlay = 4000
	};

	inline std::string ToString(Pitaya::Render::RenderQueue renderQueue) noexcept
	{
		switch (renderQueue)
		{
			case Pitaya::Render::RenderQueue::Background:   return "Background";
			case Pitaya::Render::RenderQueue::Geometry:		return "Geometry";
			case Pitaya::Render::RenderQueue::AlphaTest:	return "AlphaTest";
			case Pitaya::Render::RenderQueue::Transparent:	return "Transparent";
			case Pitaya::Render::RenderQueue::Overlay:	    return "Overlay";
			default:									    return "unknwon";
		}
	}
}
