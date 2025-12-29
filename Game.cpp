#include "Game.h"
#include "GameConfig.h"
#include "BaseGameCardFactory.h"
#include "RandomStrategy.h"
#include "GreedyStrategy.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>
#include <iomanip>

// 清屏函数：使用 ANSI 转义序列
void clearScreen()
{
    std::cout << "\033[2J\033[1;1H";
}

Game::Game()
    : currentAge(1), isGameOver(false), militaryToken(0), wondersBuiltCount(0),
      cardFactory(std::make_shared<BaseGameCardFactory>()) {}

Game::Game(std::shared_ptr<CardFactory> factory)
    : currentAge(1), isGameOver(false), militaryToken(0), wondersBuiltCount(0),
      cardFactory(factory) {}

void Game::applyConfig(const GameConfig& config) {
    // 应用配置参数
    player1Name = config.player1Name;
    player2Name = config.player2Name;
    startingCoins = config.startingCoins;
    wondersPerPlayer = config.wondersPerPlayer;
    enableProgressTokens = config.enableProgressTokens;

    // 更新卡牌工厂
    if (config.cardFactory) {
        cardFactory = config.cardFactory;
    } else {
        cardFactory = config.getCardFactory();
    }
}

void Game::clearInputBuffer()
{
    std::cin.clear();
    std::cin.ignore(1000, '\n');
}

// ==========================================
// [UI] 绘制顶部状态栏 (动态军事条)
// ==========================================
void drawHeader(int age, int militaryToken, std::string p1Name, std::string p2Name, const std::vector<ProgressToken> &tokenPool)
{
    clearScreen();
    std::cout << Color::BOLD << " >>> 7 WONDERS DUEL - AGE " << age << " <<<" << Color::RESET << "\n";

    // 绘制军事条: 范围 -9 到 9，共 19 个格。中心是 0
    std::cout << "\nMILITARY: " << std::left << std::setw(10) << p1Name << " ";

    // P1 优势区 (负数区域)
    std::cout << Color::RED;
    for (int i = -9; i < 0; ++i)
    {
        if (i == militaryToken)
            std::cout << "⚔️";
        else if (i == -9)
            std::cout << "[Win]";
        else if (i == -6 || i == -3)
            std::cout << "|";
        else
            std::cout << "-";
    }
    std::cout << Color::RESET;

    // 中心点
    if (militaryToken == 0)
        std::cout << Color::YELLOW << " ⚔️ " << Color::RESET;
    else
        std::cout << " | ";

    // P2 优势区 (正数区域)
    std::cout << Color::BLUE;
    for (int i = 1; i <= 9; ++i)
    {
        if (i == militaryToken)
            std::cout << "⚔️";
        else if (i == 9)
            std::cout << "[Win]";
        else if (i == 6 || i == 3)
            std::cout << "|";
        else
            std::cout << "-";
    }
    std::cout << Color::RESET;

    std::cout << " " << p2Name << "\n";

    // 显示可用科技标记
    if (!tokenPool.empty())
    {
        std::cout << "\n"
                  << Color::MAGENTA << "🎯 可用科技标记 (" << tokenPool.size() << "/8): " << Color::RESET;
        for (size_t i = 0; i < tokenPool.size(); ++i)
        {
            std::cout << Color::CYAN << tokenPool[i].name << Color::RESET;
            if (i < tokenPool.size() - 1)
                std::cout << " | ";
        }
        std::cout << "\n";
    }
    else
    {
        std::cout << "\n"
                  << Color::GREY << "🎯 科技标记已全部获取" << Color::RESET << "\n";
    }
    std::cout << "\n";
}

