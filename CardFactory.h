#ifndef CARDFACTORY_H
#define CARDFACTORY_H

#include <memory>
#include <vector>
#include "Card.h"
#include "CardData.h"

/**
 * @brief 卡牌工厂抽象接口
 *
 * 使用 Abstract Factory 模式，允许不同的扩展（基础游戏、Agora、Panthéon）
 * 提供各自的卡牌创建实现
 */
class CardFactory {
public:
    virtual ~CardFactory() = default;

    /**
     * @brief 创建指定时代的卡组
     * @param age 时代编号 (1, 2, 3)
     * @return 该时代的所有卡牌
     */
    virtual std::vector<std::shared_ptr<Card>> createDeck(int age) = 0;

    /**
     * @brief 根据卡牌数据创建卡牌实例
     * @param data 卡牌数据
     * @return 卡牌智能指针
     */
    std::shared_ptr<Card> createCardFromData(const CardData& data);

protected:
    /**
     * @brief 获取指定时代的卡牌数据
     * @param age 时代编号
     * @return 卡牌数据列表
     */
    virtual std::vector<CardData> getCardDataForAge(int age) = 0;
};

#endif // CARDFACTORY_H
