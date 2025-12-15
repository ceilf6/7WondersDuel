/* SmartAI.h */
#ifndef SMART_AI_H
#define SMART_AI_H

#include "Player.h"
#include "Board.h"

class SmartAI : public Player {
private:
    // 决策权重参数 (可以调整这些数值来改变 AI 性格)
    struct Weights {
        int resource_value = 120;   // 资源卡的价值 (很高，为了生存)
        int science_value = 80;     // 科技卡的价值
        int military_value = 50;    // 军事卡的价值 (每盾牌)
        int vp_value = 10;          // 每一分 VP 的价值
        int coin_penalty = 5;       // 每花费1金币扣多少分
    } w;

public:
    SmartAI(std::string name);

    // 覆盖基类的决策函数
    int makeDecision(const std::vector<int>& availableIndices, const Board& board) override;

    // 辅助：给卡牌打分
    int evaluateCard(const std::shared_ptr<Card>& card);
};

#endif // SMART_AI_H