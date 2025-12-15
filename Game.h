/* Game.h */
#ifndef GAME_H
#define GAME_H

#include <memory>
#include <vector>
#include "Board.h"
#include "Player.h"
#include "SmartAI.h" // 引入智能AI

class Game {
private:
    Board board;
    std::shared_ptr<Player> p1;
    std::shared_ptr<Player> p2;
    
    int currentAge;
    bool isGameOver;
    int militaryToken; // 0:中间, -9:P1胜, 9:P2胜

    void clearInputBuffer();

public:
    Game();
    
    // 初始化配置
    void setup();
    
    // 游戏主循环
    void run();

private:
    // 回合逻辑
    void playTurn(std::shared_ptr<Player> active, std::shared_ptr<Player> opponent);
    
    // 胜利判定 (军事/科技)
    bool checkInstantVictory();
    
    // 最终算分
    void calculateScore();
    
    // 牌堆工厂
    std::vector<std::shared_ptr<Card>> createDeck(int age);
};

#endif // GAME_H