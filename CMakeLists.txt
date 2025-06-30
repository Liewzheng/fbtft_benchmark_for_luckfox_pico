cmake_minimum_required(VERSION 3.10)
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
    set(CMAKE_C_COMPILER "${TOOLCHAIN_PATH}/arm-rockchip830-linux-uclibcgnueabihf-gcc")
    set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PATH}/arm-rockchip830-linux-uclibcgnueabihf-g++")
endif()

# 包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/Config
    ${CMAKE_SOURCE_DIR}/lib/GUI
    ${CMAKE_SOURCE_DIR}/lib/LCD
    ${CMAKE_SOURCE_DIR}/lib/Fonts
)

# 源文件
set(SOURCES
    src/main.c
    src/fbtft_benchmark.c
    src/fbtft_lcd.c
    src/bmp_loader.c
    
    # 复用原有的库文件
    ${CMAKE_SOURCE_DIR}/lib/Config/DEV_Config.c
    ${CMAKE_SOURCE_DIR}/lib/Config/dev_hardware_SPI.c
    ${CMAKE_SOURCE_DIR}/lib/Config/sysfs_gpio.c
    ${CMAKE_SOURCE_DIR}/lib/GUI/GUI_Paint.c
    ${CMAKE_SOURCE_DIR}/lib/GUI/GUI_BMP.c
    ${CMAKE_SOURCE_DIR}/lib/Fonts/font8.c
    ${CMAKE_SOURCE_DIR}/lib/Fonts/font12.c
    ${CMAKE_SOURCE_DIR}/lib/Fonts/font16.c
    ${CMAKE_SOURCE_DIR}/lib/Fonts/font20.c
    ${CMAKE_SOURCE_DIR}/lib/Fonts/font24.c
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 链接库
target_link_libraries(${PROJECT_NAME} 
    pthread
    m
)

# 安装规则
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# 打印配置信息
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
