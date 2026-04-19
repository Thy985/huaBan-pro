@echo off

REM 构建脚本 - 用于在终端中构建项目

setlocal

REM 检查是否安装了 CMake
cmake --version >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: CMake is not installed. Please download and install CMake from https://cmake.org/download/
    exit /b 1
)

REM 创建构建目录
if not exist "build" mkdir "build"

REM 进入构建目录
cd "build"

REM 配置项目
echo Configuring project...
cmake ..
if %errorlevel% neq 0 (
    echo Error: CMake configuration failed
    exit /b 1
)

REM 构建项目
echo Building project...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo Error: Build failed
    exit /b 1
)

REM 成功完成
echo Build completed successfully!
echo The executable is located in the "bin" directory.
echo You can run it by executing: bin\huaBan-pro.exe

endlocal
