#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <string>
#include <memory>
#include "CardFactory.h"

/**
 * @brief 玩家模式枚举
 */
enum class PlayerMode {
    PvP,    // 玩家 vs 玩家
    PvE,    // 玩家 vs AI
    EvE     // AI vs AI (用于测试)
};

/**
 * @brief AI难度等级
 */
enum class Difficulty {
    Random,     // 随机策略
    Easy,       // 简单策略
    Medium,     // 中等策略 (当前SmartAI)
    Hard,       // 困难策略
    Expert      // 专家策略
};

/**
 * @brief 扩展类型
 */
enum class ExtensionType {
    None,       // 无扩展
    Agora,      // Agora扩展
    Pantheon,   // Panthéon扩展
    Both        // 两个扩展都启用
};

/**
 * @brief 游戏配置类
 *
 * 包含游戏的所有配置参数，由GameBuilder构建
 */
class GameConfig {
public:
    PlayerMode playerMode = PlayerMode::PvE;
    Difficulty aiDifficulty = Difficulty::Medium;
    ExtensionType extension = ExtensionType::None;

    std::string player1Name = "Player 1";
    std::string player2Name = "AI";

    bool enableProgressTokens = true;   // 是否启用进步标记
    int wondersPerPlayer = 4;           // 每个玩家的奇迹数量
    int startingCoins = 7;              // 起始金币

    std::shared_ptr<CardFactory> cardFactory;  // 卡牌工厂

    GameConfig() = default;

    /**
     * @brief 根据配置获取对应的卡牌工厂
     * @return 卡牌工厂智能指针
     */
    std::shared_ptr<CardFactory> getCardFactory() const;

    /**
     * @brief 验证配置是否有效
     * @return true if valid
     */
    bool isValid() const {
        if (wondersPerPlayer < 1 || wondersPerPlayer > 8) return false;
        if (startingCoins < 0) return false;
        return true;
    }
};

#endif // GAMECONFIG_H
