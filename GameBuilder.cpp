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

    // [Builder Pattern] 应用游戏配置
    game.applyConfig(config);

    return game;
}
