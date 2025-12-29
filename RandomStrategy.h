#ifndef RANDOMSTRATEGY_H
#define RANDOMSTRATEGY_H

#include "AIStrategy.h"
#include <random>

/**
 * @brief 随机策略 - 从可用选项中随机选择
 *
 * 最简单的AI策略，用于基准测试和简单难度
 */
class RandomStrategy : public AIStrategy {
private:
    std::mt19937 rng;  // 随机数生成器

public:
    RandomStrategy();

    int selectCard(
        const std::vector<int>& availableIndices,
        const Board& board,
        const Player& self,
        const Player& opponent
    ) override;

    int selectCardToDestroy(
        const std::vector<std::shared_ptr<Card>>& targets
    ) override;

    int selectCardToRevive(
        const std::vector<std::shared_ptr<Card>>& discardPile
    ) override;

    int selectProgressToken(
        const std::vector<ProgressToken>& available
    ) override;

    int selectWhoStarts(
        const std::string& p1Name,
        const std::string& p2Name
    ) override;
};

#endif // RANDOMSTRATEGY_H
