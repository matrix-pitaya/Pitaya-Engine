#pragma once

#include<Hook/HookPoint.h>

#define MOUNT_BEGINFRAME_HOOK(func) Pitaya::Engine::HookFunc_BeginFrame = func;
#define MOUNT_FIXEDUPDATE_HOOK(func) Pitaya::Engine::HookFunc_FixedUpdate = func;
#define MOUNT_UPDATE_HOOK(func) Pitaya::Engine::HookFunc_Update = func;
#define MOUNT_LATEUPDATE_HOOK(func) Pitaya::Engine::HookFunc_LateUpdate = func;
#define MOUNT_ENDFRAME_HOOK(func) Pitaya::Engine::HookFunc_EndFrame = func;
#define MOUNT_POSTRENDERERINTIALIZE_HOOK(func) Pitaya::Engine::HookFunc_PostRendererInitialize = func;
#define MOUNT_POSTRENDERERRELEASE_HOOK(func) Pitaya::Engine::HookFunc_PostRendererRelease = func;
#define MOUNT_POSTRENDERERPARSECOMMAND_HOOK(func) Pitaya::Engine::HookFunc_PostRendererParseCommand = func;
#define MOUNT_POSTRENDERERSWAPBUFFER_HOOK(func) Pitaya::Engine::HookFunc_PostRendererSwapBuffer = func;
#define MOUNT_POSTRENDERCONTEXTINITIALIZED_HOOK(func) Pitaya::Engine::HookFunc_PostRenderContextInitialized = func;
#define MOUNT_PRERENDERCONTEXTINRELEASED_HOOK(func) Pitaya::Engine::HookFunc_PreRenderContextRelease = func;
#define MOUNT_SHOULDWAKEUPRENDERTHREAD_HOOK(func) Pitaya::Engine::HookFunc_ShouldWakeupRenderThread = func;
#define MOUNT_POSTCHRONOMETERTICK_HOOK(func) Pitaya::Engine::HookFunc_PostChronometerTick = func;



#define UNMOUNT_BEGINFRAME_HOOK Pitaya::Engine::HookFunc_BeginFrame = nullptr;
#define UNMOUNT_FIXEDUPDATE_HOOK Pitaya::Engine::HookFunc_FixedUpdate = nullptr;
#define UNMOUNT_UPDATE_HOOK Pitaya::Engine::HookFunc_Update = nullptr;
#define UNMOUNT_LATEUPDATE_HOOK Pitaya::Engine::HookFunc_LateUpdate = nullptr;
#define UNMOUNT_ENDFRAME_HOOK Pitaya::Engine::HookFunc_EndFrame = nullptr;
#define UNMOUNT_POSTRENDERERINTIALIZE_HOOK Pitaya::Engine::HookFunc_PostRendererInitialize = nullptr;
#define UNMOUNT_POSTRENDERERRELEASE_HOOK Pitaya::Engine::HookFunc_PostRendererRelease = nullptr;
#define UNMOUNT_POSTRENDERERPARSECOMMAND_HOOK Pitaya::Engine::HookFunc_PostRendererParseCommand = nullptr;
#define UNMOUNT_POSTRENDERERSWAPBUFFER_HOOK Pitaya::Engine::HookFunc_PostRendererSwapBuffer = nullptr;
#define UNMOUNT_POSTRENDERCONTEXTINITIALIZED_HOOK Pitaya::Engine::HookFunc_PostRenderContextInitialized = nullptr;
#define UNMOUNT_PRERENDERCONTEXTINRELEASED_HOOK Pitaya::Engine::HookFunc_PreRenderContextRelease = nullptr;
#define UNMOUNT_SHOULDWAKEUPRENDERTHREAD_HOOK Pitaya::Engine::HookFunc_ShouldWakeupRenderThread = nullptr;
#define UNMOUNT_POSTCHRONOMETERTICK_HOOK Pitaya::Engine::HookFunc_PostChronometerTick = nullptr;



