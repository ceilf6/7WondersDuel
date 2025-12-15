#include "SmartAI.h"
#include <iostream>

int SmartAI::evaluateCard(const std::shared_ptr<Card>& card) {
    int score = 0;
    if (!card->getChainCost().empty() && ownedChains.count(card->getChainCost())) score += 200; // 免费极大加分
    if (!card->getProduction().empty()) {
        bool need = false;
        for(auto r : card->getProduction()) if(getResourceCount(r)==0) need=true;
        score += (need ? w.res : 15);
    }
    if (card->getDiscountResource() != Resource::NONE) score += 60;
    if (card->getSymbol() != ScienceSymbol::NONE) score += (hasScienceSymbol(card->getSymbol()) ? 10 : w.sci);
    
    score += (card->getShields() * w.mil);
    score += (card->getVictoryPoints() * w.vp);
    score -= (card->getCostCoins() * w.cost);
    return score;
}

int SmartAI::makeDecision(const std::vector<int>& idxs, const Board& board) {
    if(idxs.empty()) return -1;
    std::cout << ">> " << name << " (SmartAI) 正在思考...\n";
    int best = -1, maxS = -99999;
    for(int idx : idxs) {
        int s = evaluateCard(board.getSlot(idx).card);
        s += (rand() % 5); 
        if(s > maxS) { maxS = s; best = idx; }
    }
    return best;
}
int SmartAI::chooseCardToDestroy(const std::vector<std::shared_ptr<Card>>& t) { return t.empty()?-1:0; } 
int SmartAI::chooseCardToRevive(const std::vector<std::shared_ptr<Card>>& dp) { return dp.empty()?-1:dp.size()-1; } 
int SmartAI::chooseProgressToken(const std::vector<ProgressToken>& av) { return 0; }
int SmartAI::chooseWhoStarts(std::string p1Name, std::string p2Name) { return (name == p1Name) ? 1 : 2; }