/* Card.cpp */
#include "Card.h"
#include <iostream>

// 基类构造
Card::Card(std::string n, CardType t, int c, std::map<Resource, int> r) 
    : name(n), type(t), costCoins(c), costResources(r) {}

std::string Card::getName() const { return name; }
CardType Card::getType() const { return type; }
int Card::getCostCoins() const { return costCoins; }
const std::map<Resource, int>& Card::getResourceCost() const { return costResources; }

// Military
MilitaryCard::MilitaryCard(std::string n, int c, std::map<Resource, int> r, int s) 
    : Card(n, CardType::MILITARY, c, r), shields(s) {}
int MilitaryCard::getShields() const { return shields; }
void MilitaryCard::display() const { std::cout << "[Mil] " << name << " (" << shields << " Shield)"; }

// Civilian
CivilianCard::CivilianCard(std::string n, int c, std::map<Resource, int> r, int vp) 
    : Card(n, CardType::CIVILIAN, c, r), victoryPoints(vp) {}
int CivilianCard::getVictoryPoints() const { return victoryPoints; }
void CivilianCard::display() const { std::cout << "[Civ] " << name << " (" << victoryPoints << " VP)"; }

// Science
ScienceCard::ScienceCard(std::string n, int c, std::map<Resource, int> r, ScienceSymbol sym, int vp)
    : Card(n, CardType::SCIENTIFIC, c, r), symbol(sym), victoryPoints(vp) {}
ScienceSymbol ScienceCard::getSymbol() const { return symbol; }
void ScienceCard::display() const { std::cout << "[Sci] " << name; }

// Resource
ResourceCard::ResourceCard(std::string n, CardType t, int c, std::vector<Resource> prod)
    : Card(n, t, c, {}), production(prod) {}
const std::vector<Resource>& ResourceCard::getProduction() const { return production; }
void ResourceCard::display() const { std::cout << "[Res] " << name; }