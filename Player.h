#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include "Card.h"

class Board; 

class Player {
protected:
    std::string name;
    int coins;
    std::map<Resource, int> resourceProduction;
    std::set<Resource> discountedResources;
    std::vector<std::shared_ptr<Card>> builtCards;
    std::vector<Wonder> wonders;
    std::map<ScienceSymbol, int> scienceSymbolCounts; 
    std::set<std::string> ownedChains; 
    std::vector<TokenType> activeTokens;

public:
    Player(std::string name);
    virtual ~Player() = default;
    
    // 决策接口
    virtual int makeDecision(const std::vector<int>& availableIndices, const Board& board) = 0; 
    virtual int chooseCardToDestroy(const std::vector<std::shared_ptr<Card>>& targets) = 0;
    virtual int chooseCardToRevive(const std::vector<std::shared_ptr<Card>>& discardPile) = 0;
    virtual int chooseProgressToken(const std::vector<ProgressToken>& available) = 0;
    // [专家] 决定下一时代先手
    virtual int chooseWhoStarts(std::string p1Name, std::string p2Name) = 0; 

    std::string getName() const;
    int getCoins() const;
    void addCoins(int amount);
    void payCoins(int amount);
    void addResource(Resource res, int amount);
    int getResourceCount(Resource res) const;
    int getCardCount(CardType type) const;

    int calculateActualCost(const std::shared_ptr<Card>& card, const std::shared_ptr<Player>& opponent) const;
    int calculateWonderCost(const Wonder& w, const std::shared_ptr<Player>& opponent) const;
    
    bool buildCard(std::shared_ptr<Card> card, int costPaid);
    
    void destroyBuiltCard(CardType type);
    void addProgressToken(TokenType t);
    bool hasToken(TokenType t) const;
    
    void assignWonder(Wonder w);
    const std::vector<Wonder>& getWonders() const;
    bool canBuildWonder(int idx, const std::shared_ptr<Player>& opponent) const;
    void buildWonder(int idx, int costPaid);

    bool hasScienceSymbol(ScienceSymbol s) const;
    int getScienceCount() const;
    const std::vector<std::shared_ptr<Card>>& getConstructedCards() const;
    
    void displayStatus() const;
};

class HumanPlayer : public Player {
public:
    HumanPlayer(std::string name) : Player(name) {}
    int makeDecision(const std::vector<int>& availableIndices, const Board& board) override;
    int chooseCardToDestroy(const std::vector<std::shared_ptr<Card>>& targets) override;
    int chooseCardToRevive(const std::vector<std::shared_ptr<Card>>& discardPile) override;
    int chooseProgressToken(const std::vector<ProgressToken>& available) override;
    int chooseWhoStarts(std::string p1Name, std::string p2Name) override;
};

class AIPlayer : public Player {
public:
    AIPlayer(std::string name) : Player(name) {}
    int makeDecision(const std::vector<int>& availableIndices, const Board& board) override;
    int chooseCardToDestroy(const std::vector<std::shared_ptr<Card>>& targets) override;
    int chooseCardToRevive(const std::vector<std::shared_ptr<Card>>& discardPile) override;
    int chooseProgressToken(const std::vector<ProgressToken>& available) override;
    int chooseWhoStarts(std::string p1Name, std::string p2Name) override;
};
#endif