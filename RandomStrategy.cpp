#include "RandomStrategy.h"
#include <chrono>

RandomStrategy::RandomStrategy() {
    // 使用当前时间作为随机种子
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng.seed(seed);
}

int RandomStrategy::selectCard(
    const std::vector<int>& availableIndices,
    const Board& board,
    const Player& self,
    const Player& opponent
) {
    if (availableIndices.empty()) return -1;

    std::uniform_int_distribution<int> dist(0, availableIndices.size() - 1);
    return availableIndices[dist(rng)];
}

int RandomStrategy::selectCardToDestroy(
    const std::vector<std::shared_ptr<Card>>& targets
) {
    if (targets.empty()) return -1;

    std::uniform_int_distribution<int> dist(0, targets.size() - 1);
    return dist(rng);
}

int RandomStrategy::selectCardToRevive(
    const std::vector<std::shared_ptr<Card>>& discardPile
) {
    if (discardPile.empty()) return -1;

    std::uniform_int_distribution<int> dist(0, discardPile.size() - 1);
    return dist(rng);
}

int RandomStrategy::selectProgressToken(
    const std::vector<ProgressToken>& available
) {
    if (available.empty()) return -1;

    std::uniform_int_distribution<int> dist(0, available.size() - 1);
    return dist(rng);
}

int RandomStrategy::selectWhoStarts(
    const std::string& p1Name,
    const std::string& p2Name
) {
    std::uniform_int_distribution<int> dist(1, 2);
    return dist(rng);
}
