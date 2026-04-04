<img src="./resource/icon/default.png" alt="Pitaya" style="zoom:8%;" />

# 🐉 Pitaya Engine 火龙果引擎

[![Language](https://img.shields.io/badge/Language-C%2B%2B20-blue.svg)](https://en.cppreference.com/w/cpp/20) [![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)]() [![Compiler](https://img.shields.io/badge/Compiler-MSVC%20v143%20(VS2022)-purple.svg)]() [![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

**Pitaya Engine** 是一个基于 **C++20** 开发的分层式游戏引擎。本项目旨在构建一个高内聚、低耦合的底层框架，通过清晰的模型抽象（如 Bootstrap、Internal 系统与 API 层）来实现现代引擎的设计思路。

---

## 🏗️ 架构设计 (Architecture)

Pitaya 采用严格的分层架构，确保核心逻辑与底层实现解耦：

*   **Application**: 宿主应用层封装，提供程序的物理启动入口`main` 函数。其职责极其轻量，仅负责拉起引擎的主体循环（Execute）。
*   **Engine**: 核心引擎层，作为引擎的大脑，专职负责生命周期总控（Bootstrap / Runtime）、主体循环、对底层各大模块（Modules）的集中注册与调度。
*   **Modules**: 核心子系统层，提供互相解耦的即插即用运行时模块，包含 时间（Time）、渲染管理（Renderer）、物理（Physics）、窗口（Window）、输入系统（Input）等。
*   **Tools**: 数据与工具层，处理资产流水线与引擎周边工具，包含资源导入转换（Importer）、序列化/反序列化（Serialization）及为未来编辑器提供核心支撑。
*   **Core**: 基础核心层，提供单例模板、对象池、全局唯一标识符（UID/GUID）及底层工具。
*   **Thirdparty**: 外部依赖库，包含渲染、物理、数学以及序列化工具。

---

## ✨ 核心特性 (Key Features)

- 🛠️ **模块化引导 (Bootstrap System):** 统一的引擎启动与关闭流程，确保子系统按依赖顺序初始化。
- 🧵 **异步日志系统 (Async Logging loop):** 基于线程接管的日志记录机制，确保磁盘 I/O 不影响主逻辑帧率。
- 📦 **高效内存管理:** 内置对象池（Object Pool）减少运行时内存碎片的产生。
- 🎮 **组件化开发 (Runtime Component):** 提供 GameObject 与 Component 抽象，方便扩展业务逻辑。
- 🧱 **解耦抽象接口:** 渲染（OpenGL）与物理（Bullet）后端高度抽象，易于替换与扩展。
- 🚀 **渲染主线程分离 (Thread Separation):** 实现渲染线程与逻辑线程解耦，通过指令队列实现异步渲染。
- ⚡ **全局事件系统 (Event System):** 基于消息总线的解耦通信机制，简化模块间交互。
- ⌨️ **输入系统 (Input System):** 支持状态轮询与事件回调，抽象了键盘、鼠标的底层输入。
- 🕒 **高精度时间系统 (Time System):** 毫秒级 deltaTime 计算与全局时钟管理。
- 📥 **异步资源管理系统(Async Asset System):** 多线程分层加载 / 释放资源，主线程提交请求、任务线程加载 CPU 数据、渲染线程创建 GPU 资源，结合引用计数保障生命周期安全。
- 🚦 **资源状态原子控制(Atomic State Flags):** CPU/GPU 双端原子标志位管控资源全生命周期，规避多线程竞态与访问异常。
- 🧩 **线程池任务系统(Thread Pool):** 轻量级线程池实现任务异步调度与优先级管理，最大化多核 CPU 利用率，减少线程创建 / 销毁开销。
- 🔒 **线程安全容器(Thread Safe Container):** 封装线程安全的队列、哈希表、双向表等容器，简化多线程场景下的并发编程。
- 🪟 **线程封装 (Thread Wrapper):** 对象化封装 Windows 原生线程操作，提供类型安全接口，兼顾易用性与原生能力。
- 🎨 **渲染命令系统 (Render Command System):** 渲染命令采用 POD 类型实现高效字节操作，通过 SortKey 排序保证渲染执行顺序与画面效果正确。
- 🏭 **自动合批系统 (Auto Batching System):** 基于渲染队列排序结果，动态聚合相同材质与网格的渲染指令，利用 **GPU Instancing** 技术大幅缩减 Draw Call，显著提升 CPU 提交效率与 GPU 吞吐量。
- 🔌 **动态接口注入系统 (Interface Injection System):** 构建以 **Context** 为核心的全局上下文环境，配合 **FuncTable**（函数指针映射表）实现逻辑单元的动态注入。子模块间不产生代码层面的强链接依赖，仅通过接口契约在运行时进行功能寻址与调用，显著提升了系统的灵活性与可扩展性。

---

## 🛠️ 技术栈 (Tech Stack)

| 类别              | 库文件                                                       | 用途                                                |
| :---------------- | :----------------------------------------------------------- | :-------------------------------------------------- |
| **Graphics**      | [GLEW](http://glew.sourceforge.net/) / [OpenGL](https://www.opengl.org/) | 图形驱动与硬件加速                                  |
| **Window**        | [GLFW](https://www.glfw.org/)                                | 窗口创建与输入处理                                  |
| **Physics**       | [Bullet Physics](https://pybullet.org/)                      | 3D 碰撞检测与动力学模拟                             |
| **Math**          | [GLM](https://github.com/g-truc/glm)                         | 针对 OpenGL 设计的数学库                            |
| **ECS**           | [EnTT](https://github.com/skypjack/entt)                     | 高性能 ECS 框架支持                                 |
| **Serialization** | [YAML-cpp](https://github.com/jbeder/yaml-cpp)               | 配置文件与场景序列化                                |
| **Scripting**     | [Mono](https://www.mono-project.com/)                        | 嵌入式 C# 脚本运行时，实现游戏逻辑热更新与 API 绑定 |

---

## 🚀 快速开始 (Getting Started)

### 环境依赖
*   **操作系统:** Windows 10/11
*   **IDE:** Visual Studio 2022 (至少支持 MSVC v143)
*   **SDK:** Windows 10 SDK (10.0.xxxxx.x)

### 编译运行
1.  **克隆仓库:**
    
    ```bash
    git clone --recursive https://github.com/matrix-pitaya/Pitaya-Engine.git
    ```
    
    下载GitHub仓库Release中的 `Pitaya-Engine-res-third-v0.1.0.zip` 压缩包并解压缩到`Pitaya.sln`目录。
    
2.  **打开工程:**
    使用 Visual Studio 2022 打开根目录下的 `Pitaya.sln`。
    
3.  **配置环境:**
    确保构建配置设为`Release` (x64)。
    
4.  **运行项目:**
    将 `Application` 设为启动项目并按 `F5` 运行。

---

## 📂 目录导航 (Project Map)

```text
Pitaya/
├── Application/     			# 程序入口
├── Engine/          			# 引擎层
├── Modules/					# 引擎子模块层
├── Tools/						# 工具层
├── Core/            			# 核心层
├── exe/             			# 可执行程序输出目录
├── intermediate/    			# 中间产物，如编译文件、临时文件等
├── library/         			# 项目库文件
├── resource/        			# 资源文件
├── thirdparty/      			# 第三方依赖与库文件
├── README.md        			# 项目说明文档
└── pitaya.sln   	 			# VS Solution 解决方案文件
```

---

## 📅 路线图 (Roadmap)

- [x] **核心架构**：实现主逻辑线程与渲染线程的物理分离。
- [x] **通信机制**：全局解耦事件总线与指令队列系统。
- [x] **日志系统**：非阻塞式异步双缓冲日志系统。
- [x] **基础框架**：基于 Bootstrap 的子系统生命周期管理。
- [x] **资源管理**：基于 GUID 的异步资源加载与内存池管理。
- [x] **线程池与安全容器**：完成轻量级线程池与核心线程安全容器的封装。
- [ ] **脚本系统**：集成 Mono 运行时，完成 C# 脚本 API 绑定。
- [ ] **编辑器界面**：集成 ImGui 并构建实时场景树与属性查看器。
- [ ] **渲染增强**：引入延迟渲染管线 (Deferred Rendering) 与 PBR 环境光。

---

## 📄 许可证 (License)

本项目基于 **[MIT License](LICENSE)** 协议开源。你可以自由地在商业或非商业项目中引用、修改。

