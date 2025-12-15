#ifndef SMART_AI_H
#define SMART_AI_H
#include "Player.h"
#include "Board.h"

class SmartAI : public Player {
    struct Weights { int res=100, sci=90, mil=60, vp=10, cost=5; } w;
public:
    SmartAI(std::string name) : Player(name) {}
    int makeDecision(const std::vector<int>& availableIndices, const Board& board) override;
    int chooseCardToDestroy(const std::vector<std::shared_ptr<Card>>& targets) override;
    int chooseCardToRevive(const std::vector<std::shared_ptr<Card>>& discardPile) override;
    int chooseProgressToken(const std::vector<ProgressToken>& available) override;
    int chooseWhoStarts(std::string p1Name, std::string p2Name) override;
    int evaluateCard(const std::shared_ptr<Card>& card);
};
#endif