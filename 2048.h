#ifndef _2048_H
#define _2048_H

#include <time.h> 
#include <stdio.h> 
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/fb.h>
#include <linux/input.h>
#include "touch.h"


#define MOVE_LEFT moveLeft// 左
#define MOVE_RIGHT moveRight// 右
#define MOVE_UP moveUp// 上
#define MOVE_DOWN moveDown// 下

#define BOARDSIZE  4    // 整个棋盘的大小BORADSIZE * BOARDSIZE
struct header
{
    int16_t type;
    int32_t size;
    int16_t reserved1;
    int16_t reserved2;
    int32_t offbytes;
}__attribute__((packed));

struct info
{
    int32_t size;
    int32_t width;
    int32_t height;
    int16_t planes;

    int16_t bit_count;
    int32_t compression;
    int32_t size_img;
    int32_t xpel;
    int32_t ypel;
    int32_t clrused;
    int32_t partant;
}__attribute__((packed));

extern bool game_over;

void game2048(void);


#endif