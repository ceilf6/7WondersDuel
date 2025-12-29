#!/bin/bash

# 7 Wonders Duel - 构建脚本
# 用法: ./build.sh [clean|run]

set -e  # 遇到错误立即退出

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 配置
OUTPUT="7WondersDuel"
BUILD_DIR="build"
COMPILER="g++"
FLAGS="-std=c++20 -Wall -Wextra"

# 源文件列表（按字母顺序）
SOURCES="
    AIStrategy.cpp
    AgoraCardFactory.cpp
    BaseGameCardFactory.cpp
    Board.cpp
    Card.cpp
    CardFactory.cpp
    Game.cpp
    GameBuilder.cpp
    GameConfig.cpp
    GreedyStrategy.cpp
    main.cpp
    PantheonCardFactory.cpp
    Player.cpp
    RandomStrategy.cpp
    SmartAI.cpp
"

echo -e "${BLUE}=== 7 Wonders Duel 构建工具 ===${NC}\n"

# 清理
if [ "$1" == "clean" ]; then
    echo -e "${YELLOW}清理旧文件...${NC}"
    rm -f $OUTPUT *.o
    rm -rf $BUILD_DIR
    echo -e "${GREEN}✓ 清理完成${NC}"
    exit 0
fi

# 使用CMake构建（推荐）
if command -v cmake &> /dev/null; then
    echo -e "${YELLOW}使用 CMake 构建...${NC}"

    # 创建构建目录
    if [ ! -d "$BUILD_DIR" ]; then
        mkdir -p $BUILD_DIR
    fi

    cd $BUILD_DIR

    # 配置
    echo -e "${YELLOW}配置 CMake...${NC}"
    cmake .. > /dev/null 2>&1

    # 编译
    echo -e "${YELLOW}编译中...${NC}"
    make -j$(nproc 2>/dev/null || echo 4)

    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ 编译成功！${NC}\n"

        # 复制到项目根目录（可选）
        cp $OUTPUT ../ 2>/dev/null || true

        cd ..

        # 自动运行
        if [ "$1" == "run" ]; then
            echo -e "${GREEN}启动游戏...${NC}\n"
            ./$BUILD_DIR/$OUTPUT
        else
            echo -e "运行游戏: ${GREEN}./$BUILD_DIR/$OUTPUT${NC}"
            echo -e "或使用: ${GREEN}./build.sh run${NC}"
        fi
    else
        echo -e "${RED}✗ 编译失败${NC}"
        cd ..
        exit 1
    fi
else
    # 备用：直接使用g++编译
    echo -e "${YELLOW}CMake 未安装，使用 g++ 直接编译...${NC}"
    echo -e "${BLUE}建议安装 CMake 以获得更好的构建体验${NC}\n"

    echo -e "${YELLOW}编译中...${NC}"
    echo "命令: $COMPILER $FLAGS $SOURCES -o $OUTPUT"

    # 执行编译
    $COMPILER $FLAGS $SOURCES -o $OUTPUT

    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ 编译成功！${NC}\n"

        # 自动运行
        if [ "$1" == "run" ]; then
            echo -e "${GREEN}启动游戏...${NC}\n"
            ./$OUTPUT
        else
            echo -e "运行游戏: ${GREEN}./build.sh run${NC}"
        fi
    else
        echo -e "${RED}✗ 编译失败${NC}"
        exit 1
    fi
fi
