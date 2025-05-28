#!/bin/bash

# 工具链配置
CC="arm-linux-gcc"

# 性能优先的编译参数（适用于 ARMv7 架构）
CFLAGS="-Wextra -O3 -march=armv7-a -mfpu=neon -mfloat-abi=softfp -funroll-loops -ftree-vectorize"

# 源文件和输出文件
SRC="2048.c bmp.c getFileName.c main.c menuTouch.c mplayerControl.c showMenu.c touch.c"
EXE="app"

echo "🚀 正在为 S5P6818 开发板交叉编译 $SRC..."

# 执行编译命令
$CC $SRC -o "$EXE" $CFLAGS

# 检查是否成功
if [ $? -eq 0 ]; then
    echo "✅ 编译成功: ./$EXE"
    echo "编译时间: $(date +"%Y-%m-%d %H:%M:%S")"
else
    echo "❌ 编译失败，请检查源代码或工具链配置。"
    exit 1
fi