void Game::setup()
{
#ifdef _WIN32
    system("chcp 65001 > nul"); // 设置 Windows 终端为 UTF-8
#endif
    clearScreen();
    std::cout << Color::BOLD << "=== 7 WONDERS DUEL: FINAL EDITION ===" << Color::RESET << "\n";
    std::cout << "1. PvP (玩家 vs 玩家)\n2. PvE (玩家 vs 电脑)\n>> ";
    int m;
    if (!(std::cin >> m))
        m = 2; // 默认 PvE

    if (m == 1)
    {
        p1 = std::make_shared<HumanPlayer>("Player 1");
        p2 = std::make_shared<HumanPlayer>("Player 2");
    }
    else
    {
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
        {"神学", TokenType::THEOLOGY, "所有奇迹获再来一回合"}};

    // 创建奇迹池 (所有可用奇迹)
    std::vector<Wonder> wonderPool = {
        Wonder("阿耳忒弥斯神庙", {{Resource::WOOD, 1}, {Resource::STONE, 1}, {Resource::GLASS, 1}}, 0, 0, WonderEffect::REPLAY, true),
        Wonder("大金字塔", {{Resource::STONE, 3}, {Resource::PAPYRUS, 1}}, 9, 0, WonderEffect::NONE, false),
        Wonder("斯芬克斯", {{Resource::STONE, 1}, {Resource::CLAY, 1}, {Resource::GLASS, 2}}, 6, 0, WonderEffect::REPLAY, true),
        Wonder("宙斯神像", {{Resource::WOOD, 1}, {Resource::STONE, 2}, {Resource::CLAY, 1}}, 3, 1, WonderEffect::DESTROY_BROWN, false),
        Wonder("大图书馆", {{Resource::WOOD, 3}, {Resource::GLASS, 1}, {Resource::PAPYRUS, 1}}, 4, 0, WonderEffect::NONE, false),
        Wonder("摩索拉斯王陵墓", {{Resource::CLAY, 2}, {Resource::GLASS, 1}, {Resource::PAPYRUS, 1}}, 2, 0, WonderEffect::REVIVE_DISCARD, false),
        Wonder("空中花园", {{Resource::WOOD, 2}, {Resource::PAPYRUS, 2}}, 3, 0, WonderEffect::REPLAY, true),
        Wonder("马克西穆斯", {{Resource::STONE, 2}, {Resource::WOOD, 1}}, 3, 1, WonderEffect::DESTROY_GREY, false),
        Wonder("大灯塔", {{Resource::CLAY, 1}, {Resource::STONE, 1}, {Resource::WOOD, 1}, {Resource::GLASS, 1}}, 4, 0, WonderEffect::NONE, false),
        Wonder("罗德岛巨像", {{Resource::CLAY, 3}, {Resource::GLASS, 1}}, 3, 2, WonderEffect::NONE, false),
        Wonder("庇伦神殿", {{Resource::STONE, 1}, {Resource::CLAY, 1}, {Resource::WOOD, 1}, {Resource::PAPYRUS, 1}}, 2, 0, WonderEffect::REPLAY, true),
        Wonder("哈利卡纳苏斯陵墓", {{Resource::CLAY, 2}, {Resource::PAPYRUS, 1}}, 3, 0, WonderEffect::REVIVE_DISCARD, false)};

    // 随机打乱奇迹池
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(wonderPool.begin(), wonderPool.end(), std::default_random_engine(seed));

    // 奇迹抽取阶段
    std::cout << "\n"
              << Color::BOLD << "=== 奇迹抽取阶段 ===" << Color::RESET << "\n";
    std::cout << "规则：两轮轮抽，每轮4张奇迹\n";
    std::cout << "每轮：先手选1张 -> 后手选2张 -> 剩余1张给先手\n";
    std::cout << "第二轮先手顺序对调\n\n";

    // 辅助函数：显示奇迹列表
    auto displayWonders = [](const std::vector<Wonder> &wonders, int startIdx = 0)
    {
        for (size_t i = 0; i < wonders.size(); ++i)
        {
            const auto &w = wonders[i];
            std::cout << (startIdx + i) << ". " << Color::BOLD << w.name << Color::RESET;
            std::cout << " - 成本:[";
            for (auto &p : w.costResources)
            {
                if (p.first == Resource::WOOD)
                    std::cout << "木" << p.second << " ";
                else if (p.first == Resource::CLAY)
                    std::cout << "泥" << p.second << " ";
                else if (p.first == Resource::STONE)
                    std::cout << "石" << p.second << " ";
                else if (p.first == Resource::GLASS)
                    std::cout << "玻" << p.second << " ";
                else if (p.first == Resource::PAPYRUS)
                    std::cout << "纸" << p.second << " ";
            }
            std::cout << "] ";
            if (w.victoryPoints > 0)
                std::cout << w.victoryPoints << "分 ";
            if (w.shields > 0)
                std::cout << w.shields << "盾 ";
            if (w.hasReplay)
                std::cout << "🔄再行动 ";
            if (w.effect == WonderEffect::DESTROY_BROWN)
                std::cout << "💥毁原料 ";
            if (w.effect == WonderEffect::DESTROY_GREY)
                std::cout << "💥毁制品 ";
            if (w.effect == WonderEffect::REVIVE_DISCARD)
                std::cout << "♻️复活 ";
            std::cout << "\n";
        }
    };

    // 辅助函数：进行选择
    auto makeChoice = [](std::shared_ptr<Player> player, const std::vector<Wonder> &options, int maxChoice) -> int
    {
        int choice = 0;
        if (auto human = std::dynamic_pointer_cast<HumanPlayer>(player))
        {
            std::cout << player->getName() << " 选择 (0-" << (maxChoice - 1) << "): ";
            std::cin >> choice;
            if (choice < 0 || choice >= maxChoice)
            {
                std::cout << Color::YELLOW << "无效选择，默认选择0\n"
                          << Color::RESET;
                choice = 0;
            }
        }
        else
        {
            // AI 简单选择策略：优先高分、有盾、有再行动
            int bestIdx = 0;
            int bestScore = -1000;
            for (size_t i = 0; i < options.size() && i < maxChoice; ++i)
            {
                int score = options[i].victoryPoints * 2 + options[i].shields * 3;
                if (options[i].hasReplay)
                    score += 5;
                if (score > bestScore)
                {
                    bestScore = score;
                    bestIdx = i;
                }
            }
            choice = bestIdx;
        }
        return choice;
    };

    // 第一轮：4张奇迹，P1先选
    std::cout << Color::BOLD << "--- 第一轮 ---" << Color::RESET << "\n";
    std::vector<Wonder> round1Wonders;
    for (int i = 0; i < 4 && !wonderPool.empty(); ++i)
    {
        round1Wonders.push_back(wonderPool.back());
        wonderPool.pop_back();
    }

    // P1 先选1张
    std::cout << "\n"
              << Color::CYAN << p1->getName() << " 先选1张:\n"
              << Color::RESET;
    displayWonders(round1Wonders);
    int choice1 = makeChoice(p1, round1Wonders, round1Wonders.size());
    p1->assignWonder(round1Wonders[choice1]);
    std::cout << Color::GREEN << "✓ " << p1->getName() << " 选择了: " << round1Wonders[choice1].name << Color::RESET << "\n";
    round1Wonders.erase(round1Wonders.begin() + choice1);

    // P2 选2张
    std::cout << "\n"
              << Color::CYAN << p2->getName() << " 选2张:\n"
              << Color::RESET;
    for (int i = 0; i < 2 && !round1Wonders.empty(); ++i)
    {
        std::cout << "\n第" << (i + 1) << "张选择:\n";
        displayWonders(round1Wonders);
        int choice2 = makeChoice(p2, round1Wonders, round1Wonders.size());
        p2->assignWonder(round1Wonders[choice2]);
        std::cout << Color::GREEN << "✓ " << p2->getName() << " 选择了: " << round1Wonders[choice2].name << Color::RESET << "\n";
        round1Wonders.erase(round1Wonders.begin() + choice2);
    }

    // 剩余1张给P1
    if (!round1Wonders.empty())
    {
        p1->assignWonder(round1Wonders[0]);
        std::cout << Color::GREEN << "✓ " << p1->getName() << " 获得剩余奇迹: " << round1Wonders[0].name << Color::RESET << "\n";
    }

    // 第二轮：4张奇迹，P2先选
    std::cout << "\n"
              << Color::BOLD << "--- 第二轮 ---" << Color::RESET << "\n";
    std::vector<Wonder> round2Wonders;
    for (int i = 0; i < 4 && !wonderPool.empty(); ++i)
    {
        round2Wonders.push_back(wonderPool.back());
        wonderPool.pop_back();
    }

    // P2 先选1张
    std::cout << "\n"
              << Color::CYAN << p2->getName() << " 先选1张:\n"
              << Color::RESET;
    displayWonders(round2Wonders);
    int choice3 = makeChoice(p2, round2Wonders, round2Wonders.size());
    p2->assignWonder(round2Wonders[choice3]);
    std::cout << Color::GREEN << "✓ " << p2->getName() << " 选择了: " << round2Wonders[choice3].name << Color::RESET << "\n";
    round2Wonders.erase(round2Wonders.begin() + choice3);

    // P1 选2张
    std::cout << "\n"
              << Color::CYAN << p1->getName() << " 选2张:\n"
              << Color::RESET;
    for (int i = 0; i < 2 && !round2Wonders.empty(); ++i)
    {
        std::cout << "\n第" << (i + 1) << "张选择:\n";
        displayWonders(round2Wonders);
        int choice4 = makeChoice(p1, round2Wonders, round2Wonders.size());
        p1->assignWonder(round2Wonders[choice4]);
        std::cout << Color::GREEN << "✓ " << p1->getName() << " 选择了: " << round2Wonders[choice4].name << Color::RESET << "\n";
        round2Wonders.erase(round2Wonders.begin() + choice4);
    }

    // 剩余1张给P2
    if (!round2Wonders.empty())
    {
        p2->assignWonder(round2Wonders[0]);
        std::cout << Color::GREEN << "✓ " << p2->getName() << " 获得剩余奇迹: " << round2Wonders[0].name << Color::RESET << "\n";
    }

    std::cout << "\n"
              << Color::BOLD << "奇迹分配完成! 按回车继续..." << Color::RESET;
    std::cin.ignore();
    std::cin.get();
}

