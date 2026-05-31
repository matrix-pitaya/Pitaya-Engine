#pragma once

#include<string>  
#include<algorithm>
#include<cctype>

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

	template <size_t N>
	inline void ToUpper(char(&arr)[N]) noexcept
	{
		for (size_t i = 0; i < N; ++i)
		{
			if (arr[i] == '\0') { break; }
			arr[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(arr[i])));
		}
	}
	template <size_t N>
	inline void ToLower(char(&arr)[N]) noexcept
	{
		for (size_t i = 0; i < N; ++i)
		{
			if (arr[i] == '\0') { break; }
			arr[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(arr[i])));
		}
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
		const size_t copyLength = std::min(sv.size(), N - 1);
		std::memcpy(arr, sv.data(), copyLength);
		arr[copyLength] = '\0';
	}
}
