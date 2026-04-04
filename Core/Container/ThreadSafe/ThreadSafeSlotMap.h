#pragma once

#include<cstdint>
#include<vector>
#include<limits>
#include<utility>
#include<shared_mutex> 
#include<mutex> 

namespace Pitaya::Core
{
    template<typename Item>
    class ThreadSafeSlotMap
    {
    public:
        struct Handle
        {
            friend class ThreadSafeSlotMap<Item>;
        public:
            constexpr Handle() noexcept
                : index(std::numeric_limits<uint32_t>::max()), generation(0) { }

        public:
            explicit operator bool() const noexcept
            {
                return index != std::numeric_limits<uint32_t>::max();
            }
            bool operator==(const Handle& other) const
            {
                return index == other.index && generation == other.generation;
            }
            bool operator!=(const Handle& other) const
            {
                return !(*this == other);
            }

        private:
            constexpr Handle(uint32_t index, uint32_t generation) noexcept
                : index(index), generation(generation) {}

        private:
            uint32_t index = 0;
            uint32_t generation = 0;

        public:
            static const Handle Invalid;
        };

    private:
        struct Slot
        {
            Item* Instance = nullptr;
            uint32_t Generation = 0;
        };

    public:
        ThreadSafeSlotMap() = default;
        ~ThreadSafeSlotMap()
        {
            Clear();
        }

    public:
        ThreadSafeSlotMap(const ThreadSafeSlotMap&) = delete;
        ThreadSafeSlotMap& operator=(const ThreadSafeSlotMap&) = delete;
        ThreadSafeSlotMap(ThreadSafeSlotMap&& other) = delete;
        ThreadSafeSlotMap& operator=(ThreadSafeSlotMap&& other) = delete;

    public:
        //[Main] 申请Slot槽位 (写操作：由 vector 扩容风险，必须独占)
        inline Handle Alloc()
        {
            std::unique_lock<std::shared_mutex> lock(_mutex);
            uint32_t index = 0;
            if (!freeIndices.empty())
            {
                index = freeIndices.back();
                freeIndices.pop_back();
            }
            else
            {
                index = static_cast<uint32_t>(slots.size());
                slots.emplace_back();
            }
            return Handle(index, slots[index].Generation);
        }
        //[Render] 向Slot槽填充实际数据 (写操作：需修改 slot 内容，必须独占)
        template<typename... Args>
        inline bool Construct(Handle handle, Args&&... args)
        {
            std::unique_lock<std::shared_mutex> lock(_mutex);
            if (handle.index >= slots.size()) { return false; }

            Slot& slot = slots[handle.index];
            if (slot.Generation != handle.generation) { return false; }
            if (slot.Instance != nullptr) { return false; }

            try
            {
                slot.Instance = new Item(std::forward<Args>(args)...);
                return true;
            }
            catch (...)
            {
                if (slot.Instance)
                {
                    delete slot.Instance;
                    slot.Instance = nullptr;
                }
                return false;
            }
        }
        //[Render] 向Slot槽填充实际数据 (写操作：需修改 slot 内容，必须独占)
        bool Attach(Handle handle, Item* ptr)
        {
            std::unique_lock<std::shared_mutex> lock(_mutex);
            if (handle.index >= slots.size()) { return false; }
            Slot& slot = slots[handle.index];
            if (slot.Generation != handle.generation) { return false; }
            if (slot.Instance != nullptr) { return false; }
            slot.Instance = ptr;
            return true;
        }
        //[Render] 移除Slot槽的资源 (写操作：需修改 slot/freeIndices，必须独占)
        inline bool Remove(Handle handle) noexcept
        {
            std::unique_lock<std::shared_mutex> lock(_mutex);
            if (handle.index >= slots.size()) { return false; }
            Slot& slot = slots[handle.index];
            if (slot.Generation != handle.generation) { return false; }
            if (slot.Instance)
            {
                delete slot.Instance;
                slot.Instance = nullptr;
            }
            slot.Generation++;
            freeIndices.push_back(handle.index);
            return true;
        }
        //[Render] 清空全部Slot槽资源
        inline void Clear() noexcept
        {
            std::unique_lock<std::shared_mutex> lock(_mutex);

            //析构对象并增加代数
            for (auto& slot : slots)
            {
                if (slot.Instance)
                {
                    delete slot.Instance;
                    slot.Instance = nullptr;
                }
                slot.Generation++;
            }

            //重建空闲链表
            freeIndices.clear();
            freeIndices.reserve(slots.size());

            size_t i = slots.size();
            while (i > 0)
            {
                freeIndices.push_back(static_cast<uint32_t>(--i));
            }
        }
        //[Main] 获取Handle句柄对应的槽数据 (读操作：允许多线程并发读)
        inline Item* Get(Handle handle) const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_mutex);

            if (handle.index >= slots.size()) { return nullptr; }
            const Slot& slot = slots[handle.index];
            if (slot.Generation != handle.generation) { return nullptr; }
            return slot.Instance;
        }
        //[Main] 检查handle句柄是否有效 (读操作)
        inline bool IsValid(Handle handle) const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_mutex);

            if (handle.index >= slots.size()) { return false; }
            const Slot& slot = slots[handle.index];
            return slot.Generation == handle.generation && slot.Instance != nullptr;
        }
        //[Main/Render] (读操作)
        inline bool IsReserved(Handle handle) const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_mutex);
            if (handle.index >= slots.size()) { return false; }
            return slots[handle.index].Generation == handle.generation;
        }
        //[Main/Render] (读操作)
        inline size_t Size() const noexcept
        {
            std::shared_lock<std::shared_mutex> lock(_mutex);
            return slots.size() - freeIndices.size();
        }

    private:
        std::vector<Slot> slots;
        std::vector<uint32_t> freeIndices;
        mutable std::shared_mutex _mutex;
    };

    template<typename Item>
    inline constexpr const typename ThreadSafeSlotMap<Item>::Handle ThreadSafeSlotMap<Item>::Handle::Invalid;
}
