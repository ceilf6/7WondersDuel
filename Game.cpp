/* Game.cpp */
#include "Game.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>
#include <iomanip>

// 清屏工具
void clearScreen() { std::cout << "\033[2J\033[1;1H"; }

Game::Game() : currentAge(1), isGameOver(false), militaryToken(0) {}

void Game::clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void Game::setup() {
    // Windows下强制UTF-8
    system("chcp 65001 > nul");
    clearScreen();
    std::cout << Color::BOLD << "=== 7 WONDERS DUEL: FINAL EDITION ===" << Color::RESET << "\n\n";
    std::cout << "1. PvP (Human vs Human)\n";
    std::cout << "2. PvE (Human vs SmartAI)\n";
    std::cout << "3. EvE (SmartAI vs RandomAI - Spectator)\n"; 
    std::cout << ">> Choice: ";
    
    int mode;
    while (!(std::cin >> mode) || mode < 1 || mode > 3) {
        clearInputBuffer(); std::cout << "Enter 1-3: ";
    }

    if (mode == 1) { 
        p1 = std::make_shared<HumanPlayer>("Player 1"); 
        p2 = std::make_shared<HumanPlayer>("Player 2"); 
    } 
    else if (mode == 2) { 
        std::string n; std::cout << "Enter Name: "; std::cin >> n;
        p1 = std::make_shared<HumanPlayer>(n); 
        p2 = std::make_shared<SmartAI>("SkyNet"); 
    } 
    else { 
        p1 = std::make_shared<SmartAI>("SmartBot"); 
        p2 = std::make_shared<AIPlayer>("DumbBot"); 
    }

    // 分配奇迹
    p1->assignWonder(Wonder("Pyramids", 3, 9, 0));
    p1->assignWonder(Wonder("Sphinx", 4, 6, 0));
    p1->assignWonder(Wonder("AppianWay", 3, 3, 0)); // 只有钱和分
    p1->assignWonder(Wonder("GreatLight", 5, 6, 0));

    p2->assignWonder(Wonder("Colossus", 3, 3, 2));
    p2->assignWonder(Wonder("ZeusStatue", 4, 3, 1));
    p2->assignWonder(Wonder("TempleArt", 4, 5, 0));
    p2->assignWonder(Wonder("Mausoleum", 5, 2, 0));

    militaryToken = 0;
    currentAge = 1;
    isGameOver = false;
}

void Game::run() {
    setup();

    // === 时代循环 1 -> 3 ===
    for (int age = 1; age <= 3; ++age) {
        if (isGameOver) break;
        currentAge = age;

        // 生成新牌堆并洗牌
        auto deck = createDeck(age);
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(deck.begin(), deck.end(), std::default_random_engine(seed));
        board.initAge1(deck); // 复用金字塔形状

        // 时代过场动画
        clearScreen();
        std::cout << "\n\n\t" << Color::MAGENTA << ">>> STARTING AGE " << age << " <<<" << Color::RESET << "\n\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));

        int turn = 0;
        while (!isGameOver) {
            // 确定先手：Age 1/3 P1先, Age 2 P2先
            bool p1Starts = (age % 2 != 0);
            auto active = (turn % 2 == 0) ? (p1Starts ? p1 : p2) : (p1Starts ? p2 : p1);
            auto opponent = (active == p1) ? p2 : p1;

            clearScreen();
            std::cout << Color::BOLD << "AGE " << age << " | " << active->getName() << Color::RESET << "\n";
            
            // 绘制军事条
            std::cout << "MILITARY: P1 ";
            for(int i=-9; i<=9; ++i) std::cout << (i==militaryToken ? "⚔️" : (i==0?"|":"-"));
            std::cout << " P2\n";

            p1->displayStatus();
            p2->displayStatus();
            board.display();

            // 检查牌堆
            auto indices = board.getAvailableCardIndices();
            if (indices.empty()) { 
                std::cout << "\n>>> Age " << age << " Complete! <<<\n";
                std::this_thread::sleep_for(std::chrono::seconds(2));
                break; 
            }

            playTurn(active, opponent);
            
            if (checkInstantVictory()) { isGameOver = true; break; }

            // 智能延时
            if (std::dynamic_pointer_cast<AIPlayer>(active) || std::dynamic_pointer_cast<SmartAI>(active)) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1200));
            } else {
                std::cout << "(Press Enter to continue...)";
                std::cin.ignore(); std::cin.get();
            }
            turn++;
        }
    }
    calculateScore();
}

