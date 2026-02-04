#pragma once

#include<chrono>
#include<cstdint>

namespace Pitaya::Engine
{
	class Engine;
}

namespace Pitaya::Engine::Internal
{
	class Time
	{
		friend class Pitaya::Engine::Engine;
	private:
		Time() = default;
		~Time() = default;

	public:
		Time(const Time&) = delete;
		Time& operator=(const Time&) = delete;
		Time(Time&&) = delete;
		Time& operator=(Time&&) = delete;

	private:
		bool Initialize();
		void Release();

	private:
		void Tick();
		void FrameSync();
		void SetFPS(float fps);
		float Seconds();
		int64_t Milliseconds();

	private:
		float delta = 0.0f;
		float fixdelta = 1.0f / 60.0f;
		float unscaledDeltaTime = 0.0f;
		float scale = 1.0f;
		float targetFrameTime = 1.0f / 144.0f;	//开启Syn垂直同步后 游戏帧率受窗口限制（屏幕刷新率）

		std::chrono::steady_clock::time_point tick = std::chrono::steady_clock::now();
		std::chrono::steady_clock::time_point lastTick = std::chrono::steady_clock::now();
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	};
}