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
     * @brief Check if the plant is dead.
     * @return true if dead, false if alive
     */
    bool isDead() const;

    /**
     * @brief Apply damage to the plant and reduce health.
     * @param damage Damage value
     */
    void takeDamage(int damage);

    /**
     * @brief Virtual attack method. Default implementation returns nullptr.
     * Subclasses that can attack (like PeaShooter) should override this.
     * @return Pointer to created Bullet, or nullptr if no attack happened
     */
    virtual Bullet* attack();

protected:
    // Protected constructor
    Plant();

    /**
     * @brief Set plant position, called in plant constructor
     * @param pos Plant initial position
     */
    void setPlantPosition(const cocos2d::Vec2& pos);

    /**
     * @brief Detect and handle enemies within attack range.
     * @param zombies Vector of all zombies currently in the scene
     */
    virtual void encounterEnemy(const std::vector<Zombie*>& zombies);

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
