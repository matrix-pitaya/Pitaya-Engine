#pragma once

#include<Hook/HookPoint.h>
#include<cstdio>

#define MOUNT_PREBEGINFRAME_HOOK(func,name) do { Pitaya::Engine::Hook_PreBeginFrame.Func = func; Pitaya::Engine::Hook_PreBeginFrame.Name = name; } while(false);
#define MOUNT_PREFIXEDUPDATE_HOOK(func,name)  do { Pitaya::Engine::Hook_PreFixedUpdate.Func = func; Pitaya::Engine::Hook_PreFixedUpdate.Name = name; } while(false);
#define MOUNT_PREUPDATE_HOOK(func,name)  do { Pitaya::Engine::Hook_PreUpdate.Func = func; Pitaya::Engine::Hook_PreUpdate.Name = name; } while(false);
#define MOUNT_PRELATEUPDATE_HOOK(func,name)  do { Pitaya::Engine::Hook_PreLateUpdate.Func = func; Pitaya::Engine::Hook_PreLateUpdate.Name = name; } while(false);
#define MOUNT_PREENDFRAME_HOOK(func,name)  do { Pitaya::Engine::Hook_PreEndFrame.Func = func; Pitaya::Engine::Hook_PreEndFrame.Name = name; } while(false);
#define MOUNT_POSTRENDERERINTIALIZE_HOOK(func,name) do { Pitaya::Engine::Hook_PostRendererInitialize.Func = func; Pitaya::Engine::Hook_PostRendererInitialize.Name = name; } while(false);
#define MOUNT_POSTRENDERERRELEASE_HOOK(func,name) do { Pitaya::Engine::Hook_PostRendererRelease.Func = func; Pitaya::Engine::Hook_PostRendererRelease.Name = name; } while(false);
#define MOUNT_POSTRENDERERPARSECOMMAND_HOOK(func,name) do { Pitaya::Engine::Hook_PostRendererParseCommand.Func = func; Pitaya::Engine::Hook_PostRendererParseCommand.Name = name; } while(false);
#define MOUNT_POSTRENDERERSWAPBUFFER_HOOK(func,name) do { Pitaya::Engine::Hook_PostRendererSwapBuffer.Func = func; Pitaya::Engine::Hook_PostRendererSwapBuffer.Name = name; } while(false);
#define MOUNT_POSTRENDERERBEGINRENDERFRAME_HOOK(func,name)do { Pitaya::Engine::Hook_PostRendererBeginRenderFrame.Func = func; Pitaya::Engine::Hook_PostRendererBeginRenderFrame.Name = name; } while(false);
#define MOUNT_PRERENDERERENDRENDERFRAME_HOOK(func,name) do { Pitaya::Engine::Hook_PreRendereEndRenderFrame.Func = func; Pitaya::Engine::Hook_PreRendereEndRenderFrame.Name = name; } while(false);
#define MOUNT_POSTRENDERCONTEXTINITIALIZED_HOOK(func,name) do { Pitaya::Engine::Hook_PostRenderContextInitialized.Func = func; Pitaya::Engine::Hook_PostRenderContextInitialized.Name = name; } while(false);
#define MOUNT_PRERENDERCONTEXTINRELEASED_HOOK(func,name) do { Pitaya::Engine::Hook_PreRenderContextRelease.Func = func; Pitaya::Engine::Hook_PreRenderContextRelease.Name = name; } while(false);
#define MOUNT_PRERENDERPIPELINEEXECUTE_HOOK(func,name) do { Pitaya::Engine::Hook_PreRenderPipelineExecute.Func = func; Pitaya::Engine::Hook_PreRenderPipelineExecute.Name = name; } while(false);
#define MOUNT_SHOULDWAKEUPRENDERTHREAD_HOOK(func,name) do { Pitaya::Engine::Hook_ShouldWakeupRenderThread.Func = func; Pitaya::Engine::Hook_ShouldWakeupRenderThread.Name = name; } while(false);
#define MOUNT_POSTCHRONOMETERTICK_HOOK(func,name) do { Pitaya::Engine::Hook_PostChronometerTick.Func = func; Pitaya::Engine::Hook_PostChronometerTick.Name = name; } while(false);
#define MOUNT_POSTLOG_HOOK(func,name) do { Pitaya::Engine::Hook_PostLog.Func = func; Pitaya::Engine::Hook_PostLog.Name = name; } while(false);


