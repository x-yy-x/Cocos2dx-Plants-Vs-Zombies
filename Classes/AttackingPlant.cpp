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

