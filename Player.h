/* Player.h */
#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include "Card.h"
#include "Commons.h"

// 前置声明，解决循环依赖
class Board; 

class Player {
protected:
    std::string name;
    int coins;
    std::map<Resource, int> resourceProduction;
    std::vector<std::shared_ptr<Card>> builtCards;
    std::vector<Wonder> wonders;
    
    // [新增] 科学符号集合 (用于判定科技胜利)
    std::set<ScienceSymbol> scienceSymbols; 

public:
    Player(std::string name);
    virtual ~Player() = default;

    // [核心接口] 决策时传入 Board，让 AI 能分析局势
    virtual int makeDecision(const std::vector<int>& availableIndices, const Board& board) = 0; 

    // 基础属性
    std::string getName() const;
    int getCoins() const;
    void addCoins(int amount);
    void payCoins(int amount);
    
    // 资源管理
    void addResource(Resource res, int amount);
    int getResourceCount(Resource res) const;

    // [核心] 动态贸易成本计算 (需传入对手以计算税率)
    int calculateActualCost(const std::shared_ptr<Card>& card, const std::shared_ptr<Player>& opponent) const;
    
    // 建造
    void buildCard(std::shared_ptr<Card> card, int costPaid);
    
    // 奇迹管理
    void assignWonder(Wonder w);
    const std::vector<Wonder>& getWonders() const;
    bool canBuildWonder(int idx) const;
    void buildWonder(int idx);

    // 科技与统计
    bool hasScienceSymbol(ScienceSymbol s) const;
    int getScienceCount() const;
    const std::vector<std::shared_ptr<Card>>& getConstructedCards() const;
    
    void displayStatus() const;
};

// 人类玩家
class HumanPlayer : public Player {
public:
    HumanPlayer(std::string name) : Player(name) {}
    int makeDecision(const std::vector<int>& availableIndices, const Board& board) override;
};

// 随机 AI (笨笨的对照组)
class AIPlayer : public Player {
public:
    AIPlayer(std::string name) : Player(name) {}
    int makeDecision(const std::vector<int>& availableIndices, const Board& board) override;
};

#endif // PLAYER_H