#include "touch.h"

// 全局变量定义
int openTouch = -1;  // 初始化为 -1，表示未打开
int touchX0 = -1, touchY0 = -1, touchX1 = -1, touchY1 = -1;

// 初始化触摸设备
void initTouch(void) {
    if (openTouch == -1) {
        openTouch = open("/dev/input/event0", O_RDONLY | O_NONBLOCK);
        if (openTouch == -1) {
            perror("open touch failed");
        }
    }
}

touchCoord fingerTouch(void) {   // 检测触摸
    touchCoord coord = { -1, -1 };
    if (openTouch == -1) {
        printf("open touch failed\n");
        return coord;
    }
    struct input_event touch;
    int hasX = 0, hasY = 0;

    read(openTouch, &touch, sizeof(touch));
    if (touch.type == EV_ABS) {
        if (touch.code == ABS_X) {
            coord.x = touch.value;
            hasX = 1;
        }
        else if (touch.code == ABS_Y) {
            coord.y = touch.value;
            hasY = 1;
        }
    }
    if (touch.type == EV_SYN && touch.code == SYN_REPORT && hasX && hasY) {
        return coord;
    }
    return coord;

}
touchCoord fingerRelease(void) {
    touchCoord coord = { -1,-1 };
    struct input_event touch;
    if (openTouch == -1) {
        printf("open touch failed\n");
        return coord;
    }
    int hasX = 0, hasY = 0;
    
    // 循环读取直到获取完整的X和Y坐标或检测到抬起事件
    while (read(openTouch, &touch, sizeof(touch)) >= (ssize_t)sizeof(touch)) {
        if (touch.type == EV_ABS) {
            if (touch.code == ABS_X) {
                coord.x = touch.value;
                hasX = 1;
            }
            else if (touch.code == ABS_Y) {
                coord.y = touch.value;
                hasY = 1;
            }
        }
        if (touch.type == EV_KEY && touch.code == BTN_TOUCH && touch.value == 0) {
            // 如果检测到抬起事件，返回当前坐标
            return coord;
        }
        if (touch.type == EV_SYN && touch.code == SYN_REPORT && hasX && hasY) {
            // 如果收到同步事件且已有X和Y坐标，返回坐标
            return coord;
        }
    }
    return coord;
}


#define TOUCH_NO_ACTION 0
#define TOUCH_ERROR -1

// 辅助函数，用于重置滑动状态变量
static void reset_swipe_state() {
    touchX0 = -1;
    touchY0 = -1;
    touchX1 = -1;
    touchY1 = -1;
}

int fingerNEWS(void) {  // 检测手指滑动方向或点击
    struct input_event ev;
    int action = TOUCH_NO_ACTION;
    static int current_x = -1, current_y = -1; // 用于记录当前坐标
    static int pressing = 0; // 标记是否处于按下状态

    if (openTouch == -1) {
        return TOUCH_ERROR;
    }

    // 尝试读取一个事件，非阻塞
    ssize_t bytes_read = read(openTouch, &ev, sizeof(ev));

    if (bytes_read < (ssize_t)sizeof(ev)) {
        if (bytes_read == 0 || (bytes_read < 0 && errno == EAGAIN)) {
            // 没有事件或资源暂时不可用 (EAGAIN for non-blocking)
            return TOUCH_NO_ACTION; 
        }
        return TOUCH_ERROR; // 返回了错误信息
    }

    if (ev.type == EV_ABS) {
        if (ev.code == ABS_X) {
            current_x = ev.value;
            if (pressing && touchX0 == -1) { // 如果已按下且起点未记录
                touchX0 = ev.value;
            }
            touchX1 = ev.value; // 持续更新当前点作为可能的终点
        } else if (ev.code == ABS_Y) {
            current_y = ev.value;
            if (pressing && touchY0 == -1) { // 如果已按下且起点未记录
                touchY0 = ev.value;
            }
            touchY1 = ev.value; // 持续更新当前点作为可能的终点
        }
    }
    else if (ev.type == EV_KEY && ev.code == BTN_TOUCH) {
        if (ev.value == 1) { // 按下
            pressing = 1;
            reset_swipe_state(); // 重置滑动起点
            // 记录按下的初始点，即使它可能被EV_ABS覆盖，确保touchX0/Y0在按下时被设置
            // 这需要确保ABS_X/Y事件通常紧随BTN_TOUCH press事件
            // 或者在第一次ABS事件时记录起点（如当前逻辑）
            if(current_x != -1) touchX0 = current_x;
            if(current_y != -1) touchY0 = current_y;

        } else if (ev.value == 0) { // 抬起
            pressing = 0;
            if (touchX0 != -1 && touchY0 != -1 && touchX1 != -1 && touchY1 != -1) {
                int xCZ = touchX1 - touchX0;
                int yCZ = touchY1 - touchY0;
                int xABS = abs(xCZ);
                int yABS = abs(yCZ);

                // 滑动判断 (阈值可以调整)
                if (xABS > 80 && xABS > 2 * yABS) {
                    action = (xCZ > 0) ? moveRight : moveLeft;
                } else if (yABS > 60 && yABS > 2 * xABS) {
                    action = (yCZ > 0) ? moveDown : moveUp;
                } else {
                    // 非滑动，判断为点击区域 (touchX1, touchY1 是抬起时的坐标)
                    // 这里的区域判断逻辑可以提取成辅助函数或更清晰的结构
                    if (touchX1 > 400 && touchY1 > (480 - touchX1 * 480 / 800) && touchY1 < (touchX1 * 480 / 800)) {
                        // printf("Area: Right Click\n");
                        action = Right; // 使用 touch.h 中的 Right (6)
                    } else if (touchX1 < 400 && touchY1 > (touchX1 * 480 / 800) && touchY1 < (480 - touchX1 * 480 / 800)) {
                        // printf("Area: Left Click\n");
                        action = Left; // 使用 touch.h 中的 Left (5)
                    } else if (touchY1 < 240) {
                        // printf("Area: Up Click\n");
                        action = Up;   // 使用 touch.h 中的 Up (7)
                    } else {
                        // printf("Area: Down Click\n");
                        action = Down; // 使用 touch.h 中的 Down (8)
                    }
                }
            } else {
                 // 如果没有足够的滑动信息 (例如只是快速点击，没有明显的X0,Y0,X1,Y1)
                 // 可以根据 current_x, current_y (即抬起时的点) 判断是否为简单点击，并赋予特定action
                 // 或者依赖上面的区域判断逻辑（如果touchX0/Y0被正确设置为按下时的点）
            }
            reset_swipe_state();
            current_x = -1; // 重置当前点记录
            current_y = -1;
            return action; // 只有在抬起时才返回确定的动作
        }
    }
    else if (ev.type == EV_SYN && ev.code == SYN_REPORT) {
        // 同步事件，通常表示一批ABS事件结束
        // 可以在这里更新最终的 touchX1, touchY1 (如果需要滑动过程中的精确点)
        // 但对于fingerNEWS，主要关注抬起时的最终判断
    }
    return TOUCH_NO_ACTION; // 如果没有发生抬起事件，则不返回具体动作
}
