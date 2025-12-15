#include "Game.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>
#include <iomanip>

// 清屏函数：使用 ANSI 转义序列
void clearScreen() {
    std::cout << "\033[2J\033[1;1H";
}

Game::Game() : currentAge(1), isGameOver(false), militaryToken(0), wondersBuiltCount(0) {}

void Game::clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(1000, '\n');
}

// ==========================================
// [UI] 绘制顶部状态栏 (动态军事条)
// ==========================================
void drawHeader(int age, int militaryToken, std::string p1Name, std::string p2Name) {
    clearScreen();
    std::cout << Color::BOLD << " >>> 7 WONDERS DUEL - AGE " << age << " <<<" << Color::RESET << "\n";

    // 绘制军事条: 范围 -9 到 9，共 19 个格。中心是 0
    std::cout << "\nMILITARY: " << std::left << std::setw(10) << p1Name << " ";

    // P1 优势区 (负数区域)
    std::cout << Color::RED;
    for (int i = -9; i < 0; ++i) {
        if (i == militaryToken) std::cout << "⚔️";
        else if (i == -9) std::cout << "[Win]";
        else if (i == -6 || i == -3) std::cout << "|";
        else std::cout << "-";
    }
    std::cout << Color::RESET;

    // 中心点
    if (militaryToken == 0) std::cout << Color::YELLOW << " ⚔️ " << Color::RESET;
    else std::cout << " | ";

    // P2 优势区 (正数区域)
    std::cout << Color::BLUE;
    for (int i = 1; i <= 9; ++i) {
        if (i == militaryToken) std::cout << "⚔️";
        else if (i == 9) std::cout << "[Win]";
        else if (i == 6 || i == 3) std::cout << "|";
        else std::cout << "-";
    }
    std::cout << Color::RESET;

    std::cout << " " << p2Name << "\n\n";
}

void Game::setup() {
    #ifdef _WIN32
    system("chcp 65001 > nul"); // 设置 Windows 终端为 UTF-8
    #endif
    clearScreen();
    std::cout << Color::BOLD << "=== 7 WONDERS DUEL: FINAL EDITION ===" << Color::RESET << "\n";
    std::cout << "1. PvP (玩家 vs 玩家)\n2. PvE (玩家 vs 电脑)\n>> ";
    int m;
    if (!(std::cin >> m)) m = 2; // 默认 PvE

    if(m==1) {
        p1 = std::make_shared<HumanPlayer>("Player 1");
        p2 = std::make_shared<HumanPlayer>("Player 2");
    } else {
        p1 = std::make_shared<HumanPlayer>("You");
        p2 = std::make_shared<SmartAI>("SkyNet");
    }

    wondersBuiltCount = 0;

    // 初始化进步标记池
    tokenPool = {
        {"农业", TokenType::AGRICULTURE, "6金币, 4分"},
        {"建筑学", TokenType::ARCHITECTURE, "奇迹少2资源"},
        {"经济学", TokenType::ECONOMY, "对手交易钱归你"},
        {"法律", TokenType::LAW, "算第7个科技符号"},
        {"数学", TokenType::MATHEMATICS, "每个标记3分"},
        {"城市规划", TokenType::URBANISM, "连锁+4金"},
        {"策略", TokenType::STRATEGY, "红卡额外1盾(暂未实装)"},
        {"神学", TokenType::THEOLOGY, "所有奇迹获再来一回合"}
    };

    // 分配奇迹 (包含 Replay、破坏、复活等所有特效)
    p1->assignWonder(Wonder("阿耳忒弥斯神庙", {{Resource::WOOD,1},{Resource::STONE,1},{Resource::GLASS,1}}, 0, 0, WonderEffect::REPLAY, true));
    p1->assignWonder(Wonder("大金字塔", {{Resource::STONE,3},{Resource::PAPYRUS,1}}, 9, 0, WonderEffect::NONE, false));
    p1->assignWonder(Wonder("斯芬克斯", {{Resource::STONE,1},{Resource::CLAY,1},{Resource::GLASS,2}}, 6, 0, WonderEffect::REPLAY, true));
    p1->assignWonder(Wonder("宙斯神像", {{Resource::WOOD,1},{Resource::STONE,2},{Resource::CLAY,1}}, 3, 1, WonderEffect::DESTROY_BROWN, false));

    p2->assignWonder(Wonder("大图书馆", {{Resource::WOOD,3},{Resource::GLASS,1},{Resource::PAPYRUS,1}}, 4, 0, WonderEffect::NONE, false));
    p2->assignWonder(Wonder("摩索拉斯王陵墓", {{Resource::CLAY,2},{Resource::GLASS,1},{Resource::PAPYRUS,1}}, 2, 0, WonderEffect::REVIVE_DISCARD, false));
    p2->assignWonder(Wonder("空中花园", {{Resource::WOOD,2},{Resource::PAPYRUS,2}}, 3, 0, WonderEffect::REPLAY, true));
    p2->assignWonder(Wonder("马克西穆斯", {{Resource::STONE,2},{Resource::WOOD,1}}, 3, 1, WonderEffect::DESTROY_GREY, false));
}

