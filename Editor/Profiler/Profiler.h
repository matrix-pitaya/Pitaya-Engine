#pragma once

#include<Editor/Profiler/FrameState.h>

namespace Pitaya::Editor
{
	class Editor;
	class Profiler
	{
		friend class Editor;
	private:
		Profiler() = default;
		~Profiler() = default;

	public:
		Profiler(const Profiler&) = delete;
		Profiler& operator=(const Profiler&) = delete;
		Profiler(Profiler&&) = delete;
		Profiler& operator=(Profiler&&) = delete;

	private:
		bool Initialize();
		void Release();
		void BeginFrame();
		void EndFrame();

	public:
		inline void SetTimeState(TimeState timeState) noexcept
		{
			front.Time = timeState;
		}
		inline TimeState GetTimeState() noexcept
		{
			return back.Time;
		}

	private:
		FrameState front;
		FrameState back;
	};
}