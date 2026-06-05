# 游戏模组开发指南

## 项目概述

这是一个通用的游戏模组加载器框架，支持通过DLL注入的方式为游戏添加模组功能。该框架提供了完整的模组开发API，包括事件系统、渲染系统、Hook系统等核心功能。

### 主要特性

- **DLL注入机制** - 通过注入器将模组加载器注入到目标游戏进程
- **模块化架构** - 支持动态加载和卸载模组
- **标准化API** - 提供统一的ABI接口供模组调用
- **事件驱动** - 完整的事件订阅和分发系统
- **渲染集成** - 支持在游戏中绘制UI和图形
- **Hook系统** - 支持拦截和修改游戏函数调用

## 目录架构层级

```
ModLoader/
├── Injector/                    # DLL注入器
│   ├── CMakeLists.txt          # 注入器构建配置
│   └── main.cpp                # 注入器主程序
│
├── Loader/                      # 核心加载器DLL
│   ├── CMakeLists.txt          # 加载器构建配置
│   ├── dllmain.cpp             # DLL入口点
│   │
│   ├── Core/                   # 核心功能模块
│   │   ├── LoaderCore.h/cpp    # 加载器核心类
│   │   ├── Mod_Manager.h/cpp   # 模组管理器
│   │   ├── Event_Bus.h/cpp     # 事件总线
│   │   └── EventQueue.h/cpp    # 事件队列
│   │
│   ├── Hook/                   # Hook系统
│   │   ├── Hook_Manager.h/cpp  # Hook管理器
│   │   └── OpenGL_Hook.h/cpp   # OpenGL Hook实现
│   │
│   ├── Rendering/              # 渲染系统
│   │   ├── Renderer.h/cpp      # 渲染器
│   │   └── RenderQueue.h/cpp   # 渲染队列
│   │
│   ├── Systems/                # 游戏系统
│   │   └── LapSystem.h/cpp     # 圈数系统示例
│   │
│   └── Utils/                  # 工具类
│       └── Logger.h/cpp        # 日志系统
│
├── Mods/                        # 模组目录
│   ├── CMakeLists.txt          # 模组构建配置
│   └── TestMod/                # 示例模组
│       ├── CMakeLists.txt
│       └── main.cpp            # 模组主程序
│
├── Shared/                      # 共享代码
│   ├── mod_abi.h              # 模组ABI接口定义
│   ├── resources.rc           # 资源文件
│   └── third_party/           # 第三方库
│       └── ImGui/             # ImGui UI库
│
├── .cmake/                      # CMake构建缓存
├── .vscode/                     # VSCode配置
├── CMakeLists.txt              # 主构建配置
├── CMakeUserPresets.json       # CMake用户预设
├── conanfile.txt               # Conan包管理配置
└── 偏移.txt                     # 游戏内存偏移量记录
```

## 核心组件说明

### 1. 注入器 (Injector)

负责将模组加载器DLL注入到目标游戏进程中：

- **功能**：创建游戏进程、注入DLL、恢复进程运行
- **使用方法**：`Injector.exe <游戏路径> <DLL路径>`
- **关键API**：
  - `CreateProcessW` - 创建挂起的游戏进程
  - `VirtualAllocEx` - 在目标进程分配内存
  - `WriteProcessMemory` - 写入DLL路径
  - `CreateRemoteThread` - 创建远程线程调用LoadLibrary

### 2. 加载器核心 (LoaderCore)

模组加载器的核心控制器，协调各个子系统：

- **初始化流程**：
  1. 初始化日志系统
  2. 创建事件总线和队列
  3. 初始化渲染器
  4. 加载所有模组
  5. 安装Hook系统
  6. 初始化游戏系统

- **核心功能**：
  - 模组生命周期管理
  - 事件订阅和分发
  - 渲染命令处理
  - 日志记录

### 3. 模组管理器 (ModManager)

负责模组的加载、卸载和调用：

- **加载流程**：
  1. 扫描模组目录
  2. 加载每个DLL文件
  3. 调用`ModEntry`函数
  4. 验证ABI兼容性
  5. 调用模组初始化函数

- **运行时管理**：
  - 每帧调用模组的`on_tick`函数
  - 管理模组导出函数
  - 提供API给模组调用

### 4. Hook系统

拦截游戏函数调用，实现功能扩展：

- **HookManager**：管理所有Hook的创建、启用和禁用
- **OpenGL_Hook**：拦截OpenGL渲染函数，集成ImGui渲染
- **使用场景**：
  - 渲染拦截
  - 游戏函数修改
  - 数据监控

### 5. 渲染系统

提供模组UI绘制功能：

- **Renderer**：负责实际的渲染操作
- **RenderQueue**：管理渲染命令队列
- **支持的绘制类型**：
  - 文本绘制
  - 矩形绘制
  - 可扩展其他图形

