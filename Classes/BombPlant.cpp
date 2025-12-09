#include "BombPlant.h"
#include "Zombie.h"
#include "GameDefs.h"
#include <cmath>

USING_NS_CC;

std::vector<Zombie*> BombPlant::getZombiesInRange(std::vector<Zombie*> allZombiesInRow[5], int centerRow, int centerCol)
{
    std::vector<Zombie*> zombiesInRange;

    // Check rows within explosion radius
    int minRow = std::max(0, centerRow - _explosionRadius);
    int maxRow = std::min(MAX_ROW - 1, centerRow + _explosionRadius);

    for (int row = minRow; row <= maxRow; ++row)
    {
        for (auto zombie : allZombiesInRow[row])
        {
            if (zombie && !zombie->isDead())
            {
                // Calculate zombie's grid position
                Vec2 zombiePos = zombie->getPosition();
                int zombieCol = (zombiePos.x - GRID_ORIGIN.x) / CELLSIZE.width;
                int zombieRow = (zombiePos.y - GRID_ORIGIN.y) / CELLSIZE.height;

                // Check if zombie is within explosion radius
                int colDist = std::abs(zombieCol - centerCol);
                int rowDist = std::abs(zombieRow - centerRow);

                if (colDist <= _explosionRadius && rowDist <= _explosionRadius)
                {
                    zombiesInRange.push_back(zombie);
                }
            }
        }
    }

    return zombiesInRange;
}

