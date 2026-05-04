#pragma once

#include<Hook/HookPoint.h>
#include<cstdio>

#define MOUNT_PREBEGINFRAME_HOOK(func,name) do { Pitaya::Engine::Global_PreBeginFrame_Hook.Func = func; Pitaya::Engine::Global_PreBeginFrame_Hook.Name = name; } while(false);
#define MOUNT_PREFIXEDUPDATE_HOOK(func,name)  do { Pitaya::Engine::Global_PreFixedUpdate_Hook.Func = func; Pitaya::Engine::Global_PreFixedUpdate_Hook.Name = name; } while(false);
#define MOUNT_PREUPDATE_HOOK(func,name)  do { Pitaya::Engine::Global_PreUpdate_Hook.Func = func; Pitaya::Engine::Global_PreUpdate_Hook.Name = name; } while(false);
#define MOUNT_PRELATEUPDATE_HOOK(func,name)  do { Pitaya::Engine::Global_PreLateUpdate_Hook.Func = func; Pitaya::Engine::Global_PreLateUpdate_Hook.Name = name; } while(false);
#define MOUNT_PREENDFRAME_HOOK(func,name)  do { Pitaya::Engine::Global_PreEndFrame_Hook.Func = func; Pitaya::Engine::Global_PreEndFrame_Hook.Name = name; } while(false);
#define MOUNT_POSTRENDERERINTIALIZE_HOOK(func,name) do { Pitaya::Engine::Global_PostRendererInitialize_Hook.Func = func; Pitaya::Engine::Global_PostRendererInitialize_Hook.Name = name; } while(false);
#define MOUNT_POSTRENDERERRELEASE_HOOK(func,name) do { Pitaya::Engine::Global_PostRendererRelease_Hook.Func = func; Pitaya::Engine::Global_PostRendererRelease_Hook.Name = name; } while(false);
#define MOUNT_POSTRENDERERPARSECOMMAND_HOOK(func,name) do { Pitaya::Engine::Global_PostRendererParseCommand_Hook.Func = func; Pitaya::Engine::Global_PostRendererParseCommand_Hook.Name = name; } while(false);
#define MOUNT_POSTRENDERERSWAPBUFFER_HOOK(func,name) do { Pitaya::Engine::Global_PostRendererSwapBuffer_Hook.Func = func; Pitaya::Engine::Global_PostRendererSwapBuffer_Hook.Name = name; } while(false);
#define MOUNT_POSTRENDERERBEGINRENDERFRAME_HOOK(func,name)do { Pitaya::Engine::Global_PostRendererBeginRenderFrame_Hook.Func = func; Pitaya::Engine::Global_PostRendererBeginRenderFrame_Hook.Name = name; } while(false);
#define MOUNT_PRERENDERERENDRENDERFRAME_HOOK(func,name) do { Pitaya::Engine::Global_PreRendereEndRenderFrame_Hook.Func = func; Pitaya::Engine::Global_PreRendereEndRenderFrame_Hook.Name = name; } while(false);
#define MOUNT_POSTRENDERCONTEXTINITIALIZED_HOOK(func,name) do { Pitaya::Engine::Global_PostRenderContextInitialized_Hook.Func = func; Pitaya::Engine::Global_PostRenderContextInitialized_Hook.Name = name; } while(false);
#define MOUNT_PRERENDERCONTEXTINRELEASED_HOOK(func,name) do { Pitaya::Engine::Global_PreRenderContextRelease_Hook.Func = func; Pitaya::Engine::Global_PreRenderContextRelease_Hook.Name = name; } while(false);
#define MOUNT_PRERENDERPIPELINEEXECUTE_HOOK(func,name) do { Pitaya::Engine::Global_PreRenderPipelineExecute_Hook.Func = func; Pitaya::Engine::Global_PreRenderPipelineExecute_Hook.Name = name; } while(false);
#define MOUNT_SHOULDWAKEUPRENDERTHREAD_HOOK(func,name) do { Pitaya::Engine::Global_ShouldWakeupRenderThread_Hook.Func = func; Pitaya::Engine::Global_ShouldWakeupRenderThread_Hook.Name = name; } while(false);
#define MOUNT_SHOULDSUBMITSCENECAMERAPASS_HOOK(func,name) do { Pitaya::Engine::Global_ShouldSubmitSceneCameraPass_Hook.Func = func; Pitaya::Engine::Global_ShouldSubmitSceneCameraPass_Hook.Name = name; } while(false);
#define MOUNT_TERMINATERENDERPIPELINESUBMITFINALBLIT_HOOK(func,name) do { Pitaya::Engine::Global_TerminateRenderPipelineSubmitFinalBlit_Hook.Func = func; Pitaya::Engine::Global_TerminateRenderPipelineSubmitFinalBlit_Hook.Name = name; } while(false);
#define MOUNT_POSTCHRONOMETERTICK_HOOK(func,name) do { Pitaya::Engine::Global_PostChronometerTick_Hook.Func = func; Pitaya::Engine::Global_PostChronometerTick_Hook.Name = name; } while(false);
#define MOUNT_POSTLOG_HOOK(func,name) do { Pitaya::Engine::Global_PostLog_Hook.Func = func; Pitaya::Engine::Global_PostLog_Hook.Name = name; } while(false);
#define MOUNT_TERMINATEFIXEDUPDATE_HOOK(func,name) do { Pitaya::Engine::Global_TerminateFixedUpdate_Hook.Func = func; Pitaya::Engine::Global_TerminateFixedUpdate_Hook.Name = name; } while(false);


