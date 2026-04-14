#pragma once

#include<vector>
#include<unordered_set>
#include<functional>
#include<stdexcept>
#include<mimalloc.h>

namespace Pitaya::Core
{
    template<typename T>
    class ObjectPool
    {
    public:
        ObjectPool(std::function<void(T*)> OnGet = nullptr, std::function<void(T*)> OnRelease = nullptr, size_t count = 0)
            : OnGet(std::move(OnGet)), OnRelease(std::move(OnRelease))
        {
            idleObjects.reserve(count);
            for (size_t i = 0; i < count; ++i)
            {
                void* mem = mi_malloc_aligned(sizeof(T), alignof(T));
                idleObjects.emplace_back(new (mem) T());
            }
        }
        ~ObjectPool()
        {
            for (T* object : activeObjects)
            {
                if (OnRelease) { OnRelease(object); }
                object->~T();
                mi_free(object);
            }
            activeObjects.clear();

            for (T* object : idleObjects)
            {
                if (OnRelease) { OnRelease(object); }
                object->~T();
                mi_free(object);
            }
            idleObjects.clear();
        }

    public:
        ObjectPool(const ObjectPool&) = delete;
        ObjectPool& operator=(const ObjectPool&) = delete;
        ObjectPool(ObjectPool&&) = delete;
        ObjectPool& operator=(ObjectPool&&) = delete;

    public:
        inline T* Get()
        {
            T* object = nullptr;
            if (idleObjects.empty())
            {
                void* mem = mi_malloc_aligned(sizeof(T), alignof(T));
                object = new (mem) T();
            }
            else
            {
                object = idleObjects.back();
                idleObjects.pop_back();
            }

            if (OnGet) { OnGet(object); }
            activeObjects.insert(object);
            return object;
        }
        inline void Release(T* object)
        {
            if (!object) { return; }

            auto iterator = activeObjects.find(object);
            if (iterator == activeObjects.end())
            {
                throw std::runtime_error("ObjectPool::Release: Object not managed by this pool!");
            }

            if (OnRelease) { OnRelease(object); }
            activeObjects.erase(iterator);
            idleObjects.push_back(object);
        }

    private:
        std::function<void(T*)> OnGet;
        std::function<void(T*)> OnRelease;
        std::vector<T*> idleObjects;
        std::unordered_set<T*, std::hash<T*>, std::equal_to<T*>> activeObjects;
    };
}
