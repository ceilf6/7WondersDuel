#include "GameConfig.h"
#include "BaseGameCardFactory.h"
#include "AgoraCardFactory.h"
#include "PantheonCardFactory.h"

std::shared_ptr<CardFactory> GameConfig::getCardFactory() const {
    // 如果已经设置了自定义工厂，使用自定义工厂
    if (cardFactory) {
        return cardFactory;
    }

    // 否则根据扩展类型创建默认工厂
    switch (extension) {
        case ExtensionType::None:
            return std::make_shared<BaseGameCardFactory>();

        case ExtensionType::Agora:
            return std::make_shared<AgoraCardFactory>();

        case ExtensionType::Pantheon:
            return std::make_shared<PantheonCardFactory>();

        case ExtensionType::Both:
            // 组合两个扩展：使用Agora为基础（因为它已经继承了基础游戏）
            // 实际项目中可以创建CombinedCardFactory
            return std::make_shared<AgoraCardFactory>();

        default:
            return std::make_shared<BaseGameCardFactory>();
    }
}
