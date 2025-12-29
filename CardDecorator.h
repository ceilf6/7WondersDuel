#ifndef CARDDECORATOR_H
#define CARDDECORATOR_H

#include "Card.h"
#include <memory>

/**
 * @brief 卡牌装饰器基类 - Decorator Pattern
 *
 * 用于动态地为卡牌添加额外效果，而无需修改Card类
 * 应用场景：
 * - 科技标记效果（如"策略"标记为红卡+1盾）
 * - Agora扩展的阴谋效果
 * - Panthéon扩展的神祇加成
 */
class CardDecorator : public Card {
protected:
    std::shared_ptr<Card> wrappedCard;  // 被装饰的卡牌

public:
    CardDecorator(std::shared_ptr<Card> card)
        : Card(card->getName(), card->getType(), card->getCostCoins(),
               card->getResourceCost(), card->getChainProvide(), card->getChainCost()),
          wrappedCard(card) {}

    virtual ~CardDecorator() = default;

    // 默认委托给被装饰的卡牌
    void display() const override {
        wrappedCard->display();
    }

    int getVictoryPoints() const override {
        return wrappedCard->getVictoryPoints();
    }

    int getShields() const override {
        return wrappedCard->getShields();
    }

    ScienceSymbol getSymbol() const override {
        return wrappedCard->getSymbol();
    }

    const std::vector<Resource>& getProduction() const override {
        return wrappedCard->getProduction();
    }

    Resource getDiscountResource() const override {
        return wrappedCard->getDiscountResource();
    }

    std::string getGuildType() const override {
        return wrappedCard->getGuildType();
    }

    int getImmediateCoins() const override {
        return wrappedCard->getImmediateCoins();
    }

    /**
     * @brief 获取原始卡牌（去除所有装饰）
     */
    std::shared_ptr<Card> getWrappedCard() const {
        return wrappedCard;
    }
};

/**
 * @brief 策略标记装饰器 - 为军事卡+1盾
 *
 * 示例：当玩家拥有"策略"进步标记时应用
 */
class StrategyTokenDecorator : public CardDecorator {
public:
    StrategyTokenDecorator(std::shared_ptr<Card> card)
        : CardDecorator(card) {}

    int getShields() const override {
        int base = wrappedCard->getShields();
        // 只对军事卡生效
        if (wrappedCard->getType() == CardType::MILITARY) {
            return base + 1;  // 策略标记效果：军事卡+1盾
        }
        return base;
    }

    void display() const override {
        wrappedCard->display();
        if (wrappedCard->getType() == CardType::MILITARY) {
            std::cout << " [策略标记: +1🛡️]";
        }
    }
};

/**
 * @brief 双倍分数装饰器（示例）
 *
 * 可用于某些特殊效果或扩展包
 */
class DoublePointsDecorator : public CardDecorator {
public:
    DoublePointsDecorator(std::shared_ptr<Card> card)
        : CardDecorator(card) {}

    int getVictoryPoints() const override {
        return wrappedCard->getVictoryPoints() * 2;
    }

    void display() const override {
        wrappedCard->display();
        std::cout << " [双倍分数]";
    }
};

#endif // CARDDECORATOR_H
