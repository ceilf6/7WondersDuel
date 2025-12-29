#ifndef CARDVISITOR_H
#define CARDVISITOR_H

#include "Card.h"
#include "Player.h"
#include <memory>

/**
 * @brief 卡牌访问者接口 - Visitor Pattern
 *
 * 用于在不修改Card类的情况下添加新的操作
 * 主要应用：工会卡得分计算、统计分析等
 */
class CardVisitor {
public:
    virtual ~CardVisitor() = default;

    virtual void visit(const MilitaryCard& card) = 0;
    virtual void visit(const CivilianCard& card) = 0;
    virtual void visit(const ScienceCard& card) = 0;
    virtual void visit(const ResourceCard& card) = 0;
    virtual void visit(const CommercialCard& card) = 0;
    virtual void visit(const GuildCard& card) = 0;
};

/**
 * @brief 工会卡得分计算访问者
 *
 * 根据工会卡类型和玩家状态计算得分
 *
 * 工会卡规则：
 * - Merchants: 自己和对手每张黄卡1分
 * - Shipowners: 自己和对手每张棕/灰卡1分
 * - Scientists: 自己和对手每张绿卡1分
 * - Magistrates: 自己和对手每张蓝卡1分
 * - Tacticians: 自己和对手每张红卡1分
 * - Builders: 自己和对手每个已建奇迹2分
 * - Moneylenders: 自己和对手每3金币1分
 */
class GuildScoreVisitor : public CardVisitor {
private:
    const Player& owner;      // 拥有工会卡的玩家
    const Player& opponent;   // 对手
    int totalScore = 0;       // 累计得分

public:
    GuildScoreVisitor(const Player& own, const Player& opp)
        : owner(own), opponent(opp) {}

    void visit(const GuildCard& card) override {
        std::string type = card.getGuildType();

        if (type == "Merchants") {
            // 商人工会：每张黄卡1分
            int yellowCount = owner.getCardCount(CardType::COMMERCIAL) +
                             opponent.getCardCount(CardType::COMMERCIAL);
            totalScore += yellowCount;
        }
        else if (type == "Shipowners") {
            // 船主工会：每张棕/灰卡1分
            int brownGrey = owner.getCardCount(CardType::RAW_MATERIAL) +
                           owner.getCardCount(CardType::MANUFACTURED) +
                           opponent.getCardCount(CardType::RAW_MATERIAL) +
                           opponent.getCardCount(CardType::MANUFACTURED);
            totalScore += brownGrey;
        }
        else if (type == "Scientists") {
            // 科学家工会：每张绿卡1分
            int greenCount = owner.getCardCount(CardType::SCIENTIFIC) +
                            opponent.getCardCount(CardType::SCIENTIFIC);
            totalScore += greenCount;
        }
        else if (type == "Magistrates") {
            // 行政官工会：每张蓝卡1分
            int blueCount = owner.getCardCount(CardType::CIVILIAN) +
                           opponent.getCardCount(CardType::CIVILIAN);
            totalScore += blueCount;
        }
        else if (type == "Tacticians") {
            // 战术家工会：每张红卡1分
            int redCount = owner.getCardCount(CardType::MILITARY) +
                          opponent.getCardCount(CardType::MILITARY);
            totalScore += redCount;
        }
        else if (type == "Builders") {
            // 建造者工会：每个已建奇迹2分
            int wonderCount = 0;
            for (const auto& wonder : owner.getWonders()) {
                if (wonder.isBuilt) wonderCount++;
            }
            for (const auto& wonder : opponent.getWonders()) {
                if (wonder.isBuilt) wonderCount++;
            }
            totalScore += wonderCount * 2;
        }
        else if (type == "Moneylenders") {
            // 放贷人工会：每3金币1分
            int coinsTotal = owner.getCoins() + opponent.getCoins();
            totalScore += coinsTotal / 3;
        }
    }

    // 其他卡牌类型不处理
    void visit(const MilitaryCard& card) override {}
    void visit(const CivilianCard& card) override {}
    void visit(const ScienceCard& card) override {}
    void visit(const ResourceCard& card) override {}
    void visit(const CommercialCard& card) override {}

    /**
     * @brief 获取计算出的总得分
     */
    int getTotalScore() const {
        return totalScore;
    }
};

/**
 * @brief 卡牌统计访问者（示例）
 *
 * 统计各类型卡牌数量
 */
class CardStatisticsVisitor : public CardVisitor {
private:
    std::map<CardType, int> counts;

public:
    void visit(const MilitaryCard& card) override {
        counts[CardType::MILITARY]++;
    }

    void visit(const CivilianCard& card) override {
        counts[CardType::CIVILIAN]++;
    }

    void visit(const ScienceCard& card) override {
        counts[CardType::SCIENTIFIC]++;
    }

    void visit(const ResourceCard& card) override {
        if (card.getType() == CardType::RAW_MATERIAL) {
            counts[CardType::RAW_MATERIAL]++;
        } else {
            counts[CardType::MANUFACTURED]++;
        }
    }

    void visit(const CommercialCard& card) override {
        counts[CardType::COMMERCIAL]++;
    }

    void visit(const GuildCard& card) override {
        counts[CardType::GUILD]++;
    }

    const std::map<CardType, int>& getCounts() const {
        return counts;
    }
};

#endif // CARDVISITOR_H
