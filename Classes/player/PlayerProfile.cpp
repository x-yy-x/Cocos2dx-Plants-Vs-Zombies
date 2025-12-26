#include "PlayerProfile.h"

PlayerProfile* PlayerProfile::_instance = nullptr;

PlayerProfile* PlayerProfile::getInstance()
{
    if (!_instance)
    {
        _instance = new (std::nothrow) PlayerProfile();
    }
    return _instance;
}

PlayerProfile::PlayerProfile()
    : _coins(10000) // Initial coins set to 10000 for debugging
{
    // Default unlock some basic plants; plant upgrades need to be purchased
    _unlockedPlants.insert(PlantName::SUNFLOWER);
    _unlockedPlants.insert(PlantName::SUNSHROOM);
    _unlockedPlants.insert(PlantName::PEASHOOTER);
    _unlockedPlants.insert(PlantName::REPEATER);
    _unlockedPlants.insert(PlantName::THREEPEATER);
    _unlockedPlants.insert(PlantName::PUFFSHROOM);
    _unlockedPlants.insert(PlantName::WALLNUT);
    _unlockedPlants.insert(PlantName::CHERRYBOMB);
    _unlockedPlants.insert(PlantName::SPIKEWEED);
    _unlockedPlants.insert(PlantName::JALAPENO);
}

long long PlayerProfile::getCoins() const
{
    return _coins;
}

void PlayerProfile::addCoins(long long amount)
{
    _coins += amount;
}

bool PlayerProfile::spendCoins(long long amount)
{
    if (_coins >= amount)
    {
        _coins -= amount;
        return true;
    }
    return false;
}

void PlayerProfile::unlockPlant(PlantName plantName)
{
    _unlockedPlants.insert(plantName);
}

bool PlayerProfile::isPlantUnlocked(PlantName plantName) const
{
    // Non-upgraded plants are unlocked by default
    switch (plantName)
    {
        case PlantName::TWINSUNFLOWER:
        case PlantName::GATLINGPEA:
        case PlantName::SPIKEROCK:
            return _unlockedPlants.count(plantName) > 0;
        default:
            return true; // Other plants are unlocked by default
    }
}

void PlayerProfile::enableRake(bool enable)
{
    _rakeEnabled = enable;
}

bool PlayerProfile::isRakeEnabled() const
{
    return _rakeEnabled;
}

void PlayerProfile::enableMower(bool enable)
{
    _mowerEnabled = enable;
}

bool PlayerProfile::isMowerEnabled() const
{
    return _mowerEnabled;
}
