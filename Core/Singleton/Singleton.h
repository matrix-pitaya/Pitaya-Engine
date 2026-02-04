#pragma once

namespace Pitaya::Core
{
    template <typename T>
    class Singleton
    {
    protected:
        Singleton() = default;
        ~Singleton() = default;

    public:
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;
        Singleton(Singleton&&) = delete;
        Singleton& operator=(Singleton&&) = delete;
    
    public:
        inline static T& Instance()
        {
            static T instance;
            return instance;
        }
    };
}