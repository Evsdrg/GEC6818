#include "bmp.h"

int* openLcd(void) {     // 打开屏幕
    int fb0 = open("/dev/fb0", O_RDWR);
    if (fb0 == -1) {
        printf("open fb0 failed\n");
        return NULL;
    }
    /* 准备好fb的内存以进行内存复制 */
    int* fb_mmap = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fb0, 0);
    if (fb_mmap == MAP_FAILED) {
        printf("mmap failed\n");
        close(fb0);
        return NULL;
    }
    close(fb0);
    return fb_mmap;
}
int* openBmp(char* bmpName) {
    int bmp1 = open(bmpName, O_RDONLY);
    if (bmp1 == -1) {
        perror("open bmp failed");
        return NULL;
    }

    // 分配内存并保存原始指针
    int* color_base = (int*)malloc(800 * 480 * sizeof(int));
    if (!color_base) {
        perror("malloc failed");
        close(bmp1);
        return NULL;
    }
    memset(color_base, 0, 800 * 480 * sizeof(int));

    unsigned char* bmp_data = mmap(NULL, 800 * 480 * 3, PROT_READ, MAP_PRIVATE, bmp1, 0);
    close(bmp1);

    if (bmp_data == MAP_FAILED) {
        perror("mmap failed");
        free(color_base);
        return NULL;
    }

    unsigned char* pixels = bmp_data + 54;
    unsigned char* src = pixels + (479 * 800 * 3); // 从最后一行开始
    int* color = color_base; // 使用临时指针遍历

    for (int y = 0; y < 480; y++) {
        unsigned char* row = src;
        for (int x = 0; x < 800; x++) {
            *color++ = (0xFF << 24) | (row[2] << 16) | (row[1] << 8) | row[0];
            row += 3;
        }
        src -= 800 * 3; // 移动到上一行
    }

    munmap(bmp_data, 800 * 480 * 3);
    return color_base; // 返回原始指针
}
void showBmp(char* colorMem, int* fb) {  // 直接展示一张图片
    int* color = openBmp(colorMem);     // 获取转换好的ARGB数据
    memcpy(fb, color, 800 * 480 * 4);   // 复制数据
    free(color);                        // 释放分配的内存
}
void picToLeft(char* colorMem, int* fb) {   // 向左飞去
    int* color = openBmp(colorMem);     // 打开转好格式的bmp文件
    for (int j = 799; j >= 0; j--) {    // 向左飞去的算法实现
        const size_t copy_size = (800 - j) * sizeof(int); // 预计算复制大小
        for (int i = 0; i < 480; i++) {
            int* fb_base = fb + 800 * i;   // 行基址预计算
            int* color_base = color + 800 * i;
            memcpy(fb_base + j, color_base + j, copy_size);
        }
        usleep(50);    // 设定飞入速度
    }
    free(color);
}
void picToRight(char* colorMem, int* fb) {  // 向右飞去
    int* color = openBmp(colorMem);
    for (int j = 0; j < 800; j++) {
        const size_t copy_size = (800 - j) * sizeof(int); // 预计算复制大小
        for (int i = 0; i < 480; i++) {
            int* fb_base = fb + 800 * i;   // 行基址预计算
            int* color_base = color + 800 * i;
            memcpy(fb_base + j, color_base + j, copy_size);
        }
        usleep(50);
    }
    free(color);
}
void picUp(char* colorMem, int* fb) {   // 向上飞去
    int* color = openBmp(colorMem);
    for (int i = 479; i >= 0; i--) {
        memcpy(fb + 800 * i,    // 目标地址
            color + 800 * i,    // 源地址
            800 * sizeof(int)); // 整行复制
        usleep(50);
    }
    free(color);
}
void picCombineIntoOne(char* colorMem, int* fb) {   // 左右相合
    int* color = openBmp(colorMem);
    for (int block = 0; block < 10; block++) {
        const int L_start = 40 * block; 
        const int L_end = L_start + 40;
        const int R_end = 799 - (block + 1) * 40;
        const int R_num = 40; // 固定每次处理40列

        for (int j = 0; j < 480; j++) {
            int* fb_row = fb + j * 800;
            int* color_row = color + j * 800;
            // 左半部分内存复制
            memcpy(fb_row + L_start, color_row + L_start, 40 * sizeof(int));
            // 右半部分内存复制
            memcpy(fb_row + (799 - block * 40 - 39), // 计算右区起始位置
                color_row + (799 - block * 40 - 39),
                40 * sizeof(int));
        }
        usleep(100000);
    }
    free(color);
}
int picCircularSpread(char* fileName, int* fb) {	// 圆形扩散
    int* color = openBmp(fileName);     // 打开转好格式的bmp文件
    for (int k = 0; k < 467; k += 3) {  // 特殊动画“圆形扩散”效果算法
        for (int i = 0; i < 480; i++) {
            for (int j = 0; j < 800; j++) {
                if ((j - 400) * (j - 400) + (i - 240) * (i - 240) <= k * k) {    //(400,240)为圆心，k为半径画圆
                    fb[800 * i + j] = color[800 * i + j];
                }
            }
        }
    }
    free(color);
}
/* 飞入背景 */
void backgroundFlyIn(char* fileName) {
    int* fb = openLcd();
    picUp(fileName, fb);
    munmap(fb, 800 * 480 * 4);
}
