#include<Import/TextureImporter/Backend/StbImage/StbImageTextureImporter.h>
#include<Import/TextureImporter/Backend/StbImage/stb_image.h>
#include<Core/Utils/String/String.h>
#include<Log/Common/FuncTable.h>

bool Pitaya::Import::StbImageTextureImporter::Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, bool isFlipVerticallyOnLoad, bool isGenerateMipmap, Pitaya::Import::Texture2DImportResult& out)
{
	stbi_set_flip_vertically_on_load(isFlipVerticallyOnLoad);
	unsigned char* data = stbi_load(file.string().c_str(),
		&out.Width, &out.Height, &out.Channels, 4);
	if (!data)
	{
		Pitaya::Log::Error("stbimage load fail GUID:" + guid.ToString() + " path:" + file.string());
		return false;
	}

	Pitaya::Log::Info("stbimage load data success GUID:" + guid.ToString() + " path:" + file.string());
    size_t totalByteSize = static_cast<size_t>(out.Width) * static_cast<size_t>(out.Height) * 4;
    out.Data.clear();
    out.Data.resize(totalByteSize);
    memcpy(out.Data.data(), data, totalByteSize);
    stbi_image_free(data);
	Pitaya::Log::Info("stbimage free data success GUID:" + guid.ToString() + " path:" + file.string());

	//png jpg jpeg tga bmp SRGB | hdr exr RGB
	std::string ext = file.extension().string();
	Pitaya::Core::ToLower(ext);
	out.isNearest = false;
	out.IsSRGB = !(ext == ".hdr" || ext == ".exr");
	out.IsGenerateMipmap = isGenerateMipmap;
	out.GUID = guid;
	return true;
}
bool Pitaya::Import::StbImageTextureImporter::Import(Pitaya::Core::GUID guid, const std::filesystem::path& file, const std::vector<std::filesystem::path>& paths,bool isFlipVerticallyOnLoad, bool isGenerateMipmap, Pitaya::Import::TextureCubemapImportResult& out)
{
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
		memcpy(out.Data[i].data(), data, totalByteSize);
		stbi_image_free(data);
		Pitaya::Log::Info("stbimage free data success GUID:" + guid.ToString() + " path:" + file.string());
	}

	//png jpg jpeg tga bmp SRGB | hdr exr RGB
	std::string ext = file.extension().string();
	Pitaya::Core::ToLower(ext);
	out.isNearest = false;
	out.IsSRGB = !(ext == ".hdr" || ext == ".exr");
	out.IsGenerateMipmap = isGenerateMipmap;
	out.GUID = guid;
	return true;
}
