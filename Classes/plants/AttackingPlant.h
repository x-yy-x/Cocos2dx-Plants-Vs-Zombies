#ifndef __ATTACKING_PLANT_H__
#define __ATTACKING_PLANT_H__

#include "Plant.h"
#include <vector>

// Forward declarations
class Zombie;
class Bullet;

/**
 * @brief Abstract base class for plants that engage in combat.
 * Inherits virtually from Plant to support multiple inheritance if needed.
 * This class provides common logic for detecting threats within specific rows.
 */
class AttackingPlant : virtual public Plant
{
public:
    /**
     * @brief Identifies this plant as part of the attacking category.
     * @return PlantCategory::ATTACKING
     */
    virtual PlantCategory getCategory() const override { return PlantCategory::ATTACKING; }

    /**
     * @brief Pure virtual function to handle the plant's unique attack logic.
     * Called by GameWorld to determine if a plant should fire or strike.
     * @param allZombiesInRow Array of zombie vectors, one for each game row.
     * @param plantRow The current row index (0-4) this plant occupies.
     * @return A vector of created Bullet pointers (can be empty if no attack occurs).
     */
    virtual std::vector<Bullet*> checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow) = 0;

protected:
    AttackingPlant() : Plant() {}
    virtual ~AttackingPlant() {}

    /**
     * @brief Detects if there is a valid target ahead of the plant in its own row.
     * @param zombiesInRow The vector of zombies occupying the same row.
     * @return true if a live zombie is found to the right of the plant.
     */
    bool isZombieInRange(const std::vector<Zombie*>& zombiesInRow);

    /**
     * @brief Detects targets across multiple rows (e.g., for Threepeater).
     * @param allZombiesInRow Global array of zombie vectors.
     * @param rowsToCheck List of row indices to scan for targets.
     * @return true if at least one live zombie is found in any of the specified rows.
     */
    bool isZombieInRangeMultiRow(std::vector<Zombie*> allZombiesInRow[MAX_ROW],
        const std::vector<int>& rowsToCheck);
};

#endif // __ATTACKING_PLANT_H__