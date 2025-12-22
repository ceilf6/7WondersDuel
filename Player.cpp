#include "Player.h"
#include "Board.h"
#include <iostream>
#include <limits>
#include <cstdlib>
#include <iomanip>
#include <algorithm>

// [汉化] 资源图标转文字
std::string resIcon(Resource r)
{
    switch (r)
    {
    case Resource::WOOD:    return "木";
    case Resource::CLAY:    return "泥";
    case Resource::STONE:   return "石";
    case Resource::GLASS:   return "玻";
    case Resource::PAPYRUS: return "纸";
    default: return "";
    }
}

Player::Player(std::string n) : name(n), coins(7) {}
std::string Player::getName() const { return name; }
int Player::getCoins() const { return coins; }
void Player::addCoins(int amount) { coins += amount; }
void Player::payCoins(int amount)
{
    coins -= amount;
    if (coins < 0)
        coins = 0;
}
void Player::addResource(Resource res, int amount) { resourceProduction[res] += amount; }
int Player::getResourceCount(Resource res) const
{
    auto it = resourceProduction.find(res);
    return (it != resourceProduction.end()) ? it->second : 0;
}

int Player::getCardCount(CardType type) const
{
    int count = 0;
    for (const auto &c : builtCards)
        if (c->getType() == type)
            count++;
    return count;
}

int Player::calculateActualCost(const std::shared_ptr<Card> &card, const std::shared_ptr<Player> &opponent) const
{
    if (!card->getChainCost().empty() && ownedChains.count(card->getChainCost()))
        return 0;
    int totalCost = card->getCostCoins();
    for (auto const &[res, needed] : card->getResourceCost())
    {
        int my = getResourceCount(res);
        if (my < needed)
        {
            int missing = needed - my;
            int price = 2;
            if (discountedResources.count(res))
                price = 1;
            else if (opponent)
                price += opponent->getResourceCount(res);
            totalCost += (missing * price);
        }
    }
    if (coins < totalCost)
        return -1;
    return totalCost;
}

int Player::calculateWonderCost(const Wonder &w, const std::shared_ptr<Player> &opponent) const
{
    int discount = hasToken(TokenType::ARCHITECTURE) ? 2 : 0;
    int totalCost = 0;
    std::map<Resource, int> remainingCost = w.costResources;

    for (auto it = remainingCost.begin(); it != remainingCost.end();)
    {
        int my = getResourceCount(it->first);
        if (my >= it->second)
        {
            it = remainingCost.erase(it);
        }
        else
        {
            it->second -= my;
            ++it;
        }
    }

    int totalMissingCount = 0;
    for (auto const &[res, count] : remainingCost)
        totalMissingCount += count;
    int finalMissing = std::max(0, totalMissingCount - discount);

    if (finalMissing > 0)
    {
        int currentMissing = 0;
        for (auto const &[res, count] : remainingCost)
        {
            if (currentMissing >= finalMissing)
                break;
            int neededForRes = count;
            int price = 2;
            if (discountedResources.count(res))
                price = 1;
            else if (opponent)
                price += opponent->getResourceCount(res);

            int take = std::min(neededForRes, finalMissing - currentMissing);
            totalCost += take * price;
            currentMissing += take;
        }
    }
    if (coins < totalCost)
        return -1;
    return totalCost;
}

bool Player::buildCard(std::shared_ptr<Card> card, int costPaid)
{
    payCoins(costPaid);
    builtCards.push_back(card);
    for (auto r : card->getProduction())
        addResource(r, 1);
    if (!card->getChainProvide().empty())
        ownedChains.insert(card->getChainProvide());
    if (!card->getChainCost().empty() && costPaid == 0 && hasToken(TokenType::URBANISM))
        addCoins(4);
    if (card->getDiscountResource() != Resource::NONE)
        discountedResources.insert(card->getDiscountResource());

    if (card->getSymbol() != ScienceSymbol::NONE)
    {
        scienceSymbolCounts[card->getSymbol()]++;
        if (scienceSymbolCounts[card->getSymbol()] == 2)
            return true;
    }
    return false;
}

void Player::destroyBuiltCard(CardType type)
{
    for (int i = builtCards.size() - 1; i >= 0; --i)
    {
        if (builtCards[i]->getType() == type)
        {
            for (auto r : builtCards[i]->getProduction())
                resourceProduction[r]--;
            if (builtCards[i]->getDiscountResource() != Resource::NONE)
                discountedResources.erase(builtCards[i]->getDiscountResource());
            std::cout << Color::RED << "!!! " << builtCards[i]->getName() << " 被摧毁了 !!!" << Color::RESET << "\n";
            builtCards.erase(builtCards.begin() + i);
            return;
        }
    }
}