#define INVOKE_PREBEGINFRAME_HOOK if(Pitaya::Engine::Global_PreBeginFrame_Hook.Func) { Pitaya::Engine::Global_PreBeginFrame_Hook.Func(); }
#define INVOKE_PREFIXEDUPDATE_HOOK if(Pitaya::Engine::Global_PreFixedUpdate_Hook.Func) { Pitaya::Engine::Global_PreFixedUpdate_Hook.Func(); }
#define INVOKE_PREUPDATE_HOOK if(Pitaya::Engine::Global_PreUpdate_Hook.Func) { Pitaya::Engine::Global_PreUpdate_Hook.Func(); }
#define INVOKE_PRELATEUPDATE_HOOK if(Pitaya::Engine::Global_PreLateUpdate_Hook.Func) { Pitaya::Engine::Global_PreLateUpdate_Hook.Func(); }
#define INVOKE_PREENDFRAME_HOOK if(Pitaya::Engine::Global_PreEndFrame_Hook.Func) { Pitaya::Engine::Global_PreEndFrame_Hook.Func(); }
#define INVOKE_POSTRENDERERINTIALIZE_HOOK(NativeWindow) if(Pitaya::Engine::Global_PostRendererInitialize_Hook.Func) { Pitaya::Engine::Global_PostRendererInitialize_Hook.Func(NativeWindow); } 
#define INVOKE_POSTRENDERERRELEASE_HOOK if(Pitaya::Engine::Global_PostRendererRelease_Hook.Func) { Pitaya::Engine::Global_PostRendererRelease_Hook.Func(); }
#define INVOKE_POSTRENDERERPARSECOMMAND_HOOK if(Pitaya::Engine::Global_PostRendererParseCommand_Hook.Func) { Pitaya::Engine::Global_PostRendererParseCommand_Hook.Func(); }
#define INVOKE_POSTRENDERERSWAPBUFFER_HOOK if(Pitaya::Engine::Global_PostRendererSwapBuffer_Hook.Func) { Pitaya::Engine::Global_PostRendererSwapBuffer_Hook.Func(); }
#define INVOKE_POSTRENDERERBEGINRENDERFRAME_HOOK if(Pitaya::Engine::Global_PostRendererBeginRenderFrame_Hook.Func){ Pitaya::Engine::Global_PostRendererBeginRenderFrame_Hook.Func(); } 
#define INVOKE_PRERENDERERENDRENDERFRAME_HOOK if(Pitaya::Engine::Global_PreRendereEndRenderFrame_Hook.Func) { Pitaya::Engine::Global_PreRendereEndRenderFrame_Hook.Func(); }
#define INVOKE_POSTRENDERCONTEXTINITIALIZED_HOOK(Passkey, RtHandle) if(Pitaya::Engine::Global_PostRenderContextInitialized_Hook.Func) { Pitaya::Engine::Global_PostRenderContextInitialized_Hook.Func(Passkey, RtHandle); }
#define INVOKE_PRERENDERCONTEXTINRELEASED_HOOK if(Pitaya::Engine::Global_PreRenderContextRelease_Hook.Func) { Pitaya::Engine::Global_PreRenderContextRelease_Hook.Func(); }
#define INVOKE_PRERENDERPIPELINEEXECUTE_HOOK(PassKey, RenderPipeline) if(Pitaya::Engine::Global_PreRenderPipelineExecute_Hook.Func) { Pitaya::Engine::Global_PreRenderPipelineExecute_Hook.Func(PassKey, RenderPipeline); }
#define INVOKE_SHOULDWAKEUPRENDERTHREAD_HOOK (Pitaya::Engine::Global_ShouldWakeupRenderThread_Hook.Func ? Pitaya::Engine::Global_ShouldWakeupRenderThread_Hook.Func() : false)
#define INVOKE_SHOULDSUBMITSCENECAMERAPASS_HOOK (Pitaya::Engine::Global_ShouldSubmitSceneCameraPass_Hook.Func ? Pitaya::Engine::Global_ShouldSubmitSceneCameraPass_Hook.Func() : true)
#define INVOKE_TERMINATERENDERPIPELINESUBMITFINALBLIT_HOOK if(Pitaya::Engine::Global_TerminateRenderPipelineSubmitFinalBlit_Hook.Func && Pitaya::Engine::Global_TerminateRenderPipelineSubmitFinalBlit_Hook.Func()) { return; }
#define INVOKE_POSTCHRONOMETERTICK_HOOK if(Pitaya::Engine::Global_PostChronometerTick_Hook.Func) { Pitaya::Engine::Global_PostChronometerTick_Hook.Func(); }
#define INVOKE_POSTLOG_HOOK(Leve,Message) if(Pitaya::Engine::Global_PostLog_Hook.Func) { Pitaya::Engine::Global_PostLog_Hook.Func(Leve,Message); }
#define INVOKE_TERMINATEFIXEDUPDATE_HOOK if(Pitaya::Engine::Global_TerminateFixedUpdate_Hook.Func && Pitaya::Engine::Global_TerminateFixedUpdate_Hook.Func()) { return; }


