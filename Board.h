/* Board.h */
#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <memory>
#include <iostream>
#include "Card.h"
#include "Commons.h"

// 卡槽结构
struct CardSlot {
    std::shared_ptr<Card> card;
    bool isFaceUp;
    bool isRemoved;
    int row;
    int col;

    CardSlot(std::shared_ptr<Card> c, bool faceUp, int r, int co)
        : card(c), isFaceUp(faceUp), isRemoved(false), row(r), col(co) {}
};

class Board {
private:
    std::vector<CardSlot> pyramid;

public:
    Board();
    
    // 初始化金字塔
    void initAge1(std::vector<std::shared_ptr<Card>>& deck);

    // 检查卡牌是否被压住
    bool isBlocked(int index) const;

    // 获取所有可选卡牌的索引
    std::vector<int> getAvailableCardIndices() const;

    // 获取指定卡槽 (用于 AI 分析和 UI 显示)
    const CardSlot& getSlot(int index) const;

    // 拿走卡牌
    std::shared_ptr<Card> removeCard(int index);

    // 显示金字塔
    void display() const;
};

#endif // BOARD_H