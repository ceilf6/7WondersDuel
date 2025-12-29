#ifndef AISTRATEGY_H
#define AISTRATEGY_H

#include <vector>
#include <memory>
#include "Card.h"
#include "Board.h"

// 前向声明
class Player;

/**
 * @brief AI策略抽象接口 - Strategy Pattern
 *
 * 定义AI决策的统一接口，允许动态切换不同的AI策略
 */
class AIStrategy {
public:
    virtual ~AIStrategy() = default;

    /**
     * @brief 选择要拿取的卡牌
     * @param availableIndices 可用卡牌索引列表
     * @param board 游戏棋盘
     * @param self AI玩家自己
     * @param opponent 对手玩家
     * @return 选中的卡牌索引
     */
    virtual int selectCard(
        const std::vector<int>& availableIndices,
        const Board& board,
        const Player& self,
        const Player& opponent
    ) = 0;

    /**
     * @brief 选择要摧毁的卡牌
     * @param targets 可摧毁的卡牌列表
     * @return 选中的卡牌索引
     */
    virtual int selectCardToDestroy(
        const std::vector<std::shared_ptr<Card>>& targets
    ) = 0;

    /**
     * @brief 选择要复活的卡牌
     * @param discardPile 弃牌堆
     * @return 选中的卡牌索引
     */
    virtual int selectCardToRevive(
        const std::vector<std::shared_ptr<Card>>& discardPile
    ) = 0;

    /**
     * @brief 选择进步标记
     * @param available 可用的进步标记列表
     * @return 选中的标记索引
     */
    virtual int selectProgressToken(
        const std::vector<ProgressToken>& available
    ) = 0;

    /**
     * @brief 选择下一时代的先手玩家
     * @param p1Name 玩家1名称
     * @param p2Name 玩家2名称
     * @return 1表示选择玩家1，2表示选择玩家2
     */
    virtual int selectWhoStarts(
        const std::string& p1Name,
        const std::string& p2Name
    ) = 0;

protected:
    /**
     * @brief 评估卡牌价值（供子类使用）
     * @param card 要评估的卡牌
     * @param self AI玩家
     * @return 卡牌价值分数
     */
    virtual int evaluateCard(
        const std::shared_ptr<Card>& card,
        const Player& self
    );
};

#endif // AISTRATEGY_H
