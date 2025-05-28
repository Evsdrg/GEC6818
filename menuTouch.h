#ifndef MENUTOUCH_H
#define MENUTOUCH_H


#include "touch.h"

#define menuVideo 12
#define menuMusic 13
#define menuGame 14
#define menuImage 15
#define menuQuit 16
#define menuLeft 17
#define menuRight 18
#define ImageShow 19
#define ImageLook 20
#define ImageBack 21

// 主菜单功能识别
int menuTouch();
// 图片浏览功能识别
int imageTouch();

#endif