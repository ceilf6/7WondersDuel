#include "GameBuilder.h"
#include <stdexcept>

Game GameBuilder::build() {
    // 验证配置
    if (!config.isValid()) {
        throw std::invalid_argument("Invalid game configuration");
    }

    // 获取卡牌工厂
    auto factory = config.getCardFactory();

    // 创建Game对象，传入工厂
    Game game(factory);

    // TODO: 在阶段1.3中，我们会创建一个Game::setConfig()方法
    // 来应用其他配置参数（玩家名称、起始金币等）
    // 目前Game类还不支持这些配置的注入

    return game;
}
