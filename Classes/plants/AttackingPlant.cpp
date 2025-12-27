#include "AttackingPlant.h"
#include "Zombie.h"

USING_NS_CC;

// ---------------------------------------------------------
// Target Detection Logic
// ---------------------------------------------------------

bool AttackingPlant::isZombieInRange(const std::vector<Zombie*>& zombiesInRow)
{
    float plantX = this->getPositionX();

    // Scan the row for targets
    for (auto it = zombiesInRow.begin(); it != zombiesInRow.end(); ++it)
    {
        Zombie* zombie = *it;

        /**
         * A zombie is a valid target if:
         * 1. The pointer is valid.
         * 2. It is not already dead/in death animation.
         * 3. It is to the right of the plant (Pea Shooters don't shoot backward).
         */
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
        // Safety check for row boundaries
        if (row < 0 || row >= MAX_ROW) continue;

        auto& zombiesInThisRow = allZombiesInRow[row];

        // Use iterators to safely traverse the zombie list
        for (auto it = zombiesInThisRow.begin(); it != zombiesInThisRow.end(); ++it)
        {
            Zombie* zombie = *it;

            // Apply standard target validation logic
            if (zombie && !zombie->isDead() && zombie->getPositionX() > plantX)
            {
                return true;
            }
        }
    }

    return false;
}