#define INVOKE_PREBEGINFRAME_HOOK if(Pitaya::Engine::Hook_PreBeginFrame.Func) { Pitaya::Engine::Hook_PreBeginFrame.Func(); }
#define INVOKE_PREFIXEDUPDATE_HOOK if(Pitaya::Engine::Hook_PreFixedUpdate.Func) { Pitaya::Engine::Hook_PreFixedUpdate.Func(); }
#define INVOKE_PREUPDATE_HOOK if(Pitaya::Engine::Hook_PreUpdate.Func) { Pitaya::Engine::Hook_PreUpdate.Func(); }
#define INVOKE_PRELATEUPDATE_HOOK if(Pitaya::Engine::Hook_PreLateUpdate.Func) { Pitaya::Engine::Hook_PreLateUpdate.Func(); }
#define INVOKE_PREENDFRAME_HOOK if(Pitaya::Engine::Hook_PreEndFrame.Func) { Pitaya::Engine::Hook_PreEndFrame.Func(); }
#define INVOKE_POSTRENDERERINTIALIZE_HOOK(NativeWindow) if(Pitaya::Engine::Hook_PostRendererInitialize.Func) { Pitaya::Engine::Hook_PostRendererInitialize.Func(NativeWindow); } 
#define INVOKE_POSTRENDERERRELEASE_HOOK if(Pitaya::Engine::Hook_PostRendererRelease.Func) { Pitaya::Engine::Hook_PostRendererRelease.Func(); }
#define INVOKE_POSTRENDERERPARSECOMMAND_HOOK if(Pitaya::Engine::Hook_PostRendererParseCommand.Func) { Pitaya::Engine::Hook_PostRendererParseCommand.Func(); }
#define INVOKE_POSTRENDERERSWAPBUFFER_HOOK if(Pitaya::Engine::Hook_PostRendererSwapBuffer.Func) { Pitaya::Engine::Hook_PostRendererSwapBuffer.Func(); }
#define INVOKE_POSTRENDERERBEGINRENDERFRAME_HOOK if(Pitaya::Engine::Hook_PostRendererBeginRenderFrame.Func){ Pitaya::Engine::Hook_PostRendererBeginRenderFrame.Func(); } 
#define INVOKE_PRERENDERERENDRENDERFRAME_HOOK if(Pitaya::Engine::Hook_PreRendereEndRenderFrame.Func) { Pitaya::Engine::Hook_PreRendereEndRenderFrame.Func(); }
#define INVOKE_POSTRENDERCONTEXTINITIALIZED_HOOK if(Pitaya::Engine::Hook_PostRenderContextInitialized.Func) { Pitaya::Engine::Hook_PostRenderContextInitialized.Func(); }
#define INVOKE_PRERENDERCONTEXTINRELEASED_HOOK if(Pitaya::Engine::Hook_PreRenderContextRelease.Func) { Pitaya::Engine::Hook_PreRenderContextRelease.Func(); }
#define INVOKE_PRERENDERPIPELINEEXECUTE_HOOK(PassKey, RenderPipeline) if(Pitaya::Engine::Hook_PreRenderPipelineExecute.Func) { Pitaya::Engine::Hook_PreRenderPipelineExecute.Func(PassKey, RenderPipeline); }
#define INVOKE_SHOULDWAKEUPRENDERTHREAD_HOOK (Pitaya::Engine::Hook_ShouldWakeupRenderThread.Func ? Pitaya::Engine::Hook_ShouldWakeupRenderThread.Func() : false)
#define INVOKE_POSTCHRONOMETERTICK_HOOK if(Pitaya::Engine::Hook_PostChronometerTick.Func) { Pitaya::Engine::Hook_PostChronometerTick.Func(); }
#define INVOKE_POSTLOG_HOOK(Leve,Message) if(Pitaya::Engine::Hook_PostLog.Func) { Pitaya::Engine::Hook_PostLog.Func(Leve,Message); }


