#include<Application/Launcher.h>
#include<Engine/API/func.h>
#include<Editor/API/def.h>
#include<Hook/def.h>
#include<Core/Utils/File.h>
#include<Core/Utils/Memory.h>
#include<Core/Utils/System.h>

#include<mimalloc.h>
#include<exception>
#include<stdexcept>
#include<filesystem>
#include<fstream>
#include<sstream>

#if defined(PITAYA_PLATFORM_WINDOWS)
#include<dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#endif

namespace
{
    void* Editordll = nullptr;
    void AttachEditorEnvironment(int argc, char** argv)
    {
        DISCARD_HOOK
#if defined(PITAYA_PLATFORM_WINDOWS)
        if (Editordll) { FreeLibrary(static_cast<HMODULE>(Editordll)); Editordll = nullptr; }
        const std::filesystem::path dllPath = Pitaya::Core::GetExecutableDirectory() / "Editor.dll";
        Editordll = LoadLibraryA(dllPath.string().c_str());
        if (Editordll)
        {
            auto func = reinterpret_cast<void(EDITOR_CALL*)(int, char**)>(GetProcAddress(static_cast<HMODULE>(Editordll), "AttachRuntimeEnv"));
            if (func) { func(argc, argv); }
        }
#endif
    }
    void DetachEditorEnvironment()
    {
        Pitaya::Core::GenerateFile(Pitaya::Core::GetWorkspace(), "hook.state",
            "Pitaya Engine Hook State File", HOOK_STATE);
        DISCARD_HOOK
#if defined(PITAYA_PLATFORM_WINDOWS)
        if (Editordll) { FreeLibrary(static_cast<HMODULE>(Editordll)); Editordll = nullptr; }
#endif
    }
    void SafeTerminateInSEH()
    {
#if defined(PITAYA_PLATFORM_WINDOWS)
        __try
        {
            Pitaya::Engine::Terminate();
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            MessageBoxA(NULL, "Engine Terminate Crash", "Error", MB_OK);
        }
#endif
    }
    void SetExceptionCrashFilter()
    {
#if defined(PITAYA_PLATFORM_WINDOWS)
        SetUnhandledExceptionFilter(
            [](EXCEPTION_POINTERS* pExceptionInfo) -> LONG WINAPI
            {
                auto* record = pExceptionInfo->ExceptionRecord;
                auto* context = pExceptionInfo->ContextRecord;
                const char* exName = "Unknown Exception";
                switch (record->ExceptionCode)
                {
                    case EXCEPTION_ACCESS_VIOLATION:         exName = "Access Violation";         break;
                    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    exName = "Array Out of Bounds";      break;
                    case EXCEPTION_STACK_OVERFLOW:           exName = "Stack Overflow";           break;
                    case EXCEPTION_ILLEGAL_INSTRUCTION:      exName = "Illegal Instruction";      break;
                    case EXCEPTION_INT_DIVIDE_BY_ZERO:       exName = "Divide By Zero";           break;
                    case EXCEPTION_DATATYPE_MISALIGNMENT:    exName = "Data Misalignment";        break;
                    case EXCEPTION_IN_PAGE_ERROR:            exName = "In Page Error";            break;
                    case EXCEPTION_FLT_DENORMAL_OPERAND:     exName = "Float Denormal Operand";   break;
                    case EXCEPTION_FLT_DIVIDE_BY_ZERO:       exName = "Float Divide By Zero";     break;
                }

                uintptr_t faultAddr = (record->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && record->NumberParameters >= 2) ?
                    record->ExceptionInformation[1] : 0;
                std::string stackTrace;
                {
                    HANDLE process = GetCurrentProcess();
                    HANDLE thread  = GetCurrentThread();
                    SymInitialize(process, nullptr, TRUE);

                    STACKFRAME64 frame = {};
                    frame.AddrPC.Offset    = context->Rip;
                    frame.AddrPC.Mode      = AddrModeFlat;
                    frame.AddrFrame.Offset = context->Rbp;
                    frame.AddrFrame.Mode   = AddrModeFlat;
                    frame.AddrStack.Offset = context->Rsp;
                    frame.AddrStack.Mode   = AddrModeFlat;

                    char symBuf[sizeof(SYMBOL_INFO) + 256];
                    auto* symbol = reinterpret_cast<SYMBOL_INFO*>(symBuf);
                    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
                    symbol->MaxNameLen   = 255;

                    IMAGEHLP_LINE64 line = {};
                    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

                    for (int i = 0; i < 32; ++i)
                    {
                        if (!StackWalk64(IMAGE_FILE_MACHINE_AMD64, process, thread, &frame, context, 
                            nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr)) { break; }

                        DWORD64 addr = frame.AddrPC.Offset;
                        char    name[256] = "???";
                        DWORD   disp = 0;

                        if (SymFromAddr(process, addr, nullptr, symbol)) 
                            { strncpy_s(name, symbol->Name, _TRUNCATE); }

                        char lineStr[64] = "";
                        if (SymGetLineFromAddr64(process, addr, &disp, &line))
                            { snprintf(lineStr, sizeof(lineStr), "  %s:%lu", line.FileName, line.LineNumber); }

                        char frameStr[512];
                        snprintf(frameStr, sizeof(frameStr), 
                            "  [%d] 0x%016llX %s + 0x%lX%s\n", i, addr, name, disp, lineStr);
                        stackTrace += frameStr;
                    }
                    SymCleanup(process);
                }

                std::ostringstream crashInfo;
                crashInfo << "Exception: " << exName << " (0x" << std::hex << record->ExceptionCode << ")\n";
                crashInfo << "Address:    0x" << std::hex << reinterpret_cast<uintptr_t>(record->ExceptionAddress) << "\n";
                if (faultAddr) { crashInfo << "FaultAddr:  0x" << std::hex << faultAddr << "\n"; }
                crashInfo << "Stack:\n" << stackTrace << "\n";
                Pitaya::Core::GenerateFile(Pitaya::Core::GetWorkspace(), "crash.log",
                    "Pitaya Engine Crash Report", crashInfo.str().c_str());

                auto dumpPath = Pitaya::Core::GetWorkspace() / "crash.log";
                std::string msg = std::string(exName) + "\n\n" + "Crash dump: " + dumpPath.string();
                Pitaya::Core::PopMessageBox("Fatal Error", msg.c_str());
                Pitaya::Core::GenerateFile(Pitaya::Core::GetWorkspace(), "memory.profile",
                    "Pitaya Engine Memory Analysis File", Pitaya::Core::GetMemoryState().c_str());
                SafeTerminateInSEH();
                DetachEditorEnvironment();
                return EXCEPTION_EXECUTE_HANDLER;
            });
#endif
    }
    void CheckRuntimeEnvironment(int argc, char** argv)
    {
        //Workspace Check
        {
            const std::filesystem::path workspace = Pitaya::Core::GetWorkspace();
            if (argc > 1)
            {
                std::error_code ec;
                std::filesystem::path inputPath = std::filesystem::absolute(argv[1]);
                inputPath = std::filesystem::weakly_canonical(inputPath, ec);
                if (!std::filesystem::exists(inputPath, ec) ||
                    !std::filesystem::is_regular_file(inputPath, ec) ||
                    inputPath.filename() != ".pitaya")
                {
                    Pitaya::Core::PopMessageBox("Error", ("Unable to open this file: " + inputPath.string()).c_str());
                    Pitaya::Core::Terminate(-1);
                }
            }

            const std::filesystem::path pitayaFile = workspace / ".pitaya";
            if (!std::filesystem::exists(pitayaFile))
            {
                std::filesystem::create_directories(pitayaFile.parent_path());
                std::ofstream file(pitayaFile);
                if (!file.is_open())
                {
                    std::error_code ec(errno, std::generic_category());
                    Pitaya::Core::PopMessageBox("Error", ("Failed to create project marker file!\npath: " + pitayaFile.string() + "\nerror: " + ec.message()).c_str());
                    Pitaya::Core::Terminate(-1);
                }
                file.close();
            }

            auto CheckFolder = [](const std::filesystem::path& folder)
                {
                    std::error_code ec;
                    if (!std::filesystem::create_directories(folder, ec) && ec)
                    {
                        Pitaya::Core::PopMessageBox("Error", ("Failed to create directory!\npath:" + folder.string() + "\nerror:" + ec.message()).c_str());
                        Pitaya::Core::Terminate(-1);
                    }
                };
            CheckFolder(workspace / "lib");
            CheckFolder(workspace / "Asset" / "Resource");
            CheckFolder(workspace / "Asset" / "Script");
            CheckFolder(workspace / "Asset" / "Scene");
        }

        //Mono Check
        //{
        //	const constexpr std::string_view CheckList[] =
        //	{
        //		"bin/mono-2.0-sgen.dll",
        //		"lib/mono/4.5/mscorlib.dll",	
        //		"lib/mono/4.5/System.dll",
        //		"lib/mono/4.5/System.Core.dll",
        //		"etc/mono/config",
        //		"etc/mono/4.5/machine.config"
        //	};
        //
        //	const std::filesystem::path folder = Pitaya::Core::GetExecutableDirectory() / "mono";
        //	for (const std::string_view fileName : CheckList)
        //	{
        //		if (fileName.empty()) { continue; }
        //		std::filesystem::path absPath = folder / fileName;
        //		if (!std::filesystem::exists(absPath))
        //		{
        //			Pitaya::Core::PopMessageBox("Error", ("miss mono core file: " + absPath.string()).c_str());
        //			Pitaya::Core::Terminate(-1);
        //		}
        //	}
        //}
    }
}

