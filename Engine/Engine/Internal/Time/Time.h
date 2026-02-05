#pragma once

#include<chrono>
#include<cstdint>
#include<thread>

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
		inline void Tick() noexcept
		{
			tick = std::chrono::steady_clock::now();
			unscaledDeltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(tick - lastTick).count();
			delta = unscaledDeltaTime * scale;
			lastTick = tick;
		}
		inline void FrameSync() noexcept
		{
			float sleepTime = targetFrameTime - std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - tick).count();
			if (sleepTime > 0.0f)
			{
				std::this_thread::sleep_for(std::chrono::duration<float>(sleepTime));
			}
		}

	public:
		inline float Getdelta() noexcept
		{
			return delta;
		}
		inline float GetFixdelta() const noexcept
		{
			return fixdelta;
		}
		inline float GetUnscaledDelta() const noexcept
		{
			return unscaledDeltaTime;
		}
		inline float GetTimeScale() const noexcept
		{
			return scale;
		}
		inline void SetFPS(float fps) noexcept
		{
			targetFrameTime = 1.0f / fps;
		}
		inline float Seconds() const noexcept
		{
			return std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - start).count();
		}
		inline int64_t Milliseconds() const noexcept
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
		}

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