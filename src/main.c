#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "fbtft_benchmark.h"
#include "fbtft_lcd.h"

void print_usage(const char *program_name) {
    printf("FBTFT LCD Benchmark Tool\n");
    printf("Usage: %s [options]\n", program_name);
    printf("Options:\n");
    printf("  -h, --help             Show this help message\n");
    printf("  -d, --device PATH      Specify framebuffer device (default: auto-detect)\n");
    printf("  -t, --test             Run LCD device test\n");
    printf("  -i, --info             Show LCD device information\n");
    printf("  -T, --transform ANGLE  Rotate image clockwise by ANGLE degrees (0, 90, 180, 270)\n");
    printf("  -M, --mirror MODE      Mirror image (horizontal, vertical, both, none)\n");
    printf("  -F, --fit MODE         Image fitting mode (scale, stretch, auto)\n");
    printf("                         scale: keep aspect ratio (default)\n");
    printf("                         stretch: stretch to fill screen\n");
    printf("                         auto: auto-rotate for best fit\n");
    printf("  benchmark              Run the benchmark test (default)\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s                         # Run benchmark with auto-detection\n", program_name);
    printf("  %s -d /dev/fb1             # Run benchmark on specific device\n", program_name);
    printf("  %s -t                      # Test LCD device\n", program_name);
    printf("  %s -i                      # Show device information\n", program_name);
    printf("  %s -T 90                   # Rotate images 90 degrees clockwise\n", program_name);
    printf("  %s -T 270 -M horizontal    # Rotate 270° and mirror horizontally\n", program_name);
    printf("  %s -F auto                 # Auto-rotate images for best fit\n", program_name);
    printf("  %s -F stretch              # Stretch images to fill screen\n", program_name);
}

void test_lcd_device(const char *device_path) {
    fbtft_lcd_t lcd;
    
    printf("Testing FBTFT LCD device: %s\n", device_path);
    
    if (fbtft_lcd_init(&lcd, device_path) != 0) {
        printf("Error: Failed to initialize LCD device\n");
        return;
    }
    
    printf("LCD device test successful!\n");
    fbtft_lcd_print_info(&lcd);
    
    // 简单的颜色测试
    printf("Running color test...\n");
    
    // 分配缓冲区
    size_t buffer_size = lcd.width * lcd.height * sizeof(uint16_t);
    uint16_t *buffer = (uint16_t *)malloc(buffer_size);
    if (!buffer) {
        printf("Error: Cannot allocate buffer\n");
        fbtft_lcd_deinit(&lcd);
        return;
    }
    
    // 测试颜色
    uint16_t colors[] = {FBTFT_RED, FBTFT_GREEN, FBTFT_BLUE, FBTFT_WHITE, FBTFT_BLACK};
    const char *color_names[] = {"Red", "Green", "Blue", "White", "Black"};
    int num_colors = sizeof(colors) / sizeof(colors[0]);
    
    for (int i = 0; i < num_colors; i++) {
        printf("  Testing %s...\n", color_names[i]);
        fbtft_lcd_clear(&lcd, colors[i]);
        sleep(1);
    }
    
    // 绘制一些图形
    printf("  Testing graphics...\n");
    fbtft_lcd_clear(&lcd, FBTFT_WHITE);
    
    // 绘制边框
    fbtft_lcd_draw_rectangle(&lcd, 10, 10, lcd.width-11, lcd.height-11, FBTFT_BLACK);
    
    // 绘制填充矩形
    fbtft_lcd_fill_rectangle(&lcd, 50, 50, 150, 100, FBTFT_RED);
    fbtft_lcd_fill_rectangle(&lcd, 170, 50, 270, 100, FBTFT_GREEN);
    fbtft_lcd_fill_rectangle(&lcd, 50, 120, 150, 170, FBTFT_BLUE);
    fbtft_lcd_fill_rectangle(&lcd, 170, 120, 270, 170, FBTFT_YELLOW);
    
    sleep(3);
    
    free(buffer);
    fbtft_lcd_deinit(&lcd);
    
    printf("LCD device test completed!\n");
}

const char* auto_detect_device(void) {
    const char* devices[] = {"/dev/fb1", "/dev/fb0", "/dev/fb2"};
    int num_devices = sizeof(devices) / sizeof(devices[0]);
    
    for (int i = 0; i < num_devices; i++) {
        if (fbtft_lcd_check_device(devices[i])) {
            printf("Auto-detected framebuffer device: %s\n", devices[i]);
            return devices[i];
        }
    }
    
    return NULL;
}

void show_device_info(const char *device_path) {
    fbtft_lcd_t lcd;
    
    printf("Getting information for device: %s\n", device_path);
    
    if (fbtft_lcd_init(&lcd, device_path) != 0) {
        printf("Error: Failed to initialize LCD device\n");
        return;
    }
    
    fbtft_lcd_print_info(&lcd);
    fbtft_lcd_deinit(&lcd);
}