// 处理奇迹特效 (破坏、复活)
void Game::handleWonderEffect(WonderEffect eff, std::shared_ptr<Player> active, std::shared_ptr<Player> opp)
{
    if (eff == WonderEffect::DESTROY_BROWN || eff == WonderEffect::DESTROY_GREY)
    {
        CardType targetType = (eff == WonderEffect::DESTROY_BROWN) ? CardType::RAW_MATERIAL : CardType::MANUFACTURED;
        std::vector<std::shared_ptr<Card>> targets;
        for (auto c : opp->getConstructedCards())
            if (c->getType() == targetType)
                targets.push_back(c);

        if (!targets.empty())
        {
            int idx = active->chooseCardToDestroy(targets);
            if (idx != -1)
                opp->destroyBuiltCard(targetType);
        }
        else
        {
            std::cout << "对手没有可摧毁的卡牌。\n";
        }
    }
    else if (eff == WonderEffect::REVIVE_DISCARD)
    {
        const auto &dp = board.getDiscardPile();
        if (!dp.empty())
        {
            int idx = active->chooseCardToRevive(dp);
            if (idx != -1)
            {
                auto card = board.takeFromDiscard(idx);
                active->buildCard(card, 0); // 免费建造
                std::cout << Color::GREEN << ">>> 复活了 " << card->getName() << "! <<<\n"
                          << Color::RESET;
            }
        }
        else
        {
            std::cout << "弃牌堆为空，无法复活。\n";
        }
    }
}