### 6. 事件系统

实现模组间通信和游戏事件处理：

- **EventBus**：事件订阅和分发
- **EventQueue**：事件队列管理
- **内置事件**：
  - `MOD_EVENT_TICK` - 每帧触发
  - `MOD_EVENT_LAP_COMPLETE` - 圈数完成事件
  - 可扩展自定义事件

## 模组开发指南

### 模组结构

一个标准的模组应该包含以下元素：

```cpp
#include "mod_abi.h"

// 全局API指针
const static mod_loader_api* g_api = nullptr;

// 事件回调函数
void OnTick(uint32_t event_type, const void* event_data, 
             uint32_t event_size, void* user_data)
{
    // 每帧调用的逻辑
}

void OnCustomEvent(uint32_t event_type, const void* event_data, 
                    uint32_t event_size, void* user_data)
{
    // 自定义事件处理
}

// 初始化函数
void OnInit()
{
    // 订阅事件
    g_api->subscribe_event(MOD_EVENT_TICK, OnTick, nullptr);
    g_api->subscribe_event(YOUR_CUSTOM_EVENT, OnCustomEvent, nullptr);
    
    // 记录日志
    g_api->log(MOD_LOG_INFO, "模组初始化完成");
}

// 模组入口函数
extern "C" MODLOADER_EXPORT uint32_t MODLOADER_CALL 
ModEntry(mod_loader_api* api, mod_exports* out_exports)
{
    // 验证ABI版本
    if (api->abi_version != MOD_ABI_VERSION || 
        api->struct_size != sizeof(mod_loader_api))
    {
        return 1; // 版本不兼容
    }

    g_api = api;

    // 设置导出函数
    out_exports->abi_version = MOD_ABI_VERSION;
    out_exports->struct_size = sizeof(mod_exports);
    out_exports->on_init = &OnInit;
    out_exports->on_tick = &YourTickFunction; // 可选

    return 0; // 成功
}
```

### API使用示例

#### 1. 日志记录

```cpp
g_api->log(MOD_LOG_DEBUG, "调试信息");
g_api->log(MOD_LOG_INFO, "普通信息");
g_api->log(MOD_LOG_ERROR, "错误信息");
```

#### 2. 事件订阅

```cpp
void OnLapComplete(uint32_t event_type, const void* event_data, 
                   uint32_t event_size, void* user_data)
{
    const auto* evt = static_cast<const mod_lap_complete_event*>(event_data);
    // 处理圈数完成事件
}

// 在初始化时订阅
g_api->subscribe_event(MOD_EVENT_LAP_COMPLETE, OnLapComplete, nullptr);
```

#### 3. 文本绘制

```cpp
mod_render_text_command cmd{};
cmd.x = 100.0f; cmd.y = 100.0f;
cmd.r = 1.0f; cmd.g = 1.0f; cmd.b = 1.0f; cmd.a = 1.0f;
std::snprintf(cmd.text, sizeof(cmd.text), "FPS: %d", fps);
cmd.text[sizeof(cmd.text) - 1] = '\0';

g_api->draw_text(&cmd);
```

#### 4. 矩形绘制

```cpp
mod_render_rect_command cmd{};
cmd.x = 50.0f; cmd.y = 50.0f;
cmd.w = 200.0f; cmd.h = 100.0f;
cmd.thickness = 2.0f;
cmd.r = 1.0f; cmd.g = 0.0f; cmd.b = 0.0f; cmd.a = 0.8f;

g_api->draw_rect(&cmd);
```

### 模组编译

模组需要编译为DLL文件，并导出`ModEntry`函数：

```cmake
cmake_minimum_required(VERSION 3.20)
project(YourMod)

set(CMAKE_CXX_STANDARD 20)

add_library(YourMod SHARED main.cpp)
target_include_directories(YourMod PRIVATE 
    "${CMAKE_SOURCE_DIR}/../Shared"
)
```

## 开发环境设置

### 必要工具

- **Visual Studio 2022** - C++开发环境
- **CMake 3.20+** - 构建系统
- **Conan** - 包管理器（可选）
- **Git** - 版本控制

### 构建步骤

1. **克隆项目**
   ```bash
   git clone <repository-url>
   cd ModLoader
   ```

2. **配置构建**
   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   ```

3. **编译项目**
   ```bash
   cmake --build build --config Release
   ```

4. **使用注入器**
   ```bash
   .\build\Injector\Release\Injector.exe <游戏路径> <DLL路径>
   ```

## API参考

### mod_loader_api 结构

模组加载器提供给模组的API接口：

```cpp
struct mod_loader_api {
    uint32_t abi_version;              // ABI版本号
    uint32_t struct_size;              // 结构体大小
    
