#ifndef GAMEBUILDER_H
#define GAMEBUILDER_H

#include "GameConfig.h"
#include "Game.h"
#include <memory>

/**
 * @brief 游戏构建器类 - Builder Pattern
 *
 * 提供流式API来配置和构建Game对象
 * 使用示例:
 *   Game game = GameBuilder()
 *                  .setPlayerMode(PlayerMode::PvE)
 *                  .setAIDifficulty(Difficulty::Hard)
 *                  .build();
 */
class GameBuilder {
private:
    GameConfig config;

public:
    GameBuilder() = default;

    /**
     * @brief 设置玩家模式
     */
    GameBuilder& setPlayerMode(PlayerMode mode) {
        config.playerMode = mode;
        return *this;
    }

    /**
     * @brief 设置AI难度
     */
    GameBuilder& setAIDifficulty(Difficulty diff) {
        config.aiDifficulty = diff;
        return *this;
    }

    /**
     * @brief 启用扩展
     */
    GameBuilder& enableExtension(ExtensionType ext) {
        config.extension = ext;
        return *this;
    }

    /**
     * @brief 设置玩家1名称
     */
    GameBuilder& setPlayer1Name(const std::string& name) {
        config.player1Name = name;
        return *this;
    }

    /**
     * @brief 设置玩家2名称
     */
    GameBuilder& setPlayer2Name(const std::string& name) {
        config.player2Name = name;
        return *this;
    }

    /**
     * @brief 设置起始金币数
     */
    GameBuilder& setStartingCoins(int coins) {
        config.startingCoins = coins;
        return *this;
    }

    /**
     * @brief 设置每个玩家的奇迹数量
     */
    GameBuilder& setWondersPerPlayer(int count) {
        config.wondersPerPlayer = count;
        return *this;
    }

    /**
     * @brief 设置是否启用进步标记
     */
    GameBuilder& enableProgressTokens(bool enable) {
        config.enableProgressTokens = enable;
        return *this;
    }

    /**
     * @brief 设置自定义卡牌工厂
     */
    GameBuilder& setCardFactory(std::shared_ptr<CardFactory> factory) {
        config.cardFactory = factory;
        return *this;
    }

    /**
     * @brief 获取当前配置（用于调试）
     */
    const GameConfig& getConfig() const {
        return config;
    }

    /**
     * @brief 构建Game对象
     * @return 配置好的Game对象
     */
    Game build();

    /**
     * @brief 快速创建PvE游戏（便捷方法）
     */
    static Game quickPvE(Difficulty diff = Difficulty::Medium) {
        return GameBuilder()
            .setPlayerMode(PlayerMode::PvE)
            .setAIDifficulty(diff)
            .build();
    }

    /**
     * @brief 快速创建PvP游戏（便捷方法）
     */
    static Game quickPvP() {
        return GameBuilder()
            .setPlayerMode(PlayerMode::PvP)
            .build();
    }
};

#endif // GAMEBUILDER_H