int main(int argc, char *argv[]) {
    const char *device_path = NULL;
    int test_mode = 0;
    int info_mode = 0;
    int benchmark_mode = 1; // 默认运行benchmark
    
    // 显示配置
    display_config_t display_config = {
        .rotation = ROTATE_0,
        .mirror = MIRROR_NONE,
        .fit_mode = FIT_SCALE
    };
    
    // 解析命令行参数
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--device") == 0) {
            if (i + 1 < argc) {
                device_path = argv[++i];
            } else {
                printf("Error: Device path required after %s\n", argv[i]);
                return 1;
            }
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--test") == 0) {
            test_mode = 1;
            benchmark_mode = 0;
        } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--info") == 0) {
            info_mode = 1;
            benchmark_mode = 0;
        } else if (strcmp(argv[i], "-T") == 0 || strcmp(argv[i], "--transform") == 0) {
            if (i + 1 < argc) {
                int angle = atoi(argv[++i]);
                switch (angle) {
                    case 0:   display_config.rotation = ROTATE_0; break;
                    case 90:  display_config.rotation = ROTATE_90; break;
                    case 180: display_config.rotation = ROTATE_180; break;
                    case 270: display_config.rotation = ROTATE_270; break;
                    default:
                        printf("Error: Invalid rotation angle %d. Use 0, 90, 180, or 270.\n", angle);
                        return 1;
                }
            } else {
                printf("Error: Rotation angle required after %s\n", argv[i]);
                return 1;
            }
        } else if (strcmp(argv[i], "-M") == 0 || strcmp(argv[i], "--mirror") == 0) {
            if (i + 1 < argc) {
                char *mode = argv[++i];
                if (strcmp(mode, "none") == 0) {
                    display_config.mirror = MIRROR_NONE;
                } else if (strcmp(mode, "horizontal") == 0) {
                    display_config.mirror = MIRROR_HORIZONTAL;
                } else if (strcmp(mode, "vertical") == 0) {
                    display_config.mirror = MIRROR_VERTICAL;
                } else if (strcmp(mode, "both") == 0) {
                    display_config.mirror = MIRROR_BOTH;
                } else {
                    printf("Error: Invalid mirror mode '%s'. Use 'none', 'horizontal', 'vertical', or 'both'.\n", mode);
                    return 1;
                }
            } else {
                printf("Error: Mirror mode required after %s\n", argv[i]);
                return 1;
            }
        } else if (strcmp(argv[i], "-F") == 0 || strcmp(argv[i], "--fit") == 0) {
            if (i + 1 < argc) {
                char *mode = argv[++i];
                if (strcmp(mode, "scale") == 0) {
                    display_config.fit_mode = FIT_SCALE;
                } else if (strcmp(mode, "stretch") == 0) {
                    display_config.fit_mode = FIT_STRETCH;
                } else if (strcmp(mode, "auto") == 0) {
                    display_config.fit_mode = FIT_AUTO;
                } else {
                    printf("Error: Invalid fit mode '%s'. Use 'scale', 'stretch', or 'auto'.\n", mode);
                    return 1;
                }
            } else {
                printf("Error: Fit mode required after %s\n", argv[i]);
                return 1;
            }
        } else if (strcmp(argv[i], "benchmark") == 0) {
            benchmark_mode = 1;
            test_mode = 0;
            info_mode = 0;
        } else {
            printf("Error: Unknown option %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }
    
    // 自动检测设备（如果没有指定）
    if (!device_path) {
        device_path = auto_detect_device();
        if (!device_path) {
            printf("Error: No framebuffer device found\n");
            printf("Please specify device manually with -d option\n");
            return 1;
        }
    } else {
        // 验证指定的设备
        if (!fbtft_lcd_check_device(device_path)) {
            printf("Error: Device %s not found or not accessible\n", device_path);
            return 1;
        }
    }
    
    printf("FBTFT LCD Benchmark Tool v1.0\n");
    printf("Using device: %s\n", device_path);
    if (display_config.rotation != ROTATE_0) {
        printf("Rotation: %d degrees\n", display_config.rotation);
    }
    if (display_config.mirror != MIRROR_NONE) {
        const char *mirror_names[] = {"none", "horizontal", "vertical", "both"};
        printf("Mirror: %s\n", mirror_names[display_config.mirror]);
    }
    if (display_config.fit_mode != FIT_SCALE) {
        const char *fit_names[] = {"scale", "stretch", "auto"};
        printf("Fit Mode: %s\n", fit_names[display_config.fit_mode]);
    }
    printf("\n");
    
    // 根据模式执行相应功能
    if (info_mode) {
        show_device_info(device_path);
    } else if (test_mode) {
        test_lcd_device(device_path);
    } else if (benchmark_mode) {
        // 检查当前目录是否有pic文件夹
        if (access("./pic", F_OK) != 0) {
            printf("Warning: ./pic directory not found\n");
            printf("Please ensure BMP files are in ./pic/ directory\n");
            
            // 尝试创建符号链接到原pic目录
            if (access("../pic", F_OK) == 0) {
                printf("Found ../pic directory, creating symbolic link...\n");
                if (symlink("../pic", "./pic") == 0) {
                    printf("Symbolic link created successfully\n");
                } else {
                    printf("Failed to create symbolic link\n");
                    return 1;
                }
            } else {
                return 1;
            }
        }
        
        fbtft_benchmark_run(&display_config);
    }
    
    return 0;
}
