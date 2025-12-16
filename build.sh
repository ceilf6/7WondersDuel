#!/bin/bash

# 7 Wonders Duel - 构建脚本
# 用法: ./build.sh [clean|run]

set -e  # 遇到错误立即退出

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 配置
OUTPUT="game"
SOURCES="main.cpp Game.cpp Board.cpp Card.cpp Player.cpp SmartAI.cpp"
COMPILER="g++"
FLAGS="-std=c++17 -Wall -Wextra"

echo -e "${YELLOW}=== 7 Wonders Duel 构建工具 ===${NC}\n"

# 清理
if [ "$1" == "clean" ]; then
    echo -e "${YELLOW}清理旧文件...${NC}"
    rm -f $OUTPUT *.o
    echo -e "${GREEN}✓ 清理完成${NC}"
    exit 0
fi

# 编译
echo -e "${YELLOW}编译中...${NC}"
echo "命令: $COMPILER $FLAGS $SOURCES -o $OUTPUT"
$COMPILER $FLAGS $SOURCES -o $OUTPUT

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ 编译成功！${NC}\n"
    
    # 自动运行
    if [ "$1" == "run" ]; then
        echo -e "${GREEN}启动游戏...${NC}\n"
        ./$OUTPUT
    else
        echo -e "运行游戏: ${GREEN}./$OUTPUT${NC}"
        echo -e "或使用: ${GREEN}./build.sh run${NC}"
    fi
else
    echo -e "${RED}✗ 编译失败${NC}"
    exit 1
fi
