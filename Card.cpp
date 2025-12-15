#include "Card.h"
#include <iostream>

Card::Card(std::string n, CardType t, int c, std::map<Resource, int> r, std::string cProv, std::string cCost)
    : name(n), type(t), costCoins(c), costResources(r), chainProvide(cProv), chainCost(cCost) {}

std::string Card::getName() const { return name; }
CardType Card::getType() const { return type; }
int Card::getCostCoins() const { return costCoins; }
const std::map<Resource, int>& Card::getResourceCost() const { return costResources; }
std::string Card::getChainProvide() const { return chainProvide; }
std::string Card::getChainCost() const { return chainCost; }

// Military
MilitaryCard::MilitaryCard(std::string n, int c, std::map<Resource, int> r, int s, std::string cProv, std::string cCost)
    : Card(n, CardType::MILITARY, c, r, cProv, cCost), shields(s) {}
int MilitaryCard::getShields() const { return shields; }
void MilitaryCard::display() const {
    std::cout << Color::RED << "[军] " << name << "(" << shields << "盾)";
    if(!chainCost.empty()) std::cout << " 需:" << chainCost;
    std::cout << Color::RESET;
}

// Civilian
CivilianCard::CivilianCard(std::string n, int c, std::map<Resource, int> r, int vp, std::string cProv, std::string cCost)
    : Card(n, CardType::CIVILIAN, c, r, cProv, cCost), victoryPoints(vp) {}
int CivilianCard::getVictoryPoints() const { return victoryPoints; }
void CivilianCard::display() const {
    std::cout << Color::BLUE << "[民] " << name << "(" << victoryPoints << "分)";
    if(!chainCost.empty()) std::cout << " 需:" << chainCost;
    std::cout << Color::RESET;
}

// Science
ScienceCard::ScienceCard(std::string n, int c, std::map<Resource, int> r, ScienceSymbol sym, int vp, std::string cProv, std::string cCost)
    : Card(n, CardType::SCIENTIFIC, c, r, cProv, cCost), symbol(sym), victoryPoints(vp) {}
ScienceSymbol ScienceCard::getSymbol() const { return symbol; }
int ScienceCard::getVictoryPoints() const { return victoryPoints; }
void ScienceCard::display() const {
    std::cout << Color::GREEN << "[科] " << name;
    if(!chainCost.empty()) std::cout << " 需:" << chainCost;
    std::cout << Color::RESET;
}

// Resource
ResourceCard::ResourceCard(std::string n, CardType t, int c, std::vector<Resource> prod)
    : Card(n, t, c, {}, "", ""), production(prod) {}
const std::vector<Resource>& ResourceCard::getProduction() const { return production; }
void ResourceCard::display() const { std::cout << (type==CardType::RAW_MATERIAL?Color::RESET:Color::CYAN) << "[资] " << name << Color::RESET; }

// Commercial [已修复构造函数]
CommercialCard::CommercialCard(std::string n, int c, Resource discountRes, int vp, int immCoins)
    : Card(n, CardType::COMMERCIAL, c, {}, "", ""), discountResource(discountRes), victoryPoints(vp), immediateCoins(immCoins) {}
Resource CommercialCard::getDiscountResource() const { return discountResource; }
int CommercialCard::getVictoryPoints() const { return victoryPoints; }
int CommercialCard::getImmediateCoins() const { return immediateCoins; }
void CommercialCard::display() const {
    std::cout << Color::YELLOW << "[商] " << name;
    if(immediateCoins>0) std::cout << "(+" << immediateCoins << "$)";
    std::cout << Color::RESET;
}

// Guild
GuildCard::GuildCard(std::string n, std::map<Resource, int> res, std::string gType)
    : Card(n, CardType::GUILD, 0, res, "", ""), guildType(gType) {}
std::string GuildCard::getGuildType() const { return guildType; }
void GuildCard::display() const { std::cout << Color::MAGENTA << "[公会] " << name << Color::RESET; }