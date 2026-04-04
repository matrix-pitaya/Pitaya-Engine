#pragma once

#include<string>  

namespace Pitaya::Core
{
	std::string ToLower(std::string_view) noexcept;
	std::string ToUpper(std::string_view) noexcept;
	void ToLower(std::string&) noexcept;
	void ToUpper(std::string&) noexcept;

	inline void CopyStringToBuffer(std::string_view sv, char* buffer, size_t size) noexcept
	{
		if (buffer == nullptr || size == 0) { return; }

		const size_t copyLength = std::min(sv.size(), size - 1);
		std::memcpy(buffer, sv.data(), copyLength);
		buffer[copyLength] = '\0';
	}

	template <size_t N>
	inline void CopyStringToCharArray(std::string_view sv, char(&arr)[N]) noexcept
	{
		size_t copyLength = std::min(sv.size(), N - 1);
		std::memcpy(arr, sv.data(), copyLength);
		arr[copyLength] = '\0';
	}
}
