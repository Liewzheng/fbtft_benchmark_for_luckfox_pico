#!/bin/bash

# fbtft_benchmark Build Script with libstaging.so
# 构建 libstaging.so 动态库和 fbtft_benchmark 可执行文件

set -e  # 遇到错误立即退出

# 脚本配置
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$SCRIPT_DIR"
BUILD_DIR="$PROJECT_ROOT/build"
BUILD_TYPE="Release"
JOBS=$(nproc)
VERBOSE=false
CLEAN=false

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

show_usage() {
    echo "FBTFT Benchmark Build Script"
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  -h, --help           Show this help message"
    echo "  -c, --clean          Clean build directory before building"
    echo "  -d, --debug          Build in Debug mode (default: Release)"
    echo "  -v, --verbose        Enable verbose output"
    echo "  -j, --jobs N         Number of parallel jobs (default: $(nproc))"
    echo "  --lib-only           Build only the libstaging.so library"
    echo "  --app-only           Build only the application"
    echo "  --check              Check library after building"
    echo ""
    echo "Examples:"
    echo "  $0                   # Build everything in Release mode"
    echo "  $0 -c -d             # Clean build in Debug mode"
    echo "  $0 --lib-only        # Build only libstaging.so"
    echo "  $0 --check           # Build and check library"
}

# 解析命令行参数
LIB_ONLY=false
APP_ONLY=false
CHECK_LIB=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_usage
            exit 0
            ;;
        -c|--clean)
            CLEAN=true
            shift
            ;;
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -j|--jobs)
            JOBS="$2"
            shift 2
            ;;
        --lib-only)
            LIB_ONLY=true
            shift
            ;;
        --app-only)
            APP_ONLY=true
            shift
            ;;
        --check)
            CHECK_LIB=true
            shift
            ;;
        *)
            print_error "Unknown option: $1"
            show_usage
            exit 1
            ;;
    esac
done

# 检查构建工具
check_build_tools() {
    print_status "检查构建工具..."
    
    # 检查 CMake
    if ! command -v cmake >/dev/null 2>&1; then
        print_error "CMake 未安装"
        exit 1
    fi
    print_status "CMake 版本: $(cmake --version | head -n1)"
    
    # 检查 Make 或 Ninja
    if command -v ninja >/dev/null 2>&1; then
        GENERATOR="Ninja"
        BUILDER="ninja"
        print_status "Ninja 版本: $(ninja --version)"
    elif command -v make >/dev/null 2>&1; then
        GENERATOR="Unix Makefiles"
        BUILDER="make"
        print_status "Make 版本: $(make --version | head -n1)"
    else
        print_error "未找到构建工具 (make 或 ninja)"
        exit 1
    fi
    
    # 检查交叉编译工具链
    TOOLCHAIN_PATH="/home/liewzheng/Workspace/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/"
    if [ -f "${TOOLCHAIN_PATH}/arm-rockchip830-linux-uclibcgnueabihf-gcc" ]; then
        print_status "工具链: $(${TOOLCHAIN_PATH}/arm-rockchip830-linux-uclibcgnueabihf-gcc --version | head -n1)"
    else
        print_warning "交叉编译工具链未找到，将使用系统默认编译器"
    fi
}

# 清理构建目录
clean_build() {
    if [ "$CLEAN" = true ] || [ ! -d "$BUILD_DIR" ]; then
        print_status "清理构建目录..."
        rm -rf "$BUILD_DIR"
        mkdir -p "$BUILD_DIR"
    fi
}

# 配置 CMake
configure_cmake() {
    print_status "配置 CMake (构建类型: $BUILD_TYPE)..."
    
    cd "$BUILD_DIR"
    
    CMAKE_ARGS=(
        -G "$GENERATOR"
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    )
    
    if [ "$VERBOSE" = true ]; then
        CMAKE_ARGS+=(-DCMAKE_VERBOSE_MAKEFILE=ON)
    fi
    
    if cmake "${CMAKE_ARGS[@]}" ..; then
        print_success "CMake 配置成功"
    else
        print_error "CMake 配置失败"
        exit 1
    fi
    
    cd "$PROJECT_ROOT"
}

