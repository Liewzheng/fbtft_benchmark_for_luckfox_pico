cmake_minimum_required(VERSION 3.16)
project(fbtft_lcd_benchmark VERSION 1.0.0 LANGUAGES C)

# 设置C标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 设置编译选项
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O2 -Wall -Wextra")

# 交叉编译工具链设置
if(DEFINED ENV{CROSS_COMPILE})
    set(CMAKE_C_COMPILER $ENV{CROSS_COMPILE}gcc)
    set(CMAKE_CXX_COMPILER $ENV{CROSS_COMPILE}g++)
else()
    # 默认使用luckfox工具链
    set(TOOLCHAIN_PATH "/home/liewzheng/Workspace/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/")
    set(TOOLCHAIN_PREFIX "${TOOLCHAIN_PATH}/arm-rockchip830-linux-uclibcgnueabihf-")

    set(CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}gcc")
    set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}g++")
    set(CMAKE_AR "${TOOLCHAIN_PREFIX}ar")
    set(CMAKE_STRIP "${TOOLCHAIN_PREFIX}strip")
    set(CMAKE_NM "${TOOLCHAIN_PREFIX}nm")
    set(CMAKE_OBJCOPY "${TOOLCHAIN_PREFIX}objcopy")
    set(CMAKE_OBJDUMP "${TOOLCHAIN_PREFIX}objdump")
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/Config
    ${CMAKE_SOURCE_DIR}/lib/GUI
    ${CMAKE_SOURCE_DIR}/lib/LCD
    ${CMAKE_SOURCE_DIR}/lib/Fonts
)

file(GLOB_RECURSE LIB_FBTFT_SOURCES
    ${CMAKE_SOURCE_DIR}/src/fbtft_lcd.c
    ${CMAKE_SOURCE_DIR}/src/fbtft_benchmark.c
    ${CMAKE_SOURCE_DIR}/src/bmp_loader.c

    ${CMAKE_SOURCE_DIR}/lib/Config/*.c
    ${CMAKE_SOURCE_DIR}/lib/GUI/*.c
    ${CMAKE_SOURCE_DIR}/lib/LCD/*.c
    ${CMAKE_SOURCE_DIR}/lib/Fonts/*.c
)

# 创建 libstaging.so 动态库
add_library(staging SHARED
    ${LIB_FBTFT_SOURCES}
)

# 设置动态库的输出名称和版本
set_target_properties(staging PROPERTIES
    OUTPUT_NAME "staging"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    C_VISIBILITY_PRESET default
    VISIBILITY_INLINES_HIDDEN OFF
)

# 为动态库设置包含目录
target_include_directories(staging PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/Config
    ${CMAKE_SOURCE_DIR}/lib/GUI
    ${CMAKE_SOURCE_DIR}/lib/LCD
    ${CMAKE_SOURCE_DIR}/lib/Fonts
)

# 为动态库链接必要的系统库
target_link_libraries(staging
    pthread
    m
)

# 设置动态库的编译标志
target_compile_options(staging PRIVATE
    -fPIC
)

# 导出符号定义 - 让所有符号默认可见
target_compile_definitions(staging PRIVATE
    STAGING_EXPORTS=1
)

# 源文件
set(SOURCES
    src/main.c
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 设置可执行文件需要先构建动态库
add_dependencies(${PROJECT_NAME} staging)

# 链接 libstaging.so 动态库到主程序
target_link_libraries(${PROJECT_NAME} 
    staging
    pthread
    m
)

# 设置运行时库路径
set_target_properties(${PROJECT_NAME} PROPERTIES
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
    BUILD_WITH_INSTALL_RPATH TRUE
)

# 打印配置信息
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Output Directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "Library Directory: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

# 安装配置
install(TARGETS staging
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# 安装头文件
install(FILES
    ${CMAKE_SOURCE_DIR}/include/fbtft_benchmark.h
    ${CMAKE_SOURCE_DIR}/include/fbtft_lcd.h
    ${CMAKE_SOURCE_DIR}/include/bmp_loader.h
    DESTINATION include
)

# 创建 pkg-config 文件
configure_file(
    "${CMAKE_SOURCE_DIR}/libstaging.pc.in"
    "${CMAKE_BINARY_DIR}/libstaging.pc"
    @ONLY
)

install(FILES "${CMAKE_BINARY_DIR}/libstaging.pc"
    DESTINATION lib/pkgconfig
)

# 添加自定义目标来显示库信息
add_custom_target(library-info
    COMMAND echo "=== Library Information ==="
    COMMAND echo "Library: libstaging.so.${PROJECT_VERSION}"
    COMMAND echo "Location: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    COMMAND echo "Headers: ${CMAKE_SOURCE_DIR}/include/"
    COMMAND echo "=== Build Commands ==="
    COMMAND echo "Build library: make staging"
    COMMAND echo "Build executable: make ${PROJECT_NAME}"
    COMMAND echo "Build all: make"
    DEPENDS staging
)

# 添加自定义目标来检查动态库
add_custom_target(check-library
    COMMAND echo "Checking libstaging.so..."
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan "Library file:"
    COMMAND ls -la ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libstaging.so*
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan "Library dependencies:"
    COMMAND ldd ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libstaging.so || echo "ldd not available (cross-compiled)"
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan "Library symbols:"
    COMMAND nm -D ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libstaging.so | head -20 || echo "nm not available"
    DEPENDS staging
)