int Pitaya::Application::Execute(int argc, char** argv)
{
    try
    {
        SetExceptionCrashFilter();
        CheckRuntimeEnvironment(argc, argv);
        AttachEditorEnvironment(argc, argv);
        int32_t exitcode = Pitaya::Engine::Execute(argc, argv);
        DetachEditorEnvironment();
        Pitaya::Core::GenerateFile(Pitaya::Core::GetWorkspace(), "memory.profile",
            "Pitaya Engine Memory Analysis File", Pitaya::Core::GetMemoryState().c_str());
        return exitcode;
    }
    catch (const std::exception& e)
    {
        Pitaya::Core::PopMessageBox("Error", e.what());
        Pitaya::Core::GenerateFile(Pitaya::Core::GetWorkspace(), "memory.profile",
            "Pitaya Engine Memory Analysis File", Pitaya::Core::GetMemoryState().c_str());
        SafeTerminateInSEH();
        DetachEditorEnvironment();
        return -1;
    }
    catch (...)
    {
        Pitaya::Core::PopMessageBox("Error", "Unknown error!");
        Pitaya::Core::GenerateFile(Pitaya::Core::GetWorkspace(), "memory.profile",
            "Pitaya Engine Memory Analysis File", Pitaya::Core::GetMemoryState().c_str());
        SafeTerminateInSEH();
        DetachEditorEnvironment();
        return -1;
    }
}
