#!/bin/bash

# FBTFT LCD Benchmark 构建脚本

echo "=== FBTFT LCD Benchmark 构建脚本 ==="

# 检查工具链
TOOLCHAIN_PATH="/home/liewzheng/Workspace/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin"
CROSS_COMPILER="${TOOLCHAIN_PATH}/arm-rockchip830-linux-uclibcgnueabihf-gcc"

if [ ! -f "$CROSS_COMPILER" ]; then
    echo "错误: 找不到交叉编译器: $CROSS_COMPILER"
    echo "请检查 luckfox-pico 工具链是否正确安装"
    exit 1
fi

echo "使用交叉编译器: $CROSS_COMPILER"

# 清理之前的构建
if [ -d "build" ]; then
    echo "清理之前的构建文件..."
    rm -rf build
fi

# 创建构建目录
mkdir -p build

# 进入构建目录
cd build

echo "配置项目..."
# 使用 CMAKE_TOOLCHAIN_FILE 来正确设置交叉编译
cat > toolchain.cmake << EOF
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER ${CROSS_COMPILER})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/arm-rockchip830-linux-uclibcgnueabihf-g++)

set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_PATH}/../arm-rockchip830-linux-uclibcgnueabihf/sysroot)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
EOF

# 配置项目
cmake -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake \
      -DCMAKE_BUILD_TYPE=Release \
      ..

if [ $? -ne 0 ]; then
    echo "错误: CMake 配置失败"
    exit 1
fi

echo "开始编译..."
# 编译项目
make -j$(nproc)

if [ $? -eq 0 ]; then
    echo "编译成功!"
    echo "可执行文件位置: $(pwd)/fbtft_lcd_benchmark"
    
    # 显示文件信息
    echo ""
    echo "=== 构建信息 ==="
    file fbtft_lcd_benchmark
    ls -lh fbtft_lcd_benchmark
    
    # 创建pic目录链接
    if [ ! -d "../pic" ] && [ -d "../../pic" ]; then
        echo "创建图片目录符号链接..."
        ln -sf ../../pic ../pic
    fi
    
    echo ""
    echo "使用方法:"
    echo "  ./fbtft_lcd_benchmark              # 运行benchmark"
    echo "  ./fbtft_lcd_benchmark -t           # 测试LCD设备"
    echo "  ./fbtft_lcd_benchmark -i           # 显示设备信息"
    echo "  ./fbtft_lcd_benchmark -h           # 显示帮助"
else
    echo "编译失败!"
    exit 1
fi
