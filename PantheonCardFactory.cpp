#include "PantheonCardFactory.h"

std::vector<std::shared_ptr<Card>> PantheonCardFactory::createDeck(int age) {
    // 先获取基础游戏的卡牌
    auto deck = BaseGameCardFactory::createDeck(age);

    // 添加Panthéon扩展的额外卡牌
    auto pantheonCards = getPantheonCardData(age);
    for (const auto& data : pantheonCards) {
        auto card = createCardFromData(data);
        if (card) {
            deck.push_back(card);
        }
    }

    return deck;
}

std::vector<CardData> PantheonCardFactory::getCardDataForAge(int age) {
    // 使用基础游戏的数据
    auto baseData = BaseGameCardFactory::getCardDataForAge(age);

    // 添加Panthéon扩展数据
    auto pantheonData = getPantheonCardData(age);
    baseData.insert(baseData.end(), pantheonData.begin(), pantheonData.end());

    return baseData;
}

std::vector<CardData> PantheonCardFactory::getPantheonCardData(int age) {
    std::vector<CardData> cards;

    // Panthéon扩展卡牌示例
    if (age == 1) {
        // 时代1的Panthéon卡牌（示例）
        cards.push_back(CardData::createCivilian("小神殿", 0,
                        {{Resource::WOOD, 1}}, 2, "Temple", ""));
        cards.push_back(CardData::createScience("神谕所", 1,
                        {{Resource::STONE, 1}}, ScienceSymbol::GLOBE, 1));
    }
    else if (age == 2) {
        // 时代2的Panthéon卡牌（示例）
        cards.push_back(CardData::createCivilian("大神殿", 0,
                        {{Resource::STONE, 2}, {Resource::GLASS, 1}}, 4, "", "Temple"));
        cards.push_back(CardData::createMilitary("战神祭坛", 0,
                        {{Resource::CLAY, 2}}, 2, "Altar", ""));
    }
    else if (age == 3) {
        // 时代3的Panthéon卡牌（示例）
        cards.push_back(CardData::createCivilian("万神殿", 0,
                        {{Resource::GLASS, 2}, {Resource::PAPYRUS, 2}}, 8));
        cards.push_back(CardData::createGuild("祭司工会",
                        {{Resource::WOOD, 1}, {Resource::STONE, 1}}, "Priests"));
    }

    return cards;
}

std::shared_ptr<Card> PantheonCardFactory::createDivinityCard(const std::string& name) {
    // 神祇卡示例（可以是特殊类型的卡牌）
    // 目前用平民卡代替，实际实现需要创建新的DivinityCard类
    return std::make_shared<CivilianCard>(name, 0, std::map<Resource, int>{}, 0);
}
