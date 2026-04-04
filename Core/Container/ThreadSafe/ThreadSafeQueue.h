#pragma once

#include<queue>
#include<mutex>

namespace Pitaya::Core
{
	template <typename T>
	class ThreadSafeQueue
	{
	public:
		ThreadSafeQueue() = default;
		~ThreadSafeQueue() = default;
		ThreadSafeQueue(const ThreadSafeQueue&) = delete;
		ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;
		ThreadSafeQueue(ThreadSafeQueue&&) = delete;
		ThreadSafeQueue& operator=(ThreadSafeQueue&&) = delete;

	public:
        inline void Push(const T& item)
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(item);
        }
        inline bool Pop(T& outItem)
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (queue.empty())
            {
                return false;
            }

            outItem = std::move(queue.front());
            queue.pop();
            return true;
        }
        inline std::queue<T> PopAll()
        {
            std::lock_guard<std::mutex> lock(mutex);
            std::queue<T> out;
            std::swap(out, queue);
            return out;
        }
        inline bool Empty() const 
        {
            std::lock_guard<std::mutex> lock(mutex);
            return queue.empty();
        }

	private:
		mutable std::mutex mutex;
		std::queue<T> queue;
	};
}
