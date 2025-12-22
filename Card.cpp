#include "Card.h"
#include <iostream>
#include <iomanip>

// [汉化] 连锁符号翻译函数
std::string translateChain(const std::string& chain) {
    if (chain == "Horseshoe") return "马蹄铁";
    if (chain == "Sword") return "剑";
    if (chain == "Tower") return "塔";
    if (chain == "Target") return "靶子";
    if (chain == "Helmet") return "头盔";
    if (chain == "Gear") return "齿轮";
    if (chain == "Scope") return "望远镜";
    if (chain == "Book") return "书";
    if (chain == "Pestle") return "药杵";
    if (chain == "Moon") return "月亮";
    if (chain == "Mask") return "面具";
    if (chain == "Drop") return "水滴";
    if (chain == "Sun") return "太阳";
    if (chain == "Temple") return "殿堂";
    if (chain == "Jug") return "酒壶";
    if (chain == "Barrel") return "木桶";
    return chain; // 未知符号则返回原名
}

Card::Card(std::string n, CardType t, int c, std::map<Resource, int> r, std::string cProv, std::string cCost)
    : name(n), type(t), costCoins(c), costResources(r), chainProvide(cProv), chainCost(cCost) {}

std::string Card::getName() const { return name; }
CardType Card::getType() const { return type; }
int Card::getCostCoins() const { return costCoins; }
const std::map<Resource, int> &Card::getResourceCost() const { return costResources; }
std::string Card::getChainProvide() const { return chainProvide; }
std::string Card::getChainCost() const { return chainCost; }

// ==========================================
// Military (红卡)
// ==========================================
MilitaryCard::MilitaryCard(std::string n, int c, std::map<Resource, int> r, int s, std::string cProv, std::string cCost)
    : Card(n, CardType::MILITARY, c, r, cProv, cCost), shields(s) {}
int MilitaryCard::getShields() const { return shields; }
void MilitaryCard::display() const
{
    std::cout << Color::RED << "[军] " << name << "(" << shields << "盾)";
    if (costCoins > 0)
        std::cout << " $" << costCoins;
    if (!costResources.empty())
    {
        std::cout << " [";
        for (auto &p : costResources)
        {
            if (p.first == Resource::WOOD)      std::cout << "木" << p.second;
            else if (p.first == Resource::CLAY) std::cout << "泥" << p.second;
            else if (p.first == Resource::STONE) std::cout << "石" << p.second;
            else if (p.first == Resource::GLASS) std::cout << "玻" << p.second;
            else if (p.first == Resource::PAPYRUS) std::cout << "纸" << p.second;
        }
        std::cout << "]";
    }
    // [修正] 使用翻译函数
    if (!chainCost.empty())
        std::cout << " 需:" << translateChain(chainCost);
    if (!chainProvide.empty())
        std::cout << " 给:" << translateChain(chainProvide);
    std::cout << Color::RESET;
}

// ==========================================
// Civilian (蓝卡)
// ==========================================
CivilianCard::CivilianCard(std::string n, int c, std::map<Resource, int> r, int vp, std::string cProv, std::string cCost)
    : Card(n, CardType::CIVILIAN, c, r, cProv, cCost), victoryPoints(vp) {}
int CivilianCard::getVictoryPoints() const { return victoryPoints; }
void CivilianCard::display() const
{
    std::cout << Color::BLUE << "[民] " << name << "(" << victoryPoints << "分)";
    if (costCoins > 0)
        std::cout << " $" << costCoins;
    if (!costResources.empty())
    {
        std::cout << " [";
        for (auto &p : costResources)
        {
            if (p.first == Resource::WOOD)      std::cout << "木" << p.second;
            else if (p.first == Resource::CLAY) std::cout << "泥" << p.second;
            else if (p.first == Resource::STONE) std::cout << "石" << p.second;
            else if (p.first == Resource::GLASS) std::cout << "玻" << p.second;
            else if (p.first == Resource::PAPYRUS) std::cout << "纸" << p.second;
        }
        std::cout << "]";
    }
    // [修正] 使用翻译函数
    if (!chainCost.empty())
        std::cout << " 需:" << translateChain(chainCost);
    if (!chainProvide.empty())
        std::cout << " 给:" << translateChain(chainProvide);
    std::cout << Color::RESET;
}

// ==========================================
// Science (绿卡)
// ==========================================
ScienceCard::ScienceCard(std::string n, int c, std::map<Resource, int> r, ScienceSymbol sym, int vp, std::string cProv, std::string cCost)
    : Card(n, CardType::SCIENTIFIC, c, r, cProv, cCost), symbol(sym), victoryPoints(vp) {}
