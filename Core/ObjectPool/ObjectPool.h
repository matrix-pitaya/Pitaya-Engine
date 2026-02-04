#pragma once

#include<queue>
#include<unordered_set>
#include<functional>
#include<stdexcept>
#include<memory_resource>

#define OBJECTPOOL_VERSION_NEW

namespace Pitaya::Core
{
#ifdef OBJECTPOOL_VERSION_NEW
    template<typename T>
    class ObjectPool
    {
    public:
		ObjectPool(std::function<void(T*)> OnGet = nullptr, std::function<void(T*)> OnRelease = nullptr, size_t count = 0)
			: OnGet(std::move(OnGet)), OnRelease(std::move(OnRelease)), pool(), idleObjects(&pool), activeObjects(&pool)
        {
            idleObjects.reserve(count);
            for (size_t i = 0; i < count; ++i)
            {
                idleObjects.emplace_back(new (pool.allocate(sizeof(T), alignof(T))) T());
            }
        }
        ~ObjectPool()
        {
			for (T* object : activeObjects)
			{
				if (OnRelease)
				{
					OnRelease(object);
				}
				
                object->~T();
				object = nullptr;
            }
			activeObjects.clear();

            for (T* object : idleObjects)
            {
				if (OnRelease)
				{
					OnRelease(object);
				}

                object->~T();
				object = nullptr;
            }
			idleObjects.clear();
        }
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
				object = new (pool.allocate(sizeof(T), alignof(T))) T();
            }
            else
            {
				object = idleObjects.back();
                idleObjects.pop_back();
            }

			if (OnGet)
			{
				OnGet(object);
			}

            activeObjects.insert(object);
            return object;
        }
		inline void Release(T* object)
        {
			if (!object)
			{
				return;
			}

            auto iterator = activeObjects.find(object);
            if (iterator == activeObjects.end())
            {
                throw std::runtime_error("ObjectPool::Release: Object not managed by this pool!");
            }

			if (OnRelease)
			{
				OnRelease(object);
			}

            activeObjects.erase(iterator);
            idleObjects.push_back(object);
        }

    private:
        std::function<void(T*)> OnGet;
        std::function<void(T*)> OnRelease;

        std::pmr::unsynchronized_pool_resource pool;

        std::pmr::vector<T*> idleObjects;
        std::pmr::unordered_set<T*> activeObjects;
    };
#endif 

#ifdef OBJECTPOOL_VERSION_OLD
	template<typename T>
	class ObjectPool
	{
	public:
		ObjectPool(std::function<void(T*)> OnGet = nullptr, std::function<void(T*)> OnRelease = nullptr, unsigned int size = 0)
			:OnGet(std::move(OnGet)), OnRelease(std::move(OnRelease))
		{
			for (int i = 0; i < size; i++)
			{
				T* object = new T();
				pool.push(object);
			}
		}
		~ObjectPool()
		{
			while (!pool.empty())
			{
				T* object = pool.front();
				pool.pop();

				if (OnRelease)
				{
					OnRelease(object);
				}

				delete object;
				object = nullptr;
			}

			for (auto& object : check)
			{
				if (OnRelease)
				{
					OnRelease(object);
				}

				delete object;
			}
			check.clear();
		}
		ObjectPool(const ObjectPool&) = delete;
		ObjectPool& operator=(const ObjectPool&) = delete;
		ObjectPool(ObjectPool&&) = delete;
		ObjectPool& operator=(ObjectPool&&) = delete;

		T* Get()
		{
			T* object = nullptr;
			if (!pool.empty())
			{
				object = pool.front();
				pool.pop();
			}
			else
			{
				object = new T();
			}

			if (!check.insert(object).second)
			{
				throw std::runtime_error("Get: object already borrowed, logic error!");
			}

			if (OnGet)
			{
				OnGet(object);
			}

			return object;
		}
		void Release(T* object)
		{
			//对象池类型检测
			if (typeid(*object) != typeid(T))
			{
				throw std::runtime_error("Type Error: Release object is not of type T");
			}

			//避免重复归还
			if (check.find(object) == check.end())
			{
				throw std::runtime_error("The object was returned multiple times");
			}

			if (OnRelease)
			{
				OnRelease(object);
			}

			pool.push(object);
			check.erase(object);
		}

	private:
		std::queue<T*> pool;
		std::unordered_set<T*> check;
		std::function<void(T*)> OnGet;
		std::function<void(T*)> OnRelease;
	};
#endif
}
