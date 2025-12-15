/* Player.cpp */
#include "Player.h"
#include "Board.h" // 必须包含 Board 定义
#include <iostream>
#include <limits>
#include <cstdlib>
#include <iomanip>

// 辅助：图标显示
std::string resIcon(Resource r) {
    switch(r) {
        case Resource::WOOD:    return "🌲";
        case Resource::CLAY:    return "🧱";
        case Resource::STONE:   return "🗿";
        case Resource::GLASS:   return "🔮";
        case Resource::PAPYRUS: return "📜";
        default: return "?";
    }
}

// 构造函数
Player::Player(std::string n) : name(n), coins(7) {
    resourceProduction[Resource::WOOD] = 0;
    resourceProduction[Resource::CLAY] = 0;
    resourceProduction[Resource::STONE] = 0;
    resourceProduction[Resource::GLASS] = 0;
    resourceProduction[Resource::PAPYRUS] = 0;
}

std::string Player::getName() const { return name; }
int Player::getCoins() const { return coins; }
void Player::addCoins(int amount) { coins += amount; }
void Player::payCoins(int amount) { coins -= amount; if(coins < 0) coins = 0; }
void Player::addResource(Resource res, int amount) { resourceProduction[res] += amount; }

int Player::getResourceCount(Resource res) const {
    auto it = resourceProduction.find(res);
    return (it != resourceProduction.end()) ? it->second : 0;
}

// [核心算法] 动态贸易成本计算
int Player::calculateActualCost(const std::shared_ptr<Card>& card, const std::shared_ptr<Player>& opponent) const {
    int totalCost = card->getCostCoins();
    
    for (auto const& [res, neededAmount] : card->getResourceCost()) {
        int myAmount = getResourceCount(res);
        if (myAmount < neededAmount) {
            int missing = neededAmount - myAmount;
            // 贸易公式: 2 + 对手资源数
            int pricePerUnit = 2;
            if (opponent) {
                pricePerUnit += opponent->getResourceCount(res);
            }
            totalCost += (missing * pricePerUnit);
        }
    }
    
    if (coins < totalCost) return -1; // 买不起
    return totalCost;
}

void Player::buildCard(std::shared_ptr<Card> card, int costPaid) {
    payCoins(costPaid);
    builtCards.push_back(card);

    // 资源产出
    if (card->getType() == CardType::RAW_MATERIAL || card->getType() == CardType::MANUFACTURED) {
        auto resCard = std::dynamic_pointer_cast<ResourceCard>(card);
        if (resCard) {
            for (auto r : resCard->getProduction()) addResource(r, 1);
        }
    }
    // 科技收集
    if (card->getType() == CardType::SCIENTIFIC) {
        auto sciCard = std::dynamic_pointer_cast<ScienceCard>(card);
        if (sciCard && sciCard->getSymbol() != ScienceSymbol::NONE) {
            scienceSymbols.insert(sciCard->getSymbol());
        }
    }
}

// 奇迹管理
void Player::assignWonder(Wonder w) { wonders.push_back(w); }
const std::vector<Wonder>& Player::getWonders() const { return wonders; }
bool Player::canBuildWonder(int idx) const {
    if (idx < 0 || idx >= wonders.size()) return false;
    return !wonders[idx].isBuilt && coins >= wonders[idx].costCoins;
}
void Player::buildWonder(int idx) {
    if (canBuildWonder(idx)) {
        payCoins(wonders[idx].costCoins);
        wonders[idx].isBuilt = true;
    }
}

bool Player::hasScienceSymbol(ScienceSymbol s) const { return scienceSymbols.count(s); }
int Player::getScienceCount() const { return scienceSymbols.size(); }
const std::vector<std::shared_ptr<Card>>& Player::getConstructedCards() const { return builtCards; }

// UI 显示
void Player::displayStatus() const {
    std::cout << "┌────────────────────────────────────────────────────────┐\n";
    std::cout << "│ " << Color::BOLD << std::left << std::setw(15) << name << Color::RESET
              << " Coins: " << Color::YELLOW << std::setw(2) << coins << Color::RESET 
              << " | Sci: " << Color::GREEN << scienceSymbols.size() << "/6" << Color::RESET 
              << "                    │\n";
    
    std::cout << "│ Res: ";
    std::cout << resIcon(Resource::WOOD) << ":" << getResourceCount(Resource::WOOD) << " ";
    std::cout << resIcon(Resource::CLAY) << ":" << getResourceCount(Resource::CLAY) << " ";
    std::cout << resIcon(Resource::STONE) << ":" << getResourceCount(Resource::STONE) << " ";
    std::cout << resIcon(Resource::GLASS) << ":" << getResourceCount(Resource::GLASS) << " ";
    std::cout << resIcon(Resource::PAPYRUS) << ":" << getResourceCount(Resource::PAPYRUS) << "    │\n";

    std::cout << "│ Wonders:                                               │\n";
    for(const auto& w : wonders) {
        std::cout << "│  ";
        if (w.isBuilt) std::cout << Color::GREEN << "[BUILT] " << w.name << " (" << w.victoryPoints << "VP)" << Color::RESET;
        else std::cout << "[     ] " << w.name << " ($" << w.costCoins << ")";
        std::cout << "\n";
    }
    std::cout << "└────────────────────────────────────────────────────────┘\n";
}

// Human 决策
int HumanPlayer::makeDecision(const std::vector<int>& availableIndices, const Board& board) {
    int choice;
    while (true) {
        std::cout << ">> " << name << ", enter card ID (-1 quit): ";
        if (!(std::cin >> choice)) {
            std::cout << Color::RED << "Invalid input!" << Color::RESET << std::endl;
            std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        if (choice == -1) return -1;
        bool valid = false;
        for (int idx : availableIndices) if (choice == idx) valid = true;
        if (valid) return choice;
        std::cout << "Invalid ID! Not available.\n";
    }
}

// Random AI 决策
int AIPlayer::makeDecision(const std::vector<int>& availableIndices, const Board& board) {
    if (availableIndices.empty()) return -1;
    return availableIndices[rand() % availableIndices.size()];
}