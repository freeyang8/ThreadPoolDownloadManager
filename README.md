# 📥 ThreadPoolDownloadManager - 高并发多线程下载器

> **基于 Qt 6 与 libcurl 构建的轻量级下载管理器**

本项目是一个使用 C++17、Qt 6 Widgets 和 libcurl 库开发的桌面端下载工具。它利用线程池技术管理并发下载任务，支持进度实时更新、任务取消以及断点续传逻辑（需服务器支持），旨在提供高效稳定的文件获取体验。

---

## 🛠️ 技术栈与架构

本项目采用现代 C++ 标准开发，核心依赖如下：

- **GUI 框架**: [Qt 6.5+](https://www.qt.io/) (Widgets)
- **网络请求**: [libcurl](https://curl.se/libcurl/) (支持 HTTP/HTTPS)
- **构建系统**: CMake 3.19
- **并发模型**: 线程池 (ThreadPool) + 任务队列
- **标准库**: C++17 (std::thread, std::atomic, std::mutex)

### 核心设计模式
- **生产者-消费者模式**：主线程（GUI）作为生产者提交任务，工作线程作为消费者处理下载。
- **观察者模式**：通过 Qt 信号槽机制（Signals & Slots）实现 UI 与业务逻辑的解耦。

---

## 📂 项目文件结构

项目结构清晰，模块化程度高，主要文件如下：

| 文件名 | 说明 |
| :--- | :--- |
| `CMakeLists.txt` | CMake 构建配置文件，定义了 Qt 和 libcurl 的依赖 |
| `main.cpp` | 程序入口，初始化 QApplication |
| `mainwindow.h/cpp` | 主窗口逻辑，包含 UI 布局、按钮事件及定时器轮询 |
| `DownloadManager.h/cpp` | **核心下载引擎**，负责任务调度、线程池管理及 libcurl 调用 |
| `DownloadTask.h` | 任务数据结构定义 (ID, URL, 状态, 进度) |
| `DownloadStatus.h` | 任务状态枚举 (等待、下载中、完成、取消) |

---

## 🚀 核心功能

### 1. 线程池管理
- 支持配置最大并发数（默认 4 个线程）。
- 使用 `std::condition_variable` 实现线程休眠与唤醒，避免资源空耗。
- 安全的线程销毁机制，确保程序退出时无内存泄漏。

### 2. 智能下载逻辑
- **文件名解析**：自动解析 HTTP 响应头中的 `Content-Disposition` 或 URL 后缀获取文件名。
- **进度回调**：利用 libcurl 的 `CURLOPT_XFERINFOFUNCTION` 实时计算下载百分比。
- **异常处理**：支持 SSL 证书跳过（测试环境）及网络错误捕获。

### 3. UI 交互
- **实时轮询**：通过 `QTimer` 每 100ms 查询一次任务状态并更新 `QProgressBar`。
- **任务控制**：支持单个任务取消及全部取消。
- **状态反馈**：下载完成后按钮恢复可用状态，并显示最终进度。

---

## 📝 编译与运行

### 环境准备
1. **Windows**: Visual Studio 2019/2022 + Qt 6.5 SDK + vcpkg (用于管理 libcurl)
2. **Linux**: g++ 9+ / clang++ 10+ + Qt 6 开发库 + libcurl4-openssl-dev

### 构建步骤
1. 克隆仓库：
   ```bash
   git clone https://github.com/yourname/ThreadPoolDownloadManager.git
   cd ThreadPoolDownloadManager
