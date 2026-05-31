#pragma once

#include<Core/Allocate/Allocate.h>
#include<Core/Storage/Storage.h>
#include<Core/PassKey/PassKey.h>
#include<Context/Common/Module.h>

#include<Hook/def.h>

#include<Physics/Command/CreateBodyCommand.h>
#include<Physics/Command/DestroyBodyCommand.h>
#include<Physics/Command/SetTransformCommand.h>
#include<Physics/Command/SetLinearVelocityCommand.h>
#include<Physics/Command/SetAngularVelocityCommand.h>
#include<Physics/Command/AddForceCommand.h>
#include<Physics/Command/AddTorqueCommand.h>
#include<Physics/Command/SetGravityCommand.h>
#include<Physics/Command/UpdateBodyParamsCommand.h>
#include<Physics/Specific/PhysicsSnapshot.h>

#include<Thread/Common/FuncTable.h>

#include<atomic>
#include<chrono>
#include<thread>
#include<vector>
#include<cstring>

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

    public:
        class PhysicsPacket
        {
            friend class PhysicsSimulator;
        public:
            struct Buffer
            {
                std::vector<std::byte> CommandBuffer;
                Pitaya::Physics::PhysicsSnapshot Snapshot;

                inline void Clear() noexcept
                {
                    CommandBuffer.clear();
                    Snapshot.Prev.clear();
                    Snapshot.Curr.clear();
                    Snapshot.StepCompletionTime = 0.0;
                    Snapshot.StepSequence = 0;
                }
            };

        private:
            struct CommandHeader
            {
                Pitaya::Physics::PhysicsCommandType Type = Pitaya::Physics::PhysicsCommandType::Invalid;
                uint32_t Size = 0;
            };

        private:
            PhysicsPacket() = default;
            ~PhysicsPacket() = default;

        public:
            PhysicsPacket(const PhysicsPacket&) = delete;
            PhysicsPacket& operator=(const PhysicsPacket&) = delete;
            PhysicsPacket(PhysicsPacket&&) = delete;
            PhysicsPacket& operator=(PhysicsPacket&&) = delete;

        public:
            template<typename T>
            inline void PushCommand(const T& cmd)
            {
                static_assert(std::is_trivially_copyable_v<T>, "Physics command must be trivially copyable");

                CommandHeader header;
                header.Type = T::Type;
                header.Size = sizeof(T);

                Buffer& writeBuffer = buffers[writeIndex.load(std::memory_order_acquire)];
                const size_t offset = writeBuffer.CommandBuffer.size();
                writeBuffer.CommandBuffer.resize(offset + sizeof(CommandHeader) + sizeof(T));
                std::memcpy(writeBuffer.CommandBuffer.data() + offset, &header, sizeof(CommandHeader));
                std::memcpy(writeBuffer.CommandBuffer.data() + offset + sizeof(CommandHeader), &cmd, sizeof(T));
            }
            inline void SwapBuffer()
            {
                uint32_t newIndex = 1 - writeIndex.load(std::memory_order_acquire);
                buffers[newIndex].Clear();
                writeIndex.store(newIndex, std::memory_order_release);
            }

        private:
            Buffer buffers[2];
            std::atomic<uint32_t> writeIndex = 0;
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
            // front buffer reserve
            physicsPacket.buffers[0].CommandBuffer.reserve(16 * 1024);
            physicsPacket.buffers[0].Snapshot.Prev.reserve(128);
            physicsPacket.buffers[0].Snapshot.Curr.reserve(128);

			// back buffer reserve
            physicsPacket.buffers[1].CommandBuffer.reserve(16 * 1024);
            physicsPacket.buffers[1].Snapshot.Prev.reserve(128);
            physicsPacket.buffers[1].Snapshot.Curr.reserve(128);

            // start render thread
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
            // terminate render thread
            isRunning.store(false, std::memory_order_release);
            Pitaya::Thread::UnregisterThread(physicsThread);

			// clear physics packet
			physicsPacket.buffers[0].Clear();
			physicsPacket.buffers[1].Clear();
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

                physicsPacket.SwapBuffer();
                // TODO B.3.7: ConsumeCommands();
                // TODO B.3.7: for (uint32_t i = 0; i < stepsToRun; ++i) world.stepSimulation(FixedTimestep);
                pendingStepCount.fetch_sub(stepsToRun, std::memory_order_release);

                // TODO B.3.7: CaptureSnapshot() → physicsPacket.GetWriteSnapshot();
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

    public:
        template<typename T>
        inline void PushCommand(Pitaya::Core::PassKey<Pitaya::Engine::Engine>, const T& cmd)
        {
            physicsPacket.PushCommand(cmd);
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
        Pitaya::Physics::PhysicsSimulator::PhysicsPacket physicsPacket;
    };
}
