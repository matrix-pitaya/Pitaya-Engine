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
        static T& Instance();   // 为防止多dll出现幽灵单例问题 请手动特例化此函数
    };
}
