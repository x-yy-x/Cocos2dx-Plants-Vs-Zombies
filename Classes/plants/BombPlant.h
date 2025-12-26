#pragma once

#include "Plant.h"
#include <vector>

// Forward declaration
class Zombie;

/**
 * @brief Abstract base class for bomb plants
 * Bomb plants explode after animation and deal massive damage to nearby zombies
 */
class BombPlant : public Plant
{
public:
    /**
     * @brief Get plant category
     */
    virtual PlantCategory getCategory() const override { return PlantCategory::BOMB; }

    /**
     * @brief Check if bomb has exploded
     */
    bool hasExploded() const { return _hasExploded; }

    /**
     * @brief Trigger explosion and return affected zombies
     * @param allZombiesInRow All zombies in each row
     * @param plantRow The row this plant is in
     * @param plantCol The column this plant is in
     */
    virtual void explode(std::vector<Zombie*> allZombiesInRow[5], int plantRow, int plantCol) = 0;

protected:
    BombPlant() : Plant(), _hasExploded(false), _animationFinished(false) {}
    virtual ~BombPlant() {}

    bool _hasExploded;           // Has the bomb exploded
    bool _animationFinished;     // Has the animation finished
    int _explosionDamage;        // Explosion damage
    int _explosionRadius;        // Explosion radius (in grid cells)

    /**
     * @brief Play explosion animation
     */
    virtual void playExplosionAnimation() = 0;

    /**
     * @brief Get zombies within explosion range
     * @param allZombiesInRow All zombies in each row
     * @param centerRow Explosion center row
     * @param centerCol Explosion center column
     * @return Vector of zombies within range
     */
    std::vector<Zombie*> getZombiesInRange(std::vector<Zombie*> allZombiesInRow[5], int centerRow, int centerCol);
};