// 处理奇迹特效 (破坏、复活)
void Game::handleWonderEffect(WonderEffect eff, std::shared_ptr<Player> active, std::shared_ptr<Player> opp) {
    if (eff == WonderEffect::DESTROY_BROWN || eff == WonderEffect::DESTROY_GREY) {
        CardType targetType = (eff == WonderEffect::DESTROY_BROWN) ? CardType::RAW_MATERIAL : CardType::MANUFACTURED;
        std::vector<std::shared_ptr<Card>> targets;
        for(auto c : opp->getConstructedCards()) if(c->getType() == targetType) targets.push_back(c);

        if(!targets.empty()) {
            int idx = active->chooseCardToDestroy(targets);
            if(idx != -1) opp->destroyBuiltCard(targetType);
        } else {
            std::cout << "对手没有可摧毁的卡牌。\n";
        }
    }
    else if (eff == WonderEffect::REVIVE_DISCARD) {
        const auto& dp = board.getDiscardPile();
        if(!dp.empty()) {
            int idx = active->chooseCardToRevive(dp);
            if(idx != -1) {
                auto card = board.takeFromDiscard(idx);
                active->buildCard(card, 0); // 免费建造
                std::cout << Color::GREEN << ">>> 复活了 " << card->getName() << "! <<<\n" << Color::RESET;
            }
        } else {
            std::cout << "弃牌堆为空，无法复活。\n";
        }
    }
}

// 处理科技标记选择
void Game::handleTokenSelection(std::shared_ptr<Player> p) {
    if(tokenPool.empty()) return;
    int idx = p->chooseProgressToken(tokenPool);
    if(idx != -1) {
        std::cout << Color::MAGENTA << ">>> " << p->getName() << " 获得了科技标记: " << tokenPool[idx].name << "! <<<\n" << Color::RESET;
        p->addProgressToken(tokenPool[idx].type);
        tokenPool.erase(tokenPool.begin() + idx);
    }
}

