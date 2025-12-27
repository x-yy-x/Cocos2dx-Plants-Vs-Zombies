#include "BombPlant.h"
#include "Zombie.h"
#include "GameDefs.h"
#include <cmath>

USING_NS_CC;

// ---------------------------------------------------------
// Explosion Range Logic
// ---------------------------------------------------------

std::vector<Zombie*> BombPlant::getZombiesInRange(std::vector<Zombie*> allZombiesInRow[5], int centerRow, int centerCol)
{
    std::vector<Zombie*> zombiesInRange;

    // Define the boundaries of the explosion based on the grid radius
    int minRow = std::max(0, centerRow - explosion_radius);
    int maxRow = std::min(MAX_ROW - 1, centerRow + explosion_radius);

    // Iterate through all rows affected by the blast
    for (int row = minRow; row <= maxRow; ++row)
    {
        auto& zombiesInThisRow = allZombiesInRow[row];

        // Use iterators to safely traverse the list of zombies in the row
        for (auto it = zombiesInThisRow.begin(); it != zombiesInThisRow.end(); ++it)
        {
            Zombie* zombie = *it;

            // Validate the zombie is alive and present
            if (zombie && !zombie->isDead())
            {
                // Convert world position back to grid coordinates for collision checking
                Vec2 zombiePos = zombie->getPosition();
                int zombieCol = static_cast<int>((zombiePos.x - GRID_ORIGIN.x) / CELLSIZE.width);
                int zombieRow = static_cast<int>((zombiePos.y - GRID_ORIGIN.y) / CELLSIZE.height);

                // Calculate the grid-based distance from the explosion center
                int colDist = std::abs(zombieCol - centerCol);
                int rowDist = std::abs(zombieRow - centerRow);

                /** * A zombie is hit if it falls within the square radius.
                 * For a Cherry Bomb, radius is 1, creating a 3x3 grid of effect.
                 */
                if (colDist <= explosion_radius && rowDist <= explosion_radius)
                {
                    zombiesInRange.push_back(zombie);
                }
            }
        }
    }

    return zombiesInRange;
}