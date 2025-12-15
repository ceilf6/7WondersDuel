#include "Board.h"
#include <iostream>
#include <iomanip>

Board::Board() {}

void Board::initAge(int age, std::vector<std::shared_ptr<Card>>& deck) {
    pyramid.clear();
    int cardIdx = 0;

    // 定义每个时代的金字塔形状
    int rows = 5;
    int cardsInRow[] = {2, 3, 4, 5, 6};

    if (age == 3) { cardsInRow[0]=2; cardsInRow[1]=3; cardsInRow[2]=4; cardsInRow[3]=5; cardsInRow[4]=6; }

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cardsInRow[r]; ++c) {
            if (cardIdx >= deck.size()) break;
            // 遮挡逻辑：
            // Age 2: 奇数行(1,3)正面; Age 1/3: 偶数行(0,2,4)正面
            bool faceUp = false;
            if (age == 2) {
                if (r % 2 != 0) faceUp = true;
            } else {
                if (r % 2 == 0) faceUp = true;
            }
            pyramid.push_back(CardSlot(deck[cardIdx++], faceUp, r, c));
        }
    }
}

// 遮挡判断
bool Board::isBlocked(int index) const {
    const CardSlot& target = pyramid[index];
    if (target.row == 4) return false; // 最底层没被压

    for (const auto& slot : pyramid) {
        if (!slot.isRemoved && slot.row == target.row + 1) {
            if (slot.col == target.col || slot.col == target.col + 1) {
                return true;
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
                // std::cout << "Debug: 翻开 " << i << "\n";
            }
        }
    }
    return taken;
}

// ==========================================
// [修复部分] 补回丢失的弃牌堆逻辑实现
// ==========================================

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

// ==========================================
// [UI部分] 保持美化后的显示逻辑
// ==========================================

void Board::display() const {
    std::cout << "\n" << Color::BOLD << "==================== 游戏版图 ====================" << Color::RESET << "\n\n";

    int currentRow = -1;
    for (size_t i = 0; i < pyramid.size(); ++i) {
        const auto& slot = pyramid[i];

        if (slot.row != currentRow) {
            if (currentRow != -1) std::cout << "\n\n";
            currentRow = slot.row;

            // 计算缩进
            int maxCols = 6;
            int currentCols = 0;
            if(currentRow==0) currentCols=2;
            else if(currentRow==1) currentCols=3;
            else if(currentRow==2) currentCols=4;
            else if(currentRow==3) currentCols=5;
            else currentCols=6;

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