// 创建完整卡牌堆
std::vector<std::shared_ptr<Card>> Game::createDeck(int age) {
    std::vector<std::shared_ptr<Card>> deck;
    if (age == 1) {
        // --- 原料 (Raw) ---
        deck.push_back(std::make_shared<ResourceCard>("伐木场", CardType::RAW_MATERIAL, 0, std::vector<Resource>{Resource::WOOD}));
        deck.push_back(std::make_shared<ResourceCard>("伐木营地", CardType::RAW_MATERIAL, 1, std::vector<Resource>{Resource::WOOD}));
        deck.push_back(std::make_shared<ResourceCard>("粘土坑", CardType::RAW_MATERIAL, 0, std::vector<Resource>{Resource::CLAY}));
        deck.push_back(std::make_shared<ResourceCard>("采泥坑", CardType::RAW_MATERIAL, 1, std::vector<Resource>{Resource::CLAY}));
        deck.push_back(std::make_shared<ResourceCard>("采石场", CardType::RAW_MATERIAL, 0, std::vector<Resource>{Resource::STONE}));
        deck.push_back(std::make_shared<ResourceCard>("石坑", CardType::RAW_MATERIAL, 1, std::vector<Resource>{Resource::STONE}));
        // --- 制造 (Manufactured) ---
        deck.push_back(std::make_shared<ResourceCard>("玻璃厂", CardType::MANUFACTURED, 1, std::vector<Resource>{Resource::GLASS}));
        deck.push_back(std::make_shared<ResourceCard>("印刷厂", CardType::MANUFACTURED, 1, std::vector<Resource>{Resource::PAPYRUS}));
        // --- 商业 (Commercial) ---
        deck.push_back(std::make_shared<CommercialCard>("木材储备", 3, Resource::WOOD, 0));
        deck.push_back(std::make_shared<CommercialCard>("粘土储备", 3, Resource::CLAY, 0));
        deck.push_back(std::make_shared<CommercialCard>("石材储备", 3, Resource::STONE, 0));
        // --- 军事 (Military) ---
        deck.push_back(std::make_shared<MilitaryCard>("马厩", 0, std::map<Resource,int>{{Resource::WOOD,1}}, 1, "Horseshoe", ""));
        deck.push_back(std::make_shared<MilitaryCard>("驻军", 0, std::map<Resource,int>{{Resource::CLAY,1}}, 1, "Sword", ""));
        deck.push_back(std::make_shared<MilitaryCard>("栅栏", 0, std::map<Resource,int>{{Resource::STONE,1}}, 1, "Tower", ""));
        deck.push_back(std::make_shared<MilitaryCard>("守卫塔", 0, std::map<Resource,int>{}, 1));
        // --- 科技 (Science) ---
        deck.push_back(std::make_shared<ScienceCard>("药剂师", 2, std::map<Resource,int>{}, ScienceSymbol::WHEEL, 1, "Gear", ""));
        deck.push_back(std::make_shared<ScienceCard>("工坊", 2, std::map<Resource,int>{}, ScienceSymbol::GLOBE, 1, "Scope", ""));
        deck.push_back(std::make_shared<ScienceCard>("写字间", 2, std::map<Resource,int>{}, ScienceSymbol::WRITING, 1, "Book", ""));
        deck.push_back(std::make_shared<ScienceCard>("药房", 2, std::map<Resource,int>{}, ScienceSymbol::MORTAR, 1, "Pestle", ""));
        // --- 平民 (Civilian) ---
        deck.push_back(std::make_shared<CivilianCard>("祭坛", 0, std::map<Resource,int>{}, 3, "Moon", ""));
        deck.push_back(std::make_shared<CivilianCard>("剧院", 0, std::map<Resource,int>{}, 3, "Mask", ""));
        deck.push_back(std::make_shared<CivilianCard>("澡堂", 0, std::map<Resource,int>{{Resource::STONE,1}}, 3, "Drop", ""));
        deck.push_back(std::make_shared<CivilianCard>("当铺", 0, std::map<Resource,int>{}, 3));
    }
    else if (age == 2) {
        // --- Raw ---
        deck.push_back(std::make_shared<ResourceCard>("锯木厂", CardType::RAW_MATERIAL, 2, std::vector<Resource>{Resource::WOOD, Resource::WOOD}));
        deck.push_back(std::make_shared<ResourceCard>("砖厂", CardType::RAW_MATERIAL, 2, std::vector<Resource>{Resource::CLAY, Resource::CLAY}));
        deck.push_back(std::make_shared<ResourceCard>("层状采石场", CardType::RAW_MATERIAL, 2, std::vector<Resource>{Resource::STONE, Resource::STONE}));
        // --- Man ---
        deck.push_back(std::make_shared<ResourceCard>("玻璃吹制", CardType::MANUFACTURED, 0, std::vector<Resource>{Resource::GLASS}));
        deck.push_back(std::make_shared<ResourceCard>("干燥室", CardType::MANUFACTURED, 0, std::vector<Resource>{Resource::PAPYRUS}));
        // --- Com ---
        deck.push_back(std::make_shared<ResourceCard>("论坛", CardType::COMMERCIAL, 3, std::vector<Resource>{Resource::GLASS, Resource::PAPYRUS}));
        deck.push_back(std::make_shared<CommercialCard>("酿酒厂", 0, Resource::NONE, 0, 6)); // 产6金币
        deck.push_back(std::make_shared<ResourceCard>("驿站", CardType::COMMERCIAL, 2, std::vector<Resource>{Resource::WOOD, Resource::CLAY, Resource::STONE}));
        // --- Mil ---
        deck.push_back(std::make_shared<MilitaryCard>("骑兵", 3, std::map<Resource,int>{{Resource::WOOD,1}}, 2, "", "Horseshoe"));
        deck.push_back(std::make_shared<MilitaryCard>("城墙", 0, std::map<Resource,int>{{Resource::STONE,2}}, 2));
        deck.push_back(std::make_shared<MilitaryCard>("军械库", 0, std::map<Resource,int>{{Resource::WOOD,2},{Resource::CLAY,1}}, 2));
        deck.push_back(std::make_shared<MilitaryCard>("靶场", 0, std::map<Resource,int>{{Resource::STONE,1},{Resource::WOOD,1}}, 2, "", "Target"));
        // --- Sci ---
        deck.push_back(std::make_shared<ScienceCard>("图书馆", 2, std::map<Resource,int>{{Resource::STONE,1},{Resource::WOOD,1}}, ScienceSymbol::WRITING, 2, "", "Book"));
        deck.push_back(std::make_shared<ScienceCard>("学校", 2, std::map<Resource,int>{{Resource::WOOD,1},{Resource::PAPYRUS,1}}, ScienceSymbol::WHEEL, 2, "", "Gear"));
        deck.push_back(std::make_shared<ScienceCard>("实验室", 2, std::map<Resource,int>{{Resource::CLAY,2}}, ScienceSymbol::GLOBE, 2, "", "Scope"));
        deck.push_back(std::make_shared<ScienceCard>("医务室", 2, std::map<Resource,int>{{Resource::STONE,2}}, ScienceSymbol::MORTAR, 2, "", "Pestle"));
        // --- Civ ---
        deck.push_back(std::make_shared<CivilianCard>("雕像", 0, std::map<Resource,int>{{Resource::CLAY,2}}, 4, "", "Theater"));
        deck.push_back(std::make_shared<CivilianCard>("神庙", 0, std::map<Resource,int>{{Resource::WOOD,1},{Resource::GLASS,1}}, 4, "", "Moon"));
        deck.push_back(std::make_shared<CivilianCard>("引水渠", 0, std::map<Resource,int>{{Resource::STONE,3}}, 5, "", "Drop"));
        deck.push_back(std::make_shared<CivilianCard>("讲坛", 0, std::map<Resource,int>{{Resource::STONE,1},{Resource::WOOD,1}}, 4, "Sun", ""));
    }
    else { // Age 3
        // --- Guilds ---
        deck.push_back(std::make_shared<GuildCard>("商人工会", std::map<Resource,int>{{Resource::WOOD,1},{Resource::GLASS,1}}, "Merchants"));
        deck.push_back(std::make_shared<GuildCard>("船主工会", std::map<Resource,int>{{Resource::CLAY,1},{Resource::GLASS,1},{Resource::PAPYRUS,1}}, "Shipowners"));
        deck.push_back(std::make_shared<GuildCard>("科学家工会", std::map<Resource,int>{{Resource::WOOD,2}}, "Scientists"));
        deck.push_back(std::make_shared<GuildCard>("行政官工会", std::map<Resource,int>{{Resource::WOOD,2},{Resource::PAPYRUS,1}}, "Magistrates"));
        deck.push_back(std::make_shared<GuildCard>("建造者工会", std::map<Resource,int>{{Resource::STONE,2},{Resource::CLAY,1}}, "Builders"));
        deck.push_back(std::make_shared<GuildCard>("战术家工会", std::map<Resource,int>{{Resource::STONE,2},{Resource::CLAY,1}}, "Tacticians"));
        deck.push_back(std::make_shared<GuildCard>("放贷人工会", std::map<Resource,int>{{Resource::WOOD,2},{Resource::STONE,2}}, "Moneylenders"));

        // --- Mil ---
        deck.push_back(std::make_shared<MilitaryCard>("要塞", 0, std::map<Resource,int>{{Resource::STONE,2},{Resource::CLAY,2}}, 3));
        deck.push_back(std::make_shared<MilitaryCard>("攻城锤", 0, std::map<Resource,int>{{Resource::WOOD,3}}, 3));
        deck.push_back(std::make_shared<MilitaryCard>("竞技场", 0, std::map<Resource,int>{{Resource::CLAY,2},{Resource::STONE,1}}, 3));

        // --- Sci ---
        deck.push_back(std::make_shared<ScienceCard>("天文台", 0, std::map<Resource,int>{{Resource::STONE,2},{Resource::PAPYRUS,1}}, ScienceSymbol::MECHANIC, 3, "", "Sun"));
        deck.push_back(std::make_shared<ScienceCard>("大学", 0, std::map<Resource,int>{{Resource::CLAY,2},{Resource::GLASS,1}}, ScienceSymbol::GLOBE, 3));
        deck.push_back(std::make_shared<ScienceCard>("研究院", 0, std::map<Resource,int>{{Resource::WOOD,2},{Resource::GLASS,1}}, ScienceSymbol::WHEEL, 3));

        // --- Civ ---
        deck.push_back(std::make_shared<CivilianCard>("宫殿", 0, std::map<Resource,int>{{Resource::STONE,1},{Resource::GLASS,1}}, 7));
        deck.push_back(std::make_shared<CivilianCard>("方尖碑", 0, std::map<Resource,int>{{Resource::STONE,2}}, 5));
        deck.push_back(std::make_shared<CivilianCard>("花园", 0, std::map<Resource,int>{{Resource::CLAY,2},{Resource::WOOD,2}}, 6));
        deck.push_back(std::make_shared<CivilianCard>("参议院", 0, std::map<Resource,int>{{Resource::CLAY,2},{Resource::PAPYRUS,1}}, 5));
    }
    return deck;
}

