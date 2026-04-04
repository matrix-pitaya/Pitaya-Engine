#include<Project/Workspace.h>
#include<Log/Common/FuncTable.h>

bool Pitaya::Project::Workspace::Initialize(int argc, char** argv)
{
    try
    {
        //确定工作空间的根目录路径
        rootPath = argc > 1 ?
            std::filesystem::path(argv[1]) :    //使用工作空间路径
            std::filesystem::absolute(std::filesystem::path(argv[0])).parent_path(); //获取当前exe所在的根目录

        //拼接需要检测或创建的路径
        pitayaFile = rootPath / ".pitaya";
        libFolder = rootPath / "lib";
        assetFolder = rootPath / "Asset";
        resourceFolder = assetFolder / "Resource";
        scriptFolder = assetFolder / "Script";

        //创建文件夹
        std::filesystem::create_directories(libFolder);
        std::filesystem::create_directories(resourceFolder);
        std::filesystem::create_directories(scriptFolder);

        //创建文件
        //检测.pitaya文件是否存在
        if (!std::filesystem::exists(pitayaFile))
        {
            //如果不存在.pitaya则创建
            std::ofstream fileStream(pitayaFile);
            if (!fileStream.is_open())
            {
                Pitaya::Log::Error("create .pitaya file error, path:" + pitayaFile.string());
                return false;
            }
            //TODO 向 .pitaya 里写入初始化的 JSON 配置之类的信息
            fileStream.close();
        }
        return true;
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::string errInfo = "workspace filesystem error: " + std::string(e.what());
        if (!e.path1().empty()) { errInfo += " | Path: " + e.path1().string(); }
        Pitaya::Log::Error(errInfo);
        return false;
    }
    catch (const std::exception& e)
    {
        Pitaya::Log::Error("work space filesystem error: " + std::string(e.what()));
        return false;
    }
    catch (...)
    {
        Pitaya::Log::Error("work space filesystem unkneown error");
        return false;
    }
}
void Pitaya::Project::Workspace::Release()
{
    rootPath.clear();
    pitayaFile.clear();
    assetFolder.clear();
    resourceFolder.clear();
    scriptFolder.clear();
}
