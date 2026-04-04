#pragma once

#include<Core/Color/Color.h>
#include<Core/Identifier/GUID.h>
#include<Core/Asset/Asset.h>

#include<Serialize/Serializable.h>

#include<Asset/Common/FuncTable.h>
#include<Asset/Common/Texture.h>
#include<Asset/Common/Shader.h>
#include<GPU/Common/ShaderVariableType.h>
#include<GPU/Common/TextureUsage.h>
#include<Render/Common/RenderQueue.h>

#include<glm.hpp>
#include<unordered_map>
#include<cstdint>
#include<atomic>

namespace Pitaya::Asset
{
	struct Material : public Pitaya::Serialize::Serializable
	{
		struct Property
		{
			struct Variable
			{
				union
				{
					float Float;
					glm::vec2 Float2;
					glm::vec3 Float3;
					glm::vec4 Float4;

					int Int;
					glm::ivec2 Int2;
					glm::ivec3 Int3;
					glm::ivec4 Int4;

					bool Bool;
				};

				Pitaya::GPU::ShaderVariableType Type = Pitaya::GPU::ShaderVariableType::Unknown;

				Variable& operator=(float value) 
				{ 
					Type = Pitaya::GPU::ShaderVariableType::Float;
					Float = value;
					return *this;
				}
				Variable& operator=(int value) 
				{
					Type = Pitaya::GPU::ShaderVariableType::Int;
					Int = value;
					return *this;
				}
				Variable& operator=(bool value) 
				{
					Type = Pitaya::GPU::ShaderVariableType::Bool;
					Bool = value;
					return *this;
				}
				Variable& operator=(const glm::vec2& value)
				{
					Type = Pitaya::GPU::ShaderVariableType::Float2;
					Float2 = value;
					return *this;
				}
				Variable& operator=(const glm::vec3& value) 
				{
					Type = Pitaya::GPU::ShaderVariableType::Float3;
					Float3 = value;
					return *this;
				}
				Variable& operator=(const glm::vec4& value) 
				{
					Type = Pitaya::GPU::ShaderVariableType::Float4;
					Float4 = value;
					return *this;
				}
				Variable& operator=(const glm::ivec2& value)
				{
					Type = Pitaya::GPU::ShaderVariableType::Int2;
					Int2 = value;
					return *this;
				}
				Variable& operator=(const glm::ivec3& value)
				{
					Type = Pitaya::GPU::ShaderVariableType::Int3;
					Int3 = value;
					return *this;
				}
				Variable& operator=(const glm::ivec4& value)
				{
					Type = Pitaya::GPU::ShaderVariableType::Int4;
					Int4 = value;
					return *this;
				}
			};

			std::unordered_map<std::string, Variable> Variables;
		};

		Pitaya::Core::Asset<Pitaya::Asset::Shader> Shader = nullptr;
		Pitaya::Core::Asset<Pitaya::Asset::Texture> Textures[static_cast<size_t>(Pitaya::GPU::TextureUsage::Unknown)] = {};
		Pitaya::Render::RenderQueue RenderQueue = Pitaya::Render::RenderQueue::Geometry;
		uint8_t DrawOrder = 0;
		uint32_t SortId = Next();		//用于生成DrawCommand的SortKey
		Property Property;

	public:
		//Material 17-22
		inline static constexpr const Pitaya::Core::GUID Default = Pitaya::Core::GUID("00000000-0000-0001-0000-000000000000");
		inline static constexpr const Pitaya::Core::GUID Backpack = Pitaya::Core::GUID("00000000-0000-0002-0000-000000000000");
		inline static constexpr const Pitaya::Core::GUID Planet = Pitaya::Core::GUID("00000000-0000-0003-0000-000000000000");
		inline static constexpr const Pitaya::Core::GUID Rock = Pitaya::Core::GUID("00000000-0000-0004-0000-000000000000");

	private:
		inline static uint32_t Next() noexcept
		{
			static std::atomic<uint32_t> sortId = 1;
			return sortId.fetch_add(1, std::memory_order_relaxed);
		}
	
	private:
		void Serialize(Pitaya::Serialize::SerializeContext& context) const override
		{
			//Shader
			auto& shader = context.GetSubContext("Shader");
			shader.Write("GUID", Shader.GetGUID().ToString());

			//textures
			auto& textures = context.GetSubContext("Textures");
			uint32_t count = static_cast<uint32_t>(Pitaya::GPU::TextureUsage::Unknown);
			for (uint32_t i = 0; i < count; i++)
			{
				auto& texture = textures.GetSubContext("Type_" + std::to_string(i));
				texture.Write("GUID", Textures[i].GetGUID().ToString());
			}

			//rednererqueue
			auto& renderQueue = context.GetSubContext("RendererQueue");
			renderQueue.Write("Type", Pitaya::Render::ToString(RenderQueue));

			//draworder
			auto& drawOrder = context.GetSubContext("Draworder");
			drawOrder.Write("Value", DrawOrder);

			//property
			auto& property = context.GetSubContext("Property");
			auto& key_value = property.GetSubContext("Key_Value");
			for (auto& [key, value] : Property.Variables)
			{
				auto& varContext = property.GetSubContext(key);
				varContext.Write("Type", static_cast<uint32_t>(value.Type));
				switch (value.Type)
				{
					case Pitaya::GPU::ShaderVariableType::Bool: varContext.Write(key, value.Bool); break;
					case Pitaya::GPU::ShaderVariableType::Int: varContext.Write(key, value.Int); break;
					case Pitaya::GPU::ShaderVariableType::Int2: varContext.Write(key, value.Int2); break;
					case Pitaya::GPU::ShaderVariableType::Int3: varContext.Write(key, value.Int3); break;
					case Pitaya::GPU::ShaderVariableType::Int4: varContext.Write(key, value.Int4); break;
					case Pitaya::GPU::ShaderVariableType::Float: varContext.Write(key, value.Float); break;
					case Pitaya::GPU::ShaderVariableType::Float2: varContext.Write(key, value.Float2); break;
					case Pitaya::GPU::ShaderVariableType::Float3: varContext.Write(key, value.Float3); break;
					case Pitaya::GPU::ShaderVariableType::Float4: varContext.Write(key, value.Float4); break;
					case Pitaya::GPU::ShaderVariableType::Unknown: break;
					default:break;
				}
			}
		}
		void Deserialize(const Pitaya::Serialize::DeserializeContext& context) override
		{
			std::string str;
			if (context.HasSubContext("Shader"))
			{
				auto& shader = context.GetSubContext("Shader");
				if (shader.Read("GUID", str)) 
				{ 
					Pitaya::Core::GUID guid = Pitaya::Core::GUID(str);
					if (guid) { Shader = Pitaya::Asset::LoadAsset<Pitaya::Asset::Shader>(guid); }
				}
			}

			if (context.HasSubContext("Textures"))
			{
				auto& textures = context.GetSubContext("Textures");
				uint32_t count = static_cast<uint32_t>(Pitaya::GPU::TextureUsage::Unknown);
				for (uint32_t i = 0; i < count; i++)
				{
					str = "Type_" + std::to_string(i);
					if (textures.HasSubContext(str))
					{
						auto& texture = textures.GetSubContext(str);
						if (texture.Read("GUID", str)) 
						{ 
							Pitaya::Core::GUID guid = Pitaya::Core::GUID(str);
							if (guid) { Textures[i] = Pitaya::Asset::LoadAsset<Pitaya::Asset::Texture>(guid); }
						}
					}
				}
			}

			//TODO 实现材质反序列化
		}
	};
}