void Game::run() {
    setup();
    std::shared_ptr<Player> nextStartPlayer = p1; // Age 1 默认 P1 先手

    for (int age = 1; age <= 3; ++age) {
        if(isGameOver) break;
        currentAge = age;

        auto deck = createDeck(age);
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(deck.begin(), deck.end(), std::default_random_engine(seed));
        board.initAge(age, deck);

        auto active = nextStartPlayer;
        auto opp = (active == p1) ? p2 : p1;

        drawHeader(age, militaryToken, p1->getName(), p2->getName());
        p1->displayStatus(); p2->displayStatus();
        std::cout << "\n>>> 准备进入时代 " << age << "... <<<\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));

        while (!isGameOver) {
            bool replay = false;
            do {
                drawHeader(age, militaryToken, p1->getName(), p2->getName());

                if(replay) std::cout << Color::MAGENTA << ">>> [再来一回合] " << active->getName() << " 继续行动! <<<\n" << Color::RESET;
                else std::cout << ">>> 轮到 " << active->getName() << " 行动 <<<\n";

                p1->displayStatus();
                p2->displayStatus();
                board.display();

                if(board.getAvailableCardIndices().empty()) { replay=false; break; }

                replay = playTurn(active, opp);

                if(checkInstantVictory()) { isGameOver=true; replay=false; break; }

                if(replay) std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            } while(replay);

            if(isGameOver) break;
            std::swap(active, opp);
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
        }

        // [专家] 时代结束，判定下一时代先手
        if(!isGameOver && age < 3) {
            std::shared_ptr<Player> chooser = nullptr;
            // militaryToken > 0 表示 P1 占优 (标记在 P2 侧)，P2 弱 -> P2 选
            // militaryToken < 0 表示 P2 占优，P1 弱 -> P1 选
            if (militaryToken < 0) chooser = p1;
            else if (militaryToken > 0) chooser = p2;
            else chooser = active; // 平局维持最后行动者

            int choice = chooser->chooseWhoStarts(p1->getName(), p2->getName());
            nextStartPlayer = (choice == 1) ? p1 : p2;
        }
    }
    calculateScore();
}

