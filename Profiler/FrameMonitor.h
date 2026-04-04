#pragma once

#include<Core/Allocate/Allocate.h>
#include<Context/Common/Module.h>
#include<Profiler/Common/FrameState.h>

namespace Pitaya::Profiler
{
	class FrameMonitor
	{
	public:
		struct Factory
		{
			friend class Pitaya::Engine::Module<FrameMonitor>;
		private:
			static FrameMonitor* Create()
			{
				return PITAYA_NEW(FrameMonitor);
			}
			static void Destroy(FrameMonitor* frameMonitor)
			{
				PITAYA_DELETE(frameMonitor);
			}
		};
		struct Lifecycle
		{
			friend class Pitaya::Engine::Module<FrameMonitor>;
		private:
			static bool Initialize(FrameMonitor* frameMonitor)
			{
				return frameMonitor->Initialize();
			}
			static void Release(FrameMonitor* frameMonitor)
			{
				frameMonitor->Release();
			}
			static void BeginFrame(FrameMonitor* frameMonitor)
			{
				frameMonitor->BeginFrame();
			}
			static void EndFrame(FrameMonitor* frameMonitor)
			{
				frameMonitor->EndFrame();
			}
		};

	private:
		FrameMonitor() = default;
		~FrameMonitor() = default;

	public:
		FrameMonitor(const FrameMonitor&) = delete;
		FrameMonitor& operator=(const FrameMonitor&) = delete;
		FrameMonitor(FrameMonitor&&) = delete;
		FrameMonitor& operator=(FrameMonitor&&) = delete;

	private:
		bool Initialize();
		void Release();
		void BeginFrame();
		void EndFrame();

	public:
		inline void WriteTimeState(TimeState timeState) noexcept
		{
			front.Time = timeState;
		}
		inline void WriteRenderState(RenderState renderState) noexcept
		{
			front.Render = renderState;
		}
		inline TimeState GetTimeState() const noexcept
		{
			return back.Time;
		}
		inline RenderState GetRenderState() const noexcept
		{
			return back.Render;
		}

	private:
		FrameState front;
		FrameState back;
	};
}