// 处理科技标记选择
void Game::handleTokenSelection(std::shared_ptr<Player> p)
{
    if (tokenPool.empty())
        return;
    int idx = p->chooseProgressToken(tokenPool);
    if (idx != -1)
    {
        std::cout << Color::MAGENTA << ">>> " << p->getName() << " 获得了科技标记: " << tokenPool[idx].name << "! <<<\n"
                  << Color::RESET;
        p->addProgressToken(tokenPool[idx].type);
        tokenPool.erase(tokenPool.begin() + idx);
    }
}

// [Factory Pattern] createDeck方法已被CardFactory替代，数据移至BaseGameCardFactory

void Game::run()
{
    setup();
    std::shared_ptr<Player> nextStartPlayer = p1; // Age 1 默认 P1 先手

    for (int age = 1; age <= 3; ++age)
    {
        if (isGameOver)
            break;
        currentAge = age;

        // [Factory Pattern] 使用工厂创建卡牌
        auto deck = cardFactory->createDeck(age);
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(deck.begin(), deck.end(), std::default_random_engine(seed));
        board.initAge(age, deck);

        auto active = nextStartPlayer;
        auto opp = (active == p1) ? p2 : p1;

        drawHeader(age, militaryToken, p1->getName(), p2->getName(), tokenPool);
        p1->displayStatus();
        p2->displayStatus();
        std::cout << "\n>>> 准备进入时代 " << age << "... <<<\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // ... (在 Game::run 函数内部) ...

        while (!isGameOver)
        {
            // [新增] 每一大回合开始前，先检查还有没有牌。如果没有牌，说明时代结束。
            if (board.getAvailableCardIndices().empty())
            {
                break; // 跳出 while(!isGameOver)，让代码向下执行去切换时代
            }

            bool replay = false;
            do
            {
                drawHeader(age, militaryToken, p1->getName(), p2->getName(), tokenPool);

                if (replay)
                    std::cout << Color::MAGENTA << ">>> [再来一回合] " << active->getName() << " 继续行动! <<<\n"
                              << Color::RESET;
                else
                    std::cout << ">>> 轮到 " << active->getName() << " 行动 <<<\n";

                p1->displayStatus();
                p2->displayStatus();
                board.display();

                if (board.getAvailableCardIndices().empty())
                {
                    replay = false;
                    break; // 这里只能跳出 do...while
                }

                replay = playTurn(active, opp);

                if (checkInstantVictory())
                {
                    isGameOver = true;
                    replay = false;
                    break;
                }

                if (replay)
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            } while (replay);

            // [新增] 再次检查：如果是因没牌而跳出内层循环的，这里必须彻底跳出外层循环
            if (board.getAvailableCardIndices().empty())
            {
                break; // 关键修复！跳出 while，进入下一时代处理逻辑
            }

            if (isGameOver)
                break;
            std::swap(active, opp);
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
        }

        // [专家] 时代结束，判定下一时代先手
        if (!isGameOver && age < 3)
        // ... (后续代码不变) ...
        {
            std::shared_ptr<Player> chooser = nullptr;
            // militaryToken > 0 表示 P1 占优 (标记在 P2 侧)，P2 弱 -> P2 选
            // militaryToken < 0 表示 P2 占优，P1 弱 -> P1 选
            if (militaryToken < 0)
                chooser = p1;
            else if (militaryToken > 0)
                chooser = p2;
            else
                chooser = active; // 平局维持最后行动者

            int choice = chooser->chooseWhoStarts(p1->getName(), p2->getName());
            nextStartPlayer = (choice == 1) ? p1 : p2;
        }
    }
    calculateScore();
}