    // 日志函数
    void (*log)(uint32_t level, const std::string& msg);
    
    // 事件订阅
    void (*subscribe_event)(uint32_t event_type, 
                           mod_event_callback_fn callback, 
                           void* user_data);
    
    // 渲染函数
    bool (*draw_text)(mod_render_text_command* cmd);
    bool (*draw_rect)(mod_render_rect_command* cmd);
};
```

### mod_exports 结构

模组需要导出的函数接口：

```cpp
struct mod_exports {
    uint32_t abi_version;              // ABI版本号
    uint32_t struct_size;              // 结构体大小
    
    // 模组生命周期函数
    void (*on_init)();                 // 初始化函数
    void (*on_tick)();                 // 每帧调用函数
};
```

### 事件类型

```cpp
enum mod_event_type {
    MOD_EVENT_TICK = 1,               // 每帧事件
    MOD_EVENT_LAP_COMPLETE = 2        // 圈数完成事件
};
```

### 日志级别

```cpp
enum mod_log_level {
    MOD_LOG_DEBUG = 0,                // 调试信息
    MOD_LOG_INFO = 1,                 // 普通信息
    MOD_LOG_ERROR = 2                 // 错误信息
};
```

## 高级主题

### 自定义事件系统

模组可以创建和触发自定义事件：

```cpp
// 定义自定义事件数据
struct custom_event_data {
    int player_id;
    float health;
};

// 触发自定义事件
custom_event_data data{1, 100.0f};
g_api->enqueue_event(CUSTOM_EVENT_ID, &data, sizeof(data));
```

### Hook游戏函数

通过Hook系统可以拦截和修改游戏函数：

```cpp
// 在加载器中创建Hook
hook_manager->CreateHook(target_function, hook_function, &original_function);
hook_manager->EnableHook(target_function);
```

### 内存访问

直接访问游戏内存（需要知道正确的偏移量）：

```cpp
// 读取游戏内存
uintptr_t base_address = GetModuleHandle(L"Game.exe");
uintptr_t player_ptr = *(uintptr_t*)(base_address + PLAYER_OFFSET);
float health = *(float*)(player_ptr + HEALTH_OFFSET);
```

## 调试技巧

### 1. 日志调试

```cpp
// 在关键位置添加日志
g_api->log(MOD_LOG_DEBUG, "当前坐标: X=%.2f Y=%.2f", x, y);
```

### 2. 渲染调试信息

```cpp
// 在屏幕上显示调试信息
mod_render_text_command cmd{};
cmd.x = 10.0f; cmd.y = 10.0f;
cmd.r = 0.0f; cmd.g = 1.0f; cmd.b = 0.0f; cmd.a = 1.0f;
std::snprintf(cmd.text, sizeof(cmd.text), "Debug: %s", debug_info);
g_api->draw_text(&cmd);
```

### 3. 附加调试器

1. 启动游戏和注入器
2. 在Visual Studio中附加到游戏进程
3. 设置断点进行调试

## 常见问题

### Q: 模组加载失败怎么办？

A: 检查以下几点：
- 确认模组DLL在正确的目录
- 验证ABI版本是否匹配
- 查看日志文件了解详细错误信息
- 确认模组导出了正确的函数

### Q: 如何找到游戏的内存偏移量？

A: 使用工具如：
- Cheat Engine
- x64dbg
- IDA Pro
- ReClass.NET

### Q: 渲染内容不显示怎么办？

A: 检查：
- OpenGL Hook是否正确安装
- 渲染命令是否正确入队
- 坐标是否在屏幕范围内
- 颜色alpha值是否正确

### Q: 模组之间如何通信？

A: 通过事件系统：
```cpp
// 模组A发送事件
g_api->enqueue_event(MY_CUSTOM_EVENT, &data, sizeof(data));

// 模组B接收事件
g_api->subscribe_event(MY_CUSTOM_EVENT, OnMyEvent, nullptr);
```

## 最佳实践

1. **错误处理**：始终检查API调用的返回值
2. **资源管理**：及时释放分配的资源
3. **性能优化**：避免在每帧中进行重计算
4. **兼容性**：使用ABI版本检查确保兼容性
5. **日志记录**：合理使用日志级别记录信息
6. **线程安全**：注意多线程环境下的数据访问

## 扩展阅读

- [CMake官方文档](https://cmake.org/documentation/)
- [ImGui官方文档](https://github.com/ocornut/imgui)
- [Windows API文档](https://docs.microsoft.com/en-us/windows/win32/api/)
- [游戏逆向工程基础](https://guidedhacking.com/)

## 许可证

请根据项目实际情况添加许可证信息。

## 贡献指南

欢迎提交Issue和Pull Request来改进这个项目。

---

**注意**：本框架仅用于学习和研究目的，请遵守相关游戏的使用条款和法律法规。