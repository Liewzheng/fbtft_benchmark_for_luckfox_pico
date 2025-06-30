# FBTFT LCD Benchmark

基于 FBTFT 驱动的 LCD 性能测试工具，使用 CMake 构建系统。

## 项目简介

这是一个专为 LuckFox Pico 开发板设计的 LCD 性能测试工具，使用 Linux 内核的 FBTFT (Framebuffer TFT) 驱动来控制 SPI LCD 显示屏。与原始的直接 SPI 控制方式不同，这个版本通过标准的 Linux framebuffer 接口来操作显示屏。

## 功能特性

- **FBTFT 驱动支持**: 通过 Linux framebuffer 接口控制 LCD
- **性能测试**: 测量 LCD 显示帧率和性能指标
- **BMP 图像支持**: 支持 24位和32位 BMP 图像加载和显示
- **实时监控**: 实时显示 FPS、最大 FPS 和总帧数
- **交叉编译**: 支持 ARM 交叉编译
- **CMake 构建**: 使用现代化的 CMake 构建系统

## 项目结构

```
fbtft_benchmark/
├── CMakeLists.txt          # CMake 构建配置
├── build.sh               # 自动构建脚本
├── README.md              # 本文档
├── include/               # 头文件目录
│   ├── fbtft_lcd.h        # FBTFT LCD 驱动接口
│   ├── fbtft_benchmark.h  # Benchmark 功能
│   └── bmp_loader.h       # BMP 图像加载器
├── src/                   # 源代码目录
│   ├── main.c             # 主程序
│   ├── fbtft_lcd.c        # FBTFT LCD 驱动实现
│   ├── fbtft_benchmark.c  # Benchmark 实现
│   └── bmp_loader.c       # BMP 加载器实现
├── pic/                   # 测试图片目录 (符号链接)
└── build/                 # 构建输出目录
```

## 编译要求

### 系统要求
- Linux 开发环境
- CMake 3.10 或更高版本
- LuckFox Pico 交叉编译工具链

### 工具链路径
默认工具链路径：
```
/home/liewzheng/Workspace/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/
```

## 编译方法

### 方法一：使用构建脚本（推荐）
```bash
cd fbtft_benchmark
./build.sh
```

### 方法二：手动编译
```bash
cd fbtft_benchmark
mkdir -p build
cd build

# 配置项目
cmake -DCMAKE_BUILD_TYPE=Release ..

# 编译
make -j$(nproc)
```

### 方法三：指定交叉编译器
```bash
export CROSS_COMPILE=/path/to/your/toolchain/arm-rockchip830-linux-uclibcgnueabihf-
cmake -B build
cmake --build build
```

## 使用方法

### 运行 Benchmark
```bash
# 默认运行 benchmark 测试
./fbtft_lcd_benchmark

# 或显式指定 benchmark 模式
./fbtft_lcd_benchmark benchmark
```

### 测试 LCD 设备
```bash
# 测试 LCD 设备连接和基本功能
./fbtft_lcd_benchmark -t
```

### 显示设备信息
```bash
# 显示 framebuffer 设备详细信息
./fbtft_lcd_benchmark -i
```

### 指定 framebuffer 设备
```bash
# 使用特定的 framebuffer 设备
./fbtft_lcd_benchmark -d /dev/fb1
```

### 显示帮助
```bash
./fbtft_lcd_benchmark -h
```

## FBTFT 驱动配置

在运行 benchmark 之前，需要确保 FBTFT 驱动已正确加载：

### 1. 加载 FBTFT 驱动模块
```bash
# 加载通用 FBTFT 驱动
modprobe fbtft_device

# 或加载特定的显示器驱动，例如 ILI9341
modprobe fbtft_device name=ili9341 gpios=reset:25,dc:24 speed=32000000
```

### 2. 检查 framebuffer 设备
```bash
# 查看可用的 framebuffer 设备
ls -la /dev/fb*

# 查看设备信息
cat /proc/fb
```

### 3. 验证设备
```bash
# 测试显示（填充红色）
dd if=/dev/zero of=/dev/fb1 bs=1024 count=150
```

## 测试图片

将 BMP 测试图片放置在 `pic/` 目录中：

```bash
# 确保 pic 目录存在
mkdir -p pic

# 复制测试图片
cp /path/to/your/images/*.bmp pic/
```

支持的图片格式：
- 24位 BMP (无压缩)
- 32位 BMP (无压缩)

## 性能指标

Benchmark 会测量以下指标：

- **当前 FPS**: 实时帧率
- **最大 FPS**: 测试过程中达到的最高帧率
- **平均 FPS**: 整个测试周期的平均帧率
- **总帧数**: 测试期间显示的总帧数
- **运行时间**: 测试持续时间

## 故障排除

### 1. 找不到 framebuffer 设备
```bash
# 检查内核模块
lsmod | grep fbtft

# 检查设备文件
ls -la /dev/fb*
```

### 2. 权限问题
```bash
# 确保有访问 framebuffer 的权限
sudo chmod 666 /dev/fb1
```

### 3. 编译错误
```bash
# 检查工具链路径
which arm-rockchip830-linux-uclibcgnueabihf-gcc

# 清理并重新编译
rm -rf build
./build.sh
```

### 4. 图片加载失败
```bash
# 检查图片目录
ls -la pic/

# 检查图片格式
file pic/*.bmp
```

## 与原版本的区别

| 特性 | 原版本 (SPI直控) | FBTFT版本 |
|------|------------------|-----------|
| 控制方式 | 直接SPI控制 | Linux framebuffer |
| 驱动依赖 | 自定义SPI驱动 | 内核FBTFT驱动 |
| 系统集成 | 用户空间独立 | 内核空间集成 |
| 性能 | 更高性能 | 标准化接口 |
| 兼容性 | 特定硬件 | 通用framebuffer |

## 开发说明

### 添加新的显示器支持
1. 修改 `include/fbtft_lcd.h` 中的尺寸定义
2. 在 FBTFT 驱动配置中指定正确的驱动参数
3. 重新编译测试

### 添加新的图像格式
1. 在 `bmp_loader.c` 中添加新的解码器
2. 更新 `bmp_loader.h` 中的接口
3. 测试新格式的兼容性

## 许可证

本项目基于原 LuckFox Pico LCD 项目开发，遵循相同的开源许可证。

## 贡献

欢迎提交问题报告和改进建议！
