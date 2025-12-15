#ifndef GAME_H
#define GAME_H
#include <memory>
#include <vector>
#include "Board.h"
#include "Player.h"
#include "SmartAI.h"

class Game {
    Board board;
    std::shared_ptr<Player> p1, p2;
    int currentAge, militaryToken;
    int wondersBuiltCount; // [专家] 全球奇迹上限计数
    bool isGameOver;
    std::vector<ProgressToken> tokenPool;
    
    void clearInputBuffer();
public:
    Game();
    void setup();
    void run();
private:
    bool playTurn(std::shared_ptr<Player> active, std::shared_ptr<Player> opponent);
    bool checkInstantVictory();
    void calculateScore();
    std::vector<std::shared_ptr<Card>> createDeck(int age);
    
    void handleTokenSelection(std::shared_ptr<Player> p);
    void handleWonderEffect(WonderEffect eff, std::shared_ptr<Player> active, std::shared_ptr<Player> opp);
};
#endif