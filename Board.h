#ifndef BOARD_H
#define BOARD_H
#include <vector>
#include <memory>
#include "Card.h"

struct CardSlot {
    std::shared_ptr<Card> card;
    bool isFaceUp; bool isRemoved; int row; int col;
    CardSlot(std::shared_ptr<Card> c, bool faceUp, int r, int co) : card(c), isFaceUp(faceUp), isRemoved(false), row(r), col(co) {}
};

class Board {
    std::vector<CardSlot> pyramid;
    std::vector<std::shared_ptr<Card>> discardPile;
public:
    Board();
    void initAge(int age, std::vector<std::shared_ptr<Card>>& deck);
    bool isBlocked(int index) const;
    std::vector<int> getAvailableCardIndices() const;
    const CardSlot& getSlot(int index) const;
    std::shared_ptr<Card> removeCard(int index);

    void addToDiscard(std::shared_ptr<Card> c);
    const std::vector<std::shared_ptr<Card>>& getDiscardPile() const;
    std::shared_ptr<Card> takeFromDiscard(int index);

    void display() const;
};
#endif