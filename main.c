#include "bmp.h"
#include "touch.h"
#include "getFileName.h"
#include "mplayerControl.h"
#include "showMenu.h"
#include "menuTouch.h"
#include "2048.h"

void showImage(void);
void showVideo(int type);

int main(void) {
    int background = 1; // 默认背景
    int menu = 0;
    initTouch();    // 触摸初始化
    int* fb = openLcd();
    showBmp("./defaultImage/open.bmp", fb); // 开机动画
    munmap(fb, 800 * 480 * 4);
    sleep(1);
    backgroundFlyIn("./menuBackground/1.bmp");
    while (1) {
        showMenu(background);
        menu = 0;
        while (1) {
            menu = menuTouch(); // 获取功能
            if (menu >= 12 && menu <= 18) {
                break;
            }
        }
        switch (menu) {   // 主菜单各项功能
        case menuImage:
            backgroundFlyIn("./defaultImage/image.bmp");
            showImage();
            menu = 0;
            break;

        case menuVideo: // 视频
            backgroundFlyIn("./defaultImage/black.bmp");
            showVideo(1);
            menu = 0;
            break;

        case menuMusic: // 音乐
            backgroundFlyIn("./defaultImage/music.bmp");
            showVideo(2);
            menu = 0;
            break;

        case menuGame:  // 游戏
            game2048();
            sleep(1);
            backgroundFlyIn("./defaultImage/black.bmp");
            menu = 0;
            break;

        case menuQuit:  // 退出
            backgroundFlyIn("./defaultImage/quit.bmp");
            sleep(1);
            showBmp("./defaultImage/black.bmp", openLcd());
            return 0;

        case menuLeft:  // 切换背景
            background = settingBackgroundPlus(background);
            menu = 0;
            break;

        case menuRight: // 切换背景
            background = settingBackgroundMinus(background);
            menu = 0;
            break;

        }
        menu = 0;
    }
}

void showImage(void) {  // 查看图片
    int showImageExpression = 0, imageNumber = 0;
    int NEWS = 0, num = 0, stop = 0;
    char** bmp = getImage("image", &imageNumber);

    while (1) {
        showImageExpression = imageTouch(); // 获取功能
        if (showImageExpression >= 19 && showImageExpression <= 21) {
            break;
        }
    }
    if (imageNumber == 0) { // 无图判断
        printf("暂无图片\n");
        freeGet(bmp, imageNumber);
        return;
    }
    else {
        int* fb = openLcd();
        if (!fb) {
            printf("打开LCD失败\n");
            freeGet(bmp, imageNumber);
            return;
        }
        while (1) {
            switch (showImageExpression) {   // 选择展示方式
            case ImageShow:     // 自动按顺序显示
                for (int num = 0;num < imageNumber;num++) {
                    showBmp(bmp[num], fb);
                    sleep(1);
                    if (num + 3 < imageNumber) {
                        num++;
                        picToLeft(bmp[num], fb);
                        sleep(1);
                        num++;
                        picCircularSpread(bmp[num], fb);
                        sleep(1);
                    }
                    else if (num + 2 < imageNumber) {
                        num++;
                        picUp(bmp[num], fb);
                        sleep(1);
                        num++;
                        picToRight(bmp[num], fb);
                        sleep(1);
                    }
                    else {
                        num++;
                        picCombineIntoOne(bmp[num], fb);
                        sleep(1);
                    }
                }
                munmap(fb, 800 * 480 * 4);
                freeGet(bmp, imageNumber);
                return;

            case ImageLook:     // 手动滑动显示
                while (1) {
                    NEWS = fingerNEWS();
                    if (NEWS == moveLeft) {
                        stop = 0;
                        if (num > 0) {
                            num--;
                        }
                        picToLeft(bmp[num], fb);
                    }
                    else if (NEWS == moveRight) {
                        stop = 0;
                        if (num < imageNumber - 1) {
                            num++;
                        }
                        picToRight(bmp[num], fb);
                    }
                    else if (NEWS == moveUp) {
                        stop = 0;
                        if (num > 0) {
                            num--;
                        }
                        showBmp(bmp[num], fb);
                    }
                    else if (NEWS == moveDown) {
                        stop++;
                        if (num < imageNumber - 1) {
                            num++;
                        }
                        showBmp(bmp[num], fb);
                    }
                    if (stop == 3) {
                        break;
                    }
                }
                munmap(fb, 800 * 480 * 4);
                freeGet(bmp, imageNumber);
                return;

            case ImageBack:
                munmap(fb, 800 * 480 * 4);
                freeGet(bmp, imageNumber);
                
                return;
            }
        }
    }
}

void showVideo(int type) {
    int videoNumber = 0;
    char** media;
    if (type == 1) {
        media = getVideo("video", &videoNumber);
    }
    else {
        media = getMusic("music", &videoNumber);
    }
    playVideoWithControl(media, videoNumber);

    // 关闭播放器
    freeGet(media, videoNumber);

}