#define DISCARD_HOOK                                                                                                                                                            \
    do                                                                                                                                                                          \
    {                                                                                                                                                                           \
        Pitaya::Engine::Global_PreBeginFrame_Hook.Func = nullptr; Pitaya::Engine::Global_PreBeginFrame_Hook.Name = nullptr;                                                     \
        Pitaya::Engine::Global_PreFixedUpdate_Hook.Func = nullptr; Pitaya::Engine::Global_PreFixedUpdate_Hook.Name = nullptr;                                                   \
        Pitaya::Engine::Global_PreUpdate_Hook.Func = nullptr; Pitaya::Engine::Global_PreUpdate_Hook.Name = nullptr;                                                             \
        Pitaya::Engine::Global_PreLateUpdate_Hook.Func = nullptr; Pitaya::Engine::Global_PreLateUpdate_Hook.Name = nullptr;                                                     \
        Pitaya::Engine::Global_PreEndFrame_Hook.Func = nullptr; Pitaya::Engine::Global_PreEndFrame_Hook.Name = nullptr;                                                         \
        Pitaya::Engine::Global_PostRendererInitialize_Hook.Func = nullptr; Pitaya::Engine::Global_PostRendererInitialize_Hook.Name = nullptr;                                   \
        Pitaya::Engine::Global_PostRendererRelease_Hook.Func = nullptr; Pitaya::Engine::Global_PostRendererRelease_Hook.Name = nullptr;                                         \
        Pitaya::Engine::Global_PostRendererParseCommand_Hook.Func = nullptr; Pitaya::Engine::Global_PostRendererParseCommand_Hook.Name = nullptr;                               \
        Pitaya::Engine::Global_PostRendererSwapBuffer_Hook.Func = nullptr; Pitaya::Engine::Global_PostRendererSwapBuffer_Hook.Name = nullptr;                                   \
        Pitaya::Engine::Global_PostRendererBeginRenderFrame_Hook.Func = nullptr; Pitaya::Engine::Global_PostRendererBeginRenderFrame_Hook.Name = nullptr;                       \
        Pitaya::Engine::Global_PreRendereEndRenderFrame_Hook.Func = nullptr; Pitaya::Engine::Global_PreRendereEndRenderFrame_Hook.Name = nullptr;                               \
        Pitaya::Engine::Global_PostRenderContextInitialized_Hook.Func = nullptr; Pitaya::Engine::Global_PostRenderContextInitialized_Hook.Name = nullptr;                       \
        Pitaya::Engine::Global_PreRenderContextRelease_Hook.Func = nullptr; Pitaya::Engine::Global_PreRenderContextRelease_Hook.Name = nullptr;                                 \
        Pitaya::Engine::Global_PreRenderPipelineExecute_Hook.Func = nullptr; Pitaya::Engine::Global_PreRenderPipelineExecute_Hook.Name = nullptr;                               \
        Pitaya::Engine::Global_ShouldWakeupRenderThread_Hook.Func = nullptr; Pitaya::Engine::Global_ShouldWakeupRenderThread_Hook.Name = nullptr;                               \
        Pitaya::Engine::Global_ShouldSubmitSceneCameraPass_Hook.Func = nullptr; Pitaya::Engine::Global_ShouldSubmitSceneCameraPass_Hook.Name = nullptr;                         \
        Pitaya::Engine::Global_TerminateRenderPipelineSubmitFinalBlit_Hook.Func = nullptr; Pitaya::Engine::Global_TerminateRenderPipelineSubmitFinalBlit_Hook.Name = nullptr;   \
        Pitaya::Engine::Global_PostChronometerTick_Hook.Func = nullptr; Pitaya::Engine::Global_PostChronometerTick_Hook.Name = nullptr;                                         \
        Pitaya::Engine::Global_PostLog_Hook.Func = nullptr; Pitaya::Engine::Global_PostLog_Hook.Name = nullptr;                                                                 \
        Pitaya::Engine::Global_TerminateFixedUpdate_Hook.Func = nullptr; Pitaya::Engine::Global_TerminateFixedUpdate_Hook.Name = nullptr;                                       \
    } while (false);


