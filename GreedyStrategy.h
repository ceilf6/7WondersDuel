#ifndef GREEDYSTRATEGY_H
#define GREEDYSTRATEGY_H

#include "AIStrategy.h"

/**
 * @brief 贪婪策略 - 选择评分最高的选项
 *
 * 基于简单的卡牌评分系统，总是选择当前价值最高的选项
 */
class GreedyStrategy : public AIStrategy {
public:
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

#endif // GREEDYSTRATEGY_H
