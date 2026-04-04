#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/PassKey/PassKey.h>
#include<Context/Common/Module.h>
#include<Config/Common/FunctionTable.h>
#include<Hook/def.h>

#include<chrono>
#include<cstdint>
#include<thread>

namespace Pitaya::Engine { class Engine; }

namespace Pitaya::Time
{
	class Chronometer
	{
	public:
		struct Factory
		{
			friend class Pitaya::Engine::Module<Chronometer>;
		private:
			static Chronometer* Create()
			{
				return PITAYA_NEW(Chronometer);
			}
			static void Destroy(Chronometer* chronometer)
			{
				PITAYA_DELETE(chronometer);
			}
		};
		struct Lifecycle
		{
			friend class Pitaya::Engine::Module<Chronometer>;
		private:
			static bool Initialize(Chronometer* chronometer)
			{
				return chronometer->Initialize();
			}
			static void Release(Chronometer* chronometer)
			{
				chronometer->Release();
			}
		};

	private:
		Chronometer() = default;
		~Chronometer() = default;

	public:
		Chronometer(const Chronometer&) = delete;
		Chronometer& operator=(const Chronometer&) = delete;
		Chronometer(Chronometer&&) = delete;
		Chronometer& operator=(Chronometer&&) = delete;

	private:
		bool Initialize();
		void Release();

	public:
		inline void Tick(Pitaya::Core::PassKey<Pitaya::Engine::Engine>) noexcept
		{
			tick = std::chrono::steady_clock::now();
			unscaledDeltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(tick - lastTick).count();
			delta = unscaledDeltaTime * scale;
			lastTick = tick;
			INVOKE_POSTCHRONOMETERTICK_HOOK
		}
		inline void FrameSync(Pitaya::Core::PassKey<Pitaya::Engine::Engine>) noexcept
		{
			if (targetFrameTime <= 0.0f) { return; }

			std::chrono::nanoseconds frameNanos = std::chrono::nanoseconds(static_cast<int64_t>(targetFrameTime * 1e9f));
			std::chrono::steady_clock::time_point targetTimePoint = tick + frameNanos - vsyncMargin;
			std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
			if (now < targetTimePoint)
			{
				std::chrono::nanoseconds timeToSleep = targetTimePoint - now;

				if (timeToSleep > std::chrono::milliseconds(2))
				{
					std::this_thread::sleep_for(timeToSleep - std::chrono::milliseconds(2));
				}

				while (std::chrono::steady_clock::now() < targetTimePoint)
				{
					std::this_thread::yield();
				}
			}
		}

	public:
		inline void SetFPS(float fps) noexcept
		{
			if (fps <= 0.0f)
			{
				targetFrameTime = 0.0f;
				return;
			}

			targetFrameTime = 1.0f / fps;
			RefreshVSyncMargin();
		}
		inline void RefreshVSyncMargin() noexcept
		{
			vsyncMargin = Pitaya::Config::GetEnableVSync() ?
				std::chrono::nanoseconds(500000) : std::chrono::nanoseconds(0);
		}

	public:
		inline float Getdelta() const noexcept
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
		inline float GetFramerate() const noexcept
		{
			return unscaledDeltaTime > 0.0f ? 1.0f / unscaledDeltaTime : 0.0f;
		}
		inline double Seconds() const noexcept
		{
			return std::chrono::duration_cast<std::chrono::duration<double>>
				(std::chrono::steady_clock::now() - start).count();
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

		std::chrono::nanoseconds vsyncMargin = std::chrono::nanoseconds(0);
	};
}
