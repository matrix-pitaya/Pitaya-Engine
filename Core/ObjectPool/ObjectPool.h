#pragma once

#include<queue>
#include<unordered_set>
#include<functional>
#include<stdexcept>

namespace Pitaya::Core
{
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
}