bool Game::playTurn(std::shared_ptr<Player> active, std::shared_ptr<Player> opp) {
    bool isHuman = (std::dynamic_pointer_cast<HumanPlayer>(active) != nullptr);
    bool replay = false;
    auto idxs = board.getAvailableCardIndices();

    if(isHuman) {
        std::cout << "可选卡牌:\n";
        for(int i : idxs) {
            auto c = board.getSlot(i).card;
            int cost = active->calculateActualCost(c, opp);
            std::cout << i << ": " << c->getName();

            if(cost==0 && !c->getChainCost().empty()) std::cout << Color::GREEN << " [连锁免费]" << Color::RESET;
            else if(cost == -1) std::cout << Color::RED << " [买不起]" << Color::RESET;
            else std::cout << " $" << cost;

            if(c->getImmediateCoins()>0) std::cout << " (得" << c->getImmediateCoins() << "$)";
            std::cout << "\n";
        }
    }

    int choice = active->makeDecision(idxs, board);
    if(choice == -1) { isGameOver=true; return false; }
    auto card = board.removeCard(choice);
    int cost = active->calculateActualCost(card, opp);

    int action = 1; // 1Build, 2Sell, 3Wonder
    if(isHuman) {
        std::cout << "1.建造  2.卖出(+" << (2 + active->getCardCount(CardType::COMMERCIAL)) << "$)  3.奇迹 >> ";
        std::cin >> action;
    } else {
        // AI 逻辑
        bool canWonder = false;
        for(int i=0; i<4; ++i) if(active->canBuildWonder(i, opp)) canWonder = true;

        if(canWonder && wondersBuiltCount < 7) action = 3;
        else if(cost != -1) action = 1;
        else action = 2;
    }

    if(action == 1 && cost != -1) {
        // 建造
        bool pairTrigger = active->buildCard(card, cost);
        if(pairTrigger) handleTokenSelection(active);

        // 即时金币
        if(card->getImmediateCoins() > 0) active->addCoins(card->getImmediateCoins());

        // 经济学 Token: 贸易费给对手
        if(opp->hasToken(TokenType::ECONOMY) && cost > 0) {
            int tradeEstimate = std::max(0, cost - card->getCostCoins());
            if(tradeEstimate > 0) opp->addCoins(tradeEstimate);
        }

        if(card->getType()==CardType::MILITARY) {
            // militaryToken 正向是 P1 优势区(右移), 负向是 P2 优势区(左移)
            // 这里我们统一定义: P1 让标记右移(+), P2 让标记左移(-)
            // 之前的 drawHeader 逻辑是: 负数区为P1优势，正数区为P2优势。
            // 让我们修正 drawHeader 的逻辑以匹配直觉: P1 把标记推向 P2 (正数方向)，P2 推向 P1 (负数方向)

            // 修正后的推力逻辑：
            int shields = card->getShields();
            if (active == p1) militaryToken += shields; // P1 向右推
            else militaryToken -= shields; // P2 向左推
        }
    }
    else if (action == 3) {
        // 奇迹
        if (wondersBuiltCount >= 7) {
            std::cout << Color::RED << "!!! 奇迹上限已满(7) !!! 强制卖出。\n" << Color::RESET;
            active->addCoins(2 + active->getCardCount(CardType::COMMERCIAL));
            board.addToDiscard(card);
        } else {
            int wid = 0;
            if(isHuman) {
                std::cout << "输入奇迹 ID (0-3): "; std::cin >> wid;
            } else {
                for(int i=0; i<4; ++i) if(active->canBuildWonder(i, opp)) wid=i;
            }

            if(active->canBuildWonder(wid, opp)) {
                active->buildWonder(wid, active->calculateWonderCost(active->getWonders()[wid], opp));
                wondersBuiltCount++;
                auto w = active->getWonders()[wid];
                if(w.hasReplay || active->hasToken(TokenType::THEOLOGY)) replay = true;
                handleWonderEffect(w.effect, active, opp);
            } else {
                std::cout << "无法建造该奇迹，转为卖出。\n";
                active->addCoins(2 + active->getCardCount(CardType::COMMERCIAL));
                board.addToDiscard(card);
            }
        }
    }
    else {
        // 卖出
        active->addCoins(2 + active->getCardCount(CardType::COMMERCIAL));
        board.addToDiscard(card);
    }
    return replay;
}

