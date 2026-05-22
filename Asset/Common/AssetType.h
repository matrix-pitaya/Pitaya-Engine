#pragma once

#include<type_traits>

namespace Pitaya::Asset
{
	struct Texture;
	struct Material;
	struct Mesh;
	struct Shader;
	struct RenderTarget;
	struct SkyBox;
}

template <typename T>
concept AssetType =
std::is_same_v<T, Pitaya::Asset::Texture> ||
std::is_same_v<T, Pitaya::Asset::Material> ||
std::is_same_v<T, Pitaya::Asset::Mesh> ||
std::is_same_v<T, Pitaya::Asset::Shader> ||
std::is_same_v<T, Pitaya::Asset::RenderTarget> ||
std::is_same_v<T, Pitaya::Asset::SkyBox>;
