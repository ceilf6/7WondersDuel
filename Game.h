#ifndef GAME_H
#define GAME_H
#include <memory>
#include <vector>
#include "Board.h"
#include "Player.h"
#include "SmartAI.h"
#include "CardFactory.h"

// 前向声明，避免循环依赖
class GameConfig;

class Game {
    Board board;
    std::shared_ptr<Player> p1, p2;
    std::shared_ptr<CardFactory> cardFactory; // [Factory Pattern] 卡牌工厂
    int currentAge, militaryToken;
    int wondersBuiltCount; // [专家] 全球奇迹上限计数
    bool isGameOver;
    std::vector<ProgressToken> tokenPool;

    // [Builder Pattern] 配置参数
    std::string player1Name = "Player 1";
    std::string player2Name = "AI";
    int startingCoins = 7;
    int wondersPerPlayer = 4;
    bool enableProgressTokens = true;

    void clearInputBuffer();
public:
    Game();
    Game(std::shared_ptr<CardFactory> factory); // 允许注入工厂

    /**
     * @brief 应用游戏配置 [Builder Pattern]
     * @param config 游戏配置对象
     */
    void applyConfig(const GameConfig& config);

    void setup();
    void run();

    // Getter方法（供扩展适配器使用）
    std::shared_ptr<Player> getPlayer1() const { return p1; }
    std::shared_ptr<Player> getPlayer2() const { return p2; }
    Board& getBoard() { return board; }

private:
    bool playTurn(std::shared_ptr<Player> active, std::shared_ptr<Player> opponent);
    bool checkInstantVictory();
    void calculateScore();

    void handleTokenSelection(std::shared_ptr<Player> p);
    void handleWonderEffect(WonderEffect eff, std::shared_ptr<Player> active, std::shared_ptr<Player> opp);
};
#endif