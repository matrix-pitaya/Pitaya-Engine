#pragma once

#ifndef DELETE_COPY_AND_MOVE
#define DELETE_COPY_AND_MOVE(Class)			  \
private:									  \
	Class(const Class&) = delete;             \
	Class(Class&&) = delete;                  \
	Class& operator=(const Class&) = delete;  \
	Class& operator=(Class&&) = delete;
#endif

#ifndef Default_Constructor_Destructor_Move_Copy
#define Default_Constructor_Destructor_Move_Copy(Class) \
public:													\
	Class() = default;									\
	~Class() = default;									\
	Class(const Class&) = default;						\
	Class& operator=(const Class&) = default;			\
	Class(Class&&) = default;							\
	Class& operator=(Class&&) = default;
#endif

#ifndef Default_Constructor_VirtualDestructor_Move_Copy
#define Default_Constructor_VirtualDestructor_Move_Copy(Class)  \
public:															\
	Class() = default;											\
	virtual ~Class() = default;									\
	Class(const Class&) = default;								\
	Class& operator=(const Class&) = default;					\
	Class(Class&&) = default;									\
	Class& operator=(Class&&) = default;
#endif

#ifndef DECLARE_ENGINE_SUBSYSTEM
#define DECLARE_ENGINE_SUBSYSTEM(Class)         \
    friend class Pitaya::Engine::Engine;        \
public:											\
	bool Initialize();							\
	void Release();								\
private:                                        \
    Class() = default;                          \
    ~Class() { Release(); }                     \
    Class(const Class&) = delete;               \
    Class& operator=(const Class&) = delete;    \
    Class(Class&&) = delete;                    \
    Class& operator=(Class&&) = delete;         \
private:                                        \
bool isInitialized = false;						\
bool isReleased = false;
#endif

#ifndef DECLARE_INTERFACE
#define DECLARE_INTERFACE(Class)              \
public:                                       \
    Class() = default;                        \
    virtual ~Class() = default;               \
    Class(const Class&) = delete;             \
    Class& operator=(const Class&) = delete;  \
    Class(Class&&) = delete;                  \
    Class& operator=(Class&&) = delete;       
#endif

#ifndef DECLARE_SINGLETON_CLASS_R
//宏定义单例模板 请在.h文件类内使用该宏 参数为类名	 注意:请在.cpp文件实现Release函数
#define DECLARE_SINGLETON_CLASS_R(Class)							\
public:																\
	void Release();													\
private:															\
	Class() = default;												\
	~Class() { Release(); }											\
	Class(const Class&) = delete;									\
	Class(Class&&) = delete;										\
	Class& operator=(const Class&) = delete;						\
	Class& operator=(Class&&) = delete;								\
public:																\
	static Class Instance;											\
private:															\
	bool isReleased = false;											
#endif

#ifndef DECLARE_SINGLETON_CLASS_RB
//宏定义单例模板 请在.h文件类内使用该宏 参数为类名	 注意:请在.cpp文件实现Bootstrap和Release函数
#define DECLARE_SINGLETON_CLASS_RB(Class)							\
public:																\
	void Release();													\
private:															\
	void Bootstrap();												\
private:															\
	Class() { Bootstrap(); }										\
	~Class() { Release(); }											\
	Class(const Class&) = delete;									\
	Class(Class&&) = delete;										\
	Class& operator=(const Class&) = delete;						\
	Class& operator=(Class&&) = delete;								\
public:																\
	static Class Instance;											\
private:															\
	bool isBootstraped = false;										\
	bool isReleased = false;											
#endif

#ifndef DECLARE_SINGLETON_CLASS_RI
//宏定义单例模板 请在.h文件类内使用该宏 参数为类名	 注意:请在.cpp文件实现Initialize和Release函数
#define DECLARE_SINGLETON_CLASS_RI(Class)							\
public:																\
	bool Initialize();												\
	void Release();													\
private:															\
	Class() = default;												\
	~Class() { Release(); }											\
	Class(const Class&) = delete;									\
	Class(Class&&) = delete;										\
	Class& operator=(const Class&) = delete;						\
	Class& operator=(Class&&) = delete;								\
public:																\
	static Class Instance;											\
private:															\
	bool isInitialized = false;										\
	bool isReleased = false;											
#endif

#ifndef DECLARE_SINGLETON_CLASS_RBI
//宏定义单例模板 请在.h文件类内使用该宏 参数为类名	 注意:请在.cpp文件实现Bootstrap和Initialize和Release函数
#define DECLARE_SINGLETON_CLASS_RBI(Class)							\
public:																\
	bool Initialize();												\
	void Release();													\
private:															\
	void Bootstrap();												\
private:															\
	Class() { Bootstrap(); }										\
	~Class() { Release(); }											\
	Class(const Class&) = delete;									\
	Class(Class&&) = delete;										\
	Class& operator=(const Class&) = delete;						\
	Class& operator=(Class&&) = delete;								\
public:																\
	static Class Instance;											\
private:															\
	bool isBootstraped = false;										\
	bool isInitialized = false;										\
	bool isReleased = false;											
#endif

#ifndef IMPLEMENT_SINGLETON_CLASS
//宏实现单例模板 请在.cpp文件使用该宏 参数为命名空间名 类名 
#define IMPLEMENT_SINGLETON_CLASS(Namespace,Class)					\
Namespace::Class Namespace::Class::Instance;								
#endif
