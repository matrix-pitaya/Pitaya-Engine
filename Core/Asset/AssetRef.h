#pragma once

#include<Core/StateFlags/AtomicStateFlags.h>
#include<Core/RefCount/AtomicRefCount.h>
#include<Core/Identifier/GUID.h>
#include<Core/Asset/AssetState.h>

#include<atomic>
#include<filesystem>
#include<unordered_set>

namespace Pitaya::Core
{
    template<typename T>
    class AssetRef
    {
    public:
        struct AssetEntry final
        {
            std::atomic<T*> Data = nullptr;
            Pitaya::Core::GUID GUID;
            Pitaya::Core::AtomicRefCount RefCount;
            Pitaya::Core::AtomicStateFlags<AssetState> State = AssetState::Empty;
        };

    public:
        AssetRef() noexcept = default;
        ~AssetRef() noexcept
        {
            if (entry) { entry->RefCount.ReduceRefCount(); }
        }
        AssetRef(std::nullptr_t) noexcept
            : entry(nullptr) {}
        explicit AssetRef(AssetEntry* entry) noexcept
            : entry(entry)
        {
            if (entry) { entry->RefCount.AddRefCount(); }
        }
        AssetRef(const AssetRef& other) noexcept
            : entry(other.entry)
        {
            if (entry) { entry->RefCount.AddRefCount(); }
        }
        AssetRef& operator=(const AssetRef& other) noexcept
        {
            if (this == &other) { return *this; }
            if (entry) { entry->RefCount.ReduceRefCount(); }
            entry = other.entry;
            if (entry) { entry->RefCount.AddRefCount(); }
            return *this;
        }
        AssetRef(AssetRef&& other) noexcept
            : entry(other.entry)
        {
            other.entry = nullptr;
        }
        AssetRef& operator=(AssetRef&& other) noexcept
        {
            if (this == &other) { return *this; }
            if (entry) { entry->RefCount.ReduceRefCount(); }
            entry = other.entry;
            other.entry = nullptr;
            return *this;
        }
        AssetRef& operator=(std::nullptr_t) noexcept
        {
            if (entry) { entry->RefCount.ReduceRefCount(); }
            entry = nullptr;
            return *this;
        }
        AssetRef& operator=(AssetEntry* entry) noexcept
        {
            if (this->entry == entry) { return *this; }
            if (this->entry) { this->entry->RefCount.ReduceRefCount(); }
            this->entry = entry;
            if (this->entry) { this->entry->RefCount.AddRefCount(); }
            return *this;
        }

    public:
        explicit operator bool() const noexcept
        {
            return entry != nullptr;
        }
        bool operator==(std::nullptr_t) const noexcept
        {
            return entry == nullptr;
        }
        bool operator!=(std::nullptr_t) const noexcept
        {
            return entry != nullptr;
        }
        bool operator==(const AssetRef& other) const noexcept
        {
            return entry == other.entry;
        }
        bool operator!=(const AssetRef& other) const noexcept
        {
            return entry != other.entry;
        }

    public:
        T& operator*() const
        {
            if (!entry)
            {
                throw std::runtime_error("Dereferencing null asset");
            }
            T* data = entry->Data.load(std::memory_order_relaxed);
            if (!data)
            {
                throw std::runtime_error("Dereferencing asset with null data GUID:" + entry->GUID.ToString());
            }
            return *data;
        }
        T* operator->() const noexcept
        {
            return entry ? entry->Data.load(std::memory_order_relaxed) : nullptr;
        }

    public:
        inline bool IsReady() const noexcept
        {
            if (!entry) { return false; }
            if (entry->State.HasBits(Pitaya::Core::AssetState::Unload))
            {
                entry->RefCount.ReduceRefCount();
                entry = nullptr;
                return false;
            }
            return entry->State.HasAllBits(Pitaya::Core::AssetState::CPULoaded, Pitaya::Core::AssetState::GPULoaded);
        }
        inline T* GetNativeAssetData() const noexcept
        {
            return entry ? entry->Data.load(std::memory_order_acquire) : nullptr;
        }
        inline Pitaya::Core::GUID GetGUID() const noexcept
        {
            return entry ? entry->GUID : Pitaya::Core::GUID();
        }

    private:
        mutable AssetEntry* entry = nullptr;
    };
}
