#include "AttackingPlant.h"
#include "Zombie.h"

USING_NS_CC;

bool AttackingPlant::isZombieInRange(const std::vector<Zombie*>& zombiesInRow)
{
    float plantX = this->getPositionX();
    
    // CRITICAL FIX: Use iterator to avoid invalidation during iteration
    for (auto it = zombiesInRow.begin(); it != zombiesInRow.end(); ++it)
    {
        Zombie* zombie = *it;
        // Check pointer validity and skip dead/dying zombies
        if (zombie && !zombie->isDead() && zombie->getPositionX() > plantX)
        {
            return true;
        }
    }
    
    return false;
}

bool AttackingPlant::isZombieInRangeMultiRow(std::vector<Zombie*> allZombiesInRow[MAX_ROW], 
                                              const std::vector<int>& rowsToCheck)
{
    float plantX = this->getPositionX();
    
    for (int row : rowsToCheck)
    {
        if (row < 0 || row >= MAX_ROW) continue;
        
        // CRITICAL FIX: Use iterator to avoid invalidation during iteration
        auto& zombiesInThisRow = allZombiesInRow[row];
        for (auto it = zombiesInThisRow.begin(); it != zombiesInThisRow.end(); ++it)
        {
            Zombie* zombie = *it;
            // Check pointer validity and skip dead/dying zombies
            if (zombie && !zombie->isDead() && zombie->getPositionX() > plantX)
            {
                return true;
            }
        }
    }
    
    return false;
}

