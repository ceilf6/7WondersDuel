#ifndef AGORACARDFACTORY_H
#define AGORACARDFACTORY_H

#include "BaseGameCardFactory.h"

/**
 * @brief Agora扩展卡牌工厂
 *
 * 包含基础游戏卡牌 + Agora扩展特有卡牌
 * 新增内容：
 * - 阴谋卡（Conspiracy Cards）
 * - 参议院相关卡牌
 * - 23张新时代卡牌
 */
class AgoraCardFactory : public BaseGameCardFactory {
public:
    std::vector<std::shared_ptr<Card>> createDeck(int age) override;

protected:
    std::vector<CardData> getCardDataForAge(int age) override;

private:
    /**
     * @brief 获取Agora扩展的额外卡牌数据
     */
    std::vector<CardData> getAgoraCardData(int age);

    /**
     * @brief 创建阴谋卡（示例）
     */
    std::shared_ptr<Card> createConspiracyCard(const std::string& name);
};

#endif // AGORACARDFACTORY_H
