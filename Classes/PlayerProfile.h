#pragma once

#include "GameDefs.h"
#include <unordered_set>

class PlayerProfile
{
public:
    static PlayerProfile* getInstance();

    // 禁止拷贝和赋值
    PlayerProfile(const PlayerProfile&) = delete;
    PlayerProfile& operator=(const PlayerProfile&) = delete;

    long long getCoins() const;
    void addCoins(long long amount);
    bool spendCoins(long long amount);

    void unlockPlant(PlantName plantName);
    bool isPlantUnlocked(PlantName plantName) const;

    // 购买类-道具（耙子/小推车）
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
