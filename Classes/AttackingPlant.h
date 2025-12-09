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
     * @param zombiesInRow All zombies in the same row as this plant
     * @return std::vector<Bullet*> Returns vector of bullets created (can be empty, single, or multiple)
     */
    virtual std::vector<Bullet*> checkAndAttack(const std::vector<Zombie*>& zombiesInRow) = 0;

protected:
    AttackingPlant() : Plant() {}
    virtual ~AttackingPlant() {}

    /**
     * @brief Check if any zombie is in attack range
     * @param zombiesInRow All zombies in the same row
     * @return true if zombie detected, false otherwise
     */
    bool isZombieInRange(const std::vector<Zombie*>& zombiesInRow);
};

