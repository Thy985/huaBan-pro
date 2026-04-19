# 构建指南 - 不依赖 Visual Studio

## 概述

本指南提供了一种不依赖 Visual Studio 的构建方法，使用 CMake 和命令行工具来编译和构建项目。

## 前置条件

在开始之前，确保你已经安装了以下软件：

1. **CMake** (3.10 或更高版本)
   - 下载地址：https://cmake.org/download/
   - 选择适合你系统的版本并安装

2. **EasyX 图形库**
   - 下载地址：https://easyx.cn/
   - 安装到默认路径：`C:\Program Files (x86)\EasyX`

3. **C++ 编译器**
   - Windows 10/11 系统：
     - 方法 1：安装 Visual Studio Build Tools
       - 下载地址：https://visualstudio.microsoft.com/visual-cpp-build-tools/
       - 安装时选择 "C++ 构建工具"
     - 方法 2：安装 MinGW-w64
       - 下载地址：https://www.mingw-w64.org/downloads/
       - 选择 x86_64 架构版本

## 构建步骤

### 方法 1：使用批处理脚本（推荐）

1. **下载项目**
   - 从 GitHub 克隆或下载项目到本地目录

2. **运行构建脚本**
   - 打开命令提示符（CMD）
   - 导航到项目根目录
   - 运行 `build.bat` 脚本：
     ```cmd
     build.bat
     ```
   - 脚本会自动执行以下操作：
     - 检查 CMake 是否安装
     - 创建构建目录
     - 配置项目
     - 构建项目

3. **运行应用**
   - 构建完成后，可执行文件将位于 `build\bin` 目录中
   - 运行应用：
     ```cmd
     build\bin\huaBan-pro.exe
     ```

### 方法 2：手动使用 CMake 命令

1. **下载项目**
   - 从 GitHub 克隆或下载项目到本地目录

2. **创建构建目录**
   - 打开命令提示符（CMD）
   - 导航到项目根目录
   - 创建构建目录：
     ```cmd
     mkdir build
     cd build
     ```

3. **配置项目**
   - 运行 CMake 配置命令：
     ```cmd
     cmake ..
     ```
   - 这将生成构建文件

4. **构建项目**
   - 运行构建命令：
     ```cmd
     cmake --build . --config Release
     ```

5. **运行应用**
   - 构建完成后，可执行文件将位于 `bin` 目录中
   - 运行应用：
     ```cmd
     bin\huaBan-pro.exe
     ```

## 构建故障排除

### 1. CMake 未找到
- 错误信息：`Error: CMake is not installed`
- 解决方法：
  - 下载并安装 CMake
  - 确保 CMake 已添加到系统 PATH 环境变量

### 2. EasyX 库未找到
- 错误信息：`EasyX library not found`
- 解决方法：
  - 下载并安装 EasyX 图形库
  - 确保安装到默认路径：`C:\Program Files (x86)\EasyX`

### 3. 编译器未找到
- 错误信息：`No CMAKE_CXX_COMPILER could be found`
- 解决方法：
  - 安装 Visual Studio Build Tools 或 MinGW-w64
  - 确保编译器已添加到系统 PATH 环境变量

### 4. 构建失败
- 错误信息：`Error: Build failed`
- 解决方法：
  - 检查错误信息，查看具体失败原因
  - 确保所有依赖项都已正确安装
  - 尝试清理构建目录并重新构建：
    ```cmd
    rd /s /q build
    build.bat
    ```

## 项目结构

- `CMakeLists.txt` - CMake 构建配置文件
- `build.bat` - 批处理构建脚本
- `src/` - 源代码目录（如果项目结构更改）
- `build/` - 构建输出目录（自动创建）
- `build/bin/` - 可执行文件输出目录

## 注意事项

1. **Windows 专用**：此项目使用 EasyX 图形库，仅支持 Windows 系统
2. **EasyX 版本**：确保安装了最新版本的 EasyX 库
3. **编译器兼容性**：推荐使用 Visual C++ 编译器或 MinGW-w64
4. **路径空格**：项目路径最好不要包含空格，以避免构建问题

## 后续步骤

构建完成后，你可以：

1. 运行应用并测试所有功能
2. 修改源代码并重新构建
3. 添加新功能或改进现有功能

---

通过这种方法，你可以完全不依赖 Visual Studio，使用命令行工具和 CMake 来构建和运行项目，降低了开发和使用的门槛。