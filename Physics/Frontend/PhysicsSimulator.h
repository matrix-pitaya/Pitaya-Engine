#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/Storage/Storage.h>
#include<Context/Common/Module.h>

#include<Hook/def.h>

#include<Thread/Common/FuncTable.h>

#include<atomic>
#include<chrono>
#include<thread>

namespace Pitaya::Physics
{
    class PhysicsSimulator
    {
    public:
        struct Factory
        {
            friend class Pitaya::Engine::Module<PhysicsSimulator>;
        private:
            static PhysicsSimulator* Create()
            {
                return PITAYA_NEW(PhysicsSimulator);
            }
            static void Destroy(PhysicsSimulator* physicsSimulator)
            {
                PITAYA_DELETE(physicsSimulator);
            }
        };
        struct Lifecycle
        {
            friend class Pitaya::Engine::Module<PhysicsSimulator>;
        private:
            static bool Initialize(PhysicsSimulator* physicsSimulator)
            {
                return physicsSimulator->Initialize();
            }
            static void Release(PhysicsSimulator* physicsSimulator)
            {
                physicsSimulator->Release();
            }
            static void FixedUpdate(PhysicsSimulator* physicsSimulator)
            {
                physicsSimulator->FixedUpdate();
            }
        };

    private:
        PhysicsSimulator() = default;
        ~PhysicsSimulator() = default;

    public:
        PhysicsSimulator(const PhysicsSimulator&) = delete;
        PhysicsSimulator& operator=(const PhysicsSimulator&) = delete;
        PhysicsSimulator(PhysicsSimulator&&) = delete;
        PhysicsSimulator& operator=(PhysicsSimulator&&) = delete;

    private:
        inline bool Initialize()
        {
            accumulator = 0.0f;
            isRunning.store(true, std::memory_order_release);
            pendingStepCount.store(0, std::memory_order_release);
            lastFixedUpdateTime = std::chrono::steady_clock::now();
            physicsThread = Pitaya::Thread::RegisterThread(Pitaya::Thread::ThreadType::Physics, "Physics", &Pitaya::Physics::PhysicsSimulator::BootstrapPhysicsThread, this, nullptr);
            if (physicsThread == Pitaya::Core::Thread::Identifier::Invalid) { throw std::runtime_error("Physics Thread Register Fail!"); }
            return true;
        }
        inline void Release()
        {
            isRunning.store(false, std::memory_order_release);
            Pitaya::Thread::UnregisterThread(physicsThread);
        }
        
    private:
        bool InitializePhysicsContext();
        void ReleasePhysicsContext();

    private:
        inline void PhysicsThread()
        {
            InitializePhysicsContext();

            while (true)
            {
                if (!isRunning.load(std::memory_order_acquire)) { break; }

                uint32_t stepsToRun = pendingStepCount.load(std::memory_order_acquire);
                if (stepsToRun == 0)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    continue;
                }

                // TODO B.3.7: ConsumeCommands();
                // TODO B.3.7: for (uint32_t i = 0; i < stepsToRun; ++i) world.stepSimulation(FixedTimestep);
                pendingStepCount.fetch_sub(stepsToRun, std::memory_order_release);

                // TODO B.3.7: CaptureSnapshot();
            }

            ReleasePhysicsContext();
        }

    private:
        inline void FixedUpdate()
        {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration<float>(now - lastFixedUpdateTime).count();
            lastFixedUpdateTime = now;
            accumulator += elapsed;

            constexpr const float maxAccumulatedTime = FixedTimestep * MaxFixedUpdateExecuteTimes;
            accumulator = accumulator > maxAccumulatedTime ? maxAccumulatedTime : accumulator;
            while (accumulator >= FixedTimestep)
            {
                pendingStepCount.fetch_add(1, std::memory_order_release);
                accumulator -= FixedTimestep;
            }
        }

    private:
        inline static void BootstrapPhysicsThread(void* physicsSimulator, void*)
        {
            static_cast<Pitaya::Physics::PhysicsSimulator*>(physicsSimulator)->PhysicsThread();
        }

    public:
        inline static constexpr const float FixedTimestep = 1.0f / 60.0f;
        inline static constexpr const uint32_t MaxFixedUpdateExecuteTimes = 5;

    private:
        float accumulator = 0.0f;
        std::atomic<bool> isRunning = false;
        std::atomic<uint32_t> pendingStepCount = 0;
        std::chrono::steady_clock::time_point lastFixedUpdateTime;
        Pitaya::Core::Thread::Identifier physicsThread;
        Pitaya::Core::Storage<40> backendStorage;
    };
}
