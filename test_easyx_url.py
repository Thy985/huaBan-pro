#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试脚本 - 验证EasyX下载链接获取功能
"""

import sys
import os
import urllib.request

# 添加当前目录到路径
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from build import get_latest_easyx_url, setup_logging

# 设置日志
logger = setup_logging()

# 测试获取EasyX下载链接
print("测试获取EasyX下载链接...")
try:
    url = get_latest_easyx_url()
    print(f"获取到的EasyX下载链接: {url}")
    
    # 测试链接是否有效
    print("测试链接是否有效...")
    with urllib.request.urlopen(url, timeout=10) as response:
        if response.getcode() == 200:
            print("✓ 链接有效！")
        else:
            print(f"✗ 链接无效，状态码: {response.getcode()}")
    
    print("测试成功!")
except Exception as e:
    print(f"测试失败: {str(e)}")
    import traceback
    traceback.print_exc()
