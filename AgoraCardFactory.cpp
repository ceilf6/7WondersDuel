#include "AgoraCardFactory.h"

std::vector<std::shared_ptr<Card>> AgoraCardFactory::createDeck(int age) {
    // 先获取基础游戏的卡牌
    auto deck = BaseGameCardFactory::createDeck(age);

    // 添加Agora扩展的额外卡牌
    auto agoraCards = getAgoraCardData(age);
    for (const auto& data : agoraCards) {
        auto card = createCardFromData(data);
        if (card) {
            deck.push_back(card);
        }
    }

    return deck;
}

std::vector<CardData> AgoraCardFactory::getCardDataForAge(int age) {
    // 使用基础游戏的数据
    auto baseData = BaseGameCardFactory::getCardDataForAge(age);

    // 添加Agora扩展数据
    auto agoraData = getAgoraCardData(age);
    baseData.insert(baseData.end(), agoraData.begin(), agoraData.end());

    return baseData;
}

std::vector<CardData> AgoraCardFactory::getAgoraCardData(int age) {
    std::vector<CardData> cards;

    // Agora扩展卡牌示例
    if (age == 1) {
        // 时代1的Agora卡牌（示例）
        cards.push_back(CardData::createCommercial("元老院", 2, Resource::NONE, 2, 3));
        cards.push_back(CardData::createMilitary("罗马军团", 1,
                        {{Resource::WOOD, 1}, {Resource::CLAY, 1}}, 2, "Legion", ""));
    }
    else if (age == 2) {
        // 时代2的Agora卡牌（示例）
        cards.push_back(CardData::createCivilian("政治联盟", 0,
                        {{Resource::PAPYRUS, 1}}, 3, "Alliance", ""));
        cards.push_back(CardData::createScience("议会大厅", 0,
                        {{Resource::STONE, 2}}, ScienceSymbol::LAW, 2));
    }
    else if (age == 3) {
        // 时代3的Agora卡牌（示例）
        cards.push_back(CardData::createGuild("政治家工会",
                        {{Resource::GLASS, 1}, {Resource::PAPYRUS, 1}}, "Politicians"));
    }

    return cards;
}

std::shared_ptr<Card> AgoraCardFactory::createConspiracyCard(const std::string& name) {
    // 阴谋卡示例（可以是特殊类型的卡牌）
    // 目前用商业卡代替，实际实现需要创建新的ConspiracyCard类
    return std::make_shared<CommercialCard>(name, 0, Resource::NONE, 0, 0);
}
