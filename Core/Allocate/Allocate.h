#pragma once

#include<mimalloc.h>
#include<new> 
#include<utility> 
#include<type_traits> 
#include<cstddef>

namespace Pitaya::Core
{
    inline void* Allocate(size_t size) noexcept 
    { 
        return mi_malloc(size);
    }
    inline void Free(void* ptr) noexcept
    { 
        mi_free(ptr); 
    }
    inline void* AllocateAligned(size_t size, size_t alignment) noexcept 
    { 
        return mi_malloc_aligned(size, alignment);
    }
    inline void FreeAligned(void* ptr, size_t alignment) noexcept 
    { 
        mi_free_aligned(ptr, alignment);
    }

    template<typename T, typename... Args>
    inline [[nodiscard]] T* New(Args&&... args)
    {
        constexpr bool overAligned = alignof(T) > alignof(std::max_align_t);
        void* memory = overAligned ?
            Pitaya::Core::AllocateAligned(sizeof(T), alignof(T)) :
            Pitaya::Core::Allocate(sizeof(T));

        if (!memory) { return nullptr; }

        try
        {
            return new (memory) T(std::forward<Args>(args)...);
        }
        catch (...)
        {
            if constexpr (overAligned)
            {
                Pitaya::Core::FreeAligned(memory, alignof(T));
            }
            else
            {
                Pitaya::Core::Free(memory);
            }
            throw;
        }
    }

    template<typename T>
    inline void Delete(T* ptr) noexcept
    {
        static_assert(sizeof(T) > 0, "Can't delete an incomplete type");
        static_assert(!std::is_array_v<T>, "Use un-implemented DeleteArray config for arrays");
        if constexpr (std::is_polymorphic_v<T>) { static_assert(std::has_virtual_destructor_v<T>, "Polymorphic types must have a virtual destructor"); }

        if (!ptr) { return; }
        void* rawMemory = nullptr;
        if constexpr (std::is_polymorphic_v<T>)
        {
            rawMemory = dynamic_cast<void*>(ptr);
        }
        else
        {
            rawMemory = static_cast<void*>(ptr);
        }

        ptr->~T();
        constexpr bool overAligned = alignof(T) > alignof(std::max_align_t);
        if constexpr (overAligned)
        {
            Pitaya::Core::FreeAligned(rawMemory, alignof(T));
        }
        else
        {
            Pitaya::Core::Free(rawMemory);
        }
    }
}

#define PITAYA_ALLOW_ALLOCATE                       \
    template<typename U, typename... UArgs>         \
    friend U* Pitaya::Core::New(UArgs&&...);        \
    template<typename U>                            \
    friend void Pitaya::Core::Delete(U*) noexcept;


#define PITAYA_NEW(Type, ...)                                                           \
    [&]() -> Type*                                                                      \
    {                                                                                   \
        constexpr bool overAligned = alignof(Type) > alignof(std::max_align_t);         \
        void* memory = overAligned ?                                                    \
            Pitaya::Core::AllocateAligned(sizeof(Type), alignof(Type)) :                \
            Pitaya::Core::Allocate(sizeof(Type));                                       \
                                                                                        \
        if (!memory) { return nullptr; }                                                \
                                                                                        \
        try                                                                             \
        {                                                                               \
            return new (memory) Type(__VA_ARGS__);                                      \
        }                                                                               \
        catch (...)                                                                     \
        {                                                                               \
            if constexpr (overAligned)                                                  \
            {                                                                           \
                Pitaya::Core::FreeAligned(memory, alignof(Type));                       \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                Pitaya::Core::Free(memory);                                             \
            }                                                                           \
            throw;                                                                      \
        }                                                                               \
    }()


#define PITAYA_DELETE(ptr)                                                              \
    do                                                                                  \
    {                                                                                   \
        auto* _p = (ptr);                                                               \
        if (!_p) { break; }                                                             \
                                                                                        \
        using T = std::remove_cv_t<std::remove_pointer_t<decltype(_p)>>;                \
                                                                                        \
        static_assert(sizeof(T) > 0, "Can't delete an incomplete type");                \
        static_assert(!std::is_array_v<T>, "Use un-implemented DeleteArray config");    \
                                                                                        \
        void* rawMemory = [](auto* p) -> void*                                          \
        {                                                                               \
            using U = std::remove_cv_t<std::remove_pointer_t<decltype(p)>>;             \
            if constexpr (std::is_polymorphic_v<U>)                                     \
            {                                                                           \
                return dynamic_cast<void*>(p);                                          \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                return static_cast<void*>(p);                                           \
            }                                                                           \
        }(_p);                                                                          \
                                                                                        \
        _p->~T();                                                                       \
                                                                                        \
        constexpr bool overAligned = alignof(T) > alignof(std::max_align_t);            \
        if constexpr (overAligned)                                                      \
        {                                                                               \
            Pitaya::Core::FreeAligned(rawMemory, alignof(T));                           \
        }                                                                               \
         else                                                                           \
        {                                                                               \
            Pitaya::Core::Free(rawMemory);                                              \
        }                                                                               \
    } while(false)
