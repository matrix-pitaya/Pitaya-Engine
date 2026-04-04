#pragma once

#include<Core/Identifier/GUID.h>
#include<GPU/Common/Identifier.h>
#include<GPU/Common/TextureType.h>

namespace Pitaya::Asset
{
	struct Texture
	{
		Pitaya::GPU::Identifier<GPU::Texture> Id;
		Pitaya::GPU::TextureType Type = GPU::TextureType::Texture2D;

		//Texture 1-12
		inline static constexpr const Pitaya::Core::GUID Icon = Pitaya::Core::GUID("00000000-0000-0000-0000-000000000000");
		inline static constexpr const Pitaya::Core::GUID White = Pitaya::Core::GUID("00000000-0000-0000-0000-000000000001");
		inline static constexpr const Pitaya::Core::GUID Box = Pitaya::Core::GUID("00000000-0000-0000-0000-000000000002");
		inline static constexpr const Pitaya::Core::GUID Face = Pitaya::Core::GUID("00000000-0000-0000-0000-000000000003");
		inline static constexpr const Pitaya::Core::GUID Gress = Pitaya::Core::GUID("00000000-0000-0000-0000-000000000004");
		inline static constexpr const Pitaya::Core::GUID Huolg = Pitaya::Core::GUID("00000000-0000-0000-0000-000000000005");
		inline static constexpr const Pitaya::Core::GUID Irony = Pitaya::Core::GUID("00000000-0000-0000-0000-000000000006");
		inline static constexpr const Pitaya::Core::GUID TrasnformWindow = Pitaya::Core::GUID("00000000-0000-0000-0000-000000000007");
		inline static constexpr const Pitaya::Core::GUID Wall = Pitaya::Core::GUID("00000000-0000-0000-0000-000000000008");
		inline static constexpr const Pitaya::Core::GUID Wood = Pitaya::Core::GUID("00000000-0000-0000-0000-000000000009");
		inline static constexpr const Pitaya::Core::GUID Glacier = Pitaya::Core::GUID("00000000-0000-0000-0000-0000000000010");
		inline static constexpr const Pitaya::Core::GUID Palace = Pitaya::Core::GUID("00000000-0000-0000-0000-000000000011");
		inline static constexpr const Pitaya::Core::GUID Backpack_Ao = Pitaya::Core::GUID("00000000-0000-0000-0000-000000000012");
		inline static constexpr const Pitaya::Core::GUID Backpack_Diffuse = Pitaya::Core::GUID("00000000-0000-0000-0000-000000000013");
		inline static constexpr const Pitaya::Core::GUID Backpack_Normal = Pitaya::Core::GUID("00000000-0000-0000-0000-000000000014");
		inline static constexpr const Pitaya::Core::GUID Backpack_Roughness = Pitaya::Core::GUID("00000000-0000-0000-0000-000000000015");
		inline static constexpr const Pitaya::Core::GUID Backpack_Specular = Pitaya::Core::GUID("00000000-0000-0000-0000-000000000016");
		inline static constexpr const Pitaya::Core::GUID Planet_Quom = Pitaya::Core::GUID("00000000-0000-0000-0000-000000000017");
		inline static constexpr const Pitaya::Core::GUID Rock_Surface = Pitaya::Core::GUID("00000000-0000-0000-0000-000000000018");
	};
}
