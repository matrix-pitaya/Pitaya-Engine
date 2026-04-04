#pragma once

#include<Hook/HookPoint.h>

#define MOUNT_BEGINFRAME_HOOK(func,name) do { Pitaya::Engine::Hook_BeginFrame.Func = func; Pitaya::Engine::Hook_BeginFrame.Name = name;} while(false);
#define MOUNT_FIXEDUPDATE_HOOK(func,name)  do { Pitaya::Engine::Hook_FixedUpdate.Func = func; Pitaya::Engine::Hook_FixedUpdate.Name = name;} while(false);
#define MOUNT_UPDATE_HOOK(func,name)  do { Pitaya::Engine::Hook_Update.Func = func; Pitaya::Engine::Hook_Update.Name = name;} while(false);
#define MOUNT_LATEUPDATE_HOOK(func,name)  do { Pitaya::Engine::Hook_LateUpdate.Func = func; Pitaya::Engine::Hook_LateUpdate.Name = name;} while(false);
#define MOUNT_ENDFRAME_HOOK(func,name)  do { Pitaya::Engine::Hook_EndFrame.Func = func; Pitaya::Engine::Hook_EndFrame.Name = name;} while(false);
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


#define INVOKE_BEGINFRAME_HOOK if(Pitaya::Engine::Hook_BeginFrame.Func) { Pitaya::Engine::Hook_BeginFrame.Func(); }
#define INVOKE_FIXEDUPDATE_HOOK if(Pitaya::Engine::Hook_FixedUpdate.Func) { Pitaya::Engine::Hook_FixedUpdate.Func(); }
#define INVOKE_UPDATE_HOOK if(Pitaya::Engine::Hook_Update.Func) { Pitaya::Engine::Hook_Update.Func(); }
#define INVOKE_LATEUPDATE_HOOK if(Pitaya::Engine::Hook_LateUpdate.Func) { Pitaya::Engine::Hook_LateUpdate.Func(); }
#define INVOKE_ENDFRAME_HOOK if(Pitaya::Engine::Hook_EndFrame.Func) { Pitaya::Engine::Hook_EndFrame.Func(); }
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


#include<cstdio>
#define HOOK_STATE                                                                            \
    []() -> const char*                                                                       \
    {                                                                                         \
        static char buffer[4096] = {};                                                        \
        sprintf_s(buffer, sizeof(buffer),                                                     \
            "Hook Name                           State\n"                                     \
            "BeginFrame                          %s\n"                                        \
            "FixedUpdate                         %s\n"                                        \
            "Update                              %s\n"                                        \
            "LateUpdate                          %s\n"                                        \
            "EndFrame                            %s\n"                                        \
            "PostRendererInitialize              %s\n"                                        \
            "PostRendererRelease                 %s\n"                                        \
            "PostRendererParseCommand            %s\n"                                        \
            "PostRendererSwapBuffer              %s\n"                                        \
            "PostRenderContextInitialized        %s\n"                                        \
            "PreRenderContextRelease             %s\n"                                        \
            "PreRenderPipelineExecute            %s\n"                                        \
            "ShouldWakeupRenderThread            %s\n"                                        \
            "PostChronometerTick                 %s\n"                                        \
            "PostLog                             %s\n",                                       \
            Pitaya::Engine::Hook_BeginFrame.Func ? Pitaya::Engine::Hook_BeginFrame.Name : "Unmounted",                                                          \
            Pitaya::Engine::Hook_FixedUpdate.Func ? Pitaya::Engine::Hook_FixedUpdate.Name : "Unmounted",                                                        \
            Pitaya::Engine::Hook_Update.Func ? Pitaya::Engine::Hook_Update.Name : "Unmounted",                                                                  \
            Pitaya::Engine::Hook_LateUpdate.Func ? Pitaya::Engine::Hook_LateUpdate.Name : "Unmounted",                                                          \
            Pitaya::Engine::Hook_EndFrame.Func ? Pitaya::Engine::Hook_EndFrame.Name : "Unmounted",                                                              \
            Pitaya::Engine::Hook_PostRendererInitialize.Func ? Pitaya::Engine::Hook_PostRendererInitialize.Name : "Unmounted",                                  \
            Pitaya::Engine::Hook_PostRendererRelease.Func ? Pitaya::Engine::Hook_PostRendererRelease.Name : "Unmounted",                                        \
            Pitaya::Engine::Hook_PostRendererParseCommand.Func ? Pitaya::Engine::Hook_PostRendererParseCommand.Name : "Unmounted",                              \
            Pitaya::Engine::Hook_PostRendererSwapBuffer.Func ? Pitaya::Engine::Hook_PostRendererSwapBuffer.Name : "Unmounted",                                  \
            Pitaya::Engine::Hook_PostRenderContextInitialized.Func ? Pitaya::Engine::Hook_PostRenderContextInitialized.Name : "Unmounted",                      \
            Pitaya::Engine::Hook_PreRenderContextRelease.Func ? Pitaya::Engine::Hook_PreRenderContextRelease.Name : "Unmounted",                                \
            Pitaya::Engine::Hook_PreRenderPipelineExecute.Func ? Pitaya::Engine::Hook_PreRenderPipelineExecute.Name : "Unmounted",                              \
            Pitaya::Engine::Hook_ShouldWakeupRenderThread.Func ? Pitaya::Engine::Hook_ShouldWakeupRenderThread.Name : "Unmounted",                              \
            Pitaya::Engine::Hook_PostChronometerTick.Func ? Pitaya::Engine::Hook_PostChronometerTick.Name : "Unmounted",                                        \
            Pitaya::Engine::Hook_PostLog.Func ? Pitaya::Engine::Hook_PostLog.Name : "Unmounted"                                                                 \
        );                                                                                                                                                      \
        return buffer;                                                                                                                                          \
    }()