bool Game::checkInstantVictory() {
    // 军事胜利：如果标记达到两端
    if(militaryToken <= -9) {
        std::cout << Color::MAGENTA << "\n>>> 军事胜利! " << p2->getName() << " 征服了对手! <<<\n" << Color::RESET;
        return true;
    }
    if(militaryToken >= 9) {
        std::cout << Color::MAGENTA << "\n>>> 军事胜利! " << p1->getName() << " 征服了对手! <<<\n" << Color::RESET;
        return true;
    }

    // 科技胜利
    if(p1->getScienceCount() >= 6) {
        std::cout << Color::CYAN << "\n>>> 科技胜利! " << p1->getName() << " 取得了科技霸权! <<<\n" << Color::RESET;
        return true;
    }
    if(p2->getScienceCount() >= 6) {
        std::cout << Color::CYAN << "\n>>> 科技胜利! " << p2->getName() << " 取得了科技霸权! <<<\n" << Color::RESET;
        return true;
    }
    return false;
}

void Game::calculateScore() {
    std::cout << "\n" << Color::BOLD << "=== 最终结算 (CIVILIAN VICTORY) ===" << Color::RESET << "\n";
    auto calc = [&](std::shared_ptr<Player> p, std::shared_ptr<Player> opp) {
        int s = p->getCoins()/3;
        for(auto w : p->getWonders()) if(w.isBuilt) s+=w.victoryPoints;
        for(auto c : p->getConstructedCards()) {
            s += c->getVictoryPoints();
            if(c->getType() == CardType::GUILD) {
                std::string g = c->getGuildType();
                if(g == "Merchants") s += 1 * std::max(p->getCardCount(CardType::COMMERCIAL), opp->getCardCount(CardType::COMMERCIAL));
                if(g == "Shipowners") s += 1 * std::max(p->getCardCount(CardType::RAW_MATERIAL)+p->getCardCount(CardType::MANUFACTURED), opp->getCardCount(CardType::RAW_MATERIAL)+opp->getCardCount(CardType::MANUFACTURED));
                if(g == "Scientists") s += 1 * std::max(p->getCardCount(CardType::SCIENTIFIC), opp->getCardCount(CardType::SCIENTIFIC));
                if(g == "Magistrates") s += 1 * std::max(p->getCardCount(CardType::CIVILIAN), opp->getCardCount(CardType::CIVILIAN));
                if(g == "Tacticians") s += 1 * std::max(p->getCardCount(CardType::MILITARY), opp->getCardCount(CardType::MILITARY));
                if(g == "Builders") {
                    int w1=0, w2=0;
                    for(auto w:p->getWonders()) if(w.isBuilt) w1++;
                    for(auto w:opp->getWonders()) if(w.isBuilt) w2++;
                    s += 2 * std::max(w1, w2);
                }
                if(g == "Moneylenders") s += 1 * std::max(p->getCoins()/3, opp->getCoins()/3);
            }
        }
        if(p->hasToken(TokenType::MATHEMATICS)) s += 3;
        return s;
    };

    int s1 = calc(p1, p2);
    int s2 = calc(p2, p1);

    // 军事分结算 (根据标记位置给分)
    // 简化规则：每格 1 分
    if(militaryToken > 0) s1 += militaryToken;
    else s2 += abs(militaryToken);

    std::cout << p1->getName() << ": " << s1 << " 分\n";
    std::cout << p2->getName() << ": " << s2 << " 分\n";

    if (s1 > s2) std::cout << Color::GREEN << "胜者: " << p1->getName() << " !!!\n" << Color::RESET;
    else if (s2 > s1) std::cout << Color::GREEN << "胜者: " << p2->getName() << " !!!\n" << Color::RESET;
    else {
        // 同分判定：比蓝卡分
        int blue1 = 0, blue2 = 0;
        for(auto c : p1->getConstructedCards()) if(c->getType()==CardType::CIVILIAN) blue1 += c->getVictoryPoints();
        for(auto c : p2->getConstructedCards()) if(c->getType()==CardType::CIVILIAN) blue2 += c->getVictoryPoints();
        std::cout << "平分! 判定蓝卡分: " << blue1 << " vs " << blue2 << "\n";
        if (blue1 > blue2) std::cout << "胜者: " << p1->getName() << "\n";
        else if (blue2 > blue1) std::cout << "胜者: " << p2->getName() << "\n";
        else std::cout << "完全平局!\n";
    }
}