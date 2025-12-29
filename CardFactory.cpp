#include "CardFactory.h"

std::shared_ptr<Card> CardFactory::createCardFromData(const CardData& data) {
    switch (data.type) {
        case CardType::MILITARY:
            return std::make_shared<MilitaryCard>(
                data.name, data.costCoins, data.costResources,
                data.shields, data.chainProvide, data.chainCost
            );

        case CardType::CIVILIAN:
            return std::make_shared<CivilianCard>(
                data.name, data.costCoins, data.costResources,
                data.victoryPoints, data.chainProvide, data.chainCost
            );

        case CardType::SCIENTIFIC:
            return std::make_shared<ScienceCard>(
                data.name, data.costCoins, data.costResources,
                data.scienceSymbol, data.victoryPoints,
                data.chainProvide, data.chainCost
            );

        case CardType::RAW_MATERIAL:
        case CardType::MANUFACTURED:
            return std::make_shared<ResourceCard>(
                data.name, data.type, data.costCoins, data.production
            );

        case CardType::COMMERCIAL:
            return std::make_shared<CommercialCard>(
                data.name, data.costCoins, data.discountResource,
                data.victoryPoints, data.immediateCoins
            );

        case CardType::GUILD:
            return std::make_shared<GuildCard>(
                data.name, data.costResources, data.guildType
            );

        default:
            return nullptr;
    }
}
