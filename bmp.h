#ifndef BMP_H
#define BMP_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

static int tmp[800 * 480];   // 转化和暂存颜色数据


// 打开屏幕
int* openLcd(void);     
// 打开并将24位BMP转为32位ARGB数据
int* openBmp(char* bmpName);    
// 直接展示一张图片
void showBmp(char* colorMem, int* fb);   
// 向左飞去
void picToLeft(char* colorMem, int* fb); 
// 向右飞去
void picToRight(char* colorMem, int* fb);
// 向上飞去
void picUp(char* colorMem, int* fb);
// 左右相合
void picCombineIntoOne(char* colorMem, int* fb);
// 圆形扩散
int picCircularSpread(char* fileName, int* fb);
// 飞入黑色背景
void backgroundFlyIn(char* fileName);

#endif