void Player::addProgressToken(TokenType t)
{
    activeTokens.push_back(t);
    if (t == TokenType::AGRICULTURE)
        addCoins(6);
}
bool Player::hasToken(TokenType t) const
{
    for (auto tk : activeTokens)
        if (tk == t)
            return true;
    return false;
}

int Player::getScienceCount() const
{
    int count = scienceSymbolCounts.size();
    if (hasToken(TokenType::LAW))
        count++;
    return count;
}

void Player::assignWonder(Wonder w) { wonders.push_back(w); }
const std::vector<Wonder> &Player::getWonders() const { return wonders; }
bool Player::canBuildWonder(int idx, const std::shared_ptr<Player> &opp) const
{
    return idx >= 0 && idx < wonders.size() && !wonders[idx].isBuilt && calculateWonderCost(wonders[idx], opp) != -1;
}
void Player::buildWonder(int idx, int cost)
{
    payCoins(cost);
    wonders[idx].isBuilt = true;
}
bool Player::hasScienceSymbol(ScienceSymbol s) const { return scienceSymbolCounts.count(s); }
const std::vector<std::shared_ptr<Card>> &Player::getConstructedCards() const { return builtCards; }

// ==========================================
// [UI美化] 汉化 + 详细状态栏
// ==========================================
void Player::displayStatus() const
{
    std::string border = "┌────────────────────────────────────────────────────────┐";
    std::string bottom = "└────────────────────────────────────────────────────────┘";

    std::cout << border << "\n";

    // Line 1: Name, Coins, Science
    std::cout << "│ " << std::left << std::setw(10) << name
              << " 金币: " << std::setw(2) << coins
              << " | 科技: " << getScienceCount() << "/6"
              << std::string(24, ' ') << "│\n";

    // Line 2: Resources (汉化 + 文字版)
    std::cout << "│ 产出: 木:" << getResourceCount(Resource::WOOD)
              << " 泥:" << getResourceCount(Resource::CLAY)
              << " 石:" << getResourceCount(Resource::STONE)
              << " 玻:" << getResourceCount(Resource::GLASS)
              << " 纸:" << getResourceCount(Resource::PAPYRUS)
              << std::string(10, ' ') << "│\n";

    // Line 3: 具体的科技符号
    std::cout << "│ 符号: ";
    bool hasSym = false;
    for (auto const& [sym, count] : scienceSymbolCounts) {
        if (count > 0) {
            std::string sName;
            switch(sym) {
                case ScienceSymbol::GLOBE: sName = "地球仪"; break;
                case ScienceSymbol::WHEEL: sName = "轮子"; break;
                case ScienceSymbol::MORTAR: sName = "药杵"; break;
                case ScienceSymbol::LAW: sName = "天平"; break;
                case ScienceSymbol::MECHANIC: sName = "圆规"; break;
                case ScienceSymbol::WRITING: sName = "羽毛笔"; break;
                default: sName = "?";
            }
            std::cout << sName << " ";
            hasSym = true;
        }
    }
    if(hasToken(TokenType::LAW)) { std::cout << "法律(百搭) "; hasSym = true; }
    if(!hasSym) std::cout << "无";
    std::cout << "\n";

    // Line 4: 奇迹列表 (带需求)
    std::cout << "│ 奇迹列表:                                              │\n";
    for (const auto &w : wonders)
    {
        std::cout << "│  [" << (w.isBuilt ? "已建" : "未建") << "] " << w.name << " ";

        // 显示奇迹效果简述
        if (w.hasReplay) std::cout << "[再行动]";
        if (w.victoryPoints > 0) std::cout << "[" << w.victoryPoints << "分]";
        if (w.shields > 0) std::cout << "[" << w.shields << "盾]";
        if (w.effect == WonderEffect::DESTROY_BROWN) std::cout << "[毁原料卡]";
        if (w.effect == WonderEffect::DESTROY_GREY) std::cout << "[毁制品卡]";
        if (w.effect == WonderEffect::REVIVE_DISCARD) std::cout << "[复活弃牌]";

        std::cout << "\n│      需求: ";
        for(auto const& [res, count] : w.costResources) {
            std::cout << resIcon(res) << count << " ";
        }
        std::cout << "\n";
    }

    // Line 5: 科技标记 (汉化)
    if (!activeTokens.empty())
    {
        std::cout << "│ [科技标记]: ";
        for (size_t i = 0; i < activeTokens.size(); ++i)
        {
            auto t = activeTokens[i];
            std::string tn = "";
            if (t == TokenType::AGRICULTURE) tn = "农业(+6金+4分)";
            else if (t == TokenType::ARCHITECTURE) tn = "建筑(奇迹-2资)";
            else if (t == TokenType::ECONOMY) tn = "经济(对手交易钱归你)";
            else if (t == TokenType::LAW) tn = "法律(+1科技符号)";
            else if (t == TokenType::MATHEMATICS) tn = "数学(每标+3分)";
            else if (t == TokenType::STRATEGY) tn = "策略(红卡额外1盾)";
            else if (t == TokenType::URBANISM) tn = "城规(建卡时免费拿4金)";
            else if (t == TokenType::THEOLOGY) tn = "神学(奇迹再行动)";
            std::cout << tn;
            if (i < activeTokens.size() - 1) std::cout << ", ";
        }
        std::cout << "\n";
    }

    std::cout << bottom << "\n";
}

