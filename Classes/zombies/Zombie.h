
#pragma once

#include "cocos2d.h"
#include "GameObject.h"
#include "GameDefs.h"
#include "audio/include/AudioEngine.h"
#include <vector>
#include <string>

// Forward declaration
class Plant;

/**
 * @brief Zombie class, inherits from GameObject.
 * Zombies have different states: walking, eating plants, dying.
 * Zombie can be directly instantiated as a normal zombie.
 */
class Zombie : public GameObject
{
public:

    virtual bool init() = 0;

    virtual void update(float delta);

    virtual void updateEating(float delta);

    virtual void updateMoving(float delta);

    void setState(int newState);

    virtual void setAnimationForState() = 0;

    virtual void takeDamage(float damage);

    virtual void encounterPlant(const std::vector<Plant*>& plants);

    bool isDead() const { return _isDead && !_isDying; }

    virtual void startEating(Plant* plant);

    virtual void onPlantDied();

    /**
     * @brief Get coin drop bonus multiplier for this zombie type
     * @return Coin drop bonus multiplier (default 1.0f)
     */
    virtual float getCoinDropBonus() const { return 1.0f; }

    /**
     * @brief Check if this zombie should play metal hit sound effect when hit by bullet
     * @return true if metal hit sound should be played, false for regular hit sound
     */
    virtual bool playsMetalHitSound() const { return false; }

    /**
     * @brief Check if this zombie is a Zomboni type
     * @return true if this is a Zomboni zombie, false otherwise
     */
    virtual bool isZomboni() const { return false; }

    /**
     * @brief Check if this zombie has been attacked by spikeweed/rock
     * @return true if already attacked, false otherwise
     */
    virtual bool hasBeenAttackedBySpike() const { return false; }

    /**
     * @brief Set special death state for certain zombie types (like Zomboni)
     */
    virtual void setSpecialDeath() { /* Default implementation does nothing */ }

protected:
    //0 dying
    //1 walking
    //2 eating
    int _currentState = 1;

    bool _isDying = false;
    bool _isDead = false;
    int _currentHealth = 200;
    float _accumulatedTime = 0.0f;
    bool _isEating = false;
    Plant* _targetPlant = nullptr;
    float _currentSpeed = 20.0f;
    float MOVE_SPEED = 20.0f;
    float ATTACK_DAMAGE = 10.0f;
    float ATTACK_INTERVAL = 0.5f;
    int MAX_HEALTH = 200;
    float X_CORRECTION = 40.0f;
    float SIZE_CORRECTION = 100.0f;
};