void Game::playTurn(std::shared_ptr<Player> active, std::shared_ptr<Player> opponent) {
    auto validIndices = board.getAvailableCardIndices();
    bool isHuman = (std::dynamic_pointer_cast<HumanPlayer>(active) != nullptr);

    // 1. 显示详细列表 (仅人类)
    if (isHuman) {
        std::cout << "Available:\n";
        for (int idx : validIndices) {
            auto c = board.getSlot(idx).card;
            int cost = active->calculateActualCost(c, opponent);
            
            std::cout << "[" << std::setw(2) << idx << "] " << std::left << std::setw(14) << c->getName();
            
            if (cost == -1) std::cout << " | " << Color::RED << "Too Expensive" << Color::RESET;
            else if (cost == 0) std::cout << " | FREE";
            else std::cout << " | " << Color::YELLOW << "$" << cost << Color::RESET;

            // 显示类型标记
            if(c->getType()==CardType::MILITARY) std::cout << " (Mil)";
            if(c->getType()==CardType::SCIENTIFIC) std::cout << " (Sci)";
            if(c->getType()==CardType::GUILD) std::cout << " (Guild)";
            std::cout << "\n";
        }
    }

    // 2. 决策：选哪张牌
    int choice = active->makeDecision(validIndices, board);
    if (choice == -1) { isGameOver = true; return; }

    auto card = board.removeCard(choice); // 从版图移除
    int actualCost = active->calculateActualCost(card, opponent); // 计算含税价格

    // 3. 动作选择
    int action = 1; // 1:Build, 2:Sell, 3:Wonder
    int wonderIdx = -1;

    // AI 逻辑
    if (!isHuman) {
        // 优先检查能不能造奇迹
        for(size_t i=0; i<active->getWonders().size(); ++i) {
            if (active->canBuildWonder(i)) { 
                action = 3; wonderIdx = i; break; 
            }
        }
        // 如果不造奇迹，且买得起卡，就造卡；否则卖卡
        if (action != 3) {
            if (actualCost != -1) action = 1;
            else action = 2;
        }
    } 
    // 人类逻辑
    else {
        std::cout << "Selected: " << Color::CYAN << card->getName() << Color::RESET << "\n";
        std::cout << "1. Build (Cost: " << (actualCost==-1 ? "N/A" : std::to_string(actualCost)) << ")\n";
        std::cout << "2. Sell (+2 Coins)\n";
        
        bool canWonder = false;
        for(size_t i=0; i<active->getWonders().size(); ++i) {
            if (active->canBuildWonder(i)) {
                if(!canWonder) std::cout << "3. Build Wonder:\n";
                canWonder = true;
                std::cout << "   3" << i << ". " << active->getWonders()[i].name << " ($" << active->getWonders()[i].costCoins << ")\n";
            }
        }

        std::cout << ">> Action: ";
        int in;
        while(true) {
            if(std::cin >> in) {
                if(in==1 || in==2) { action=in; break; }
                if(in==3 && canWonder) {
                    std::cout << "   Wonder Index: "; std::cin >> wonderIdx;
                    if(active->canBuildWonder(wonderIdx)) { action=3; break; }
                }
            } else clearInputBuffer();
            std::cout << "Invalid. Try again: ";
        }
    }

    // 4. 执行
    if (action == 1 && actualCost != -1) {
        active->buildCard(card, actualCost);
        std::cout << ">> " << active->getName() << " " << Color::GREEN << "BUILT" << Color::RESET << " " << card->getName() << "\n";
        
        // 军事结算
        if (card->getType() == CardType::MILITARY) {
            auto mc = std::dynamic_pointer_cast<MilitaryCard>(card);
            if(mc) {
                int dir = (active == p1) ? -1 : 1;
                militaryToken += (mc->getShields() * dir);
                std::cout << Color::RED << "   ⚔️ Military Advance!\n" << Color::RESET;
            }
        }
    } 
    else if (action == 3) {
        active->buildWonder(wonderIdx);
        auto w = active->getWonders()[wonderIdx];
        std::cout << ">> " << active->getName() << " " << Color::MAGENTA << "BUILT WONDER: " << w.name << "!!!" << Color::RESET << "\n";
        if (w.shields > 0) {
            int dir = (active == p1) ? -1 : 1;
            militaryToken += (w.shields * dir);
            std::cout << Color::RED << "   ⚔️ Wonder Military Strike!\n" << Color::RESET;
        }
        // 注意：用来造奇迹的那张卡(card)在这里被消耗掉了(不进入手牌)
    } 
    else {
        std::cout << ">> " << active->getName() << " " << Color::YELLOW << "SOLD" << Color::RESET << " " << card->getName() << " (+2)\n";
        active->addCoins(2);
    }
}