// ==========================================
// [HumanPlayer & AIPlayer 实现]
// 之前缺失的这部分导致了链接错误
// ==========================================

int HumanPlayer::makeDecision(const std::vector<int> &availableIndices, const Board &board)
{
    int choice;
    while (true)
    {
        std::cout << ">> " << name << ", 请输入卡牌 ID (-1退出): ";
        if (!(std::cin >> choice))
        {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }
        if (choice == -1)
            return -1;
        for (int idx : availableIndices)
            if (choice == idx)
                return choice;
        std::cout << "无效的选择，请重试。\n";
    }
}
int HumanPlayer::chooseCardToDestroy(const std::vector<std::shared_ptr<Card>> &targets)
{
    if (targets.empty())
        return -1;
    std::cout << "选择要摧毁的卡牌 (输入序号 0-" << targets.size() - 1 << "): \n";
    for (size_t i = 0; i < targets.size(); ++i)
        std::cout << i << ". " << targets[i]->getName() << "\n";
    int c;
    std::cin >> c;
    return (c >= 0 && c < targets.size()) ? c : 0;
}
int HumanPlayer::chooseCardToRevive(const std::vector<std::shared_ptr<Card>> &dp)
{
    if (dp.empty())
        return -1;
    std::cout << "选择要复活的卡牌 (输入序号 0-" << dp.size() - 1 << "): \n";
    for (size_t i = 0; i < dp.size(); ++i)
        std::cout << i << ". " << dp[i]->getName() << "\n";
    int c;
    std::cin >> c;
    return (c >= 0 && c < dp.size()) ? c : 0;
}
int HumanPlayer::chooseProgressToken(const std::vector<ProgressToken> &av)
{
    std::cout << "\n"
              << Color::MAGENTA << "🎯 选择一个科技标记 (输入序号 0-" << av.size() - 1 << "):" << Color::RESET << "\n";
    for (size_t i = 0; i < av.size(); ++i)
        std::cout << Color::CYAN << i << ". " << Color::BOLD << av[i].name << Color::RESET
                  << " - " << Color::GREY << av[i].desc << Color::RESET << "\n";
    std::cout << ">> 你的选择: ";
    int c;
    std::cin >> c;
    return (c >= 0 && c < av.size()) ? c : 0;
}
int HumanPlayer::chooseWhoStarts(std::string p1Name, std::string p2Name)
{
    std::cout << ">>> " << name << ", 请决定下一时代谁先手:\n1. " << p1Name << "\n2. " << p2Name << "\n选择 (1/2): ";
    int c;
    std::cin >> c;
    return c;
}

// AI 实现 (简单逻辑)
int AIPlayer::makeDecision(const std::vector<int> &idx, const Board &) { return idx.empty() ? -1 : idx[rand() % idx.size()]; }
int AIPlayer::chooseCardToDestroy(const std::vector<std::shared_ptr<Card>> &t) { return t.empty() ? -1 : 0; }
int AIPlayer::chooseCardToRevive(const std::vector<std::shared_ptr<Card>> &dp) { return dp.empty() ? -1 : dp.size() - 1; }
int AIPlayer::chooseProgressToken(const std::vector<ProgressToken> &) { return 0; }
int AIPlayer::chooseWhoStarts(std::string p1Name, std::string p2Name) { return (name == p1Name) ? 1 : 2; }