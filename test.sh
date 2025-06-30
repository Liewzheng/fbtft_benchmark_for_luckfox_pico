#!/bin/bash

# FBTFT LCD Benchmark 测试脚本

echo "=== FBTFT LCD Benchmark 测试脚本 ==="

BENCHMARK_BIN="./build/fbtft_lcd_benchmark"

if [ ! -f "$BENCHMARK_BIN" ]; then
    echo "错误: 找不到可执行文件 $BENCHMARK_BIN"
    echo "请先运行 ./build.sh 编译项目"
    exit 1
fi

echo "找到可执行文件: $BENCHMARK_BIN"

# 显示帮助信息
echo ""
echo "=== 显示帮助信息 ==="
$BENCHMARK_BIN -h

echo ""
echo "=== 检查 framebuffer 设备 ==="

# 检查常见的framebuffer设备
for fb in /dev/fb0 /dev/fb1 /dev/fb2; do
    if [ -e "$fb" ]; then
        echo "发现设备: $fb"
        ls -la "$fb"
        
        # 显示设备信息
        echo ""
        echo "=== $fb 设备信息 ==="
        $BENCHMARK_BIN -d "$fb" -i 2>/dev/null || echo "无法获取 $fb 的信息"
    else
        echo "设备不存在: $fb"
    fi
done

echo ""
echo "=== 检查 FBTFT 相关模块 ==="
lsmod | grep -E "(fbtft|fb)" || echo "未找到 FBTFT 相关模块"

echo ""
echo "=== 系统 framebuffer 信息 ==="
cat /proc/fb 2>/dev/null || echo "无法读取 /proc/fb"

echo ""
echo "=== 测试建议 ==="
echo "1. 如果没有找到 /dev/fb1 设备，请确保 FBTFT 驱动已加载"
echo "2. 加载 FBTFT 驱动示例："
echo "   modprobe fbtft_device name=ili9341 gpios=reset:25,dc:24 speed=32000000"
echo "3. 如果权限不足，请使用 sudo 运行："
echo "   sudo $BENCHMARK_BIN"
echo "4. 确保 pic/ 目录中有测试图片："
echo "   ls -la pic/"

# 检查图片目录
echo ""
echo "=== 检查测试图片 ==="
if [ -d "pic" ]; then
    echo "图片目录存在:"
    ls -la pic/
    
    bmp_count=$(ls pic/*.bmp 2>/dev/null | wc -l)
    echo "找到 $bmp_count 个 BMP 文件"
    
    if [ "$bmp_count" -eq 0 ]; then
        echo "警告: pic/ 目录中没有 BMP 文件"
        echo "建议: 复制一些 BMP 测试图片到 pic/ 目录"
    fi
else
    echo "警告: pic/ 目录不存在"
    
    # 尝试创建到上级目录的链接
    if [ -d "../pic" ]; then
        echo "尝试创建到 ../pic 的符号链接..."
        ln -sf ../pic ./pic
        echo "符号链接创建完成"
    else
        echo "建议: 创建 pic/ 目录并添加测试图片"
    fi
fi

echo ""
echo "=== 测试完成 ==="
echo "如果一切正常，可以运行以下命令进行测试："
echo "  $BENCHMARK_BIN -t    # 设备测试"
echo "  $BENCHMARK_BIN       # 运行benchmark"