bool Game::checkInstantVictory() {
    // 军事
    if (militaryToken <= -9) { clearScreen(); std::cout << Color::RED << "\n!!! P1 MILITARY VICTORY !!!\n" << Color::RESET; return true; }
    if (militaryToken >= 9) { clearScreen(); std::cout << Color::RED << "\n!!! P2 MILITARY VICTORY !!!\n" << Color::RESET; return true; }
    
    // 科技 (满分逻辑)
    if (p1->getScienceCount() >= 6) { clearScreen(); std::cout << Color::GREEN << "\n!!! P1 SCIENCE VICTORY !!!\n" << Color::RESET; return true; }
    if (p2->getScienceCount() >= 6) { clearScreen(); std::cout << Color::GREEN << "\n!!! P2 SCIENCE VICTORY !!!\n" << Color::RESET; return true; }
    
    return false;
}

void Game::calculateScore() {
    if (isGameOver) return;
    std::cout << "\n" << Color::BOLD << "=== FINAL SCORING ===" << Color::RESET << "\n";

    auto calc = [](std::shared_ptr<Player> p) {
        int s = p->getCoins() / 3;
        for(auto w : p->getWonders()) if(w.isBuilt) s += w.victoryPoints;
        for(auto c : p->getConstructedCards()) {
            if(c->getType() == CardType::CIVILIAN) s += std::dynamic_pointer_cast<CivilianCard>(c)->getVictoryPoints();
            if(c->getType() == CardType::GUILD) s += 10; // 简化公会分
            if(c->getType() == CardType::SCIENTIFIC) s += 2; // 科技卡也给点分
        }
        return s;
    };

    int s1 = calc(p1);
    int s2 = calc(p2);

    std::cout << p1->getName() << ": " << s1 << "\n";
    std::cout << p2->getName() << ": " << s2 << "\n";
    std::cout << "WINNER: " << (s1 > s2 ? p1->getName() : p2->getName()) << "\n";
}

