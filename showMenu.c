#include "showMenu.h"
// 背景图片显示
void showMenu(int backGround) {
    int* fb = openLcd();
    char fileName[64];
    snprintf(fileName, sizeof(fileName), "./menuBackground/%d.bmp", backGround);
    showBmp(fileName, fb);
    munmap(fb, 800 * 480 * 4);
}
// 
int settingBackgroundPlus(int num) {
    if (num < 7) {
        return ++num;
    }
    else {
        return num;
    }
}
int settingBackgroundMinus(int num) {
    if (num > 1) {
        return --num;
    }
    else {
        return num;
    }
}