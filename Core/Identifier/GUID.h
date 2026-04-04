#pragma once

#include<string>
#include<cstdint>
#include<stdexcept>

namespace Pitaya::Core
{
	struct GUID
	{
		constexpr GUID() = default;
		constexpr GUID(std::string_view strview)
		{
			constexpr auto hex_to_byte = [](char c) constexpr -> uint8_t
				{
					if (c >= '0' && c <= '9') return static_cast<uint8_t>(c - '0');
					if (c >= 'a' && c <= 'f') return static_cast<uint8_t>(c - 'a' + 10);
					if (c >= 'A' && c <= 'F') return static_cast<uint8_t>(c - 'A' + 10);
					return 0;
				};

			uint8_t bytes[16] = {};
			size_t byte_idx = 0;
			for (size_t i = 0; i < strview.size() && byte_idx < 16; ++i)
			{
				if (strview[i] == '-') { continue; }
				if (i + 1 >= strview.size()) { break; }

				uint8_t high = hex_to_byte(strview[i]);
				uint8_t low = hex_to_byte(strview[i + 1]);

				bytes[byte_idx++] = static_cast<uint8_t>((high << 4) | low);
				++i;
			}

			auto pack_u64 = [](const uint8_t* p) constexpr -> uint64_t {
				return (static_cast<uint64_t>(p[0]) << 0)
					| (static_cast<uint64_t>(p[1]) << 8)
					| (static_cast<uint64_t>(p[2]) << 16)
					| (static_cast<uint64_t>(p[3]) << 24)
					| (static_cast<uint64_t>(p[4]) << 32)
					| (static_cast<uint64_t>(p[5]) << 40)
					| (static_cast<uint64_t>(p[6]) << 48)
					| (static_cast<uint64_t>(p[7]) << 56); };

			value[0] = pack_u64(&bytes[0]);
			value[1] = pack_u64(&bytes[8]);
		}

		constexpr bool operator==(const GUID& other) const noexcept
		{ 
			return value[1] == other.value[1] && value[0] == other.value[0];
		}
		constexpr bool operator!=(const GUID& other) const noexcept
		{ 
			return !(*this == other);
		}
		constexpr bool operator<(const GUID& other) const noexcept
		{
			return value[1] < other.value[1] || (value[1] == other.value[1] && value[0] < other.value[0]);
		}

		explicit constexpr operator bool() const noexcept
		{
			return value[1] != 0 || value[0] != 0;
		}
		constexpr uint64_t& operator[](size_t index) noexcept
		{
			return value[index];
		}
		constexpr const uint64_t& operator[](size_t index) const noexcept
		{
			return value[index];
		}

		inline uint64_t at(size_t index) const
		{
			if (!(index == 0 || index == 1))
			{
				throw std::runtime_error("GUID Index Error!");
			}

			return value[index];
		}

		std::string ToString() const noexcept
		{
			uint64_t value[2] = {};
			value[0] = this->value[0];
			value[1] = this->value[1];

			char buf[37] = {};
			const uint8_t* p = reinterpret_cast<const uint8_t*>(value);

			std::snprintf(buf, sizeof(buf),
				"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
				p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);

			return std::string(buf);
		}

		static GUID New();

	private:
		uint64_t value[2] = {};
	};
}

namespace std 
{
	template<> 
	struct hash<Pitaya::Core::GUID>
	{
		size_t operator()(const Pitaya::Core::GUID& guid) const noexcept
		{
			auto h1 = std::hash<uint64_t>{}(guid[1]);
			auto h2 = std::hash<uint64_t>{}(guid[0]);
			return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
		}
	};
}
