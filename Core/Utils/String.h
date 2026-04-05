#pragma once

#include<string>  
#include<algorithm>

namespace Pitaya::Core
{
	inline std::string ToLower(std::string_view strview) noexcept
	{
		std::string str = std::string(strview);
		std::transform(str.begin(), str.end(), str.begin(),
			[](unsigned char c) { return std::tolower(c); });
		return str;
	}
	inline std::string ToUpper(std::string_view strview) noexcept
	{
		std::string str = std::string(strview);
		std::transform(str.begin(), str.end(), str.begin(),
			[](unsigned char c) { return std::toupper(c); });
		return str;
	}
	inline void ToLower(std::string& str) noexcept
	{
		std::transform(str.begin(), str.end(), str.begin(),
			[](unsigned char c) { return std::tolower(c); });
	}
	inline void ToUpper(std::string& str) noexcept
	{
		std::transform(str.begin(), str.end(), str.begin(),
			[](unsigned char c) { return std::toupper(c); });
	}

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