std::string getCardDesc(std::shared_ptr<Card> c) {
    if (c->getType() == CardType::GUILD) {
        std::string g = c->getGuildType();
        if (g == "Merchants") return "效果: 结算时，自己和对手每有一张[黄卡]，得1分。";
        if (g == "Shipowners") return "效果: 结算时，自己和对手每有一张[棕卡]和[灰卡]，得1分。";
        if (g == "Scientists") return "效果: 结算时，自己和对手每有一张[绿卡]，得1分。";
        if (g == "Magistrates") return "效果: 结算时，自己和对手每有一张[蓝卡]，得1分。";
        if (g == "Tacticians") return "效果: 结算时，自己和对手每有一张[红卡]，得1分。";
        if (g == "Builders") return "效果: 结算时，自己和对手每有一个[已建奇迹]，得2分。";
        if (g == "Moneylenders") return "效果: 结算时，自己和对手每有3枚金币，得1分。";
    }
    // 其他卡的简单描述
    if (c->getType() == CardType::COMMERCIAL) {
        std::string desc = "";
        if(c->getImmediateCoins() > 0) desc += "立刻获得" + std::to_string(c->getImmediateCoins()) + "金币。";
        if(c->getVictoryPoints() > 0) desc += " " + std::to_string(c->getVictoryPoints()) + "分。";
        if(c->getDiscountResource() != Resource::NONE) desc += " 购买资源时减免1金。";
        return desc;
    }
    if (c->getType() == CardType::SCIENTIFIC) return "集齐6种不同符号获科技胜利，或集齐2个相同获科技标记。";
    if (c->getType() == CardType::MILITARY) return "增加盾牌，推进军事条。";
    return "";
}

