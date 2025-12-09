#include "AttackingPlant.h"
#include "Zombie.h"

USING_NS_CC;

bool AttackingPlant::isZombieInRange(const std::vector<Zombie*>& zombiesInRow)
{
    float plantX = this->getPositionX();
    
    for (auto zombie : zombiesInRow)
    {
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
        
        for (auto zombie : allZombiesInRow[row])
        {
            if (zombie && !zombie->isDead() && zombie->getPositionX() > plantX)
            {
                return true;
            }
        }
    }
    
    return false;
}

