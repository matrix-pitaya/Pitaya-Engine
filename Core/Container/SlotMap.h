#pragma once

#include<cstdint>
#include<vector>
#include<limits>

namespace Pitaya::Core
{
	template<typename Item>
	class SlotMap
	{
	public:
        struct Handle
        {
            friend class SlotMap<Item>;
        public:
            constexpr Handle() noexcept = default;

        public:
            explicit operator bool() const noexcept
            {
                return index != std::numeric_limits<uint32_t>::max();
            }
            bool operator==(const Handle& other) const noexcept
            {
                return index == other.index && generation == other.generation;
            }
            bool operator!=(const Handle& other) const noexcept
            {
                return !(*this == other);
            }

        private:
            constexpr Handle(uint32_t index, uint32_t generation) noexcept
                : index(index), generation(generation) { }

        private:
            uint32_t index = std::numeric_limits<uint32_t>::max();
            uint32_t generation = 0;

        public:
            static const Handle Invalid;
        };

    private:
        struct Slot 
        {
            Item data;
            uint32_t generation = 1;
            bool isActive = false;
        };

    public:
        SlotMap()
        {
            pool.reserve(256);
        }
        ~SlotMap() = default;

    public:
        SlotMap(const SlotMap&) = delete;
        SlotMap& operator=(const SlotMap&) = delete;
        SlotMap(SlotMap&& other) = delete;
        SlotMap& operator=(SlotMap&& other) = delete;

    public:
        inline Handle Emplace(const Item& item)
        {
            if (!freeList.empty()) 
            {
                uint32_t index = freeList.back();
                freeList.pop_back();
                Slot& slot = pool[index];
                slot.data = item;
                slot.isActive = true;
                return { index, slot.generation };
            }

            uint32_t index = static_cast<uint32_t>(pool.size());
            pool.push_back({ item, 1, true });
            return { index ,1 };
        }
        inline bool Update(Handle handle, const Item& newItem)
        {
            if (!CheckHandleValid(handle)) { return false; }

            pool[handle.index].data = newItem;
            return true;
        }
        inline bool Get(Handle handle, Item& out) const
        {
            if (!CheckHandleValid(handle)) { return false; }

            out = pool[handle.index].data;
            return true;
        }
        inline bool Remove(Handle handle)
        {
            if (!CheckHandleValid(handle)) { return false; }

            Slot& slot = pool[handle.index];
            slot.isActive = false;
            slot.generation++;
            freeList.push_back(handle.index);
            return true;
        }

    private:
        inline bool CheckHandleValid(Handle handle) const
        {
            if (!handle || handle.index >= pool.size()) { return false; }

            const Slot& slot = pool[handle.index];
            return slot.isActive && slot.generation == handle.generation;
        }

    private:
        std::vector<Slot> pool;
        std::vector<uint32_t> freeList;
	};

    template<typename Item>
    inline constexpr const typename SlotMap<Item>::Handle SlotMap<Item>::Handle::Invalid = {};
}
