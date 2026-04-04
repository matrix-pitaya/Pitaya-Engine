#pragma once

#include<unordered_map>
#include<cstdint>
#include<optional>
#include<initializer_list> 
#include<utility>

namespace Pitaya::Core
{
    template<typename Key1, typename Key2>
    class BidirectionalMap
    {
    public:
        BidirectionalMap() = default;
        ~BidirectionalMap() = default;

    public:
        BidirectionalMap(const BidirectionalMap&) = default;
        BidirectionalMap& operator=(const BidirectionalMap&) = default;
        BidirectionalMap(BidirectionalMap&&) = default;
        BidirectionalMap& operator=(BidirectionalMap&&) = default;
        BidirectionalMap(std::initializer_list<std::pair<Key1, Key2>> init_list)
        {
            for (const auto& pair : init_list)
            {
                map1[pair.first] = pair.second;
                map2[pair.second] = pair.first;
            }
        }
        BidirectionalMap& operator=(std::initializer_list<std::pair<Key1, Key2>> init_list)
        {
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
            map1[k1] = k2;
            map2[k2] = k1;
        }
        inline bool Contains(const Key1& k1) const noexcept
        {
            return map1.count(k1) > 0;
        }
        inline bool Contains(const Key2& k2) const noexcept
        {
            return map2.count(k2) > 0;
        }
        inline std::optional<Key2> GetBy(const Key1& k1) const noexcept
        {
            auto it = map1.find(k1);
            return it != map1.end() ? std::make_optional(it->second) : std::nullopt;
        }
        inline std::optional<Key1> GetBy(const Key2& k2) const noexcept
        {
            auto it = map2.find(k2);
            return it != map2.end() ? std::make_optional(it->second) : std::nullopt;
        }
        inline void Clear() noexcept
        {
            map1.clear();
            map2.clear();
        }
        inline bool Erase(const Key1& k1) noexcept
        {
            auto it = map1.find(k1);
            if (it == map1.end()) { return false; }

            map2.erase(it->second);
            map1.erase(it);
            return true;
        }
        inline bool Erase(const Key2& k2) noexcept
        {
            auto it = map2.find(k2);
            if (it == map2.end()) { return false; }

            map1.erase(it->second);
            map2.erase(it);
            return true;
        }

    private:
        std::unordered_map<Key1, Key2> map1;
        std::unordered_map<Key2, Key1> map2;
    };
}
