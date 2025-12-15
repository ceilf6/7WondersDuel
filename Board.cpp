/* Board.cpp */
#include "Board.h"
#include <iomanip>

Board::Board() {}

void Board::initAge1(std::vector<std::shared_ptr<Card>>& deck) {
    pyramid.clear();
    int cardIdx = 0;
    // 简单的 5 行金字塔结构 (2-3-4-5-6)
    int rows = 5;
    int cardsInRow[] = {2, 3, 4, 5, 6};
    
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cardsInRow[r]; ++c) {
            if (cardIdx >= deck.size()) break;
            // 奇数行背面朝下，偶数行正面朝上
            bool faceUp = (r % 2 == 0);
            pyramid.push_back(CardSlot(deck[cardIdx], faceUp, r, c));
            cardIdx++;
        }
    }
}

bool Board::isBlocked(int index) const {
    const CardSlot& target = pyramid[index];
    if (target.row == 4) return false; // 底层无遮挡

    for (const auto& slot : pyramid) {
        if (!slot.isRemoved) {
            // 简单的压盖逻辑：下一行的 (col) 和 (col+1) 压住上一行的 (col)
            if (slot.row == target.row + 1) {
                if (slot.col == target.col || slot.col == target.col + 1) {
                    return true;
                }
            }
        }
    }
    return false;
}

std::vector<int> Board::getAvailableCardIndices() const {
    std::vector<int> indices;
    for (size_t i = 0; i < pyramid.size(); ++i) {
        if (!pyramid[i].isRemoved && pyramid[i].isFaceUp && !isBlocked(i)) {
            indices.push_back(i);
        }
    }
    return indices;
}

const CardSlot& Board::getSlot(int index) const {
    return pyramid[index];
}

std::shared_ptr<Card> Board::removeCard(int index) {
    if (index < 0 || index >= pyramid.size()) return nullptr;
    if (pyramid[index].isRemoved) return nullptr;

    pyramid[index].isRemoved = true;
    std::shared_ptr<Card> takenCard = pyramid[index].card;

    // 翻牌逻辑：检查被拿走牌上方的牌是否解除了锁定
    for (size_t i = 0; i < pyramid.size(); ++i) {
        if (!pyramid[i].isRemoved && !pyramid[i].isFaceUp) {
            if (!isBlocked(i)) {
                pyramid[i].isFaceUp = true;
                std::cout << Color::MAGENTA << "[!] A card has been revealed!" << Color::RESET << std::endl;
            }
        }
    }
    return takenCard;
}

void Board::display() const {
    std::cout << "\n" << Color::BOLD << "=== THE PYRAMID ===" << Color::RESET << "\n\n";
    int currentRow = -1;
    for (size_t i = 0; i < pyramid.size(); ++i) {
        const auto& slot = pyramid[i];
        if (slot.row != currentRow) {
            currentRow = slot.row;
            std::cout << "\n";
            int indent = (5 - currentRow) * 6; 
            for (int k = 0; k < indent; ++k) std::cout << " ";
        }

        if (slot.isRemoved) {
            std::cout << " [      ]   ";
        } else if (!slot.isFaceUp) {
            std::cout << Color::GREY << " [ BACK ]   " << Color::RESET;
        } else {
            bool avail = !isBlocked(i);
            if (avail) std::cout << Color::GREEN;
            std::cout << "[" << std::setw(2) << i << ": " << slot.card->getName().substr(0, 4) << "]   ";
            if (avail) std::cout << Color::RESET;
        }
    }
    std::cout << "\n\n";
}