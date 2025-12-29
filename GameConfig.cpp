#include "GameConfig.h"
#include "BaseGameCardFactory.h"

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
            // TODO: 返回 AgoraCardFactory (阶段5实现)
            return std::make_shared<BaseGameCardFactory>();

        case ExtensionType::Pantheon:
            // TODO: 返回 PantheonCardFactory (阶段5实现)
            return std::make_shared<BaseGameCardFactory>();

        case ExtensionType::Both:
            // TODO: 返回组合工厂 (阶段5实现)
            return std::make_shared<BaseGameCardFactory>();

        default:
            return std::make_shared<BaseGameCardFactory>();
    }
}
