#pragma once

#include "GameDefs.h"
#include <unordered_set>

class PlayerProfile
{
public:
    static PlayerProfile* getInstance();

    // Prohibit copy and assignment
    PlayerProfile(const PlayerProfile&) = delete;
    PlayerProfile& operator=(const PlayerProfile&) = delete;

    long long getCoins() const;
    void addCoins(long long amount);
    bool spendCoins(long long amount);

    void unlockPlant(PlantName plantName);
    bool isPlantUnlocked(PlantName plantName) const;

    // Purchaseable items (rake/mower)
    void enableRake(bool enable = true);
    bool isRakeEnabled() const;
    void enableMower(bool enable = true);
    bool isMowerEnabled() const;

private:
    PlayerProfile();
    static PlayerProfile* _instance;

    long long _coins;
    std::unordered_set<PlantName> _unlockedPlants;
    bool _rakeEnabled{false};
    bool _mowerEnabled{false};
};
