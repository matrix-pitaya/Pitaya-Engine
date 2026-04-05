#pragma once

#include<Core/Utils/StateFlags.h>
#include<GPU/Common/ShaderType.h>
#include<GPU/Common/ShaderVariableType.h>
#include<GPU/Common/Identifier.h>

namespace Pitaya::GPU
{
	class RHIDevice;
	class Shader
	{
		friend class Pitaya::GPU::RHIDevice;
	public:
		struct Location
		{
		public:
			constexpr Location(int32_t value = -1) noexcept
				:value(value) {}

			constexpr bool operator==(const Location& other) const noexcept
			{
				return value == other.value;
			}
			constexpr bool operator!=(const Location& other) const noexcept
			{
				return value != other.value;
			}
			constexpr bool operator<(const Location& other) const noexcept
			{
				return value < other.value;
			}

			explicit constexpr operator bool() const noexcept
			{
				return value >= 0;
			}
			constexpr operator int32_t() const noexcept
			{
				return value;
			}
			constexpr Location& operator=(int32_t value) noexcept
			{
				this->value = value;
				return *this;
			}

		public:
			static const Location Invalid;

		private:
			int32_t value = -1;
		};

	protected:
		Shader(const char* vertexSource, const char* fragmentSource) {}
		Shader(const char* vertexSource, const char* fragmentSource, const char* geometrySource) {}
		virtual ~Shader() = default;

	public:
		virtual Identifier<Shader> GetGPUIdentifier() const = 0;

		inline static uint32_t GetShaderVariableTypeSize(ShaderVariableType type) noexcept
		{
			switch (type)
			{
				case Pitaya::GPU::ShaderVariableType::Float:    return 4;
				case Pitaya::GPU::ShaderVariableType::Float2:   return 4 * 2;
				case Pitaya::GPU::ShaderVariableType::Float3:   return 4 * 3;
				case Pitaya::GPU::ShaderVariableType::Float4:   return 4 * 4;
				case Pitaya::GPU::ShaderVariableType::Mat3:     return 4 * 3 * 3;
				case Pitaya::GPU::ShaderVariableType::Mat4:     return 4 * 4 * 4;
				case Pitaya::GPU::ShaderVariableType::Int:      return 4;
				case Pitaya::GPU::ShaderVariableType::Int2:     return 4 * 2;
				case Pitaya::GPU::ShaderVariableType::Int3:     return 4 * 3;
				case Pitaya::GPU::ShaderVariableType::Int4:     return 4 * 4;
				case Pitaya::GPU::ShaderVariableType::Bool:     return 1;
				case Pitaya::GPU::ShaderVariableType::Unknown:  return 0;
				default:										return 0;
			}
		}

	private:
		static Shader* Create(const char* vertexSource, const char* fragmentSource);
		static Shader* Create(const char* vertexSource, const char* fragmentSource, const char* geometrySource);

	public:
		inline static const ShaderType VF = static_cast<ShaderType>(Pitaya::Core::CombineMask<ShaderType>(
			ShaderType::Vertex, ShaderType::Fragment));
		inline static const ShaderType VFG = static_cast<ShaderType>(Pitaya::Core::CombineMask<ShaderType>(
			ShaderType::Vertex, ShaderType::Geometry, ShaderType::Fragment));
	};

	inline constexpr const Shader::Location Shader::Location::Invalid = Location(0);
}
