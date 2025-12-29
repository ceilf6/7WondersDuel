#include "GreedyStrategy.h"
#include "Player.h"

int GreedyStrategy::selectCard(
    const std::vector<int>& availableIndices,
    const Board& board,
    const Player& self,
    const Player& opponent
) {
    if (availableIndices.empty()) return -1;

    int bestIndex = availableIndices[0];
    int bestScore = -1;

    for (int idx : availableIndices) {
        auto card = board.getSlot(idx).card;
        int score = evaluateCard(card, self);

        if (score > bestScore) {
            bestScore = score;
            bestIndex = idx;
        }
    }

    return bestIndex;
}

int GreedyStrategy::selectCardToDestroy(
    const std::vector<std::shared_ptr<Card>>& targets
) {
    if (targets.empty()) return -1;

    // 摧毁对手最有价值的卡牌
    // 这里简单地选择第一张资源卡或制造卡
    for (size_t i = 0; i < targets.size(); ++i) {
        auto type = targets[i]->getType();
        if (type == CardType::RAW_MATERIAL || type == CardType::MANUFACTURED) {
            return i;
        }
    }

    return 0;  // 如果没有资源卡，选第一张
}

int GreedyStrategy::selectCardToRevive(
    const std::vector<std::shared_ptr<Card>>& discardPile
) {
    if (discardPile.empty()) return -1;

    // 简化实现：选择第一张科技卡或资源卡
    for (size_t i = 0; i < discardPile.size(); ++i) {
        auto type = discardPile[i]->getType();
        if (type == CardType::SCIENTIFIC || type == CardType::RAW_MATERIAL) {
            return i;
        }
    }

    return 0;
}

int GreedyStrategy::selectProgressToken(
    const std::vector<ProgressToken>& available
) {
    if (available.empty()) return -1;

    // 简单策略：优先选择"法律"或"数学"
    for (size_t i = 0; i < available.size(); ++i) {
        if (available[i].type == TokenType::LAW ||
            available[i].type == TokenType::MATHEMATICS) {
            return i;
        }
    }

    return 0;
}

int GreedyStrategy::selectWhoStarts(
    const std::string& p1Name,
    const std::string& p2Name
) {
    // 简单策略：总是选择让自己先手（假设AI是玩家2）
    // 这个逻辑需要根据实际情况调整
    return 2;
}
