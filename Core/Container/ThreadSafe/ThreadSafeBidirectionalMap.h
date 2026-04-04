#pragma once

#include<unordered_map>
#include<optional>
#include<shared_mutex>
#include<initializer_list> 
#include<utility>
#include<vector>

namespace Pitaya::Core
{
    template<typename Key1, typename Key2>
    class ThreadSafeBidirectionalMap
    {
    public:
        ThreadSafeBidirectionalMap() = default;
        ~ThreadSafeBidirectionalMap() = default;

    public:
        ThreadSafeBidirectionalMap(const ThreadSafeBidirectionalMap&) = delete;
        ThreadSafeBidirectionalMap& operator=(const ThreadSafeBidirectionalMap&) = delete;
        ThreadSafeBidirectionalMap(ThreadSafeBidirectionalMap&&) = default;
        ThreadSafeBidirectionalMap& operator=(ThreadSafeBidirectionalMap&&) = default;
        ThreadSafeBidirectionalMap(std::initializer_list<std::pair<Key1, Key2>> init_list)
        {
            std::unique_lock lock(mutex);
            for (const auto& pair : init_list) 
            {
                map1[pair.first] = pair.second;
                map2[pair.second] = pair.first;
            }
        }
        ThreadSafeBidirectionalMap& operator=(std::initializer_list<std::pair<Key1, Key2>> init_list)
        {
            std::unique_lock lock(mutex);
            map1.clear();
            map2.clear();
            for (const auto& pair : init_list) 
            {
                map1[pair.first] = pair.second;
                map2[pair.second] = pair.first;
            }
            return *this;
        }

    public:
        inline void Insert(const Key1& k1, const Key2& k2) noexcept
        {
            std::unique_lock lock(mutex);
            map1[k1] = k2;
            map2[k2] = k1;
        }
        inline void Insert(const Key2& k1, const Key1& k2) noexcept
        {
            std::unique_lock lock(mutex);
            map1[k2] = k1;
            map2[k1] = k2;
        }
        inline void BatchInsert(const std::initializer_list<std::pair<Key1, Key2>>& init_list) noexcept
        {
            std::unique_lock lock(mutex);
            for (const auto& pair : init_list)
            {
                map1[pair.first] = pair.second;
                map2[pair.second] = pair.first;
            }
        }
        inline void BatchInsert(const std::initializer_list<std::pair<Key2, Key1>>& init_list) noexcept
        {
            std::unique_lock lock(mutex);
            for (const auto& pair : init_list)
            {
                map1[pair.second] = pair.first;
                map2[pair.first] = pair.second;
            }
        }
        inline void BatchInsert(const std::vector<std::pair<Key1, Key2>>& pairs) noexcept
        {
            std::unique_lock lock(mutex);
            for (const auto& pair : pairs)
            {
                map1[pair.first] = pair.second;
                map2[pair.second] = pair.first;
            }
        }
        inline void BatchInsert(const std::vector<std::pair<Key2, Key1>>& pairs) noexcept
        {
            std::unique_lock lock(mutex);
            for (const auto& pair : pairs)
            {
                map1[pair.second] = pair.first;
                map2[pair.first] = pair.second;
            }
        }
        inline std::vector<std::pair<Key1, Key2>> GetAllKeyValuePairs() const noexcept
        {
            std::shared_lock lock(mutex);
            std::vector<std::pair<Key1, Key2>> result;
            result.reserve(map1.size());
            for (const auto& [k1, k2] : map1)
            {
                result.emplace_back(k1, k2);
            }
            return result;
        }

        inline bool Contains(const Key1& k1) const noexcept
        {
            std::shared_lock lock(mutex);
            return map1.count(k1) > 0;
        }
        inline bool Contains(const Key2& k2) const noexcept
        {
            std::shared_lock lock(mutex);
            return map2.count(k2) > 0;
        }
        inline std::optional<Key2> Get(const Key1& k1) const noexcept
        {
            std::shared_lock lock(mutex);
            auto it = map1.find(k1);
            return it != map1.end() ? std::make_optional(it->second) : std::nullopt;
        }
        inline std::optional<Key1> Get(const Key2& k2) const noexcept
        {
            std::shared_lock lock(mutex);
            auto it = map2.find(k2);
            return it != map2.end() ? std::make_optional(it->second) : std::nullopt;
        }
        inline void Clear() noexcept
        {
            std::unique_lock lock(mutex);
            map1.clear();
            map2.clear();
        }
        inline bool Erase(const Key1& k1) noexcept
        {
            std::unique_lock lock(mutex);
            auto it = map1.find(k1);
            if (it == map1.end()) { return false; }

            map2.erase(it->second);
            map1.erase(it);
            return true;
        }
        inline bool Erase(const Key2& k2) noexcept
        {
            std::unique_lock lock(mutex);
            auto it = map2.find(k2);
            if (it == map2.end()) { return false; }

            map1.erase(it->second);
            map2.erase(it);
            return true;
        }
        template <typename SuccessCallback, typename FailCallback>
        inline void FindOperateKV(const Key1& key, SuccessCallback&& successCallback, FailCallback&& failCallback)
        {
            std::unique_lock lock(mutex);
            auto it = map1.find(key);
            if (it != map1.end())
            {
                std::forward<SuccessCallback>(successCallback)(it->first, it->second);
            }
            else
            {
                std::forward<FailCallback>(failCallback)(key);
            }
        }
        template <typename SuccessCallback, typename FailCallback>
        inline void FindOperateKV(const Key2& key, SuccessCallback&& successCallback, FailCallback&& failCallback)
        {
            std::unique_lock lock(mutex);
            auto it = map2.find(key);
            if (it != map2.end())
            {
                std::forward<SuccessCallback>(successCallback)(it->first, it->second);
            }
            else
            {
                std::forward<FailCallback>(failCallback)(key);
            }
        }

    private:
        mutable std::shared_mutex mutex;
        std::unordered_map<Key1, Key2> map1;
        std::unordered_map<Key2, Key1> map2;
    };
}
