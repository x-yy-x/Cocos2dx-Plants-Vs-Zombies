#pragma once

#include "cocos2d.h"
#include "GameObject.h"
#include "GameDefs.h"
#include <vector>

// Forward declaration
class Zombie;
class Bullet;

/**
 * @brief Plant class, inherits from GameObject.
 * Contains basic plant attributes such as health, cooldown time, attack range, etc.
 * Plant is an abstract base class and should not be instantiated directly.
 */
class Plant : public GameObject
{
public:
    // Virtual destructor
    virtual ~Plant();

    /**
     * @brief Initialization function, calls GameObject::init()
     */
    virtual bool init() override;

    /**
     * @brief Update function called every frame for cooldown logic and attack checks.
     * @param delta Time delta (time elapsed since last frame)
     */
    virtual void update(float delta) override;

    /**
     * @brief Get plant category (Sun-producing, Attacking, or Bomb)
     * @return PlantCategory enum value
     */
    virtual PlantCategory getCategory() const = 0;

    /**
     * @brief Check if the plant is dead.
     * @return true if dead, false if alive
     */
    bool isDead() const;

    /**
     * @brief Check if the plant is a spike plant (SpikeWeed or SpikeRock).
     * This function is used to avoid dynamic_cast usage.
     * @return true if spike plant, false otherwise
     */
    virtual bool isSpike() const { return false; }

    /**
     * @brief Check if this plant can be upgraded to the specified plant type.
     * This function is used to avoid dynamic_cast usage in upgrade checks.
     * @param upgradePlantName The target plant type to upgrade to
     * @return true if this plant can be upgraded to the target type
     */
    virtual bool canBeUpgradedTo(PlantName upgradePlantName) const { return false; }

    /**
     * @brief Apply damage to the plant and reduce health.
     * @param damage Damage value
     */
    void takeDamage(float damage);

protected:
    // Protected constructor
    Plant();

    /**
     * @brief Set plant position, called in plant constructor
     * @param pos Plant initial position
     */
    void setPlantPosition(const cocos2d::Vec2& pos);

    /**
     * @brief Template method for creating plant at position
     * Reduces code duplication across all plant subclasses
     */
    template<typename T>
    static T* createPlantAtPosition(const cocos2d::Vec2& globalPos, int dx = 30, int dy = 8)
    {
        int col = static_cast<int>((globalPos.x - GRID_ORIGIN.x) / CELLSIZE.width);
        int row = static_cast<int>((globalPos.y - GRID_ORIGIN.y) / CELLSIZE.height);

        if (col < 0 || col >= MAX_COL || row < 0 || row >= MAX_ROW) {
            return nullptr;
        }

        float centerX = GRID_ORIGIN.x + col * CELLSIZE.width + CELLSIZE.width * 0.5f;
        float centerY = GRID_ORIGIN.y + row * CELLSIZE.height + CELLSIZE.height * 0.5f;

        cocos2d::Vec2 plantPos(centerX + static_cast<float>(dx), centerY + static_cast<float>(dy));

        auto plant = T::create();
        if (plant)
        {
            plant->setPlantPosition(plantPos);
        }

        return plant;
    }

    /**
     * @brief Helper method to initialize plant with common settings
     * @param imageFile Image file name
     * @param initialRect Initial sprite rectangle
     * @param maxHealth Maximum health
     * @param cooldown Cooldown interval
     * @return true if successful
     */
    bool initPlantWithSettings(const std::string& imageFile, 
                               const cocos2d::Rect& initialRect,
                               int maxHealth, 
                               float cooldown);


    /**
     * @brief Set up animation frames. To be implemented by subclasses.
     */
    virtual void setAnimation();

    // ----------------------------------------------------
    // Static constants
    // ----------------------------------------------------
    static const float ATTACK_RANGE;  // Attack range

    // ----------------------------------------------------
    // Member variables
    // ----------------------------------------------------
    bool _isDead;              // Is dead flag
    int _maxHealth;            // Maximum health
    int _currentHealth;        // Current health
    float _cooldownInterval;   // Cooldown interval (in seconds)
    float _accumulatedTime;    // Current cooldown accumulated time
    cocos2d::Vec2 _plantPos;   // Plant position
};
