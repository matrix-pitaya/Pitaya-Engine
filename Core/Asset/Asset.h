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
	class Asset
	{
	public:
		struct AssetEntry : public Pitaya::Core::AtomicRefCount
		{
			std::atomic<T*> Data = nullptr;
			Pitaya::Core::AtomicStateFlags<AssetState> State = AssetState::Empty;
			Pitaya::Core::GUID GUID;
		};
	public:
		Asset() noexcept = default;
		~Asset() noexcept
		{
			ReduceRefCount();
		}
		Asset(std::nullptr_t) noexcept
			: entry(nullptr) {}
		explicit Asset(Pitaya::Core::Asset<T>::AssetEntry* entry) noexcept
			: entry(entry)
		{
			AddRefCount();
		}
		Asset(const Asset& other) noexcept
			: entry(other.entry)
		{
			AddRefCount();
		}
		Asset& operator=(const Asset& other) noexcept
		{
			if (this == &other) { return *this; }

			ReduceRefCount();
			entry = other.entry;
			AddRefCount();
			return *this;
		}
		Asset(Asset&& other) noexcept
			: entry(other.entry)
		{
			other.entry = nullptr;
		}
		Asset& operator=(Asset&& other) noexcept
		{
			if (this == &other)
			{
				return *this;
			}

			ReduceRefCount();
			entry = other.entry;
			other.entry = nullptr;
			return *this;
		}
		Asset& operator=(std::nullptr_t) noexcept
		{
			ReduceRefCount();
			entry = nullptr;
			return *this;
		}
		Asset& operator=(Pitaya::Core::Asset<T>::AssetEntry* entry) noexcept
		{
			if (this->entry == entry) { return *this; }

			ReduceRefCount();
			this->entry = entry;
			AddRefCount();
			return *this;
		}

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
		bool operator==(const Asset& other) const noexcept
		{
			return entry == other.entry;
		}
		bool operator!=(const Asset& other) const noexcept
		{
			return entry != other.entry;
		}

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

		bool IsReady() const noexcept
		{
			if (!entry) { return false; }
			if (entry->State.HasBits(Pitaya::Core::AssetState::Unload))
			{
				ReduceRefCount();
				entry = nullptr;
				return false;
			}
			return entry->State.HasAllBits(Pitaya::Core::AssetState::CPULoaded, Pitaya::Core::AssetState::GPULoaded);
		}
		T* GetNativeAssetData() const noexcept
		{
			return entry ? entry->Data.load(std::memory_order_acquire) : nullptr;
		}
		Pitaya::Core::GUID GetGUID() const noexcept
		{
			return entry ? entry->GUID : Pitaya::Core::GUID();
		}

	private:
		inline void AddRefCount() const noexcept
		{
			if (!entry) { return; }
			entry->AddRefCount();
		}
		inline void ReduceRefCount() const noexcept
		{
			if (!entry) { return; }
			entry->ReduceRefCount();
		}

	private:
		mutable Pitaya::Core::Asset<T>::AssetEntry* entry = nullptr;
	};
}
