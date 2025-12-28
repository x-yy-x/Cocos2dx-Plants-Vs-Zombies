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
 * @brief Zombie base class, inherits from GameObject.
 * All zombie types should derive from this class.
 * Handles common zombie behaviors such as movement, eating, taking damage, and state switching.
 */
class Zombie : public GameObject
{
public:

    /**
     * @brief Initialize zombie object.
     * Must be implemented by derived zombie classes.
     */
    virtual bool init() = 0;

    /**
     * @brief Per-frame update function.
     * Dispatches update logic based on current zombie state.
     */
    virtual void update(float delta);

    /**
     * @brief Update logic when zombie is eating a plant.
     * Handles attack timing and damage dealing.
     */
    virtual void updateEating(float delta);

    /**
     * @brief Update logic when zombie is moving forward.
     * Default behavior moves zombie horizontally based on speed.
     */
    virtual void updateMoving(float delta);

    /**
     * @brief Set zombie state.
     * Triggers animation update if state changes.
     */
    void setState(int newState);

    /**
     * @brief Set animation corresponding to current zombie state.
     * Must be implemented by derived classes.
     */
    virtual void setAnimationForState() = 0;

    /**
     * @brief Apply damage to zombie.
     * Handles death transition when health reaches zero.
     */
    virtual void takeDamage(float damage);

    /**
     * @brief Check collision with plants and handle eating logic.
     * Called by GameWorld or row manager.
     */
    virtual void encounterPlant(const std::vector<Plant*>& plants);

    /**
     * @brief Check whether zombie is fully dead.
     * @return true if zombie is dead and not in dying animation.
     */
    inline bool isDead() const { return is_dead && !is_dying; }

    /**
     * @brief Start eating a plant.
     * Switches zombie into eating state and stops movement.
     */
    virtual void startEating(Plant* plant);

    /**
     * @brief Called when the current target plant is destroyed.
     * Zombie resumes walking behavior.
     */
    virtual void onPlantDied();

    /**
     * @brief Get coin drop bonus multiplier for this zombie type.
     * @return Coin drop bonus multiplier (default 1.0f)
     */
    virtual float getCoinDropBonus() const { return 1.0f; }

    /**
     * @brief Check if this zombie should play metal hit sound effect when hit by bullet.
     * @return true if metal hit sound should be played, false otherwise.
     */
    virtual bool playsMetalHitSound() const { return false; }

    /**
     * @brief Check if this zombie is a Zomboni type.
     * @return true if this is a Zomboni zombie, false otherwise.
     */
    virtual bool isZomboni() const { return false; }

    /**
     * @brief Check if this zombie has already been damaged by spike-type plants.
     * @return true if already attacked, false otherwise.
     */
    virtual bool hasBeenAttackedBySpike() const { return false; }

    /**
     * @brief Trigger special death logic for certain zombie types.
     * Default implementation does nothing.
     */
    virtual void setSpecialDeath() { /* Default implementation does nothing */ }

protected:
    /// Current zombie state (walking, eating, dying, etc.)
    int current_state = 1;

    /// Whether zombie is currently playing death animation
    bool is_dying = false;

    /// Whether zombie has completely died and should be removed
    bool is_dead = false;

    /// Current health value
    int current_health = 200;

    /// Accumulated time used for attack cooldown timing
    float accumulated_time = 0.0f;

    /// Whether zombie is currently eating a plant
    bool is_eating = false;

    /// Pointer to the plant currently being eaten
    Plant* target_plant = nullptr;

    /// Current horizontal movement speed
    float current_speed = 20.0f;

    /// Default walking speed
    float MOVE_SPEED = 20.0f;

    /// Damage dealt to plant per attack
    float ATTACK_DAMAGE = 10.0f;

    /// Time interval between attacks when eating
    float ATTACK_INTERVAL = 0.5f;

    /// Maximum health value
    int MAX_HEALTH = 200;

    /// X-axis offset for collision detection (bite position correction)
    float X_CORRECTION = 40.0f;

    /// Width reduction for collision box to avoid early collisions
    float SIZE_CORRECTION = 100.0f;
};
