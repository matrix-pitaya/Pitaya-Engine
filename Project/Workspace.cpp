#include<Project/Workspace.h>
#include<Log/Common/FuncTable.h>

bool Pitaya::Project::Workspace::Initialize()
{
	return true;
}
void Pitaya::Project::Workspace::Release()
{
    rootPath.clear();
    pitayaFile.clear();
    assetFolder.clear();
    resourceFolder.clear();
    scriptFolder.clear();
}
void Pitaya::Project::Workspace::Resolve(int argc, char** argv)
{
	//获取exe所在目录
	std::filesystem::path exePath = std::filesystem::absolute(std::filesystem::path(argv[0]));
	std::filesystem::path exeDir = exePath.parent_path();

	//确定工作目录
	if (argc <= 1)
	{
		//无参数 使用默认路径 ExeDir/Projects
		rootPath = exeDir / "Projects";
	}
	else
	{
		//有参数 校验.pitaya标识文件
		std::error_code ec;
		std::filesystem::path inputPath = std::filesystem::absolute(std::filesystem::path(argv[1]));
		inputPath = std::filesystem::weakly_canonical(inputPath, ec);

		//必须为.pitaya文件
		if (!std::filesystem::exists(inputPath, ec) ||
			!std::filesystem::is_regular_file(inputPath, ec) ||
			inputPath.extension() != ".pitaya")
		{ throw std::runtime_error("Unable to open this file: " + inputPath.string()); }

		//使用.pitaya所在目录为工作区
		rootPath = inputPath.parent_path();
	}

	//确保工作目录存在
	std::filesystem::create_directories(rootPath);

	//创建固定目录结构
	libFolder = rootPath / "lib";
	assetFolder = rootPath / "Asset";
	resourceFolder = assetFolder / "Resource";
	scriptFolder = assetFolder / "Script";

	std::filesystem::create_directories(libFolder);
	std::filesystem::create_directories(resourceFolder);
	std::filesystem::create_directories(scriptFolder);

	//创建.pitaya作为工作区标识文件
	pitayaFile = rootPath / ".pitaya";
	if (!std::filesystem::exists(pitayaFile))
	{
		std::ofstream file(pitayaFile);
		if (!file.is_open())
		{
			throw std::runtime_error("Failed to create project marker file: " + pitayaFile.string());
		}
	}
}
