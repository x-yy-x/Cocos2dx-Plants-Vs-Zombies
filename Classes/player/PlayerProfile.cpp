#include "PlayerProfile.h"
#include <cocos2d.h>

PlayerProfile* PlayerProfile::instance = nullptr;

PlayerProfile* PlayerProfile::getInstance()
{
    if (!instance)
    {
        // Using std::nothrow to ensure the app doesn't crash on allocation failure
        instance = new (std::nothrow) PlayerProfile();
    }
    return instance;
}

PlayerProfile::PlayerProfile()
    : coins(10000) // Starting coins (elevated for debugging/testing)
{
    /**
     * Initialize with default starting plants.
     * Note: In a production version, these would be loaded from a save file.
     */
    unlocked_plants.insert(PlantName::SUNFLOWER);
    unlocked_plants.insert(PlantName::SUNSHROOM);
    unlocked_plants.insert(PlantName::PEASHOOTER);
    unlocked_plants.insert(PlantName::REPEATER);
    unlocked_plants.insert(PlantName::THREEPEATER);
    unlocked_plants.insert(PlantName::PUFFSHROOM);
    unlocked_plants.insert(PlantName::WALLNUT);
    unlocked_plants.insert(PlantName::CHERRYBOMB);
    unlocked_plants.insert(PlantName::SPIKEWEED);
    unlocked_plants.insert(PlantName::JALAPENO);
}

// ----------------------------------------------------
// Economic Logic
// ----------------------------------------------------

long long PlayerProfile::getCoins() const
{
    return coins;
}

void PlayerProfile::addCoins(long long amount)
{
    coins += amount;
}

bool PlayerProfile::spendCoins(long long amount)
{
    if (coins >= amount)
    {
        coins -= amount;
        return true;
    }
    return false;
}

// ----------------------------------------------------
// Progression Logic
// ----------------------------------------------------

void PlayerProfile::unlockPlant(PlantName plantName)
{
    unlocked_plants.insert(plantName);
}

bool PlayerProfile::isPlantUnlocked(PlantName plantName) const
{
    /**
     * Logic: Standard plants are available by default.
     * Only "Upgrade" plants (found in Crazy Dave's shop) require explicit unlocking.
     */
    switch (plantName)
    {
        case PlantName::TWINSUNFLOWER:
        case PlantName::GATLINGPEA:
        case PlantName::SPIKEROCK:
            return unlocked_plants.count(plantName) > 0;
        default:
            return true;
    }
}

// ----------------------------------------------------
// Power-up Management
// ----------------------------------------------------

void PlayerProfile::enableRake(bool enable)
{
    rake_enabled = enable;
}

bool PlayerProfile::isRakeEnabled() const
{
    return rake_enabled;
}

void PlayerProfile::enableMower(bool enable)
{
    mower_enabled = enable;
}

bool PlayerProfile::isMowerEnabled() const
{
    return mower_enabled;
}