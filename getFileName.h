#ifndef GETFILENAME_H
#define GETFILENAME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fnmatch.h>

#define otherFile 0
#define isVideo 1
#define isMusic 2
#define isImage 3

// 文件信息结构体，char* filePath文件路径，int fileType文件类型
typedef struct {
    char* filePath;  // 文件路径
    int fileType;    // 文件类型
} FileInfo;

// 判断文件类型
int fileType(const char* fileName);
// 获取文件列表及类型信息，需要输入目录和用作计数的变量，用完后记得释放内存
FileInfo* getFilesInfo(const char* dir_path, int* out_count);
// 释放文件信息列表内存
void freeFilesInfo(FileInfo* fileInfoList, int count);
// 释放获取方法的内存
void freeGet(char** list, int count);

// 记得释放内存
char** getVideo(const char* dir_path, int* videoCount);
char** getImage(const char* dir_path, int* imageCount);
char** getMusic(const char* dir_path, int* musicCount);


#endif