ScienceSymbol ScienceCard::getSymbol() const { return symbol; }
int ScienceCard::getVictoryPoints() const { return victoryPoints; }
void ScienceCard::display() const
{
    std::cout << Color::GREEN << "[科] " << name;
    if (victoryPoints > 0)
        std::cout << "(" << victoryPoints << "分)";

    // [修正] 修复了重复定义 bug，并完成了汉化
    std::string symStr = "";
    if (symbol == ScienceSymbol::WHEEL)         symStr = "轮子";
    else if (symbol == ScienceSymbol::GLOBE)    symStr = "地球仪";
    else if (symbol == ScienceSymbol::WRITING)  symStr = "羽毛笔";
    else if (symbol == ScienceSymbol::MORTAR)   symStr = "药杵";
    else if (symbol == ScienceSymbol::MECHANIC) symStr = "圆规";
    else if (symbol == ScienceSymbol::LAW)      symStr = "法律";

    if (!symStr.empty())
        std::cout << " [" << symStr << "]";

    if (costCoins > 0)
        std::cout << " $" << costCoins;

    if (!costResources.empty())
    {
        std::cout << " [";
        for (auto &p : costResources)
        {
            if (p.first == Resource::WOOD)      std::cout << "木" << p.second;
            else if (p.first == Resource::CLAY) std::cout << "泥" << p.second;
            else if (p.first == Resource::STONE) std::cout << "石" << p.second;
            else if (p.first == Resource::GLASS) std::cout << "玻" << p.second;
            else if (p.first == Resource::PAPYRUS) std::cout << "纸" << p.second;
        }
        std::cout << "]";
    }
    // [修正] 使用翻译函数
    if (!chainCost.empty())
        std::cout << " 需:" << translateChain(chainCost);
    if (!chainProvide.empty())
        std::cout << " 给:" << translateChain(chainProvide);
    std::cout << Color::RESET;
}

// ==========================================
// Resource (棕/灰卡)
// ==========================================
ResourceCard::ResourceCard(std::string n, CardType t, int c, std::vector<Resource> prod)
    : Card(n, t, c, {}, "", ""), production(prod) {}
const std::vector<Resource> &ResourceCard::getProduction() const { return production; }
void ResourceCard::display() const
{
    // 棕卡用普通色，灰卡用青色
    std::cout << (type == CardType::RAW_MATERIAL ? Color::RESET : Color::CYAN) << "[资] " << name;
    if (costCoins > 0)
        std::cout << " $" << costCoins;

    // [修正] 汉化产出
    std::cout << " 产:";
    for (auto r : production)
    {
        if (r == Resource::WOOD)      std::cout << "木";
        else if (r == Resource::CLAY) std::cout << "泥";
        else if (r == Resource::STONE) std::cout << "石";
        else if (r == Resource::GLASS) std::cout << "玻";
        else if (r == Resource::PAPYRUS) std::cout << "纸";
    }
    std::cout << Color::RESET;
}

// ==========================================
// Commercial (黄卡)
// ==========================================
CommercialCard::CommercialCard(std::string n, int c, Resource discountRes, int vp, int immCoins)
    : Card(n, CardType::COMMERCIAL, c, {}, "", ""), discountResource(discountRes), victoryPoints(vp), immediateCoins(immCoins) {}
Resource CommercialCard::getDiscountResource() const { return discountResource; }
int CommercialCard::getVictoryPoints() const { return victoryPoints; }
int CommercialCard::getImmediateCoins() const { return immediateCoins; }
void CommercialCard::display() const
{
    std::cout << Color::YELLOW << "[商] " << name;
    if (costCoins > 0)
        std::cout << " $" << costCoins;

    // [修正] 汉化金币
    if (immediateCoins > 0)
        std::cout << " +" << immediateCoins << "金";

    if (victoryPoints > 0)
        std::cout << " +" << victoryPoints << "分";

    if (discountResource != Resource::NONE)
    {
        std::cout << " 减:";
        if (discountResource == Resource::WOOD)      std::cout << "木";
        else if (discountResource == Resource::CLAY) std::cout << "泥";
        else if (discountResource == Resource::STONE) std::cout << "石";
    }
    std::cout << Color::RESET;
}

// ==========================================
// Guild (紫卡)
// ==========================================
GuildCard::GuildCard(std::string n, std::map<Resource, int> res, std::string gType)
    : Card(n, CardType::GUILD, 0, res, "", ""), guildType(gType) {}
std::string GuildCard::getGuildType() const { return guildType; }
void GuildCard::display() const
{
    std::cout << Color::MAGENTA << "[公会] " << name;
    if (!costResources.empty())
    {
        std::cout << " [";
        for (auto &p : costResources)
        {
            if (p.first == Resource::WOOD)      std::cout << "木" << p.second;
            else if (p.first == Resource::CLAY) std::cout << "泥" << p.second;
            else if (p.first == Resource::STONE) std::cout << "石" << p.second;
            else if (p.first == Resource::GLASS) std::cout << "玻" << p.second;
            else if (p.first == Resource::PAPYRUS) std::cout << "纸" << p.second;
        }
        std::cout << "]";
    }
    // 注意：工会的具体效果已在 Game.cpp 的 playTurn 中通过 getCardDesc 函数详细展示
    // 这里只保留类型标识
    std::cout << " (类型:" << guildType << ")";
    std::cout << Color::RESET;
}