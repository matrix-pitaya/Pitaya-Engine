#pragma once

#include<Core/Color/Color.h>
#include<Editor/GUI/ImGui/imgui.h>

namespace Pitaya::Editor
{
	inline ImColor ToImColor(Pitaya::Core::Color color) noexcept
	{
		return { color.r, color.g, color.b, color.a };
	}
	inline ImVec4 ToImVec4(Pitaya::Core::Color color) noexcept
	{
		return { color.r, color.g, color.b, color.a };
	}
}