#include "BaseGameCardFactory.h"

std::vector<std::shared_ptr<Card>> BaseGameCardFactory::createDeck(int age) {
    auto cardDataList = getCardDataForAge(age);
    std::vector<std::shared_ptr<Card>> deck;

    for (const auto& data : cardDataList) {
        auto card = createCardFromData(data);
        if (card) {
            deck.push_back(card);
        }
    }

    return deck;
}

std::vector<CardData> BaseGameCardFactory::getCardDataForAge(int age) {
    switch (age) {
        case 1: return getAge1CardData();
        case 2: return getAge2CardData();
        case 3: return getAge3CardData();
        default: return {};
    }
}

std::vector<CardData> BaseGameCardFactory::getAge1CardData() {
    std::vector<CardData> cards;

    // --- 原料 (Raw Material) ---
    cards.push_back(CardData::createResource("伐木场", CardType::RAW_MATERIAL, 0,
                    {Resource::WOOD}));
    cards.push_back(CardData::createResource("伐木营地", CardType::RAW_MATERIAL, 1,
                    {Resource::WOOD}));
    cards.push_back(CardData::createResource("粘土坑", CardType::RAW_MATERIAL, 0,
                    {Resource::CLAY}));
    cards.push_back(CardData::createResource("采泥坑", CardType::RAW_MATERIAL, 1,
                    {Resource::CLAY}));
    cards.push_back(CardData::createResource("采石场", CardType::RAW_MATERIAL, 0,
                    {Resource::STONE}));
    cards.push_back(CardData::createResource("石坑", CardType::RAW_MATERIAL, 1,
                    {Resource::STONE}));

    // --- 制造 (Manufactured) ---
    cards.push_back(CardData::createResource("玻璃厂", CardType::MANUFACTURED, 1,
                    {Resource::GLASS}));
    cards.push_back(CardData::createResource("印刷厂", CardType::MANUFACTURED, 1,
                    {Resource::PAPYRUS}));

    // --- 商业 (Commercial) ---
    cards.push_back(CardData::createCommercial("木材储备", 3, Resource::WOOD, 0));
    cards.push_back(CardData::createCommercial("粘土储备", 3, Resource::CLAY, 0));
    cards.push_back(CardData::createCommercial("石材储备", 3, Resource::STONE, 0));

    // --- 军事 (Military) ---
    cards.push_back(CardData::createMilitary("马厩", 0,
                    {{Resource::WOOD, 1}}, 1, "Horseshoe", ""));
    cards.push_back(CardData::createMilitary("驻军", 0,
                    {{Resource::CLAY, 1}}, 1, "Sword", ""));
    cards.push_back(CardData::createMilitary("栅栏", 0,
                    {{Resource::STONE, 1}}, 1, "Tower", ""));
    cards.push_back(CardData::createMilitary("守卫塔", 0, {}, 1));

    // --- 科技 (Science) ---
    cards.push_back(CardData::createScience("药剂师", 2, {},
                    ScienceSymbol::WHEEL, 1, "Gear", ""));
    cards.push_back(CardData::createScience("工坊", 2, {},
                    ScienceSymbol::GLOBE, 1, "Scope", ""));
    cards.push_back(CardData::createScience("写字间", 2, {},
                    ScienceSymbol::WRITING, 1, "Book", ""));
    cards.push_back(CardData::createScience("药房", 2, {},
                    ScienceSymbol::MORTAR, 1, "Pestle", ""));

    // --- 平民 (Civilian) ---
    cards.push_back(CardData::createCivilian("祭坛", 0, {}, 3, "Moon", ""));
    cards.push_back(CardData::createCivilian("剧院", 0, {}, 3, "Mask", ""));
    cards.push_back(CardData::createCivilian("澡堂", 0,
                    {{Resource::STONE, 1}}, 3, "Drop", ""));
    cards.push_back(CardData::createCivilian("当铺", 0, {}, 3));

    return cards;
}

