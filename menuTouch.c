#include "menuTouch.h"

int menuTouch() {   // 主菜单触摸判断
    touchCoord fingerxy = fingerRelease();
    if (fingerxy.x >= 50 && fingerxy.y >= 320 && fingerxy.x <= 150 && fingerxy.y <= 400) {
        return menuVideo;    //返回视频库
    }

    if (fingerxy.x >= 200 && fingerxy.y >= 320 && fingerxy.x <= 300 && fingerxy.y <= 400) {
        return menuMusic;    //返回音乐库
    }

    if (fingerxy.x >= 350 && fingerxy.y >= 320 && fingerxy.x <= 450 && fingerxy.y <= 400) {
        return menuGame;    //返回游戏
    }

    if (fingerxy.x >= 500 && fingerxy.y >= 320 && fingerxy.x <= 600 && fingerxy.y <= 400) {
        return menuImage;    //返回图库
    }

    if (fingerxy.x >= 650 && fingerxy.y >= 320 && fingerxy.x <= 750 && fingerxy.y <= 400) {
        return menuQuit;    //返回结束
    }

    if (0 <= fingerxy.x && fingerxy.x <= 400 && 0 <= fingerxy.y && fingerxy.y < 120) {
        return menuLeft;    //返回向左
    }

    if (fingerxy.x > 400 && fingerxy.y < 120) {
        return menuRight;    //返回向右
    }

    return -1;
}
int imageTouch() {  // 图库菜单触摸判断
    touchCoord fingerxy = fingerRelease();
    if (fingerxy.y > 320)
    {
        if (fingerxy.x < 250)
        {
            return ImageShow;
        }
        else if (fingerxy.x > 300 && fingerxy.x < 500)
        {
            return ImageLook;
        }
        else if (fingerxy.x > 550)
        {
            return ImageBack;
        }
    }


}