/* Card.h */
#ifndef CARD_H
#define CARD_H

#include "Commons.h"
#include <iomanip>
#include <memory>
#include <vector>
#include <map>

class Card {
protected:
    std::string name;
    CardType type;
    int costCoins;
    // [新增] 资源造价：比如 {WOOD: 2} 表示需要2个木头
    std::map<Resource, int> costResources; 

public:
    // 构造函数更新，支持资源成本
    Card(std::string name, CardType type, int coins, std::map<Resource, int> resCost = {});
    virtual ~Card() = default;

    std::string getName() const;
    CardType getType() const;
    int getCostCoins() const;
    const std::map<Resource, int>& getResourceCost() const;
    
    virtual void display() const = 0;
};

// 子类保持结构，但在 Card.cpp 里需要适配新的构造函数
class MilitaryCard : public Card {
    int shields;
public:
    MilitaryCard(std::string name, int coins, std::map<Resource, int> res, int shields);
    void display() const override;
    int getShields() const;
};

class CivilianCard : public Card {
    int victoryPoints;
public:
    CivilianCard(std::string name, int coins, std::map<Resource, int> res, int vp);
    void display() const override;
    int getVictoryPoints() const;
};

class ScienceCard : public Card {
    ScienceSymbol symbol;
    int victoryPoints;
public:
    ScienceCard(std::string name, int coins, std::map<Resource, int> res, ScienceSymbol sym, int vp = 0);
    void display() const override;
    ScienceSymbol getSymbol() const; // [新增]
};

class ResourceCard : public Card {
    std::vector<Resource> production;
public:
    ResourceCard(std::string name, CardType type, int coins, std::vector<Resource> prod);
    void display() const override;
    const std::vector<Resource>& getProduction() const;
};

#endif // CARD_H