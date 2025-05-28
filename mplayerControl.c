#include "mplayerControl.h"
#include "touch.h"
/* 视频播放控制 */
void playVideoWithControl(char** videos, int videoNumber) {
    if (videoNumber == 0) {
        printf("No videos to play.\n");
        return;
    }

    int fingerType = 0;
    FILE* mplayer = NULL;
    int current = 0;
    char command[512];
    initTouch();

    while (current >= 0 && current < videoNumber) {
        // 构建并启动 MPlayer 命令
        snprintf(command, sizeof(command), "mplayer -slave -quiet \"%s\"", videos[current]);
        mplayer = popen(command, "w");
        if (!mplayer) {
            perror("Failed to start mplayer");
            break;
        }

        int shouldContinue = 1;
        while (shouldContinue) {
            fingerType = fingerNEWS();
            switch (fingerType) {
            case 8: // 暂停/继续
                fprintf(mplayer, "pause\n");
                fflush(mplayer);
                break;

            case 2: // 下一视频
                shouldContinue = 0;
                if (current < videoNumber - 1) current++;
                break;

            case 1: // 上一视频
                shouldContinue = 0;
                if (current > 0) current--;
                break;

            case 6: // 快进 5 秒
                fprintf(mplayer, "seek +5\n");
                fflush(mplayer);
                break;

            case 5: // 快退 5 秒
                fprintf(mplayer, "seek -5\n");
                fflush(mplayer);
                break;

            case 7: // 退出
                fprintf(mplayer, "quit\n");
                fflush(mplayer);
                pclose(mplayer);
                system("killall mplayer >/dev/null 2>&1"); // 确保所有mplayer进程终止
                system("reset"); // 重置终端状态
                return;

            default:
                break;
            }

            // 检查mplayer是否仍在运行
            if (feof(mplayer) || ferror(mplayer)) {
                shouldContinue = 0;
                if (current < videoNumber - 1) {
                    current++; // 自动播放下一个
                }
                else {
                    current = -1; // 结束播放
                }
            }
        }

        // 关闭当前mplayer实例
        if (mplayer) {
            fprintf(mplayer, "quit\n");
            fflush(mplayer);
            pclose(mplayer);
            mplayer = NULL;
        }
    }

    // 最终清理
    system("killall mplayer >/dev/null 2>&1");
    system("reset");
}