// [Game.cpp] 替换 playTurn 函数
bool Game::playTurn(std::shared_ptr<Player> active, std::shared_ptr<Player> opp)
{
    bool isHuman = (std::dynamic_pointer_cast<HumanPlayer>(active) != nullptr);
    bool replay = false;
    auto idxs = board.getAvailableCardIndices();

    if (isHuman)
    {
        std::cout << Color::BOLD << "\n>>> 你的回合 (" << active->getName() << ") <<<\n" << Color::RESET;
        std::cout << "可选卡牌列表:\n";
        for (int i : idxs)
        {
            auto c = board.getSlot(i).card;
            int cost = active->calculateActualCost(c, opp);

            // 打印卡牌基本信息
            std::cout << Color::YELLOW << "ID " << std::left << std::setw(2) << i << Color::RESET
                      << " : " << std::setw(12) << c->getName();

            // 打印价格
            if (cost == 0 && !c->getChainCost().empty())
                std::cout << Color::GREEN << " [连锁免费]" << Color::RESET;
            else if (cost == -1)
                std::cout << Color::RED << " [资源不足且钱不够]" << Color::RESET;
            else if (cost == 0)
                 std::cout << Color::GREEN << " [免费]" << Color::RESET;
            else
                std::cout << " [需支付: " << cost << "金]";

            // 打印详细说明 (工会效果等)
            std::string desc = getCardDesc(c);
            if(!desc.empty()) std::cout << " | " << Color::GREY << desc << Color::RESET;

            std::cout << "\n";
        }
    }

    int choice = active->makeDecision(idxs, board);
    if (choice == -1)
    {
        isGameOver = true;
        return false;
    }
    auto card = board.removeCard(choice);
    int cost = active->calculateActualCost(card, opp);

    int action = 1; // 1Build, 2Sell, 3Wonder
    if (isHuman)
    {
        std::cout << "\n你选择了卡牌: " << Color::BOLD << card->getName() << Color::RESET << "\n";
        std::cout << "请选择操作:\n";

        // 选项1: 建造
        if(cost != -1)
            std::cout << "1. 建造 (花费 " << cost << " 金币)\n";
        else
            std::cout << Color::GREY << "1. 建造 (不可用: 钱不够)\n" << Color::RESET;

        // 选项2: 卖出
        int sellValue = 2 + active->getCardCount(CardType::COMMERCIAL);
        std::cout << "2. 卖出 (获得 " << sellValue << " 金币)\n";

        // 选项3: 建造奇迹 (新增：显示每个奇迹的预估花费)
        std::cout << "3. 用此牌建造奇迹 (查看下方详情):\n";
        bool anyWonderBuildable = false;
        const auto& myWonders = active->getWonders();
        for(size_t w=0; w<myWonders.size(); ++w) {
            if(!myWonders[w].isBuilt) {
                int wCost = active->calculateWonderCost(myWonders[w], opp);
                std::cout << "   - 按 " << w << " 键建造 [" << myWonders[w].name << "]";
                if(wCost == -1)
                    std::cout << Color::RED << " (不可用: 资源/钱不足)" << Color::RESET;
                else
                    std::cout << Color::GREEN << " (需补资源费: " << wCost << " 金)" << Color::RESET;
                std::cout << "\n";
                if(wCost != -1) anyWonderBuildable = true;
            }
        }
        if(!anyWonderBuildable) std::cout << "   (没有可建造的奇迹)\n";

        std::cout << ">> 请输入指令 (1=建造, 2=卖出, 3=建造奇迹): ";
        std::cin >> action;
    }
    else
    {
        // AI 逻辑保持不变...
        bool canWonder = false;
        for (int i = 0; i < 4; ++i)
            if (active->canBuildWonder(i, opp))
                canWonder = true;

        if (canWonder && wondersBuiltCount < 7) action = 3;
        else if (cost != -1) action = 1;
        else action = 2;
    }

    // 执行逻辑
    if (action == 1 && cost != -1)
    {
        // 建议从: bool pairTrigger = active->buildCard(card, cost); 开始
        bool pairTrigger = active->buildCard(card, cost);
        if (pairTrigger) handleTokenSelection(active);
        if (card->getImmediateCoins() > 0) active->addCoins(card->getImmediateCoins());
        if (opp->hasToken(TokenType::ECONOMY) && cost > 0) {
            int tradeEstimate = std::max(0, cost - card->getCostCoins());
            if (tradeEstimate > 0) opp->addCoins(tradeEstimate);
        }
        if (card->getType() == CardType::MILITARY) {
            int shields = card->getShields();
            if (active == p1) militaryToken += shields;
            else militaryToken -= shields;
        }
        std::cout << Color::GREEN << ">>> 建造成功! <<<" << Color::RESET << "\n";
    }
    else if (action == 3) // 奇迹逻辑修改以支持子选项
    {
        if (wondersBuiltCount >= 7) {
            std::cout << Color::RED << "!!! 奇迹上限已满(7) !!! 强制卖出。\n" << Color::RESET;
            active->addCoins(2 + active->getCardCount(CardType::COMMERCIAL));
            board.addToDiscard(card);
        } else {
            int wid = 0;
            if (isHuman) {
                std::cout << "请输入要建造的奇迹 ID (0-3): ";
                std::cin >> wid;
            } else {
                for (int i = 0; i < 4; ++i) if (active->canBuildWonder(i, opp)) wid = i;
            }

            if (active->canBuildWonder(wid, opp)) {
                int wCost = active->calculateWonderCost(active->getWonders()[wid], opp);
                active->buildWonder(wid, wCost);
                wondersBuiltCount++;
                std::cout << Color::MAGENTA << ">>> 奇迹建造成功! <<<" << Color::RESET << "\n";

                auto w = active->getWonders()[wid];
                if (w.hasReplay || active->hasToken(TokenType::THEOLOGY)) replay = true;
                handleWonderEffect(w.effect, active, opp);
            } else {
                std::cout << Color::RED << "无法建造该奇迹(钱不够或已建)，操作取消，自动转为卖出。\n" << Color::RESET;
                active->addCoins(2 + active->getCardCount(CardType::COMMERCIAL));
                board.addToDiscard(card);
            }
        }
    }
    else // 卖出逻辑
    {
        int val = 2 + active->getCardCount(CardType::COMMERCIAL);
        std::cout << Color::YELLOW << ">>> 卖出卡牌，获得 " << val << " 金币 <<<" << Color::RESET << "\n";
        active->addCoins(val);
        board.addToDiscard(card);
    }
    return replay;
}

