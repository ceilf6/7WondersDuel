#ifndef GAME_H
#define GAME_H
#include <memory>
#include <vector>
#include "Board.h"
#include "Player.h"
#include "SmartAI.h"
#include "CardFactory.h"

class Game {
    Board board;
    std::shared_ptr<Player> p1, p2;
    std::shared_ptr<CardFactory> cardFactory; // [Factory Pattern] 卡牌工厂
    int currentAge, militaryToken;
    int wondersBuiltCount; // [专家] 全球奇迹上限计数
    bool isGameOver;
    std::vector<ProgressToken> tokenPool;

    void clearInputBuffer();
public:
    Game();
    Game(std::shared_ptr<CardFactory> factory); // 允许注入工厂
    void setup();
    void run();
private:
    bool playTurn(std::shared_ptr<Player> active, std::shared_ptr<Player> opponent);
    bool checkInstantVictory();
    void calculateScore();

    void handleTokenSelection(std::shared_ptr<Player> p);
    void handleWonderEffect(WonderEffect eff, std::shared_ptr<Player> active, std::shared_ptr<Player> opp);
};
#endif