#define DISCARD_HOOK                                                                                                                                \
    do                                                                                                                                              \
    {                                                                                                                                               \
        Pitaya::Engine::Hook_PreBeginFrame.Func = nullptr; Pitaya::Engine::Hook_PreBeginFrame.Name = nullptr;                                       \
        Pitaya::Engine::Hook_PreFixedUpdate.Func = nullptr; Pitaya::Engine::Hook_PreFixedUpdate.Name = nullptr;                                     \
        Pitaya::Engine::Hook_PreUpdate.Func = nullptr; Pitaya::Engine::Hook_PreUpdate.Name = nullptr;                                               \
        Pitaya::Engine::Hook_PreLateUpdate.Func = nullptr; Pitaya::Engine::Hook_PreLateUpdate.Name = nullptr;                                       \
        Pitaya::Engine::Hook_PreEndFrame.Func = nullptr; Pitaya::Engine::Hook_PreEndFrame.Name = nullptr;                                           \
        Pitaya::Engine::Hook_PostRendererInitialize.Func = nullptr; Pitaya::Engine::Hook_PostRendererInitialize.Name = nullptr;                     \
        Pitaya::Engine::Hook_PostRendererRelease.Func = nullptr; Pitaya::Engine::Hook_PostRendererRelease.Name = nullptr;                           \
        Pitaya::Engine::Hook_PostRendererParseCommand.Func = nullptr; Pitaya::Engine::Hook_PostRendererParseCommand.Name = nullptr;                 \
        Pitaya::Engine::Hook_PostRendererSwapBuffer.Func = nullptr; Pitaya::Engine::Hook_PostRendererSwapBuffer.Name = nullptr;                     \
        Pitaya::Engine::Hook_PostRendererBeginRenderFrame.Func = nullptr; Pitaya::Engine::Hook_PostRendererBeginRenderFrame.Name = nullptr;         \
        Pitaya::Engine::Hook_PreRendereEndRenderFrame.Func = nullptr; Pitaya::Engine::Hook_PreRendereEndRenderFrame.Name = nullptr;                 \
        Pitaya::Engine::Hook_PostRenderContextInitialized.Func = nullptr; Pitaya::Engine::Hook_PostRenderContextInitialized.Name = nullptr;         \
        Pitaya::Engine::Hook_PreRenderContextRelease.Func = nullptr; Pitaya::Engine::Hook_PreRenderContextRelease.Name = nullptr;                   \
        Pitaya::Engine::Hook_PreRenderPipelineExecute.Func = nullptr; Pitaya::Engine::Hook_PreRenderPipelineExecute.Name = nullptr;                 \
        Pitaya::Engine::Hook_ShouldWakeupRenderThread.Func = nullptr; Pitaya::Engine::Hook_ShouldWakeupRenderThread.Name = nullptr;                 \
        Pitaya::Engine::Hook_PostChronometerTick.Func = nullptr; Pitaya::Engine::Hook_PostChronometerTick.Name = nullptr;                           \
        Pitaya::Engine::Hook_PostLog.Func = nullptr; Pitaya::Engine::Hook_PostLog.Name = nullptr;                                                   \
    } while (false);