# 构建项目
build_project() {
    print_status "开始构建 (使用 $JOBS 个并行作业)..."
    
    cd "$BUILD_DIR"
    
    BUILD_ARGS=()
    if [ "$GENERATOR" = "Ninja" ]; then
        BUILD_ARGS+=(-j "$JOBS")
        if [ "$VERBOSE" = true ]; then
            BUILD_ARGS+=(-v)
        fi
    else
        BUILD_ARGS+=(-j "$JOBS")
        if [ "$VERBOSE" = true ]; then
            BUILD_ARGS+=(VERBOSE=1)
        fi
    fi
    
    # 构建目标
    if [ "$LIB_ONLY" = true ]; then
        print_status "构建 libstaging.so 动态库..."
        if $BUILDER "${BUILD_ARGS[@]}" staging; then
            print_success "libstaging.so 构建成功"
        else
            print_error "libstaging.so 构建失败"
            exit 1
        fi
    elif [ "$APP_ONLY" = true ]; then
        print_status "构建应用程序..."
        if $BUILDER "${BUILD_ARGS[@]}" fbtft_lcd_benchmark; then
            print_success "应用程序构建成功"
        else
            print_error "应用程序构建失败"
            exit 1
        fi
    else
        print_status "构建所有目标..."
        if $BUILDER "${BUILD_ARGS[@]}"; then
            print_success "构建成功"
        else
            print_error "构建失败"
            exit 1
        fi
    fi
    
    cd "$PROJECT_ROOT"
}

# 检查构建结果
check_build_result() {
    print_status "检查构建结果..."
    
    # 检查动态库
    if [ -f "$BUILD_DIR/lib/libstaging.so" ]; then
        print_success "libstaging.so 已生成"
        ls -la "$BUILD_DIR/lib/libstaging.so"*
        
        # 检查动态库架构
        if command -v file >/dev/null 2>&1; then
            print_status "动态库架构:"
            file "$BUILD_DIR/lib/libstaging.so"
        fi
    else
        print_error "libstaging.so 未生成"
    fi
    
    # 检查可执行文件
    if [ -f "$BUILD_DIR/bin/fbtft_lcd_benchmark" ]; then
        print_success "fbtft_lcd_benchmark 可执行文件已生成"
        ls -la "$BUILD_DIR/bin/fbtft_lcd_benchmark"
        
        # 检查可执行文件架构
        if command -v file >/dev/null 2>&1; then
            print_status "可执行文件架构:"
            file "$BUILD_DIR/bin/fbtft_lcd_benchmark"
        fi
    else
        print_warning "fbtft_lcd_benchmark 可执行文件未生成"
    fi
}

# 检查库依赖
check_library_info() {
    if [ "$CHECK_LIB" = true ] && [ -f "$BUILD_DIR/lib/libstaging.so" ]; then
        print_status "检查库信息..."
        
        # 使用构建目标检查库
        cd "$BUILD_DIR"
        if $BUILDER check-library 2>/dev/null; then
            print_success "库检查完成"
        else
            print_warning "库检查工具不可用"
        fi
        cd "$PROJECT_ROOT"
    fi
}

# 显示使用说明
show_deployment_info() {
    print_status "=== 部署说明 ==="
    echo "1. 动态库文件:"
    if [ -f "$BUILD_DIR/lib/libstaging.so" ]; then
        echo "   - libstaging.so: $BUILD_DIR/lib/libstaging.so"
    fi
    
    echo "2. 可执行文件:"
    if [ -f "$BUILD_DIR/bin/fbtft_lcd_benchmark" ]; then
        echo "   - fbtft_lcd_benchmark: $BUILD_DIR/bin/fbtft_lcd_benchmark"
    fi
    
    echo "3. 部署到目标设备:"
    echo "   scp $BUILD_DIR/lib/libstaging.so root@<target_ip>:/usr/lib/"
    echo "   scp $BUILD_DIR/bin/fbtft_lcd_benchmark root@<target_ip>:/tmp/"
    
    echo "4. 在目标设备上运行:"
    echo "   export LD_LIBRARY_PATH=/usr/lib:\$LD_LIBRARY_PATH"
    echo "   /tmp/fbtft_lcd_benchmark"
    
    echo "5. 使用 pkg-config:"
    echo "   export PKG_CONFIG_PATH=$BUILD_DIR:\$PKG_CONFIG_PATH"
    echo "   pkg-config --cflags --libs libstaging"
}

# 主函数
main() {
    echo "=== FBTFT Benchmark 构建脚本 ==="
    echo "构建类型: $BUILD_TYPE"
    echo "并行作业: $JOBS"
    echo "详细输出: $VERBOSE"
    echo "清理重建: $CLEAN"
    
    check_build_tools
    clean_build
    configure_cmake
    build_project
    check_build_result
    check_library_info
    show_deployment_info
    
    print_success "构建脚本执行完成!"
}

# 执行主函数
main "$@"
