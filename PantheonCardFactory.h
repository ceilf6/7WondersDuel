#ifndef PANTHEONCARDFACTORY_H
#define PANTHEONCARDFACTORY_H

#include "BaseGameCardFactory.h"

/**
 * @brief Panthéon扩展卡牌工厂
 *
 * 包含基础游戏卡牌 + Panthéon扩展特有卡牌
 * 新增内容：
 * - 神祇卡（Divinity Cards）
 * - 神殿建筑
 * - 神话标记机制相关卡牌
 */
class PantheonCardFactory : public BaseGameCardFactory {
public:
    std::vector<std::shared_ptr<Card>> createDeck(int age) override;

protected:
    std::vector<CardData> getCardDataForAge(int age) override;

private:
    /**
     * @brief 获取Panthéon扩展的额外卡牌数据
     */
    std::vector<CardData> getPantheonCardData(int age);

    /**
     * @brief 创建神祇卡（示例）
     */
    std::shared_ptr<Card> createDivinityCard(const std::string& name);
};

#endif // PANTHEONCARDFACTORY_H
