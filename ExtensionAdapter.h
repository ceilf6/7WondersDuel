#ifndef EXTENSIONADAPTER_H
#define EXTENSIONADAPTER_H

#include "Player.h"
#include "Board.h"
#include <memory>

// 前向声明
class Game;

/**
 * @brief 扩展适配器接口 - Adapter Pattern
 *
 * 为扩展包提供统一的生命周期钩子
 * 使扩展包能够无缝集成到游戏流程中，而无需修改Game类
 */
class ExtensionAdapter {
public:
    virtual ~ExtensionAdapter() = default;

    /**
     * @brief 游戏开始时调用
     */
    virtual void onGameStart(Game& game) {}

    /**
     * @brief 每个时代开始时调用
     */
    virtual void onAgeStart(int age, Game& game) {}

    /**
     * @brief 回合开始时调用
     */
    virtual void onTurnStart(Player& active, Player& opponent) {}

    /**
     * @brief 回合结束时调用
     */
    virtual void onTurnEnd(Player& active, Player& opponent) {}

    /**
     * @brief 时代结束时调用
     */
    virtual void onAgeEnd(int age, Game& game) {}

    /**
     * @brief 游戏结束时调用
     */
    virtual void onGameEnd(Game& game) {}

    /**
     * @brief 卡牌被建造时调用
     */
    virtual void onCardBuilt(std::shared_ptr<Card> card, Player& player) {}

    /**
     * @brief 奇迹被建造时调用
     */
    virtual void onWonderBuilt(const Wonder& wonder, Player& player) {}
};

/**
 * @brief Agora扩展适配器（示例）
 *
 * 展示如何为Agora扩展添加新机制
 */
class AgoraAdapter : public ExtensionAdapter {
private:
    // Agora特有状态
    bool senatePhaseActive = false;
    int conspiracyCount = 0;

public:
    void onGameStart(Game& game) override {
        std::cout << Color::MAGENTA
                  << "\n🏛️  Agora扩展已启用！"
                  << Color::RESET << "\n";
        // TODO: 初始化阴谋卡、参议院标记等
    }

    void onTurnEnd(Player& active, Player& opponent) override {
        // 检查是否触发参议院阶段
        conspiracyCount++;
        if (conspiracyCount >= 5 && !senatePhaseActive) {
            triggerSenatePhase(active, opponent);
        }
    }

    void onAgeEnd(int age, Game& game) override {
        if (age == 2) {
            std::cout << Color::YELLOW
                      << "\n📜 Agora特有事件：政治影响力结算"
                      << Color::RESET << "\n";
            // TODO: 执行Agora时代2结束特殊规则
        }
    }

private:
    void triggerSenatePhase(Player& p1, Player& p2) {
        senatePhaseActive = true;
        std::cout << Color::CYAN
                  << "\n🏛️  参议院阶段触发！玩家需要进行政治决策..."
                  << Color::RESET << "\n";
        // TODO: 实现参议院决策逻辑
    }
};

/**
 * @brief Panthéon扩展适配器（示例）
 */
class PantheonAdapter : public ExtensionAdapter {
private:
    std::vector<std::string> activeDivinities;  // 激活的神祇

public:
    void onGameStart(Game& game) override {
        std::cout << Color::MAGENTA
                  << "\n⚡ Panthéon扩展已启用！"
                  << Color::RESET << "\n";
        // TODO: 初始化神殿、神祇卡等
    }

    void onCardBuilt(std::shared_ptr<Card> card, Player& player) override {
        // 检查是否触发神祇效果
        if (card->getType() == CardType::GUILD) {
            std::cout << Color::YELLOW
                      << "\n⚡ 神祇响应：" << card->getName()
                      << Color::RESET << "\n";
            // TODO: 应用神祇加成
        }
    }
};

/**
 * @brief 扩展管理器
 *
 * 管理多个扩展适配器的生命周期调用
 */
class ExtensionManager {
private:
    std::vector<std::unique_ptr<ExtensionAdapter>> extensions;

public:
    void addExtension(std::unique_ptr<ExtensionAdapter> ext) {
        extensions.push_back(std::move(ext));
    }

    void triggerGameStart(Game& game) {
        for (auto& ext : extensions) {
            ext->onGameStart(game);
        }
    }

    void triggerAgeStart(int age, Game& game) {
        for (auto& ext : extensions) {
            ext->onAgeStart(age, game);
        }
    }

    void triggerTurnEnd(Player& active, Player& opponent) {
        for (auto& ext : extensions) {
            ext->onTurnEnd(active, opponent);
        }
    }

    void triggerCardBuilt(std::shared_ptr<Card> card, Player& player) {
        for (auto& ext : extensions) {
            ext->onCardBuilt(card, player);
        }
    }

    // ... 其他触发方法
};

#endif // EXTENSIONADAPTER_H
