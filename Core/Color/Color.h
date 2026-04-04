#pragma once

#include<glm.hpp>

namespace Pitaya::Core
{
	struct Color
	{
		constexpr Color() noexcept = default;
		explicit constexpr Color(float value) noexcept
			:r(value), g(value), b(value), a(1.0f) {}
		explicit constexpr Color(float r, float g, float b, float a = 1.0f) noexcept
			: r(r), g(g), b(b), a(a) {}
		constexpr Color(glm::vec4 color) noexcept
			: r(color.r), g(color.g), b(color.b), a(color.a) {}

		constexpr operator glm::vec4() const noexcept { return glm::vec4(r,g,b,a); }

	public:
		static const Color White;
		static const Color Black;
		static const Color Red;
		static const Color Green;
		static const Color Blue;
		static const Color Yellow;
		static const Color Gray;
		static const Color Dark;
		static const Color Purple;
		static const Color Clear;
		static const Color SkyBlue;

	public:
		float r = 1.0f;
		float g = 1.0f;
		float b = 1.0f;
		float a = 1.0f;
	};

	inline constexpr const Color Color::White = Color(1.0f, 1.0f, 1.0f, 1.0f);
	inline constexpr const Color Color::Black = Color(0.0f, 0.0f, 0.0f, 1.0f);
	inline constexpr const Color Color::Red = Color(1.0f, 0.0f, 0.0f, 1.0f);
	inline constexpr const Color Color::Green = Color(0.0f, 1.0f, 0.0f, 1.0f);
	inline constexpr const Color Color::Blue = Color(0.0f, 0.0f, 1.0f, 1.0f);
	inline constexpr const Color Color::Yellow = Color(1.0f, 1.0f, 0.0f, 1.0f);
	inline constexpr const Color Color::Gray = Color(0.5f, 0.5f, 0.5f, 1.0f);
	inline constexpr const Color Color::Dark = Color(0.22f, 0.22f, 0.22f, 1.0f);
	inline constexpr const Color Color::Purple = Color(0.5f, 0.0f, 1.0f, 1.0f);
	inline constexpr const Color Color::Clear = Color(0.0f, 0.0f, 0.0f, 0.0f);
	inline constexpr const Color Color::SkyBlue = Color(0.1f, 0.2f, 0.3f, 1.0f);
}
