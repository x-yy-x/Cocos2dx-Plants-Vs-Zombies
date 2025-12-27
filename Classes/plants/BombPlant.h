#ifndef __BOMB_PLANT_H__
#define __BOMB_PLANT_H__

#include "Plant.h"
#include <vector>

// Forward declaration
class Zombie;

/**
 * @brief Abstract base class for explosive plants (e.g., Cherry Bomb, Jalapeno).
 * Bomb plants trigger an explosion after an animation sequence, dealing high
 * area-of-effect (AoE) damage to zombies within a specified grid radius.
 */
class BombPlant : public Plant
{
public:
    /**
     * @brief Identifies this plant as part of the bomb category.
     * @return PlantCategory::BOMB
     */
    virtual PlantCategory getCategory() const override { return PlantCategory::BOMB; }

    /**
     * @brief Checks if the bomb has already triggered its explosion logic.
     */
    bool hasExploded() const { return has_exploded; }

    /**
     * @brief Pure virtual function to execute the explosion.
     * Must be implemented by subclasses to handle specific damage values and effects.
     * @param allZombiesInRow Global array of zombie vectors.
     * @param plantRow The row coordinate of the explosion center.
     * @param plantCol The column coordinate of the explosion center.
     */
    virtual void explode(std::vector<Zombie*> allZombiesInRow[5], int plantRow, int plantCol) = 0;

protected:
    BombPlant() : Plant(), has_exploded(false), animation_finished(false) {}
    virtual ~BombPlant() {}

    bool has_exploded;           // State flag to prevent multiple explosions
    bool animation_finished;     // Flag to track the visual state
    int explosion_damage;        // Amount of damage dealt to zombies
    int explosion_radius;        // Radius in grid cells (e.g., 1 for a 3x3 area)

    /**
     * @brief Pure virtual function to handle specific explosion visuals.
     */
    virtual void playExplosionAnimation() = 0;

    /**
     * @brief Helper method to identify which zombies are inside the blast zone.
     * @param allZombiesInRow Global array of zombie vectors.
     * @param centerRow Grid row of the explosion.
     * @param centerCol Grid column of the explosion.
     * @return A vector containing pointers to all affected live zombies.
     */
    std::vector<Zombie*> getZombiesInRange(std::vector<Zombie*> allZombiesInRow[5], int centerRow, int centerCol);
};

#endif // __BOMB_PLANT_H__