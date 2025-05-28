#ifndef SHOWMENU_H
#define SHOWMENU_H


#include "bmp.h"
/* 显示菜单 总共6个背景 */
void showMenu(int backGround);
/* 下一张图片 */
int settingBackgroundPlus(int num);
/* 上一张图片 */
int settingBackgroundMinus(int num);

#endif