#define HOOK_STATE                                                                            \
    []() -> const char*                                                                       \
    {                                                                                         \
        static char buffer[4096] = {};                                                        \
        sprintf_s(buffer, sizeof(buffer),                                                     \
            "Hook Name                           State\n"                                     \
            "PreBeginFrame                       %s\n"                                        \
            "PreFixedUpdate                      %s\n"                                        \
            "PreUpdate                           %s\n"                                        \
            "PreLateUpdate                       %s\n"                                        \
            "PreEndFrame                         %s\n"                                        \
            "PostRendererInitialize              %s\n"                                        \
            "PostRendererRelease                 %s\n"                                        \
            "PostRendererParseCommand            %s\n"                                        \
            "PostRendererSwapBuffer              %s\n"                                        \
            "PostRendererBeginRenderFrame        %s\n"                                        \
            "PreRendereEndRenderFrame            %s\n"                                        \
            "PostRenderContextInitialized        %s\n"                                        \
            "PreRenderContextRelease             %s\n"                                        \
            "PreRenderPipelineExecute            %s\n"                                        \
            "ShouldWakeupRenderThread            %s\n"                                        \
            "PostChronometerTick                 %s\n"                                        \
            "PostLog                             %s\n",                                       \
            Pitaya::Engine::Hook_PreBeginFrame.Func ? (Pitaya::Engine::Hook_PreBeginFrame.Name ? Pitaya::Engine::Hook_PreBeginFrame.Name : "UnNamed") : "Unmounted",                                                \
            Pitaya::Engine::Hook_PreFixedUpdate.Func ? (Pitaya::Engine::Hook_PreFixedUpdate.Name ? Pitaya::Engine::Hook_PreFixedUpdate.Name : "UnNamed") : "Unmounted",                                             \
            Pitaya::Engine::Hook_PreUpdate.Func ? (Pitaya::Engine::Hook_PreUpdate.Name ? Pitaya::Engine::Hook_PreUpdate.Name : "UnNamed") : "Unmounted",                                                            \
            Pitaya::Engine::Hook_PreLateUpdate.Func ? (Pitaya::Engine::Hook_PreLateUpdate.Name ? Pitaya::Engine::Hook_PreLateUpdate.Name : "UnNamed") : "Unmounted",                                                \
            Pitaya::Engine::Hook_PreEndFrame.Func ? (Pitaya::Engine::Hook_PreEndFrame.Name ? Pitaya::Engine::Hook_PreEndFrame.Name : "UnNamed") : "Unmounted",                                                      \
            Pitaya::Engine::Hook_PostRendererInitialize.Func ? (Pitaya::Engine::Hook_PostRendererInitialize.Name ? Pitaya::Engine::Hook_PostRendererInitialize.Name : "UnNamed") : "Unmounted",                     \
            Pitaya::Engine::Hook_PostRendererRelease.Func ? (Pitaya::Engine::Hook_PostRendererRelease.Name ? Pitaya::Engine::Hook_PostRendererRelease.Name : "UnNamed") : "Unmounted",                              \
            Pitaya::Engine::Hook_PostRendererParseCommand.Func ? (Pitaya::Engine::Hook_PostRendererParseCommand.Name ? Pitaya::Engine::Hook_PostRendererParseCommand.Name : "UnNamed") : "Unmounted",               \
            Pitaya::Engine::Hook_PostRendererSwapBuffer.Func ? (Pitaya::Engine::Hook_PostRendererSwapBuffer.Name ? Pitaya::Engine::Hook_PostRendererSwapBuffer.Name : "UnNamed") : "Unmounted",                     \
            Pitaya::Engine::Hook_PostRendererBeginRenderFrame.Func ? (Pitaya::Engine::Hook_PostRendererBeginRenderFrame.Name ? Pitaya::Engine::Hook_PostRendererBeginRenderFrame.Name : "UnNamed") : "Unmounted",   \
            Pitaya::Engine::Hook_PreRendereEndRenderFrame.Func ? (Pitaya::Engine::Hook_PreRendereEndRenderFrame.Name ? Pitaya::Engine::Hook_PreRendereEndRenderFrame.Name : "UnNamed") : "Unmounted",               \
            Pitaya::Engine::Hook_PostRenderContextInitialized.Func ? (Pitaya::Engine::Hook_PostRenderContextInitialized.Name ? Pitaya::Engine::Hook_PostRenderContextInitialized.Name : "UnNamed") : "Unmounted",   \
            Pitaya::Engine::Hook_PreRenderContextRelease.Func ? (Pitaya::Engine::Hook_PreRenderContextRelease.Name ? Pitaya::Engine::Hook_PreRenderContextRelease.Name : "UnNamed") : "Unmounted",                  \
            Pitaya::Engine::Hook_PreRenderPipelineExecute.Func ? (Pitaya::Engine::Hook_PreRenderPipelineExecute.Name ? Pitaya::Engine::Hook_PreRenderPipelineExecute.Name : "UnNamed") : "Unmounted",               \
            Pitaya::Engine::Hook_ShouldWakeupRenderThread.Func ? (Pitaya::Engine::Hook_ShouldWakeupRenderThread.Name ? Pitaya::Engine::Hook_ShouldWakeupRenderThread.Name : "UnNamed") : "Unmounted",               \
            Pitaya::Engine::Hook_PostChronometerTick.Func ? (Pitaya::Engine::Hook_PostChronometerTick.Name ? Pitaya::Engine::Hook_PostChronometerTick.Name : "UnNamed") : "Unmounted",                              \
            Pitaya::Engine::Hook_PostLog.Func ? (Pitaya::Engine::Hook_PostLog.Name ? Pitaya::Engine::Hook_PostLog.Name : "UnNamed") : "Unmounted"                                                                   \
        );                                                                                                                                                                                                          \
        return buffer;                                                                                                                                                                                              \
    }()
