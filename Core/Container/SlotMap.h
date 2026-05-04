#pragma once

#include<cstdint>
#include<vector>
#include<limits>
#include<utility> 

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
                return index != std::numeric_limits<uint32_t>::max() && generation != 0;
            }
            bool operator==(const Handle& other) const noexcept
            {
                return index == other.index && generation == other.generation;
            }
            bool operator!=(const Handle& other) const noexcept
            {
                return !(*this == other);
            }

        public:
            inline uint64_t AsUint64() const noexcept
            {
                return (static_cast<uint64_t>(generation) << 32) | static_cast<uint64_t>(index);
            }
            inline uint32_t Index() const noexcept
            {
                return index;
            }
            inline uint32_t Generation() const noexcept
            {
                return generation;
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
        struct SparseSlot
        {
            uint32_t denseIndex = 0;
            uint32_t generation = 1;
            bool isActive = false;
        };

    public:
        class Iterator
        {
        public:
            Iterator(SlotMap* map, uint32_t idx)
                : map(map), denseIndex(idx) { }

        public:
            inline std::pair<Handle, Item&> operator*()
            {
                uint32_t sparseIndex = map->denseToSparse[denseIndex];
                uint32_t gen = map->sparse[sparseIndex].generation;
                return std::pair<Handle, Item&>(Handle(sparseIndex, gen), map->dense[denseIndex]);
            }
            inline Iterator& operator++()
            {
                denseIndex++;
                return *this;
            }
            inline bool operator!=(const Iterator& other) const
            {
                return denseIndex != other.denseIndex;
            }

        private:
            SlotMap* map;
            uint32_t denseIndex;
        };
        struct View
        {
            View(SlotMap* map)
                : map(map) { }

        public:
            inline Iterator begin()
            {
                return Iterator(map, 0);
            }
            inline Iterator end()
            {
                return Iterator(map, static_cast<uint32_t>(map->dense.size()));
            }

        private:
            SlotMap* map;
        };

    public:
        SlotMap(size_t count = 256)
        {
            sparse.reserve(count);
            dense.reserve(count);
            denseToSparse.reserve(count);
        }
        ~SlotMap() = default;

    public:
        SlotMap(const SlotMap&) = delete;
        SlotMap& operator=(const SlotMap&) = delete;
        SlotMap(SlotMap&&) = default;
        SlotMap& operator=(SlotMap&&) = default;

    public:
        inline Handle Emplace(Item&& item)
        {
            uint32_t sparseIndex = 0;
            if (!freeList.empty())
            {
                sparseIndex = freeList.back();
                freeList.pop_back();
            }
            else
            {
                sparseIndex = static_cast<uint32_t>(sparse.size());
                sparse.emplace_back();
            }

            uint32_t denseIndex = static_cast<uint32_t>(dense.size());
            dense.emplace_back(std::move(item));
            denseToSparse.push_back(sparseIndex);

            SparseSlot& slot = sparse[sparseIndex];
            slot.denseIndex = denseIndex;
            slot.isActive = true;

            return { sparseIndex, slot.generation };
        }
        inline bool Update(Handle handle, Item&& newItem)
        {
            if (!CheckHandleValid(handle)) { return false; }
            dense[sparse[handle.index].denseIndex] = std::move(newItem);
            return true;
        }
        inline bool Get(Handle handle, Item& out) const
        {
            if (!CheckHandleValid(handle)) { return false; }
            out = dense[sparse[handle.index].denseIndex];
            return true;
        }
        inline Item* Get(Handle handle)
        {
            return CheckHandleValid(handle) ?
                &dense[sparse[handle.index].denseIndex] :
                nullptr;
        }
        inline const Item* Get(Handle handle) const
        {
            return CheckHandleValid(handle) ?
                &dense[sparse[handle.index].denseIndex] :
                nullptr;
        }
        inline bool Remove(Handle handle)
        {
            if (!CheckHandleValid(handle)) { return false; }

            uint32_t deletedDenseIdx = sparse[handle.index].denseIndex;
            uint32_t lastDenseIdx = static_cast<uint32_t>(dense.size() - 1);

            if (deletedDenseIdx != lastDenseIdx)
            {
                dense[deletedDenseIdx] = std::move(dense.back());
                uint32_t lastSparseIdx = denseToSparse.back();
                denseToSparse[deletedDenseIdx] = lastSparseIdx;
                sparse[lastSparseIdx].denseIndex = deletedDenseIdx;
            }

            dense.pop_back();
            denseToSparse.pop_back();

            SparseSlot& slot = sparse[handle.index];
            slot.isActive = false;
            slot.generation++;
            freeList.push_back(handle.index);
            return true;
        }
        inline void Clear()
        {
            dense.clear();
            denseToSparse.clear();
            freeList.clear();

            for (uint32_t i = 0; i < sparse.size(); ++i)
            {
                if (sparse[i].isActive)
                {
                    sparse[i].isActive = false;
                    sparse[i].generation++;
                }
            }
            for (uint32_t i = static_cast<uint32_t>(sparse.size()); i > 0; --i)
            {
                freeList.push_back(i - 1);
            }
        }
        inline View Each() noexcept
        {
            return { this };
        }

    private:
        inline bool CheckHandleValid(Handle handle) const
        {
            if (!handle || handle.index >= sparse.size()) { return false; }
            const SparseSlot& slot = sparse[handle.index];
            return slot.isActive && slot.generation == handle.generation;
        }

    private:
        std::vector<SparseSlot> sparse;
        std::vector<Item> dense;
        std::vector<uint32_t> denseToSparse;
        std::vector<uint32_t> freeList;
    };

    template<typename Item>
    inline constexpr const typename SlotMap<Item>::Handle SlotMap<Item>::Handle::Invalid = {};
}