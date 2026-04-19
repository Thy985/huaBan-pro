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
# 注意：EasyX 官方下载链接可能会变化，请访问官网获取最新版本
EASYX_URL = "https://easyx.cn/download/EasyX_20241018.exe"  # 更新为最新版本
EASYX_WEBSITE = "https://easyx.cn/"
CMAKE_URL = "https://github.com/Kitware/CMake/releases/download/v3.30.3/cmake-3.30.3-windows-x86_64.zip"
CMAKE_WEBSITE = "https://cmake.org/download/"
MSVC_WEBSITE = "https://visualstudio.microsoft.com/visual-cpp-build-tools/"
MINGW_WEBSITE = "https://www.mingw-w64.org/downloads/"

# 安装模式配置
INSTALL_MODES = {
    "auto": "自动安装所有依赖（推荐）",
    "manual": "手动安装（跳过自动安装，仅检查）",
    "winget": "使用 winget 安装编译器",
}

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
        logger.info(f"尝试从官方网站下载: {EASYX_URL}")
        download_file(EASYX_URL, easyx_exe)
        
        logger.info("运行 EasyX 安装程序...")
        subprocess.run([easyx_exe], check=True)
        
        # 清理临时文件
        if os.path.exists(easyx_exe):
            os.remove(easyx_exe)
        
        logger.info("EasyX 安装完成")
    except Exception as e:
        logger.error(f"EasyX 自动安装失败: {str(e)}")
        logger.error("=====================================")
        logger.error("请手动安装 EasyX:")
        logger.error(f"1. 访问 EasyX 官网: {EASYX_WEBSITE}")
        logger.error("2. 下载最新版本的 EasyX 安装包")
        logger.error("3. 双击安装到默认路径")
        logger.error("4. 确认路径 C:\\Program Files (x86)\\EasyX\\include\\graphics.h 存在")
        logger.error("=====================================")
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
    logger.error("=====================================")
    logger.error("请安装以下编译器之一:")
    logger.error("方案 A（推荐）: Visual Studio Build Tools")
    logger.error(f"   1. 访问: {MSVC_WEBSITE}")
    logger.error("   2. 下载并运行安装程序")
    logger.error("   3. 勾选 '使用 C++ 的桌面开发'")
    logger.error("   4. 完成安装")
    logger.error("")
    logger.error("方案 B: MinGW-w64")
    logger.error(f"   1. 访问: {MINGW_WEBSITE}")
    logger.error("   2. 推荐使用 MSYS2 安装")
    logger.error("   3. 安装后运行: pacman -S mingw-w64-ucrt-x86_64-gcc")
    logger.error("   4. 将 MinGW/bin 添加到系统 PATH")
    logger.error("=====================================")
    return False



# 运行应用
def run_application():
    app_path = os.path.join("build", "bin", f"{PROJECT_NAME}.exe")
    if os.path.exists(app_path):
        logger.info(f"运行应用: {app_path}")
        subprocess.run([app_path])
    else:
        logger.error(f"错误: 找不到可执行文件: {app_path}")

# 检查 winget 是否可用
def check_winget():
    """检查 Windows 包管理器 winget 是否可用"""
    try:
        result = subprocess.run(["winget", "--version"], capture_output=True, text=True, timeout=10)
        if result.returncode == 0:
            logger.info(f"找到 winget: {result.stdout.strip()}")
            return True
        return False
    except Exception:
        return False

# 使用 winget 安装 CMake
def install_cmake_winget():
    """使用 winget 安装 CMake"""
    logger.info("尝试使用 winget 安装 CMake...")
    try:
        result = subprocess.run(
            ["winget", "install", "--id", "Kitware.CMake", "--accept-package-agreements", "--accept-source-agreements"],
            check=True,
            timeout=300
        )
        if result.returncode == 0:
            logger.info("CMake 安装成功！")
            # 刷新环境变量
            os.environ["PATH"] = os.environ["PATH"] + ";C:\\Program Files\\CMake\\bin"
            return True
        return False
    except Exception as e:
        logger.error(f"使用 winget 安装 CMake 失败: {str(e)}")
        return False

# 使用 winget 安装 Visual Studio Build Tools
def install_msvc_winget():
    """使用 winget 安装 Visual Studio Build Tools"""
    logger.info("尝试使用 winget 安装 Visual Studio Build Tools...")
    try:
        # 安装 Visual Studio Build Tools 并包含 C++ 工作负载
        result = subprocess.run(
            [
                "winget", "install", "--id", "Microsoft.VisualStudio.2022.BuildTools",
                "--override", "--wait --quiet --add Microsoft.VisualStudio.Workload.VCTools;includeRecommended",
                "--accept-package-agreements", "--accept-source-agreements"
            ],
            check=True,
            timeout=600  # 给足够的时间安装
        )
        if result.returncode == 0:
            logger.info("Visual Studio Build Tools 安装成功！")
            return True
        return False
    except Exception as e:
        logger.error(f"使用 winget 安装 MSVC 失败: {str(e)}")
        return False

# 使用 winget 安装 MinGW
def install_mingw_winget():
    """使用 winget 安装 MinGW-w64"""
    logger.info("尝试使用 winget 安装 MinGW-w64...")
    try:
        # 使用 MSYS2 安装 MinGW
        result = subprocess.run(
            ["winget", "install", "--id", "MSYS2.MSYS2", "--accept-package-agreements", "--accept-source-agreements"],
            check=True,
            timeout=300
        )
        if result.returncode == 0:
            logger.info("MinGW-w64 安装成功！")
            # 添加 MinGW 到 PATH
            os.environ["PATH"] = os.environ["PATH"] + ";C:\\msys64\\ucrt64\\bin"
            return True
        return False
    except Exception as e:
        logger.error(f"使用 winget 安装 MinGW 失败: {str(e)}")
        return False

