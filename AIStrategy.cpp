#include "AIStrategy.h"
#include "Player.h"

int AIStrategy::evaluateCard(const std::shared_ptr<Card>& card, const Player& self) {
    int score = 0;

    // 基础评分逻辑（子类可覆盖）
    switch (card->getType()) {
        case CardType::RAW_MATERIAL:
        case CardType::MANUFACTURED:
            score = 80;  // 资源卡高价值
            break;

        case CardType::SCIENTIFIC:
            score = 70;  // 科技卡中等价值
            if (self.hasScienceSymbol(card->getSymbol())) {
                score += 50;  // 如果能凑对，价值大增
            }
            break;

        case CardType::MILITARY:
            score = 50 + card->getShields() * 10;
            break;

        case CardType::CIVILIAN:
            score = card->getVictoryPoints() * 3;
            break;

        case CardType::COMMERCIAL:
            score = 40 + card->getImmediateCoins();
            break;

        case CardType::GUILD:
            score = 60;  // 工会卡通常有好的得分效果
            break;
    }

    return score;
}