#define HOOK_STATE                                                                            \
    []() -> const char*                                                                       \
    {                                                                                         \
        static char buffer[4096] = {};                                                        \
        sprintf_s(buffer, sizeof(buffer),                                                     \
            "Hook Name                                  State\n"                                     \
            "PreBeginFrame                              %s\n"                                        \
            "PreFixedUpdate                             %s\n"                                        \
            "PreUpdate                                  %s\n"                                        \
            "PreLateUpdate                              %s\n"                                        \
            "PreEndFrame                                %s\n"                                        \
            "PostRendererInitialize                     %s\n"                                        \
            "PostRendererRelease                        %s\n"                                        \
            "PostRendererParseCommand                   %s\n"                                        \
            "PostRendererSwapBuffer                     %s\n"                                        \
            "PostRendererBeginRenderFrame               %s\n"                                        \
            "PreRendereEndRenderFrame                   %s\n"                                        \
            "PostRenderContextInitialized               %s\n"                                        \
            "PreRenderContextRelease                    %s\n"                                        \
            "PreRenderPipelineExecute                   %s\n"                                        \
            "ShouldWakeupRenderThread                   %s\n"                                        \
            "ShouldSubmitSceneCameraPass                %s\n"                                        \
            "TerminateRenderPipelineSubmitFinalBlit     %s\n"                                        \
            "PostChronometerTick                        %s\n"                                        \
            "PostLog                                    %s\n"                                        \
            "TerminateFixedUpdate                       %s\n",                                       \
            Pitaya::Engine::Global_PreBeginFrame_Hook.Func ? (Pitaya::Engine::Global_PreBeginFrame_Hook.Name ? Pitaya::Engine::Global_PreBeginFrame_Hook.Name : "UnNamed") : "Unmounted",                                                                               \
            Pitaya::Engine::Global_PreFixedUpdate_Hook.Func ? (Pitaya::Engine::Global_PreFixedUpdate_Hook.Name ? Pitaya::Engine::Global_PreFixedUpdate_Hook.Name : "UnNamed") : "Unmounted",                                                                            \
            Pitaya::Engine::Global_PreUpdate_Hook.Func ? (Pitaya::Engine::Global_PreUpdate_Hook.Name ? Pitaya::Engine::Global_PreUpdate_Hook.Name : "UnNamed") : "Unmounted",                                                                                           \
            Pitaya::Engine::Global_PreLateUpdate_Hook.Func ? (Pitaya::Engine::Global_PreLateUpdate_Hook.Name ? Pitaya::Engine::Global_PreLateUpdate_Hook.Name : "UnNamed") : "Unmounted",                                                                               \
            Pitaya::Engine::Global_PreEndFrame_Hook.Func ? (Pitaya::Engine::Global_PreEndFrame_Hook.Name ? Pitaya::Engine::Global_PreEndFrame_Hook.Name : "UnNamed") : "Unmounted",                                                                                     \
            Pitaya::Engine::Global_PostRendererInitialize_Hook.Func ? (Pitaya::Engine::Global_PostRendererInitialize_Hook.Name ? Pitaya::Engine::Global_PostRendererInitialize_Hook.Name : "UnNamed") : "Unmounted",                                                    \
            Pitaya::Engine::Global_PostRendererRelease_Hook.Func ? (Pitaya::Engine::Global_PostRendererRelease_Hook.Name ? Pitaya::Engine::Global_PostRendererRelease_Hook.Name : "UnNamed") : "Unmounted",                                                             \
            Pitaya::Engine::Global_PostRendererParseCommand_Hook.Func ? (Pitaya::Engine::Global_PostRendererParseCommand_Hook.Name ? Pitaya::Engine::Global_PostRendererParseCommand_Hook.Name : "UnNamed") : "Unmounted",                                              \
            Pitaya::Engine::Global_PostRendererSwapBuffer_Hook.Func ? (Pitaya::Engine::Global_PostRendererSwapBuffer_Hook.Name ? Pitaya::Engine::Global_PostRendererSwapBuffer_Hook.Name : "UnNamed") : "Unmounted",                                                    \
            Pitaya::Engine::Global_PostRendererBeginRenderFrame_Hook.Func ? (Pitaya::Engine::Global_PostRendererBeginRenderFrame_Hook.Name ? Pitaya::Engine::Global_PostRendererBeginRenderFrame_Hook.Name : "UnNamed") : "Unmounted",                                  \
            Pitaya::Engine::Global_PreRendereEndRenderFrame_Hook.Func ? (Pitaya::Engine::Global_PreRendereEndRenderFrame_Hook.Name ? Pitaya::Engine::Global_PreRendereEndRenderFrame_Hook.Name : "UnNamed") : "Unmounted",                                              \
            Pitaya::Engine::Global_PostRenderContextInitialized_Hook.Func ? (Pitaya::Engine::Global_PostRenderContextInitialized_Hook.Name ? Pitaya::Engine::Global_PostRenderContextInitialized_Hook.Name : "UnNamed") : "Unmounted",                                  \
            Pitaya::Engine::Global_PreRenderContextRelease_Hook.Func ? (Pitaya::Engine::Global_PreRenderContextRelease_Hook.Name ? Pitaya::Engine::Global_PreRenderContextRelease_Hook.Name : "UnNamed") : "Unmounted",                                                 \
            Pitaya::Engine::Global_PreRenderPipelineExecute_Hook.Func ? (Pitaya::Engine::Global_PreRenderPipelineExecute_Hook.Name ? Pitaya::Engine::Global_PreRenderPipelineExecute_Hook.Name : "UnNamed") : "Unmounted",                                              \
            Pitaya::Engine::Global_ShouldWakeupRenderThread_Hook.Func ? (Pitaya::Engine::Global_ShouldWakeupRenderThread_Hook.Name ? Pitaya::Engine::Global_ShouldWakeupRenderThread_Hook.Name : "UnNamed") : "Unmounted",                                              \
            Pitaya::Engine::Global_ShouldSubmitSceneCameraPass_Hook.Func ? (Pitaya::Engine::Global_ShouldSubmitSceneCameraPass_Hook.Name ? Pitaya::Engine::Global_ShouldSubmitSceneCameraPass_Hook.Name : "UnNamed") : "Unmounted",                                     \
            Pitaya::Engine::Global_TerminateRenderPipelineSubmitFinalBlit_Hook.Func ? (Pitaya::Engine::Global_TerminateRenderPipelineSubmitFinalBlit_Hook.Name ? Pitaya::Engine::Global_TerminateRenderPipelineSubmitFinalBlit_Hook.Name : "UnNamed") : "Unmounted",    \
            Pitaya::Engine::Global_PostChronometerTick_Hook.Func ? (Pitaya::Engine::Global_PostChronometerTick_Hook.Name ? Pitaya::Engine::Global_PostChronometerTick_Hook.Name : "UnNamed") : "Unmounted",                                                             \
            Pitaya::Engine::Global_PostLog_Hook.Func ? (Pitaya::Engine::Global_PostLog_Hook.Name ? Pitaya::Engine::Global_PostLog_Hook.Name : "UnNamed") : "Unmounted",                                                                                                 \
            Pitaya::Engine::Global_TerminateFixedUpdate_Hook.Func ? (Pitaya::Engine::Global_TerminateFixedUpdate_Hook.Name ? Pitaya::Engine::Global_TerminateFixedUpdate_Hook.Name : "UnNamed") : "Unmounted"                                                           \
        );                                                                                                                                                                                                                                                              \
        return buffer;                                                                                                                                                                                                                                                  \
    }()