bool Game::checkInstantVictory()
{
    // 军事胜利：如果标记达到两端
    if (militaryToken <= -9)
    {
        std::cout << Color::MAGENTA << "\n>>> 军事胜利! " << p2->getName() << " 征服了对手! <<<\n"
                  << Color::RESET;
        return true;
    }
    if (militaryToken >= 9)
    {
        std::cout << Color::MAGENTA << "\n>>> 军事胜利! " << p1->getName() << " 征服了对手! <<<\n"
                  << Color::RESET;
        return true;
    }

    // 科技胜利
    if (p1->getScienceCount() >= 6)
    {
        std::cout << Color::CYAN << "\n>>> 科技胜利! " << p1->getName() << " 取得了科技霸权! <<<\n"
                  << Color::RESET;
        return true;
    }
    if (p2->getScienceCount() >= 6)
    {
        std::cout << Color::CYAN << "\n>>> 科技胜利! " << p2->getName() << " 取得了科技霸权! <<<\n"
                  << Color::RESET;
        return true;
    }
    return false;
}

void Game::calculateScore()
{
    std::cout << "\n"
              << Color::BOLD << "=== 最终结算 (CIVILIAN VICTORY) ===" << Color::RESET << "\n";
    auto calc = [&](std::shared_ptr<Player> p, std::shared_ptr<Player> opp)
    {
        int s = p->getCoins() / 3;
        for (auto w : p->getWonders())
            if (w.isBuilt)
                s += w.victoryPoints;
        for (auto c : p->getConstructedCards())
        {
            s += c->getVictoryPoints();
            if (c->getType() == CardType::GUILD)
            {
                std::string g = c->getGuildType();
                if (g == "Merchants")
                    s += 1 * std::max(p->getCardCount(CardType::COMMERCIAL), opp->getCardCount(CardType::COMMERCIAL));
                if (g == "Shipowners")
                    s += 1 * std::max(p->getCardCount(CardType::RAW_MATERIAL) + p->getCardCount(CardType::MANUFACTURED), opp->getCardCount(CardType::RAW_MATERIAL) + opp->getCardCount(CardType::MANUFACTURED));
                if (g == "Scientists")
                    s += 1 * std::max(p->getCardCount(CardType::SCIENTIFIC), opp->getCardCount(CardType::SCIENTIFIC));
                if (g == "Magistrates")
                    s += 1 * std::max(p->getCardCount(CardType::CIVILIAN), opp->getCardCount(CardType::CIVILIAN));
                if (g == "Tacticians")
                    s += 1 * std::max(p->getCardCount(CardType::MILITARY), opp->getCardCount(CardType::MILITARY));
                if (g == "Builders")
                {
                    int w1 = 0, w2 = 0;
                    for (auto w : p->getWonders())
                        if (w.isBuilt)
                            w1++;
                    for (auto w : opp->getWonders())
                        if (w.isBuilt)
                            w2++;
                    s += 2 * std::max(w1, w2);
                }
                if (g == "Moneylenders")
                    s += 1 * std::max(p->getCoins() / 3, opp->getCoins() / 3);
            }
        }
        if (p->hasToken(TokenType::MATHEMATICS))
            s += 3;
        return s;
    };

    int s1 = calc(p1, p2);
    int s2 = calc(p2, p1);

    // 军事分结算 (根据标记位置给分)
    // 简化规则：每格 1 分
    if (militaryToken > 0)
        s1 += militaryToken;
    else
        s2 += abs(militaryToken);

    std::cout << p1->getName() << ": " << s1 << " 分\n";
    std::cout << p2->getName() << ": " << s2 << " 分\n";

    if (s1 > s2)
        std::cout << Color::GREEN << "胜者: " << p1->getName() << " !!!\n"
                  << Color::RESET;
    else if (s2 > s1)
        std::cout << Color::GREEN << "胜者: " << p2->getName() << " !!!\n"
                  << Color::RESET;
    else
    {
        // 同分判定：比蓝卡分
        int blue1 = 0, blue2 = 0;
        for (auto c : p1->getConstructedCards())
            if (c->getType() == CardType::CIVILIAN)
                blue1 += c->getVictoryPoints();
        for (auto c : p2->getConstructedCards())
            if (c->getType() == CardType::CIVILIAN)
                blue2 += c->getVictoryPoints();
        std::cout << "平分! 判定蓝卡分: " << blue1 << " vs " << blue2 << "\n";
        if (blue1 > blue2)
            std::cout << "胜者: " << p1->getName() << "\n";
        else if (blue2 > blue1)
            std::cout << "胜者: " << p2->getName() << "\n";
        else
            std::cout << "完全平局!\n";
    }
}