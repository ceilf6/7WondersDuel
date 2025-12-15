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
    std::map<Resource, int> costResources;
    std::string chainProvide; // 提供的连锁符号
    std::string chainCost;    // 需要的连锁符号

public:
    Card(std::string name, CardType type, int coins, std::map<Resource, int> resCost, std::string cProv="", std::string cCost="");
    virtual ~Card() = default;

    std::string getName() const;
    CardType getType() const;
    int getCostCoins() const;
    const std::map<Resource, int>& getResourceCost() const;
    std::string getChainProvide() const;
    std::string getChainCost() const;

    // 虚函数接口
    virtual void display() const = 0;
    virtual int getVictoryPoints() const { return 0; }
    virtual int getShields() const { return 0; }
    virtual ScienceSymbol getSymbol() const { return ScienceSymbol::NONE; }
    virtual const std::vector<Resource>& getProduction() const { static std::vector<Resource> e; return e; }
    virtual Resource getDiscountResource() const { return Resource::NONE; }
    virtual std::string getGuildType() const { return ""; }
    virtual int getImmediateCoins() const { return 0; } // [新增] 即时金币
};

class MilitaryCard : public Card {
    int shields;
public:
    MilitaryCard(std::string name, int coins, std::map<Resource, int> res, int shields, std::string cProv="", std::string cCost="");
    void display() const override;
    int getShields() const override;
};

class CivilianCard : public Card {
    int victoryPoints;
public:
    CivilianCard(std::string name, int coins, std::map<Resource, int> res, int vp, std::string cProv="", std::string cCost="");
    void display() const override;
    int getVictoryPoints() const override;
};

class ScienceCard : public Card {
    ScienceSymbol symbol;
    int victoryPoints;
public:
    ScienceCard(std::string name, int coins, std::map<Resource, int> res, ScienceSymbol sym, int vp = 0, std::string cProv="", std::string cCost="");
    void display() const override;
    ScienceSymbol getSymbol() const override;
    int getVictoryPoints() const override;
};

class ResourceCard : public Card {
    std::vector<Resource> production;
public:
    ResourceCard(std::string name, CardType type, int coins, std::vector<Resource> prod);
    void display() const override;
    const std::vector<Resource>& getProduction() const override;
};

class CommercialCard : public Card {
    Resource discountResource;
    int victoryPoints;
    int immediateCoins; // [新增]
public:
    CommercialCard(std::string name, int coins, Resource discountRes, int vp = 0, int immCoins = 0);
    void display() const override;
    Resource getDiscountResource() const override;
    int getVictoryPoints() const override;
    int getImmediateCoins() const override;
};

class GuildCard : public Card {
    std::string guildType;
public:
    GuildCard(std::string name, std::map<Resource, int> res, std::string gType);
    void display() const override;
    std::string getGuildType() const override;
};

#endif // CARD_H