# 命令行参数处理
def parse_args():
    """解析命令行参数"""
    import argparse
    parser = argparse.ArgumentParser(description='构建脚本 - 用于自动安装依赖、构建项目并运行应用')
    parser.add_argument('--install-mode', choices=list(INSTALL_MODES.keys()), default='auto', 
                       help=f'安装模式: {", ".join([f"{k} ({v})" for k, v in INSTALL_MODES.items()])}')
    parser.add_argument('--skip-easyx', action='store_true', help='跳过 EasyX 安装检查')
    parser.add_argument('--skip-cmake', action='store_true', help='跳过 CMake 安装检查')
    parser.add_argument('--build-only', action='store_true', help='只构建项目，不运行')
    parser.add_argument('--clean', action='store_true', help='构建前清理旧的构建文件')
    parser.add_argument('--verbose', action='store_true', help='显示详细输出')
    return parser.parse_args()

# 构建项目
def build_project(clean=False):
    logger.info("开始构建项目...")
    
    # 创建构建目录
    build_dir = "build"
    
    # 清理旧的构建文件
    if clean and os.path.exists(build_dir):
        logger.info("清理旧的构建文件...")
        shutil.rmtree(build_dir)
    
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

# 显示欢迎信息和安装指南
def show_welcome():
    """显示欢迎信息和快速安装指南"""
    logger.info("=" * 60)
    logger.info(f"{PROJECT_NAME} 构建系统")
    logger.info("=" * 60)
    logger.info("")
    logger.info("快速开始:")
    logger.info("1. 确保以管理员身份运行此脚本")
    logger.info("2. 脚本会尝试自动安装所需依赖")
    logger.info("3. 如果自动安装失败，请参考手动安装指南")
    logger.info("")
    logger.info("命令行参数:")
    logger.info("  --install-mode=auto    自动安装（默认）")
    logger.info("  --install-mode=winget  使用 winget 安装")
    logger.info("  --install-mode=manual  仅检查，不自动安装")
    logger.info("  --skip-easyx           跳过 EasyX 安装检查")
    logger.info("  --skip-cmake           跳过 CMake 安装检查")
    logger.info("  --build-only           只构建，不运行")
    logger.info("  --clean                清理旧构建文件")
    logger.info("  --verbose              显示详细输出")
    logger.info("")
    logger.info("=" * 60)

# 主函数
def main():
    # 解析命令行参数
    args = parse_args()
    
    # 如果启用详细模式，设置日志级别为DEBUG
    if args.verbose:
        logger.setLevel(logging.DEBUG)
    
    # 显示欢迎信息
    show_welcome()
    
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
    
    # 检查 winget 是否可用（仅在自动模式下）
    winget_available = False
    if args.install_mode in ["auto", "winget"]:
        winget_available = check_winget()
        if winget_available:
            logger.info("✅ winget 可用，将尝试使用它安装依赖")
        else:
            logger.warning("⚠️ winget 不可用，将尝试其他安装方法")
    
    try:
        # 检查并安装 EasyX
        if not args.skip_easyx:
            easyx_installed, _ = check_easyx()
            if not easyx_installed:
                if args.install_mode != "manual":
                    install_easyx()
            else:
                logger.info("✅ EasyX 已安装")
        else:
            logger.info("⏭️ 跳过 EasyX 安装检查")
        
        # 检查并安装 CMake
        if not args.skip_cmake:
            if not command_exists("cmake"):
                if args.install_mode == "manual":
                    logger.error("❌ 请手动安装 CMake")
                    return 1
                # 先尝试 winget，失败则用传统方法
                if winget_available and install_cmake_winget():
                    pass  # winget 安装成功
                elif not install_cmake():
                    return 1
            else:
                logger.info("✅ CMake 已安装")
        else:
            logger.info("⏭️ 跳过 CMake 安装检查")
        
        # 检查并安装编译器
        compiler_found = command_exists("cl") or command_exists("g++")
        if not compiler_found:
            if args.install_mode == "manual":
                if not check_compiler():
                    return 1
            else:
                # 尝试使用 winget 安装编译器
                if winget_available:
                    logger.info("尝试自动安装 C++ 编译器...")
                    # 优先尝试 MSVC
                    if not install_msvc_winget():
                        # MSVC 失败则尝试 MinGW
                        logger.warning("MSVC 安装失败，尝试 MinGW...")
                        install_mingw_winget()
                
                # 再次检查编译器
                if not check_compiler():
                    logger.error("❌ 编译器安装失败，请手动安装")
                    return 1
        
        # 构建项目
        logger.info("=" * 60)
        logger.info("开始构建项目...")
        logger.info("=" * 60)
        if not build_project(clean=args.clean):
            return 1
        
        # 运行应用
        if not args.build_only:
            run_application()
        else:
            logger.info("✅ 构建完成，跳过运行步骤")
        
        logger.info("=" * 60)
        logger.info("🎉 构建脚本执行完成!")
        logger.info("=" * 60)
        return 0
    except Exception as e:
        logger.error(f"❌ 构建过程中发生错误: {str(e)}")
        import traceback
        logger.debug(traceback.format_exc())
        logger.info("")
        logger.info("💡 提示:")
        logger.info("1. 请确保以管理员身份运行")
        logger.info("2. 如果自动安装失败，请参考手动安装指南")
        logger.info("3. 运行 --help 查看更多选项")
        return 1

if __name__ == "__main__":
    sys.exit(main())
