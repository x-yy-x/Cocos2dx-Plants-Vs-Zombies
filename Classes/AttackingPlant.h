#pragma once

#include "Plant.h"
#include <vector>

// Forward declarations
class Zombie;
class Bullet;

/**
 * @brief Abstract base class for attacking plants
 */
class AttackingPlant : public Plant
{
public:
    /**
     * @brief Get plant category
     */
    virtual PlantCategory getCategory() const override { return PlantCategory::ATTACKING; }

    /**
     * @brief Check for zombies in range and attack if possible
     * @param allZombiesInRow All zombies in each row (array of 5 vectors)
     * @param plantRow The row this plant is in
     * @return std::vector<Bullet*> Returns vector of bullets created (can be empty, single, or multiple)
     */
    virtual std::vector<Bullet*> checkAndAttack(std::vector<Zombie*> allZombiesInRow[MAX_ROW], int plantRow) = 0;

protected:
    AttackingPlant() : Plant() {}
    virtual ~AttackingPlant() {}

    /**
     * @brief Check if any zombie is in attack range (single row)
     * @param zombiesInRow All zombies in the same row
     * @return true if zombie detected, false otherwise
     */
    bool isZombieInRange(const std::vector<Zombie*>& zombiesInRow);

    /**
     * @brief Check if any zombie is in attack range (multiple rows)
     * @param allZombiesInRow All zombies in each row
     * @param rowsToCheck Vector of row indices to check
     * @return true if zombie detected, false otherwise
     */
    bool isZombieInRangeMultiRow(std::vector<Zombie*> allZombiesInRow[MAX_ROW], 
                                 const std::vector<int>& rowsToCheck);
};

