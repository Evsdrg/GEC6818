#include "getFileName.h"

static char** _getSpecificFiles(const char* dir_path, int targetType, int* out_count);

int fileType(const char* fileName) {    // 判断文件类型
    const char* ext = strrchr(fileName, '.');
    if (!ext) {
        return otherFile;
    }
    else if (strcmp(ext, ".avi") == 0 || strcmp(ext, ".mp4") == 0) {
        return isVideo;
    }
    else if (strcmp(ext, ".bmp") == 0) {
        return isImage;
    }
    else if (strcmp(ext, ".mp3") == 0) {
        return isMusic;
    }
    return otherFile;
}

FileInfo* getFilesInfo(const char* dir_path, int* out_count) {  // 获取文件列表及类型信息
    DIR* dir;
    struct dirent* entry;
    int count = 0;      // 初始文件数
    int capacity = 16;  // 初始容量
    FileInfo* fileInfoList = malloc(capacity * sizeof(FileInfo));
    if ((dir = opendir(dir_path)) == NULL) {
        perror("无法打开目录");
        *out_count = 0;     // 文件数量为0
        free(fileInfoList);
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) {// 遍历每个文件
        int type = fileType(entry->d_name); // 判断文件类型
        if (type != otherFile) {
            if (count >= capacity) {    // 如果空间不足，扩大初始容量，然后重新分配内存
                capacity *= 2;
                fileInfoList = realloc(fileInfoList, capacity * sizeof(FileInfo));
            }
            /* 计算完整路径：dir_path + '/' + fileName所需长度 */
            int pathLen = strlen(dir_path) + strlen(entry->d_name) + 2;
            fileInfoList[count].filePath = malloc(pathLen); // 分配内存存储文件路径字符串
            snprintf( // 安全地将entry结构体中存储的文件名d_name与路径pathLen拼接，并存储到fileInfoList结构体中
                fileInfoList[count].filePath, pathLen,
                "%s/%s", dir_path, entry->d_name
            );
            fileInfoList[count].fileType = type;    // 存储文件类型
            count++;
        }
    }
    closedir(dir);  // 关闭目录

    *out_count = count; // 设置输出数量
    fileInfoList = realloc(fileInfoList, count * sizeof(FileInfo)); // 缩小内存至实际需要大小
    return fileInfoList;
}

/* 获取视频列表const char* dir_path及数量int* videoCount 记得释放内存*/
char** getVideo(const char* dir_path, int* videoCount) {
    return _getSpecificFiles(dir_path, isVideo, videoCount);
}
/* 获取图片列表const char* dir_path及数量int* videoCount 记得释放内存*/
char** getImage(const char* dir_path, int* imageCount) {
    return _getSpecificFiles(dir_path, isImage, imageCount);
}
/* 获取音乐列表const char* dir_path及数量int* videoCount 记得释放内存*/
char** getMusic(const char* dir_path, int* musicCount) {
    return _getSpecificFiles(dir_path, isMusic, musicCount);
}
/* 释放文件信息列表内存 */
void freeFilesInfo(FileInfo* fileInfoList, int count) {
    if (fileInfoList == NULL) {
        return;
    }
    for (int i = 0; i < count; ++i) {
        free(fileInfoList[i].filePath);
    }
    free(fileInfoList);
}
/* 释放获取方法的内存 */
void freeGet(char** list, int count) {
    if (list == NULL || count <= 0) {
        return;
    }
    for (int i = 0; i < count; ++i) {
        free(list[i]);  // 释放每个字符串
    }
    free(list);         // 释放数组本身
}

/* 通用筛选函数 */
static char** _getSpecificFiles(const char* dir_path, int targetType, int* out_count) {
    int fileCount;  // 文件总数
    FileInfo* files = getFilesInfo(dir_path, &fileCount);
    if (!files) {
        *out_count = 0; // 没有符合要求的文件
        return NULL;
    }

    int resultCount = 0;
    int capacity = 8;   // 初始空间大小
    char** result = malloc(capacity * sizeof(char*));   // 初始分配空间

    for (int i = 0; i < fileCount; ++i) {   // 匹配目标类型
        if (files[i].fileType == targetType) {
            if (resultCount >= capacity) {  // 如果空间不足，扩大初始容量，然后重新分配内存
                capacity *= 2;
                result = realloc(result, capacity * sizeof(char*));
            }

            result[resultCount] = strdup(files[i].filePath);  // 复制文件路径
            resultCount++;
        }
    }

    freeFilesInfo(files, fileCount);    // 释放内存

    *out_count = resultCount;
    result = realloc(result, resultCount * sizeof(char*));  // 缩小至实际大小
    return result;
}
