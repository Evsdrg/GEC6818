#ifndef TOUCH_H
#define TOUCH_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <linux/input.h>

#define moveLeft 1
#define moveRight 2
#define moveUp 3
#define moveDown 4
#define Left 5
#define Right 6
#define Up 7
#define Down 8

extern int openTouch;  // 使用 extern 避免重复定义
extern int touchX0, touchY0, touchX1, touchY1;  // 滑动坐标跟踪

typedef struct {
    int x;
    int y;
} touchCoord;

void initTouch(void);   // 初始化触摸设备（必须先调用）

touchCoord fingerTouch(void);   // 检测触摸坐标

touchCoord fingerRelease(void); // 检测抬手坐标

int fingerNEWS(void);


#endif