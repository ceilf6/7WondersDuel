/* Commons.h */
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

// 6种核心符号 + 1个填充
enum class ScienceSymbol { GLOBE, WHEEL, MORTAR, LAW, MECHANIC, WRITING, NONE };

struct Wonder {
    std::string name;
    int costCoins;
    int victoryPoints;
    int shields;
    bool isBuilt;
    Wonder(std::string n, int c, int vp, int s) : name(n), costCoins(c), victoryPoints(vp), shields(s), isBuilt(false) {}
};

#endif // COMMONS_H