std::vector<CardData> BaseGameCardFactory::getAge2CardData() {
    std::vector<CardData> cards;

    // --- 原料 (Raw Material) ---
    cards.push_back(CardData::createResource("锯木厂", CardType::RAW_MATERIAL, 2,
                    {Resource::WOOD, Resource::WOOD}));
    cards.push_back(CardData::createResource("砖厂", CardType::RAW_MATERIAL, 2,
                    {Resource::CLAY, Resource::CLAY}));
    cards.push_back(CardData::createResource("层状采石场", CardType::RAW_MATERIAL, 2,
                    {Resource::STONE, Resource::STONE}));

    // --- 制造 (Manufactured) ---
    cards.push_back(CardData::createResource("玻璃吹制", CardType::MANUFACTURED, 0,
                    {Resource::GLASS}));
    cards.push_back(CardData::createResource("干燥室", CardType::MANUFACTURED, 0,
                    {Resource::PAPYRUS}));

    // --- 商业 (Commercial) ---
    cards.push_back(CardData::createResource("论坛", CardType::COMMERCIAL, 3,
                    {Resource::GLASS, Resource::PAPYRUS}));
    cards.push_back(CardData::createCommercial("酿酒厂", 0, Resource::NONE, 0, 6));
    cards.push_back(CardData::createResource("驿站", CardType::COMMERCIAL, 2,
                    {Resource::WOOD, Resource::CLAY, Resource::STONE}));

    // --- 军事 (Military) ---
    cards.push_back(CardData::createMilitary("骑兵", 3,
                    {{Resource::WOOD, 1}}, 2, "", "Horseshoe"));
    cards.push_back(CardData::createMilitary("城墙", 0,
                    {{Resource::STONE, 2}}, 2));
    cards.push_back(CardData::createMilitary("军械库", 0,
                    {{Resource::WOOD, 2}, {Resource::CLAY, 1}}, 2));
    cards.push_back(CardData::createMilitary("靶场", 0,
                    {{Resource::STONE, 1}, {Resource::WOOD, 1}}, 2, "", "Target"));

    // --- 科技 (Science) ---
    cards.push_back(CardData::createScience("图书馆", 2,
                    {{Resource::STONE, 1}, {Resource::WOOD, 1}},
                    ScienceSymbol::WRITING, 2, "", "Book"));
    cards.push_back(CardData::createScience("学校", 2,
                    {{Resource::WOOD, 1}, {Resource::PAPYRUS, 1}},
                    ScienceSymbol::WHEEL, 2, "", "Gear"));
    cards.push_back(CardData::createScience("实验室", 2,
                    {{Resource::CLAY, 2}},
                    ScienceSymbol::GLOBE, 2, "", "Scope"));
    cards.push_back(CardData::createScience("医务室", 2,
                    {{Resource::STONE, 2}},
                    ScienceSymbol::MORTAR, 2, "", "Pestle"));

    // --- 平民 (Civilian) ---
    cards.push_back(CardData::createCivilian("雕像", 0,
                    {{Resource::CLAY, 2}}, 4, "", "Theater"));
    cards.push_back(CardData::createCivilian("神庙", 0,
                    {{Resource::WOOD, 1}, {Resource::GLASS, 1}}, 4, "", "Moon"));
    cards.push_back(CardData::createCivilian("引水渠", 0,
                    {{Resource::STONE, 3}}, 5, "", "Drop"));
    cards.push_back(CardData::createCivilian("讲坛", 0,
                    {{Resource::STONE, 1}, {Resource::WOOD, 1}}, 4, "Sun", ""));

    return cards;
}

std::vector<CardData> BaseGameCardFactory::getAge3CardData() {
    std::vector<CardData> cards;

    // --- 工会 (Guilds) ---
    cards.push_back(CardData::createGuild("商人工会",
                    {{Resource::WOOD, 1}, {Resource::GLASS, 1}}, "Merchants"));
    cards.push_back(CardData::createGuild("船主工会",
                    {{Resource::CLAY, 1}, {Resource::GLASS, 1}, {Resource::PAPYRUS, 1}},
                    "Shipowners"));
    cards.push_back(CardData::createGuild("科学家工会",
                    {{Resource::WOOD, 2}}, "Scientists"));
    cards.push_back(CardData::createGuild("行政官工会",
                    {{Resource::WOOD, 2}, {Resource::PAPYRUS, 1}}, "Magistrates"));
    cards.push_back(CardData::createGuild("建造者工会",
                    {{Resource::STONE, 2}, {Resource::CLAY, 1}}, "Builders"));
    cards.push_back(CardData::createGuild("战术家工会",
                    {{Resource::STONE, 2}, {Resource::CLAY, 1}}, "Tacticians"));
    cards.push_back(CardData::createGuild("放贷人工会",
                    {{Resource::WOOD, 2}, {Resource::STONE, 2}}, "Moneylenders"));

    // --- 军事 (Military) ---
    cards.push_back(CardData::createMilitary("要塞", 0,
                    {{Resource::STONE, 2}, {Resource::CLAY, 2}}, 3));
    cards.push_back(CardData::createMilitary("攻城锤", 0,
                    {{Resource::WOOD, 3}}, 3));
    cards.push_back(CardData::createMilitary("竞技场", 0,
                    {{Resource::CLAY, 2}, {Resource::STONE, 1}}, 3));

    // --- 科技 (Science) ---
    cards.push_back(CardData::createScience("天文台", 0,
                    {{Resource::STONE, 2}, {Resource::PAPYRUS, 1}},
                    ScienceSymbol::MECHANIC, 3, "", "Sun"));
    cards.push_back(CardData::createScience("大学", 0,
                    {{Resource::CLAY, 2}, {Resource::GLASS, 1}},
                    ScienceSymbol::GLOBE, 3));
    cards.push_back(CardData::createScience("研究院", 0,
                    {{Resource::WOOD, 2}, {Resource::GLASS, 1}},
                    ScienceSymbol::WHEEL, 3));

    // --- 平民 (Civilian) ---
    cards.push_back(CardData::createCivilian("宫殿", 0,
                    {{Resource::STONE, 1}, {Resource::GLASS, 1}}, 7));
    cards.push_back(CardData::createCivilian("方尖碑", 0,
                    {{Resource::STONE, 2}}, 5));
    cards.push_back(CardData::createCivilian("花园", 0,
                    {{Resource::CLAY, 2}, {Resource::WOOD, 2}}, 6));
    cards.push_back(CardData::createCivilian("参议院", 0,
                    {{Resource::CLAY, 2}, {Resource::PAPYRUS, 1}}, 5));
    cards.push_back(CardData::createMilitary("马戏团", 0,
                    {{Resource::STONE, 2}, {Resource::WOOD, 2}}, 3));
    cards.push_back(CardData::createCivilian("市政厅", 0,
                    {{Resource::STONE, 3}}, 6));
    cards.push_back(CardData::createScience("学院", 0,
                    {{Resource::GLASS, 1}, {Resource::PAPYRUS, 1}},
                    ScienceSymbol::WRITING, 3));

    return cards;
}
