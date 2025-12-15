/* SmartAI.cpp */
#include "SmartAI.h"
#include <iostream>
#include <algorithm>

SmartAI::SmartAI(std::string name) : Player(name) {}

// 核心估值函数
int SmartAI::evaluateCard(const std::shared_ptr<Card>& card) {
    int score = 0;

    // 1. 资源卡评估 (最优先)
    if (card->getType() == CardType::RAW_MATERIAL || card->getType() == CardType::MANUFACTURED) {
        auto rc = std::dynamic_pointer_cast<ResourceCard>(card);
        if (rc) {
            bool needIt = false;
            // 检查这卡产出的资源我是否已经有了
            for (auto r : rc->getProduction()) {
                if (getResourceCount(r) == 0) needIt = true; 
            }
            if (needIt) score += w.resource_value; // 缺资源？买！
            else score += 20; // 不缺？也可以买来囤积
        }
    }

    // 2. 科技卡评估
    if (card->getType() == CardType::SCIENTIFIC) {
        auto sc = std::dynamic_pointer_cast<ScienceCard>(card);
        if (sc) {
            // 如果我有这个符号了，价值低；没有，价值高
            if (!hasScienceSymbol(sc->getSymbol())) score += w.science_value;
            else score += 10;
        }
    }

    // 3. 军事卡评估
    if (card->getType() == CardType::MILITARY) {
        auto mc = std::dynamic_pointer_cast<MilitaryCard>(card);
        if (mc) score += (mc->getShields() * w.military_value);
    }

    // 4. 平民/公会卡评估 (纯分数)
    if (card->getType() == CardType::CIVILIAN) {
        auto cc = std::dynamic_pointer_cast<CivilianCard>(card);
        if (cc) score += (cc->getVictoryPoints() * w.vp_value);
    }
    // 简单处理公会卡，假设它平均值很高
    if (card->getType() == CardType::GUILD) {
        score += 80; 
    }

    // 5. 成本惩罚 (估算)
    // 越贵的牌，分数扣得越多。防止 AI 乱买买不起的牌。
    int estimatedCost = card->getCostCoins();
    // 简单的资源缺口估算 (每缺一个资源假设要花3块钱)
    for(auto const& [res, amt] : card->getResourceCost()) {
        if(getResourceCount(res) < amt) estimatedCost += 3;
    }
    
    score -= (estimatedCost * w.coin_penalty);

    return score;
}

int SmartAI::makeDecision(const std::vector<int>& availableIndices, const Board& board) {
    if (availableIndices.empty()) return -1;

    std::cout << ">> " << name << " (SmartAI) evaluating " << availableIndices.size() << " cards... ";

    int bestIndex = -1;
    int maxScore = -99999; // 初始化为负无穷

    for (int idx : availableIndices) {
        auto card = board.getSlot(idx).card;
        
        // 计算分数
        int currentScore = evaluateCard(card);
        
        // 加入微小的随机波动 (0-5分)，避免 AI 行为过于机械
        currentScore += (rand() % 5);

        // 择优
        if (currentScore > maxScore) {
            maxScore = currentScore;
            bestIndex = idx;
        }
    }

    // 打印简报 (可选，调试用)
    // std::cout << "Best Score: " << maxScore << " "; 

    return bestIndex;
}