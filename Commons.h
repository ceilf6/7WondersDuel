#ifndef COMMONS_H
#define COMMONS_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>

namespace Color {
    const std::string RESET   = "\033[0m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN    = "\033[36m";
    const std::string GREY    = "\033[90m";
    const std::string BOLD    = "\033[1m";
}

enum class CardType { RAW_MATERIAL, MANUFACTURED, CIVILIAN, SCIENTIFIC, COMMERCIAL, MILITARY, GUILD };
enum class Resource { WOOD, CLAY, STONE, GLASS, PAPYRUS, COIN, NONE };
enum class ScienceSymbol { GLOBE, WHEEL, MORTAR, LAW, MECHANIC, WRITING, NONE };
enum class WonderEffect { NONE, REPLAY, REVIVE_DISCARD, DESTROY_BROWN, DESTROY_GREY };
enum class TokenType { AGRICULTURE, ARCHITECTURE, ECONOMY, STRATEGY, THEOLOGY, URBANISM, MATHEMATICS, LAW };

struct Wonder {
    std::string name;
    std::map<Resource, int> costResources;
    int victoryPoints;
    int shields;
    WonderEffect effect;
    bool hasReplay;
    bool isBuilt;

    Wonder(std::string n, std::map<Resource, int> c, int vp, int s, WonderEffect eff, bool replay)
        : name(n), costResources(c), victoryPoints(vp), shields(s), effect(eff), hasReplay(replay), isBuilt(false) {}
};

struct ProgressToken {
    std::string name;
    TokenType type;
    std::string desc;
};

#endif // COMMONS_H