cmake_minimum_required(VERSION 3.10)
project(huaBan-pro)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 源文件
set(SOURCES
    main.cpp
    canvas.cpp
    tools.cpp
    history.cpp
    ui.cpp
    fileio.cpp
    layer.cpp
    filter.cpp
    animation.cpp
    plugin_manager.cpp
)

# 头文件
set(HEADERS
    huaBan.h
    canvas.h
    tools.h
    history.h
    ui.h
    fileio.h
    layer.h
    filter.h
    animation.h
    plugin.h
    plugin_manager.h
)

# 查找 EasyX 库
find_library(EASYX_LIB NAMES easyx PATHS "C:/Program Files (x86)/EasyX/lib64" "C:/Program Files (x86)/EasyX/lib32")
find_path(EASYX_INCLUDE NAMES graphics.h PATHS "C:/Program Files (x86)/EasyX/include")

if(NOT EASYX_LIB OR NOT EASYX_INCLUDE)
    message(FATAL_ERROR "EasyX library not found. Please install EasyX from https://easyx.cn/")
endif()

# 添加包含目录
include_directories(${EASYX_INCLUDE})

# 可执行文件
add_executable(huaBan-pro ${SOURCES} ${HEADERS})

# 链接库
target_link_libraries(huaBan-pro ${EASYX_LIB} gdi32 comdlg32 uuid ole32 oleaut32)

# 设置输出目录
set_target_properties(huaBan-pro PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 复制资源文件（如果有）
# file(COPY resources DESTINATION ${CMAKE_BINARY_DIR}/bin)

message(STATUS "Configuration completed successfully!")
message(STATUS "Use 'cmake --build .' to build the project")
message(STATUS "The executable will be in ${CMAKE_BINARY_DIR}/bin")