#define INVOKE_BEGINFRAME_HOOK if(Pitaya::Engine::HookFunc_BeginFrame) { Pitaya::Engine::HookFunc_BeginFrame(); }
#define INVOKE_FIXEDUPDATE_HOOK if(Pitaya::Engine::HookFunc_FixedUpdate) { Pitaya::Engine::HookFunc_FixedUpdate(); }
#define INVOKE_UPDATE_HOOK if(Pitaya::Engine::HookFunc_Update) { Pitaya::Engine::HookFunc_Update(); }
#define INVOKE_LATEUPDATE_HOOK if(Pitaya::Engine::HookFunc_LateUpdate) { Pitaya::Engine::HookFunc_LateUpdate(); }
#define INVOKE_ENDFRAME_HOOK if(Pitaya::Engine::HookFunc_EndFrame) { Pitaya::Engine::HookFunc_EndFrame(); }
#define INVOKE_POSTRENDERERINTIALIZE_HOOK if(Pitaya::Engine::HookFunc_PostRendererInitialize) { Pitaya::Engine::HookFunc_PostRendererInitialize(); } 
#define INVOKE_POSTRENDERERRELEASE_HOOK if(Pitaya::Engine::HookFunc_PostRendererRelease) { Pitaya::Engine::HookFunc_PostRendererRelease(); }
#define INVOKE_POSTRENDERERPARSECOMMAND_HOOK if(Pitaya::Engine::HookFunc_PostRendererParseCommand) { Pitaya::Engine::HookFunc_PostRendererParseCommand(); }
#define INVOKE_POSTRENDERERSWAPBUFFER_HOOK if(Pitaya::Engine::HookFunc_PostRendererSwapBuffer) { Pitaya::Engine::HookFunc_PostRendererSwapBuffer(); }
#define INVOKE_POSTRENDERCONTEXTINITIALIZED_HOOK if(Pitaya::Engine::HookFunc_PostRenderContextInitialized) { Pitaya::Engine::HookFunc_PostRenderContextInitialized(); }
#define INVOKE_PRERENDERCONTEXTINRELEASED_HOOK if(Pitaya::Engine::HookFunc_PreRenderContextRelease) { Pitaya::Engine::HookFunc_PreRenderContextRelease(); }
#define INVOKE_SHOULDWAKEUPRENDERTHREAD_HOOK (Pitaya::Engine::HookFunc_ShouldWakeupRenderThread ? Pitaya::Engine::HookFunc_ShouldWakeupRenderThread() : false)
#define INVOKE_POSTCHRONOMETERTICK_HOOK if(Pitaya::Engine::HookFunc_PostChronometerTick) { Pitaya::Engine::HookFunc_PostChronometerTick(); }


#include<cstdio>
#define HOOK_STATE                                                                           \
    []() -> const char*                                                                      \
    {                                                                                        \
        static char buffer[4096] = {};                                                       \
        sprintf_s(buffer, sizeof(buffer),                                                    \
            "Hook Name                          State\n"                                     \
            "BeginFrame                         %s\n"                                        \
            "FixedUpdate                        %s\n"                                        \
            "Update                             %s\n"                                        \
            "LateUpdate                         %s\n"                                        \
            "EndFrame                           %s\n"                                        \
            "PostRendererInitialize             %s\n"                                        \
            "PostRendererRelease                %s\n"                                        \
            "PostRendererParseCommand           %s\n"                                        \
            "PostRendererSwapBuffer             %s\n"                                        \
            "PostRenderContextInitialized       %s\n"                                        \
            "PreRenderContextRelease            %s\n"                                        \
            "ShouldWakeupRenderThread           %s\n"                                        \
            "PostChronometerTick                %s\n",                                       \
            Pitaya::Engine::HookFunc_BeginFrame ? "Mounted" : "Unmounted",                   \
            Pitaya::Engine::HookFunc_FixedUpdate ? "Mounted" : "Unmounted",                  \
            Pitaya::Engine::HookFunc_Update ? "Mounted" : "Unmounted",                       \
            Pitaya::Engine::HookFunc_LateUpdate ? "Mounted" : "Unmounted",                   \
            Pitaya::Engine::HookFunc_EndFrame ? "Mounted" : "Unmounted",                     \
            Pitaya::Engine::HookFunc_PostRendererInitialize ? "Mounted" : "Unmounted",       \
            Pitaya::Engine::HookFunc_PostRendererRelease ? "Mounted" : "Unmounted",          \
            Pitaya::Engine::HookFunc_PostRendererParseCommand ? "Mounted" : "Unmounted",     \
            Pitaya::Engine::HookFunc_PostRendererSwapBuffer ? "Mounted" : "Unmounted",       \
            Pitaya::Engine::HookFunc_PostRenderContextInitialized ? "Mounted" : "Unmounted", \
            Pitaya::Engine::HookFunc_PreRenderContextRelease ? "Mounted" : "Unmounted",      \
            Pitaya::Engine::HookFunc_ShouldWakeupRenderThread ? "Mounted" : "Unmounted",     \
            Pitaya::Engine::HookFunc_PostChronometerTick ? "Mounted" : "Unmounted"           \
        );                                                                                   \
        return buffer;                                                                       \
    }()