// === 牌堆生成工厂 ===
std::vector<std::shared_ptr<Card>> Game::createDeck(int age) {
    std::vector<std::shared_ptr<Card>> deck;
    
    if (age == 1) {
        // Age 1: 资源积累
        for(int i=0;i<3;i++) deck.push_back(std::make_shared<ResourceCard>("LumberYard", CardType::RAW_MATERIAL, 0, std::vector<Resource>{Resource::WOOD}));
        for(int i=0;i<3;i++) deck.push_back(std::make_shared<ResourceCard>("LoggingCamp", CardType::RAW_MATERIAL, 1, std::vector<Resource>{Resource::WOOD}));
        for(int i=0;i<3;i++) deck.push_back(std::make_shared<ResourceCard>("ClayPool", CardType::RAW_MATERIAL, 0, std::vector<Resource>{Resource::CLAY}));
        for(int i=0;i<3;i++) deck.push_back(std::make_shared<ResourceCard>("ClayPit", CardType::RAW_MATERIAL, 1, std::vector<Resource>{Resource::CLAY}));
        for(int i=0;i<3;i++) deck.push_back(std::make_shared<ResourceCard>("Quarry", CardType::RAW_MATERIAL, 0, std::vector<Resource>{Resource::STONE}));
        
        // 科技 (有符号)
        deck.push_back(std::make_shared<ScienceCard>("Apothecary", 2, std::map<Resource,int>{}, ScienceSymbol::WHEEL, 1));
        deck.push_back(std::make_shared<ScienceCard>("Workshop", 2, std::map<Resource,int>{}, ScienceSymbol::GLOBE, 1));
        
        // 军事与平民
        for(int i=0;i<3;i++) deck.push_back(std::make_shared<MilitaryCard>("Tower", 0, std::map<Resource,int>{}, 1));
        for(int i=0;i<3;i++) deck.push_back(std::make_shared<CivilianCard>("Baths", 2, std::map<Resource,int>{}, 3));
    } 
    else if (age == 2) {
        // Age 2: 昂贵，需要资源，更多科技
        std::map<Resource,int> costW; costW[Resource::WOOD]=2;
        deck.push_back(std::make_shared<ScienceCard>("Library", 2, costW, ScienceSymbol::WRITING, 2));
        deck.push_back(std::make_shared<ScienceCard>("School", 2, costW, ScienceSymbol::WHEEL, 2));
        deck.push_back(std::make_shared<ScienceCard>("Lab", 3, std::map<Resource,int>{}, ScienceSymbol::MORTAR, 2));
        deck.push_back(std::make_shared<ScienceCard>("Dispensary", 3, std::map<Resource,int>{}, ScienceSymbol::MORTAR, 2));

        for(int i=0;i<3;i++) deck.push_back(std::make_shared<MilitaryCard>("Walls", 3, std::map<Resource,int>{}, 2)); // 2盾
        for(int i=0;i<4;i++) deck.push_back(std::make_shared<CivilianCard>("Statue", 3, std::map<Resource,int>{}, 4));
        
        // 高级资源 (产2个)
        deck.push_back(std::make_shared<ResourceCard>("Sawmill", CardType::RAW_MATERIAL, 2, std::vector<Resource>{Resource::WOOD, Resource::WOOD}));
        deck.push_back(std::make_shared<ResourceCard>("BrickYard", CardType::RAW_MATERIAL, 2, std::vector<Resource>{Resource::CLAY, Resource::CLAY}));
    } 
    else {
        // Age 3: 只有军事、科技、公会、高分蓝卡 (无资源卡)
        // 科技
        deck.push_back(std::make_shared<ScienceCard>("Observatory", 5, std::map<Resource,int>{}, ScienceSymbol::MECHANIC, 3));
        deck.push_back(std::make_shared<ScienceCard>("University", 6, std::map<Resource,int>{}, ScienceSymbol::LAW, 3));
        deck.push_back(std::make_shared<ScienceCard>("Academy", 5, std::map<Resource,int>{}, ScienceSymbol::GLOBE, 3));
        
        // 强力军事
        std::map<Resource,int> costMil; costMil[Resource::CLAY]=2; costMil[Resource::WOOD]=1;
        for(int i=0;i<3;i++) deck.push_back(std::make_shared<MilitaryCard>("Fort", 5, costMil, 3)); // 3盾!
        
        // 公会
        deck.push_back(std::make_shared<CivilianCard>("TradersGld", 0, std::map<Resource,int>{}, 10)); // 这里借用Civilian模拟Guild
        deck.push_back(std::make_shared<CivilianCard>("BuildersGld", 0, std::map<Resource,int>{}, 10));
        deck.push_back(std::make_shared<CivilianCard>("SciGld", 0, std::map<Resource,int>{}, 10));
        
        // 高分蓝卡
        std::map<Resource,int> costP; costP[Resource::STONE]=3;
        for(int i=0;i<3;i++) deck.push_back(std::make_shared<CivilianCard>("Palace", 6, costP, 7));
    }

    // 填充到20张
    while(deck.size() < 20) {
        deck.push_back(std::make_shared<CivilianCard>("Filler", 0, std::map<Resource,int>{}, 1));
    }
    
    return deck;
}