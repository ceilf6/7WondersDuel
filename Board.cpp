#include "Board.h"
#include <iostream>
#include <iomanip>

Board::Board() {}

void Board::initAge(int age, std::vector<std::shared_ptr<Card>>& deck) {
    pyramid.clear();
    int cardIdx = 0;

    int rows = 5;
    // 默认 Age 1 和 Age 3 的形状
    std::vector<int> cardsInRow = {2, 3, 4, 5, 6};

    // [修正] Age 2 使用倒金字塔
    if (age == 2) {
        cardsInRow = {6, 5, 4, 3, 2};
    }

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cardsInRow[r]; ++c) {
            if (cardIdx >= deck.size()) break;

            bool faceUp = false;
            if (age == 2) {
                // Age 2 倒金字塔: 第 0, 2, 4 行是正面 (0行有6张, 2行有4张, 4行有2张)
                // 规则书: Age 2 第一行(6张)正面，第二行(5张)背面...
                // 实际上 Board 坐标 r=0 是顶部。
                // Age 2: Row 0(6)-FaceUp, Row 1(5)-FaceDown, Row 2(4)-FaceUp, Row 3(3)-FaceDown, Row 4(2)-FaceUp
                if (r % 2 == 0) faceUp = true;
            } else {
                // Age 1 & 3 正金字塔:
                // Row 0(2)-FaceUp, Row 1(3)-FaceDown, Row 2(4)-FaceUp...
                // 注意：原代码 Age 2 的奇偶逻辑可能写反了，这里统一修正为标准规则
                // 现在的逻辑：偶数行(0,2,4)总是正面，奇数行(1,3)总是背面。这适用于所有时代。
                if (r % 2 == 0) faceUp = true;
            }
            pyramid.push_back(CardSlot(deck[cardIdx++], faceUp, r, c));
        }
    }
}

// 遮挡判断
bool Board::isBlocked(int index) const {
    const CardSlot& target = pyramid[index];
    if (target.row == 4) return false; // 最底层(Row 4) 永远不会被“下方”压住，因为它是最底部

    // 判断当前是否为倒金字塔 (Age 2)
    // 依据：查看第一行(Row 0)是否有 6 张牌
    bool isInverted = false;
    if (!pyramid.empty() && pyramid[0].row == 0) {
        // 简单统计 Row 0 的数量
        int row0Count = 0;
        for(const auto& s : pyramid) if(s.row == 0) row0Count++;
        if (row0Count == 6) isInverted = true;
    }

    for (const auto& slot : pyramid) {
        // 寻找位于 target 下一行的牌 (row + 1)
        if (!slot.isRemoved && slot.row == target.row + 1) {
            if (isInverted) {
                // [修正] 倒金字塔遮挡逻辑 (上一层多，下一层少)
                // 下一层的 slot(r+1, c) 会压住 上一层的 target(r, c) 和 target(r, c+1)
                // 反过来：target(r, c) 被 slot(r+1, ?) 压住
                // 关系是：slot.col == target.col || slot.col == target.col - 1
                if (slot.col == target.col || slot.col == target.col - 1) {
                    return true;
                }
            } else {
                // 正金字塔遮挡逻辑 (上一层少，下一层多)
                // 下一层的 slot(r+1, c) 和 slot(r+1, c+1) 压住 上一层的 target(r, c)
                if (slot.col == target.col || slot.col == target.col + 1) {
                    return true;
                }
            }
        }
    }
    return false;
}

std::vector<int> Board::getAvailableCardIndices() const {
    std::vector<int> idxs;
    for (size_t i = 0; i < pyramid.size(); ++i) {
        if (!pyramid[i].isRemoved && pyramid[i].isFaceUp && !isBlocked(i)) {
            idxs.push_back(i);
        }
    }
    return idxs;
}

const CardSlot& Board::getSlot(int index) const { return pyramid[index]; }

std::shared_ptr<Card> Board::removeCard(int index) {
    if (index < 0 || index >= pyramid.size() || pyramid[index].isRemoved) return nullptr;
    pyramid[index].isRemoved = true;
    auto taken = pyramid[index].card;

    // 自动翻面逻辑
    for (size_t i = 0; i < pyramid.size(); ++i) {
        if (!pyramid[i].isRemoved && !pyramid[i].isFaceUp) {
            if (!isBlocked(i)) {
                pyramid[i].isFaceUp = true;
            }
        }
    }
    return taken;
}

void Board::addToDiscard(std::shared_ptr<Card> c) {
    discardPile.push_back(c);
}

const std::vector<std::shared_ptr<Card>>& Board::getDiscardPile() const {
    return discardPile;
}

std::shared_ptr<Card> Board::takeFromDiscard(int index) {
    if(index < 0 || index >= discardPile.size()) return nullptr;
    auto c = discardPile[index];
    discardPile.erase(discardPile.begin() + index);
    return c;
}

void Board::display() const {
    std::cout << "\n" << Color::BOLD << "==================== 游戏版图 ====================" << Color::RESET << "\n\n";

    // 判断是否倒金字塔用于 UI 缩进
    bool isInverted = false;
    if (!pyramid.empty()) {
        int row0Count = 0;
        for(const auto& s : pyramid) if(s.row == 0) row0Count++;
        if (row0Count == 6) isInverted = true;
    }

    int currentRow = -1;
    for (size_t i = 0; i < pyramid.size(); ++i) {
        const auto& slot = pyramid[i];

        if (slot.row != currentRow) {
            if (currentRow != -1) std::cout << "\n\n";
            currentRow = slot.row;

            // [修正] UI 缩进逻辑
            int maxCols = 6;
            int currentCols = 0;

            // 根据行号硬编码列数，或者动态计算
            if (isInverted) {
                // Age 2: 6, 5, 4, 3, 2
                if(currentRow==0) currentCols=6;
                else if(currentRow==1) currentCols=5;
                else if(currentRow==2) currentCols=4;
                else if(currentRow==3) currentCols=3;
                else currentCols=2;
            } else {
                // Age 1/3: 2, 3, 4, 5, 6
                if(currentRow==0) currentCols=2;
                else if(currentRow==1) currentCols=3;
                else if(currentRow==2) currentCols=4;
                else if(currentRow==3) currentCols=5;
                else currentCols=6;
            }

            int spaces = (maxCols - currentCols) * 10;
            if (spaces > 0) std::cout << std::string(spaces, ' ');
        }

        if (slot.isRemoved) {
            std::cout << "      [      ]      ";
        }
        else if (!slot.isFaceUp) {
            std::cout << Color::GREY << "      [ 背面 ]      " << Color::RESET;
        }
        else {
            bool avail = !isBlocked(i);
            if (avail) std::cout << Color::BOLD << Color::YELLOW << "->" << std::setw(2) << i << Color::RESET;
            else std::cout << "  " << std::setw(2) << i;

            slot.card->display();

            if (avail) std::cout << Color::BOLD << Color::YELLOW << "<-" << Color::RESET;
            else std::cout << "  ";

            std::cout << " ";
        }
    }
    std::cout << "\n\n" << Color::BOLD << "==================================================" << Color::RESET << "\n";
}