#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
构建脚本 - 用于自动安装依赖、构建项目并运行应用

此脚本将自动：
1. 检查系统环境和Python版本
2. 下载并安装 CMake（如果需要）
3. 下载并安装 EasyX 图形库（如果需要）
4. 检查并安装 C++ 编译器（如果需要）
5. 构建项目
6. 运行应用
"""

import os
import sys
import shutil
import subprocess
import urllib.request
import zipfile
import tempfile
import ctypes
import platform
import logging

# 配置日志
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('build.log'),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)

# 全局变量
PROJECT_NAME = "huaBan-pro"
EASYX_URL = "https://easyx.cn/download/EasyX_20240913(beta).exe"
CMAKE_URL = "https://github.com/Kitware/CMake/releases/download/v3.30.3/cmake-3.30.3-windows-x86_64.zip"

# 检查Python版本
def check_python_version():
    logger.info("检查Python版本...")
    if sys.version_info < (3, 6):
        logger.error("Python版本过低，需要Python 3.6或更高版本")
        return False
    logger.info(f"Python版本: {sys.version}")
    return True

# 检查是否以管理员权限运行
def is_admin():
    try:
        return ctypes.windll.shell32.IsUserAnAdmin()
    except:
        return False

# 以管理员权限重新运行脚本
def run_as_admin():
    logger.info("需要管理员权限来安装依赖")
    logger.info("正在以管理员权限重新运行...")
    ctypes.windll.shell32.ShellExecuteW(
        None, "runas", sys.executable, " ".join(sys.argv), None, 1
    )

# 下载文件
def download_file(url, save_path):
    logger.info(f"正在下载: {url}")
    try:
        urllib.request.urlretrieve(url, save_path)
        logger.info(f"下载完成: {save_path}")
    except Exception as e:
        logger.error(f"下载失败: {str(e)}")
        raise

# 解压文件
def extract_zip(zip_path, extract_dir):
    logger.info(f"正在解压: {zip_path}")
    try:
        with zipfile.ZipFile(zip_path, 'r') as zip_ref:
            zip_ref.extractall(extract_dir)
        logger.info(f"解压完成: {extract_dir}")
    except Exception as e:
        logger.error(f"解压失败: {str(e)}")
        raise

# 检查命令是否存在
def command_exists(cmd):
    try:
        subprocess.run([cmd, "--version"], capture_output=True, check=True)
        return True
    except (subprocess.CalledProcessError, FileNotFoundError):
        return False

# 检查 EasyX 是否安装
def check_easyx():
    easyx_paths = [
        "C:\\Program Files (x86)\\EasyX",
        "C:\\Program Files\\EasyX"
    ]
    for path in easyx_paths:
        if os.path.exists(os.path.join(path, "include", "graphics.h")):
            return True, path
    return False, None

# 安装 EasyX
def install_easyx():
    logger.info("正在安装 EasyX 图形库...")
    temp_dir = tempfile.gettempdir()
    easyx_exe = os.path.join(temp_dir, "EasyX.exe")
    
    try:
        download_file(EASYX_URL, easyx_exe)
        
        logger.info("运行 EasyX 安装程序...")
        subprocess.run([easyx_exe], check=True)
        
        # 清理临时文件
        if os.path.exists(easyx_exe):
            os.remove(easyx_exe)
        
        logger.info("EasyX 安装完成")
    except Exception as e:
        logger.error(f"EasyX 安装失败: {str(e)}")
        raise

# 安装 CMake
def install_cmake():
    logger.info("正在安装 CMake...")
    temp_dir = tempfile.gettempdir()
    cmake_zip = os.path.join(temp_dir, "cmake.zip")
    cmake_extract_dir = os.path.join(temp_dir, "cmake")
    
    try:
        download_file(CMAKE_URL, cmake_zip)
        extract_zip(cmake_zip, cmake_extract_dir)
        
        # 找到 CMake 可执行文件路径
        cmake_bin_dir = None
        for root, dirs, files in os.walk(cmake_extract_dir):
            if "cmake.exe" in files:
                cmake_bin_dir = root
                break
        
        if not cmake_bin_dir:
            logger.error("错误: 找不到 CMake 可执行文件")
            return False
        
        # 将 CMake 添加到环境变量
        current_path = os.environ.get("PATH", "")
        if cmake_bin_dir not in current_path:
            os.environ["PATH"] = f"{cmake_bin_dir};{current_path}"
            # 永久添加到环境变量
            try:
                subprocess.run([
                    "setx", "PATH", f"{cmake_bin_dir};{current_path}"
                ], check=True)
            except subprocess.CalledProcessError:
                logger.warning("警告: 无法永久添加 CMake 到环境变量，请手动添加")
        
        # 清理临时文件
        if os.path.exists(cmake_zip):
            os.remove(cmake_zip)
        if os.path.exists(cmake_extract_dir):
            shutil.rmtree(cmake_extract_dir)
        
        logger.info("CMake 安装完成")
        return True
    except Exception as e:
        logger.error(f"CMake 安装失败: {str(e)}")
        return False

# 检查并安装编译器
def check_compiler():
    logger.info("检查 C++ 编译器...")
    
    # 检查 Visual C++ 编译器
    if command_exists("cl"):
        logger.info("找到 Visual C++ 编译器")
        return True
    
    # 检查 MinGW
    if command_exists("g++"):
        logger.info("找到 MinGW g++ 编译器")
        return True
    
    logger.error("未找到 C++ 编译器")
    logger.error("请安装以下之一:")
    logger.error("1. Visual Studio Build Tools: https://visualstudio.microsoft.com/visual-cpp-build-tools/")
    logger.error("2. MinGW-w64: https://www.mingw-w64.org/downloads/")
    return False

# 构建项目
def build_project():
    logger.info("开始构建项目...")
    
    # 创建构建目录
    build_dir = "build"
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)
    
    # 进入构建目录
    original_dir = os.getcwd()
    os.chdir(build_dir)
    
    try:
        # 配置项目
        logger.info("配置项目...")
        result = subprocess.run(["cmake", ".."], capture_output=True, text=True)
        if result.returncode != 0:
            logger.error(f"配置失败: {result.stderr}")
            return False
        
        # 构建项目
        logger.info("构建项目...")
        result = subprocess.run(["cmake", "--build", ".", "--config", "Release"], capture_output=True, text=True)
        if result.returncode != 0:
            logger.error(f"构建失败: {result.stderr}")
            return False
        
        logger.info("构建成功!")
        return True
    finally:
        # 回到原始目录
        os.chdir(original_dir)

# 运行应用
def run_application():
    app_path = os.path.join("build", "bin", f"{PROJECT_NAME}.exe")
    if os.path.exists(app_path):
        logger.info(f"运行应用: {app_path}")
        subprocess.run([app_path])
    else:
        logger.error(f"错误: 找不到可执行文件: {app_path}")

# 主函数
def main():
    logger.info(f"=== {PROJECT_NAME} 自动构建脚本 ===")
    logger.info("正在检查系统环境...")
    
    # 检查Python版本
    if not check_python_version():
        return 1
    
    # 检查是否为 Windows 系统
    if platform.system() != "Windows":
        logger.error("错误: 此脚本仅支持 Windows 系统")
        return 1
    
    # 检查管理员权限
    if not is_admin():
        run_as_admin()
        return 0
    
    try:
        # 检查并安装 EasyX
        easyx_installed, _ = check_easyx()
        if not easyx_installed:
            install_easyx()
        else:
            logger.info("EasyX 已安装")
        
        # 检查并安装 CMake
        if not command_exists("cmake"):
            if not install_cmake():
                return 1
        else:
            logger.info("CMake 已安装")
        
        # 检查编译器
        if not check_compiler():
            return 1
        
        # 构建项目
        if not build_project():
            return 1
        
        # 运行应用
        run_application()
        
        logger.info("构建脚本执行完成!")
        return 0
    except Exception as e:
        logger.error(f"构建过程中发生错误: {str(e)}")
        return 1

if __name__ == "__main__":
    sys.exit(main())
