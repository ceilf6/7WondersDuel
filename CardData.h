#ifndef CARDDATA_H
#define CARDDATA_H

#include "Commons.h"
#include <string>
#include <map>
#include <vector>

/**
 * @brief 卡牌数据结构 - 用于数据驱动的卡牌创建
 *
 * 将卡牌数据与卡牌类分离，便于从配置文件加载或扩展包添加
 */
struct CardData {
    std::string name;
    CardType type;
    int costCoins;
    std::map<Resource, int> costResources;
    std::string chainProvide;
    std::string chainCost;

    // 类型特定数据
    int shields = 0;                    // 军事卡
    int victoryPoints = 0;              // 平民卡/科技卡/商业卡
    ScienceSymbol scienceSymbol = ScienceSymbol::NONE;  // 科技卡
    std::vector<Resource> production;   // 资源卡
    Resource discountResource = Resource::NONE;  // 商业卡
    int immediateCoins = 0;             // 商业卡即时金币
    std::string guildType;              // 工会卡

    // 构造函数
    CardData(std::string n, CardType t)
        : name(n), type(t), costCoins(0) {}

    // 便捷构造函数 - 军事卡
    static CardData createMilitary(std::string name, int coins,
                                   std::map<Resource, int> resCost,
                                   int shields,
                                   std::string chainProv = "",
                                   std::string chainCost = "") {
        CardData data(name, CardType::MILITARY);
        data.costCoins = coins;
        data.costResources = resCost;
        data.shields = shields;
        data.chainProvide = chainProv;
        data.chainCost = chainCost;
        return data;
    }

    // 便捷构造函数 - 平民卡
    static CardData createCivilian(std::string name, int coins,
                                   std::map<Resource, int> resCost,
                                   int vp,
                                   std::string chainProv = "",
                                   std::string chainCost = "") {
        CardData data(name, CardType::CIVILIAN);
        data.costCoins = coins;
        data.costResources = resCost;
        data.victoryPoints = vp;
        data.chainProvide = chainProv;
        data.chainCost = chainCost;
        return data;
    }

    // 便捷构造函数 - 科技卡
    static CardData createScience(std::string name, int coins,
                                  std::map<Resource, int> resCost,
                                  ScienceSymbol symbol, int vp = 0,
                                  std::string chainProv = "",
                                  std::string chainCost = "") {
        CardData data(name, CardType::SCIENTIFIC);
        data.costCoins = coins;
        data.costResources = resCost;
        data.scienceSymbol = symbol;
        data.victoryPoints = vp;
        data.chainProvide = chainProv;
        data.chainCost = chainCost;
        return data;
    }

    // 便捷构造函数 - 资源卡
    static CardData createResource(std::string name, CardType type,
                                   int coins, std::vector<Resource> prod) {
        CardData data(name, type);
        data.costCoins = coins;
        data.production = prod;
        return data;
    }

    // 便捷构造函数 - 商业卡
    static CardData createCommercial(std::string name, int coins,
                                     Resource discountRes,
                                     int vp = 0, int immCoins = 0) {
        CardData data(name, CardType::COMMERCIAL);
        data.costCoins = coins;
        data.discountResource = discountRes;
        data.victoryPoints = vp;
        data.immediateCoins = immCoins;
        return data;
    }

    // 便捷构造函数 - 工会卡
    static CardData createGuild(std::string name,
                                std::map<Resource, int> resCost,
                                std::string gType) {
        CardData data(name, CardType::GUILD);
        data.costResources = resCost;
        data.guildType = gType;
        return data;
    }
};

#endif // CARDDATA_H
