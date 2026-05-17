#include<Import/TextureImporter/Backend/StbImage/StbImageTextureImporter.h>
#include<Import/TextureImporter/Backend/StbImage/stb_image.h>
#include<Log/Common/FuncTable.h>

bool Pitaya::Import::StbImageTextureImporter::Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, bool isFlipVerticallyOnLoad, bool isGenerateMipmap, Pitaya::Import::Texture2DImportResult& out)
{
	const std::string filePath = file.string();

	// 启发式 Usage 推断（用户可在 Editor 显式覆盖）：
	//   stbi_is_hdr 检测 Radiance .hdr 文件头 → HDR
	//   其他 → Color（即 sRGB）
	// 注意：stb 不解析 sRGB/Linear 元信息，Normal/Metallic/Roughness/AO 等"数据类"贴图
	// 默认会被推断为 Color，PBR 时需调用方显式设为 Linear。
	// 注意：当前路径仍走 stbi_load (8-bit)，HDR 数据会被截断；阶段 5 IBL 上线前切 stbi_loadf。
	const bool isHDR = stbi_is_hdr(filePath.c_str()) != 0;
	stbi_set_flip_vertically_on_load(isFlipVerticallyOnLoad);
	unsigned char* data = stbi_load(filePath.c_str(),
		&out.Width, &out.Height, &out.Channels, 4);
	if (!data)
	{
		Pitaya::Log::Error("stbimage load fail GUID:" + guid.ToString() + " path:" + filePath);
		return false;
	}

	Pitaya::Log::Info("stbimage load data success GUID:" + guid.ToString() + " path:" + filePath);
    size_t totalByteSize = static_cast<size_t>(out.Width) * static_cast<size_t>(out.Height) * 4;
    out.Data.clear();
    out.Data.resize(totalByteSize);
    std::memcpy(out.Data.data(), data, totalByteSize);
    stbi_image_free(data);
	Pitaya::Log::Info("stbimage free data success GUID:" + guid.ToString() + " path:" + filePath);

	out.isNearest = false;
	out.Usage = isHDR ? Pitaya::Asset::TextureUsage::HDR : Pitaya::Asset::TextureUsage::Color;
	out.IsGenerateMipmap = isGenerateMipmap;
	out.GUID = guid;
	return true;
}
bool Pitaya::Import::StbImageTextureImporter::Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, const std::vector<std::filesystem::path>& paths,bool isFlipVerticallyOnLoad, bool isGenerateMipmap, Pitaya::Import::TextureCubemapImportResult& out)
{
	// 6 面统一推断（用首面作准；混 HDR/LDR 不在支持范围）
	const bool isHDR = !paths.empty() && (stbi_is_hdr(paths[0].string().c_str()) != 0);

	stbi_set_flip_vertically_on_load(isFlipVerticallyOnLoad);
	for (uint32_t i = 0; i < 6; i++)
	{
		unsigned char* data = stbi_load(paths[i].string().c_str(),
			&out.Width[i], &out.Height[i], &out.Channels[i], 4);
		if (!data)
		{
			Pitaya::Log::Error("stbimage load fail in cubemap GUID:" + guid.ToString() + " path:" + file.string());
			return false;
		}

		Pitaya::Log::Info("stbimage load data success GUID:" + guid.ToString() + " path:" + file.string());
		size_t totalByteSize = static_cast<size_t>(out.Width[i]) * static_cast<size_t>(out.Height[i]) * 4;
		out.Data[i].clear();
		out.Data[i].resize(totalByteSize);
		std::memcpy(out.Data[i].data(), data, totalByteSize);
		stbi_image_free(data);
		Pitaya::Log::Info("stbimage free data success GUID:" + guid.ToString() + " path:" + file.string());
	}

	out.isNearest = false;
	out.Usage = isHDR ? Pitaya::Asset::TextureUsage::HDR : Pitaya::Asset::TextureUsage::Color;
	out.IsGenerateMipmap = isGenerateMipmap;
	out.GUID = guid;
	return true;
}
