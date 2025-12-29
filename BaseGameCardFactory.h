#ifndef BASEGAMECARDFACTORY_H
#define BASEGAMECARDFACTORY_H

#include "CardFactory.h"

/**
 * @brief 基础游戏卡牌工厂
 *
 * 实现基础版《七大奇迹：对决》的所有卡牌创建
 * 不包含任何扩展包内容
 */
class BaseGameCardFactory : public CardFactory {
public:
    std::vector<std::shared_ptr<Card>> createDeck(int age) override;

protected:
    std::vector<CardData> getCardDataForAge(int age) override;

private:
    // 各时代的卡牌数据
    std::vector<CardData> getAge1CardData();
    std::vector<CardData> getAge2CardData();
    std::vector<CardData> getAge3CardData();
};

#endif // BASEGAMECARDFACTORY_H
